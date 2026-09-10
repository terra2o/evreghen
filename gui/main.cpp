/* evreghen - A Cross-Platform Game Engine and Build System for Terminal Games
Copyright (C)  2026 Emir Baha Yıldırım <jayshozie@gmail.com>
Copyright (C)  2026 terra2o <terra2o@protonmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>. */

#include "evreghen/enemy.hpp"
#include "evreghen/map.hpp"
#include "evreghen/map_loader.hpp"
#include "evreghen/npc.hpp"
#include "evreghen/player.hpp"
#include "evreghen/resource.hpp"
#include "evreghen/tile_registry.hpp"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <nlohmann/json.hpp>

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#if defined(_WIN32)
#include <shellapi.h>
#include <windows.h>
#else
#include <cerrno>
#include <csignal>
#include <sys/types.h>
#include <unistd.h>
#endif

namespace {

constexpr const char *defaultMapJson = R"json({
 "compressionlevel":-1,
 "height":16,
 "infinite":false,
 "layers":[
        {
         "data":[
            10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
            10,  2,  2,  2,  2,  2,  2, 10,  2,  2,  2,  2,  2,  2,  4, 10,
            10,  2, 10, 10,  2, 10,  2, 10,  2, 10, 10,  2, 10, 10,  2, 10,
            10,  2, 10, 10,  2, 10,  2,  2,  2, 10, 10,  2, 10, 10,  2, 10,
            10,  2,  2,  2,  2,  2,  2, 10,  2,  2,  2,  2,  2,  2,  2, 10,
            10, 10, 10,  2, 10, 10, 10, 10, 10, 10, 10,  2, 10, 10, 10, 10,
            10,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 10,
            10,  2, 10, 10, 10, 10,  2, 10, 10, 10, 10, 10, 10, 10,  2, 10,
            10,  2,  2,  2,  2, 10,  2,  2,  2,  2,  2,  2,  2,  2,  2, 10,
            10, 10, 10, 10,  2, 10,  2, 10, 10, 10, 10,  2, 10, 10, 10, 10,
            10,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 10,
            10,  2, 10, 10,  2, 10, 10, 10, 10, 10, 10,  2, 10, 10,  2, 10,
            10,  2, 10, 10,  2,  2,  2,  2,  2,  2,  2,  2, 10, 10,  2, 10,
            10,  2,  2,  2,  2, 10, 10,  2, 10, 10,  2,  2,  2,  2,  2, 10,
            10,  2,  2,  2,  2, 10, 10,  2, 10, 10,  2,  2,  2,  2,  2, 10,
            10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10
         ],
         "height":16,
         "id":1,
         "name":"Tile Layer 1",
         "opacity":1,
         "type":"tilelayer",
         "visible":true,
         "width":16,
         "x":0,
         "y":0
        },
        {
         "id":2,
         "name":"Objects",
         "opacity":1,
         "type":"objectgroup",
         "visible":true,
         "objects":[
            {"id":1, "name":"Hero", "type":"player", "visible":true, "x":32, "y":32},
            {"id":2, "name":"Elder", "type":"npc", "visible":true, "x":96, "y":96,
             "properties":[
                {"name":"dialogue", "type":"string", "value":"Welcome to Evreghen, brave adventurer."},
                {"name":"dialogue", "type":"string", "value":"This UI was built for editing entities live."}
             ]},
            {"id":3, "name":"Dungeon Goblin", "type":"enemy", "visible":true, "x":384, "y":256,
             "properties":[
                {"name":"hp", "type":"int", "value":40},
                {"name":"attack", "type":"int", "value":12}
             ]}
         ]
        }],
 "nextlayerid":3,
 "nextobjectid":4,
 "orientation":"orthogonal",
 "renderorder":"right-down",
 "tiledversion":"1.12.2",
 "tileheight":32,
 "tilesets":[
        {
         "firstgid":1,
         "source":"evreghen.tsx"
        }],
 "tilewidth":32,
 "type":"map",
 "version":"1.10",
 "width":16
})json";

enum class SelectionType { None, Player, NPC, Enemy };

enum class InspectorMode { None, Entity, FileResource };

enum class TerminalMode { AutoDetect, Manual };

enum class LogLevel { Info, Warning, Error };

struct LogEntry {
	LogLevel level{LogLevel::Info};
	std::string timestamp;
	std::string message;
};

// in-memory log buffer with filtering and autoscroll
struct LogBuffer {
	std::vector<LogEntry> entries;
	bool showInfo{true};
	bool showWarning{true};
	bool showError{true};
	bool autoScroll{true};
	char searchFilter[128]{""};

	void add(LogLevel level, const std::string &msg)
	{
		auto now = std::chrono::system_clock::now();
		auto timeT = std::chrono::system_clock::to_time_t(now);
		std::tm tm{};
#if defined(_WIN32)
		localtime_s(&tm, &timeT);
#else
		localtime_r(&timeT, &tm);
#endif
		char timeBuf[16];
		std::strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", &tm);

		entries.push_back({level, timeBuf, msg});
	}

	void clear()
	{
		entries.clear();
	}
};

struct EditorSettings {
	TerminalMode terminalMode{TerminalMode::AutoDetect};
	char manualTerminalCmd[256]{"kitty -e"};
	char cliBinaryPath[256]{""};
	std::string detectedTerminal;
};

// project manifest mapping to project.evreghen on disk
struct ProjectConfig {
	std::string name{"Untitled Project"};
	std::string version{"1.0.0"};
	std::string mainScene{""};
	std::filesystem::path projectFilePath;
	bool loaded{false};

	bool loadFromFile(const std::filesystem::path &path)
	{
		std::ifstream file(path);
		if (!file.is_open()) {
			return false;
		}
		try {
			nlohmann::json j;
			file >> j;
			if (j.contains("name") && j["name"].is_string()) {
				name = j["name"].get<std::string>();
			}
			if (j.contains("version") && j["version"].is_string()) {
				version = j["version"].get<std::string>();
			}
			if (j.contains("main_scene") && j["main_scene"].is_string()) {
				mainScene = j["main_scene"].get<std::string>();
			}
			projectFilePath = path;
			loaded = true;
			return true;
		} catch (...) {
			return false;
		}
	}

	bool saveToFile(const std::filesystem::path &path) const
	{
		std::ofstream file(path);
		if (!file.is_open()) {
			return false;
		}
		nlohmann::json j;
		j["name"] = name;
		j["version"] = version;
		j["main_scene"] = mainScene;
		file << j.dump(4) << "\n";
		return file.good();
	}
};

// transient cache for file and entity properties inspected in the right panel
struct InspectedResourceData {
	std::string type{"generic"};
	std::filesystem::path path;
	std::string name;
	char glyph{'?'};
	int hp{30};
	int attack{10};
	std::vector<std::string> dialogues;
	std::string mainScene;
	std::string projectVersion{"1.0.0"};
	size_t fileSize{0};
	int mapWidth{0};
	int mapHeight{0};
	char newDialogueBuffer[256]{""};
};

// global editor state encapsulating active scene, navigation, and settings
struct EditorState {
	evreghen::TileRegistry registry;
	evreghen::Map map;
	std::string currentFilePath;
	std::string argv0{"evreghen-gui"};

	std::filesystem::path projectRoot;
	std::filesystem::path currentBrowserPath;
	ProjectConfig project;

	SelectionType selectionType{SelectionType::None};
	int selectedIndex{-1};

	InspectorMode inspectorMode{InspectorMode::None};
	InspectedResourceData inspectedData;

	LogBuffer logger;
	EditorSettings settings;

	char filePathBuffer[256]{""};
	char resourcePathBuffer[256]{""};
	char newDialogueBuffer[256]{""};
	char newMapPathBuffer[256]{"map.json"};
	int newMapWidth{16};
	int newMapHeight{16};

	bool openMapModalOpen{false};
	bool saveMapModalOpen{false};
	bool resModalOpen{false};
	bool isExportingResource{false};
	bool editorSettingsModalOpen{false};
	bool aboutModalOpen{false};
	bool newMapModalOpen{false};
};

// fallback to searching common terminal binary names across path
std::string detectTerminalCommand()
{
	const char *candidates[] = {
			"kitty",		  "alacritty", "gnome-terminal",	 "konsole",
			"xfce4-terminal", "xterm",	   "x-terminal-emulator"};

	const char *pathEnv = std::getenv("PATH");
	if (pathEnv == nullptr) {
		return "kitty -e";
	}

	std::string pathStr = pathEnv;
	std::vector<std::string> dirs;
	size_t start = 0;
	size_t end = pathStr.find(':');
	while (end != std::string::npos) {
		dirs.push_back(pathStr.substr(start, end - start));
		start = end + 1;
		end = pathStr.find(':', start);
	}
	dirs.push_back(pathStr.substr(start));

	for (const auto *cand : candidates) {
		for (const auto &dir : dirs) {
			std::filesystem::path p = std::filesystem::path(dir) / cand;
			std::error_code ec;
			if (std::filesystem::is_regular_file(p, ec)) {
				if (std::strcmp(cand, "gnome-terminal") == 0) {
					return std::string(cand) + " --";
				}
				if (std::strcmp(cand, "xfce4-terminal") == 0) {
					return std::string(cand) + " -x";
				}
				return std::string(cand) + " -e";
			}
		}
	}

	return "xterm -e";
}

// discovers evreghen binary relative to gui executable or in build tree
std::filesystem::path findCliBinary(const char *argv0,
									const std::string &customPath)
{
	if (!customPath.empty()) {
		std::error_code ec;
		if (std::filesystem::is_regular_file(customPath, ec)) {
			return std::filesystem::canonical(customPath, ec);
		}
	}

	std::error_code ec;
	std::filesystem::path selfPath =
			std::filesystem::canonical(std::filesystem::path(argv0), ec);
	if (!ec) {
		std::filesystem::path sibling =
				selfPath.parent_path() / "../cli/evreghen";
		if (std::filesystem::is_regular_file(sibling, ec)) {
			return std::filesystem::canonical(sibling, ec);
		}
		sibling = selfPath.parent_path() / "evreghen";
		if (std::filesystem::is_regular_file(sibling, ec)) {
			return std::filesystem::canonical(sibling, ec);
		}
	}

	std::filesystem::path cwdCandidate = "build/cli/evreghen";
	if (std::filesystem::is_regular_file(cwdCandidate, ec)) {
		return std::filesystem::canonical(cwdCandidate, ec);
	}

	return "evreghen";
}

// spawns cli binary in a dedicated external terminal emulator
bool launchTerminalCommand(const EditorSettings &settings,
						   const std::filesystem::path &cliBin,
						   const std::vector<std::string> &args,
						   std::string &errorMsg)
{
	std::string termCmd = (settings.terminalMode == TerminalMode::Manual)
								  ? std::string(settings.manualTerminalCmd)
								  : settings.detectedTerminal;

#if defined(_WIN32)
	std::string cmd = "start cmd /k " + cliBin.string();
	for (const auto &a : args) {
		cmd += " \"" + a + "\"";
	}
	int res = std::system(cmd.c_str());
	if (res != 0) {
		errorMsg = "system call failed with code: " + std::to_string(res);
		return false;
	}
	return true;
#else
	// parse terminal command into separate argument tokens for execvp
	std::vector<std::string> tokens;
	std::istringstream iss(termCmd);
	std::string s;
	while (iss >> s) {
		tokens.push_back(s);
	}
	if (tokens.empty()) {
		errorMsg = "terminal command string is empty";
		return false;
	}

	tokens.push_back(cliBin.string());
	for (const auto &arg : args) {
		tokens.push_back(arg);
	}

	std::vector<char *> argvList;
	argvList.reserve(tokens.size() + 1);
	for (auto &tok : tokens) {
		argvList.push_back(tok.data());
	}
	argvList.push_back(nullptr);

	pid_t pid = fork();
	if (pid < 0) {
		errorMsg =
				"failed to fork process: " + std::string(std::strerror(errno));
		return false;
	}
	if (pid == 0) {
		execvp(argvList[0], argvList.data());
		std::perror("execvp");
		_exit(127);
	}

	return true;
#endif
}

// walks parent directories to locate nearest project.evreghen root
std::filesystem::path detectProjectRoot(const std::filesystem::path &startPath,
										ProjectConfig &outConfig)
{
	std::filesystem::path cur = std::filesystem::canonical(startPath);
	std::filesystem::path probe = cur;

	while (true) {
		std::error_code ec;
		for (const auto &entry :
			 std::filesystem::directory_iterator(probe, ec)) {
			if (entry.path().extension() == ".evreghen") {
				outConfig.loadFromFile(entry.path());
				return probe;
			}
		}
		if (!probe.has_parent_path() || probe == probe.parent_path()) {
			break;
		}
		probe = probe.parent_path();
	}

	return cur;
}

// populates inspector state based on file extension and parsed json content
void inspectFile(EditorState &state, const std::filesystem::path &path)
{
	state.inspectorMode = InspectorMode::FileResource;
	state.inspectedData = InspectedResourceData{};
	state.inspectedData.path = path;
	state.inspectedData.name = path.stem().string();

	std::error_code ec;
	state.inspectedData.fileSize = std::filesystem::file_size(path, ec);

	if (path.extension() == ".evreghen") {
		state.inspectedData.type = "project";
		std::ifstream file(path);
		if (file.is_open()) {
			try {
				nlohmann::json j;
				file >> j;
				if (j.contains("name") && j["name"].is_string()) {
					state.inspectedData.name = j["name"].get<std::string>();
				}
				if (j.contains("version") && j["version"].is_string()) {
					state.inspectedData.projectVersion =
							j["version"].get<std::string>();
				}
				if (j.contains("main_scene") && j["main_scene"].is_string()) {
					state.inspectedData.mainScene =
							j["main_scene"].get<std::string>();
				}
			} catch (...) {
			}
		}
		return;
	}

	if (path.extension() == ".json") {
		auto res = evreghen::Resource::loadResource(path);
		if (res) {
			if (auto *npcRes =
						dynamic_cast<evreghen::NPCResource *>(res.get())) {
				state.inspectedData.type = "npc";
				state.inspectedData.name = npcRes->getName();
				state.inspectedData.glyph = npcRes->getGlyph();
				state.inspectedData.dialogues = npcRes->getDialogues();
				return;
			}
			if (auto *enemyRes =
						dynamic_cast<evreghen::EnemyResource *>(res.get())) {
				state.inspectedData.type = "enemy";
				state.inspectedData.name = enemyRes->getName();
				state.inspectedData.glyph = enemyRes->getGlyph();
				state.inspectedData.hp = enemyRes->getMaxHealth();
				state.inspectedData.attack = enemyRes->getAttackPower();
				return;
			}
		}

		std::ifstream file(path);
		if (file.is_open()) {
			try {
				nlohmann::json j;
				file >> j;
				if (j.contains("layers") && j.contains("width") &&
					j.contains("height")) {
					state.inspectedData.type = "map";
					state.inspectedData.mapWidth = j["width"].get<int>();
					state.inspectedData.mapHeight = j["height"].get<int>();
					return;
				}
			} catch (...) {
			}
		}
	}

	state.inspectedData.type = "generic";
}

// loads map file into editor state and resets selection
bool loadMapScene(EditorState &state, const std::filesystem::path &path)
{
	try {
		state.map = evreghen::MapLoader::loadFromFile(path, state.registry);
		state.currentFilePath = path.string();
		state.selectionType = SelectionType::None;
		state.selectedIndex = -1;
		state.inspectorMode = InspectorMode::None;
		state.logger.add(LogLevel::Info, "Loaded scene: " + path.string());
		return true;
	} catch (const std::exception &ex) {
		state.logger.add(LogLevel::Error,
						 "Failed to load map: " + std::string(ex.what()));
		return false;
	}
}

// renders godot-style scene hierarchy tree with entity quick-add actions
void renderNodeTree(EditorState &state, ImGuiWindowFlags panelFlags)
{
	ImGui::Begin("Node Tree", nullptr, panelFlags);

	std::string rootLabel = "Root (Map " +
							std::to_string(state.map.getWidth()) + "x" +
							std::to_string(state.map.getHeight()) + ")";
	if (ImGui::TreeNodeEx(rootLabel.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
		if (state.map.getPlayer() != nullptr) {
			bool isSelected = (state.inspectorMode == InspectorMode::Entity &&
							   state.selectionType == SelectionType::Player);
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf |
									   ImGuiTreeNodeFlags_NoTreePushOnOpen;
			if (isSelected) {
				flags |= ImGuiTreeNodeFlags_Selected;
			}
			std::string pLabel = "[Player] " + state.map.getPlayer()->getName();
			ImGui::TreeNodeEx(pLabel.c_str(), flags);
			if (ImGui::IsItemClicked()) {
				state.inspectorMode = InspectorMode::Entity;
				state.selectionType = SelectionType::Player;
				state.selectedIndex = 0;
			}
		}

		const auto &npcs = state.map.getNpcs();
		std::string npcsHeader = "NPCs (" + std::to_string(npcs.size()) + ")";
		if (ImGui::TreeNodeEx(npcsHeader.c_str(),
							  ImGuiTreeNodeFlags_DefaultOpen)) {
			for (size_t i = 0; i < npcs.size(); ++i) {
				if (!npcs[i]) {
					continue;
				}
				bool isSelected =
						(state.inspectorMode == InspectorMode::Entity &&
						 state.selectionType == SelectionType::NPC &&
						 state.selectedIndex == static_cast<int>(i));
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf |
										   ImGuiTreeNodeFlags_NoTreePushOnOpen;
				if (isSelected) {
					flags |= ImGuiTreeNodeFlags_Selected;
				}
				std::string label = "[NPC] " + npcs[i]->getName() + " (" +
									std::to_string(npcs[i]->getX()) + ", " +
									std::to_string(npcs[i]->getY()) + ")";
				ImGui::TreeNodeEx(label.c_str(), flags);
				if (ImGui::IsItemClicked()) {
					state.inspectorMode = InspectorMode::Entity;
					state.selectionType = SelectionType::NPC;
					state.selectedIndex = static_cast<int>(i);
				}
			}
			ImGui::TreePop();
		}

		const auto &enemies = state.map.getEnemies();
		std::string enemiesHeader =
				"Enemies (" + std::to_string(enemies.size()) + ")";
		if (ImGui::TreeNodeEx(enemiesHeader.c_str(),
							  ImGuiTreeNodeFlags_DefaultOpen)) {
			for (size_t i = 0; i < enemies.size(); ++i) {
				if (!enemies[i]) {
					continue;
				}
				bool isSelected =
						(state.inspectorMode == InspectorMode::Entity &&
						 state.selectionType == SelectionType::Enemy &&
						 state.selectedIndex == static_cast<int>(i));
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf |
										   ImGuiTreeNodeFlags_NoTreePushOnOpen;
				if (isSelected) {
					flags |= ImGuiTreeNodeFlags_Selected;
				}
				std::string label = "[Enemy] " + enemies[i]->getName() + " (" +
									std::to_string(enemies[i]->getX()) + ", " +
									std::to_string(enemies[i]->getY()) + ")";
				ImGui::TreeNodeEx(label.c_str(), flags);
				if (ImGui::IsItemClicked()) {
					state.inspectorMode = InspectorMode::Entity;
					state.selectionType = SelectionType::Enemy;
					state.selectedIndex = static_cast<int>(i);
				}
			}
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}

	ImGui::Separator();
	if (ImGui::Button("+ Add NPC", ImVec2(-1, 0))) {
		state.map.addNpc(std::make_unique<evreghen::NPC>(
				"New NPC", 1, 1, std::vector<std::string>{"Hello there!"}));
		state.inspectorMode = InspectorMode::Entity;
		state.selectionType = SelectionType::NPC;
		state.selectedIndex = static_cast<int>(state.map.getNpcs().size()) - 1;
		state.logger.add(LogLevel::Info, "Added new NPC to scene");
	}

	if (ImGui::Button("+ Add Enemy", ImVec2(-1, 0))) {
		state.map.addEnemy(
				std::make_unique<evreghen::Enemy>("New Enemy", 2, 2, 30, 10));
		state.inspectorMode = InspectorMode::Entity;
		state.selectionType = SelectionType::Enemy;
		state.selectedIndex =
				static_cast<int>(state.map.getEnemies().size()) - 1;
		state.logger.add(LogLevel::Info, "Added new Enemy to scene");
	}

	ImGui::End();
}

// renders workspace file browser with double-click opening and type colors
void renderFileSystem(EditorState &state, ImGuiWindowFlags panelFlags)
{
	ImGui::Begin("File System", nullptr, panelFlags);

	std::string rootDisplay = state.project.loaded
									  ? state.project.name
									  : state.projectRoot.filename().string();
	ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "[Root: %s]",
					   rootDisplay.c_str());

	std::error_code ec;
	std::filesystem::path relPath = std::filesystem::relative(
			state.currentBrowserPath, state.projectRoot, ec);
	std::string pathText = relPath.empty() ? "." : relPath.string();
	ImGui::TextDisabled("Path: %s", pathText.c_str());

	if (state.currentBrowserPath != state.projectRoot) {
		if (ImGui::SmallButton(".. (Parent Directory)")) {
			state.currentBrowserPath = state.currentBrowserPath.parent_path();
		}
	}

	ImGui::Separator();

	ImGui::BeginChild("FileListRegion", ImVec2(0, -30), true);

	std::vector<std::filesystem::directory_entry> dirs;
	std::vector<std::filesystem::directory_entry> files;

	for (const auto &entry :
		 std::filesystem::directory_iterator(state.currentBrowserPath, ec)) {
		if (entry.is_directory()) {
			dirs.push_back(entry);
		} else if (entry.is_regular_file()) {
			files.push_back(entry);
		}
	}

	auto sortName = [](const auto &a, const auto &b) {
		return a.path().filename().string() < b.path().filename().string();
	};
	std::sort(dirs.begin(), dirs.end(), sortName);
	std::sort(files.begin(), files.end(), sortName);

	for (const auto &dir : dirs) {
		std::string label = "[DIR] " + dir.path().filename().string();
		if (ImGui::Selectable(label.c_str(), false,
							  ImGuiSelectableFlags_AllowDoubleClick)) {
			if (ImGui::IsMouseDoubleClicked(0)) {
				state.currentBrowserPath = dir.path();
			}
		}
	}

	for (const auto &file : files) {
		std::string ext = file.path().extension().string();
		std::string label;
		ImVec4 color(0.85f, 0.85f, 0.85f, 1.0f);

		if (ext == ".evreghen") {
			label = "[PROJECT] " + file.path().filename().string();
			color = ImVec4(1.0f, 0.85f, 0.35f, 1.0f);
		} else if (ext == ".json") {
			label = "[SCENE/RES] " + file.path().filename().string();
			color = ImVec4(0.4f, 0.9f, 0.6f, 1.0f);
		} else {
			label = "[FILE] " + file.path().filename().string();
		}

		bool isSelected = (state.inspectorMode == InspectorMode::FileResource &&
						   state.inspectedData.path == file.path());

		ImGui::PushStyleColor(ImGuiCol_Text, color);
		if (ImGui::Selectable(label.c_str(), isSelected,
							  ImGuiSelectableFlags_AllowDoubleClick)) {
			inspectFile(state, file.path());
			if (ImGui::IsMouseDoubleClicked(0)) {
				if (ext == ".json") {
					loadMapScene(state, file.path());
				} else if (ext == ".evreghen") {
					state.project.loadFromFile(file.path());
					state.logger.add(LogLevel::Info,
									 "Loaded project: " + state.project.name);
				}
			}
		}
		ImGui::PopStyleColor();
	}

	ImGui::EndChild();

	if (ImGui::Button("+ New Map File", ImVec2(-1, 0))) {
		state.newMapModalOpen = true;
	}

	ImGui::End();
}

// context-sensitive property editor for selected nodes or inspected files
void renderInspector(EditorState &state, ImGuiWindowFlags panelFlags)
{
	ImGui::Begin("Inspector", nullptr, panelFlags);

	if (state.inspectorMode == InspectorMode::None) {
		ImGui::TextDisabled("Select an entity or file to inspect.");
		ImGui::End();
		return;
	}

	if (state.inspectorMode == InspectorMode::Entity) {
		if (state.selectionType == SelectionType::Player) {
			auto *player = state.map.getPlayer();
			if (player == nullptr) {
				ImGui::TextDisabled("No player in scene.");
				ImGui::End();
				return;
			}

			ImGui::TextColored(ImVec4(0.35f, 0.85f, 0.45f, 1.0f),
							   "[ Player Node ]");
			ImGui::Separator();

			ImGui::Text("Name: %s", player->getName().c_str());
			ImGui::Text("Health: %d / %d", player->getHealth(),
						player->getMaxHealth());
			ImGui::Text("Attack Power: %d", player->getAttackPower());

			int pos[2] = {player->getX(), player->getY()};
			if (ImGui::DragInt2("Position (X, Y)", pos, 1.0f, 0,
								std::max(state.map.getWidth() - 1, 0))) {
				player->setPosition(pos[0], pos[1]);
			}
		} else if (state.selectionType == SelectionType::NPC) {
			auto &npcs = state.map.getNpcs();
			if (state.selectedIndex < 0 ||
				state.selectedIndex >= static_cast<int>(npcs.size()) ||
				!npcs[state.selectedIndex]) {
				ImGui::TextDisabled("Invalid NPC selected.");
				ImGui::End();
				return;
			}

			auto &npc = npcs[state.selectedIndex];
			ImGui::TextColored(ImVec4(0.35f, 0.75f, 0.95f, 1.0f),
							   "[ NPC Node ]");
			ImGui::Separator();

			char nameBuf[128];
			std::strncpy(nameBuf, npc->getName().c_str(), sizeof(nameBuf) - 1);
			nameBuf[sizeof(nameBuf) - 1] = '\0';
			if (ImGui::InputText("Name", nameBuf, sizeof(nameBuf))) {
				int nx = npc->getX();
				int ny = npc->getY();
				auto lines = npc->getDialogues();
				npc = std::make_unique<evreghen::NPC>(nameBuf, nx, ny,
													  std::move(lines));
			}

			int pos[2] = {npc->getX(), npc->getY()};
			if (ImGui::DragInt2("Position (X, Y)", pos, 1.0f, 0,
								std::max(state.map.getWidth() - 1, 0))) {
				npc->setPosition(pos[0], pos[1]);
			}

			ImGui::Spacing();
			ImGui::Text("Dialogue Lines:");
			auto dialogues = npc->getDialogues();
			int deleteIdx = -1;

			for (size_t i = 0; i < dialogues.size(); ++i) {
				ImGui::PushID(static_cast<int>(i));
				char lineBuf[256];
				std::strncpy(lineBuf, dialogues[i].c_str(),
							 sizeof(lineBuf) - 1);
				lineBuf[sizeof(lineBuf) - 1] = '\0';

				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 45);
				if (ImGui::InputText("##dlg", lineBuf, sizeof(lineBuf))) {
					dialogues[i] = lineBuf;
					npc->setDialogues(dialogues);
				}
				ImGui::SameLine();
				if (ImGui::Button("-")) {
					deleteIdx = static_cast<int>(i);
				}
				ImGui::PopID();
			}

			if (deleteIdx >= 0) {
				dialogues.erase(dialogues.begin() + deleteIdx);
				npc->setDialogues(dialogues);
			}

			ImGui::InputTextWithHint("##newdlg", "Add dialogue line...",
									 state.newDialogueBuffer,
									 sizeof(state.newDialogueBuffer));
			ImGui::SameLine();
			if (ImGui::Button("+ Add") && state.newDialogueBuffer[0] != '\0') {
				npc->addDialogue(state.newDialogueBuffer);
				state.newDialogueBuffer[0] = '\0';
			}

			ImGui::Separator();
			ImGui::Text("Resource Actions:");
			if (ImGui::Button("Export to .json Resource")) {
				state.isExportingResource = true;
				state.resModalOpen = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("Import from .json Resource")) {
				state.isExportingResource = false;
				state.resModalOpen = true;
			}

			ImGui::Spacing();
			if (ImGui::Button("Delete NPC", ImVec2(-1, 0))) {
				npcs.erase(npcs.begin() + state.selectedIndex);
				state.selectionType = SelectionType::None;
				state.selectedIndex = -1;
				state.inspectorMode = InspectorMode::None;
				state.logger.add(LogLevel::Info, "Deleted NPC node");
			}
		} else if (state.selectionType == SelectionType::Enemy) {
			auto &enemies = state.map.getEnemies();
			if (state.selectedIndex < 0 ||
				state.selectedIndex >= static_cast<int>(enemies.size()) ||
				!enemies[state.selectedIndex]) {
				ImGui::TextDisabled("Invalid Enemy selected.");
				ImGui::End();
				return;
			}

			auto &enemy = enemies[state.selectedIndex];
			ImGui::TextColored(ImVec4(0.95f, 0.40f, 0.40f, 1.0f),
							   "[ Enemy Node ]");
			ImGui::Separator();

			char nameBuf[128];
			std::strncpy(nameBuf, enemy->getName().c_str(),
						 sizeof(nameBuf) - 1);
			nameBuf[sizeof(nameBuf) - 1] = '\0';
			if (ImGui::InputText("Name", nameBuf, sizeof(nameBuf))) {
				int ex = enemy->getX();
				int ey = enemy->getY();
				int ehp = enemy->getHealth();
				int eatk = enemy->getAttackPower();
				enemy = std::make_unique<evreghen::Enemy>(nameBuf, ex, ey, ehp,
														  eatk);
			}

			int pos[2] = {enemy->getX(), enemy->getY()};
			if (ImGui::DragInt2("Position (X, Y)", pos, 1.0f, 0,
								std::max(state.map.getWidth() - 1, 0))) {
				enemy->setPosition(pos[0], pos[1]);
			}

			int hp = enemy->getHealth();
			if (ImGui::SliderInt("HP", &hp, 1, 300)) {
				int ex = enemy->getX();
				int ey = enemy->getY();
				int eatk = enemy->getAttackPower();
				enemy = std::make_unique<evreghen::Enemy>(enemy->getName(), ex,
														  ey, hp, eatk);
			}

			int atk = enemy->getAttackPower();
			if (ImGui::SliderInt("Attack Power", &atk, 1, 100)) {
				int ex = enemy->getX();
				int ey = enemy->getY();
				int ehp = enemy->getHealth();
				enemy = std::make_unique<evreghen::Enemy>(enemy->getName(), ex,
														  ey, ehp, atk);
			}

			ImGui::Separator();
			ImGui::Text("Resource Actions:");
			if (ImGui::Button("Export to .json Resource")) {
				state.isExportingResource = true;
				state.resModalOpen = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("Import from .json Resource")) {
				state.isExportingResource = false;
				state.resModalOpen = true;
			}

			ImGui::Spacing();
			if (ImGui::Button("Delete Enemy", ImVec2(-1, 0))) {
				enemies.erase(enemies.begin() + state.selectedIndex);
				state.selectionType = SelectionType::None;
				state.selectedIndex = -1;
				state.inspectorMode = InspectorMode::None;
				state.logger.add(LogLevel::Info, "Deleted Enemy node");
			}
		}
	} else if (state.inspectorMode == InspectorMode::FileResource) {
		const auto &data = state.inspectedData;
		ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.4f, 1.0f),
						   "[ File Inspector ]");
		ImGui::TextWrapped("File: %s", data.path.filename().string().c_str());
		ImGui::TextDisabled("Size: %zu bytes", data.fileSize);
		ImGui::Separator();

		if (data.type == "project") {
			ImGui::TextColored(ImVec4(0.35f, 0.85f, 0.95f, 1.0f),
							   "Project Configuration (.evreghen)");
			char nameBuf[128];
			std::strncpy(nameBuf, state.inspectedData.name.c_str(),
						 sizeof(nameBuf) - 1);
			nameBuf[sizeof(nameBuf) - 1] = '\0';
			if (ImGui::InputText("Project Name", nameBuf, sizeof(nameBuf))) {
				state.inspectedData.name = nameBuf;
			}

			char verBuf[64];
			std::strncpy(verBuf, state.inspectedData.projectVersion.c_str(),
						 sizeof(verBuf) - 1);
			verBuf[sizeof(verBuf) - 1] = '\0';
			if (ImGui::InputText("Version", verBuf, sizeof(verBuf))) {
				state.inspectedData.projectVersion = verBuf;
			}

			char mainSceneBuf[256];
			std::strncpy(mainSceneBuf, state.inspectedData.mainScene.c_str(),
						 sizeof(mainSceneBuf) - 1);
			mainSceneBuf[sizeof(mainSceneBuf) - 1] = '\0';
			if (ImGui::InputText("Main Scene", mainSceneBuf,
								 sizeof(mainSceneBuf))) {
				state.inspectedData.mainScene = mainSceneBuf;
			}

			if (ImGui::Button("Set to Current Scene") &&
				!state.currentFilePath.empty()) {
				std::error_code ec;
				std::filesystem::path rel = std::filesystem::relative(
						state.currentFilePath, state.projectRoot, ec);
				state.inspectedData.mainScene =
						rel.empty() ? state.currentFilePath : rel.string();
			}

			ImGui::Spacing();
			if (ImGui::Button("Save Project Configuration", ImVec2(-1, 0))) {
				ProjectConfig cfg;
				cfg.name = state.inspectedData.name;
				cfg.version = state.inspectedData.projectVersion;
				cfg.mainScene = state.inspectedData.mainScene;
				if (cfg.saveToFile(state.inspectedData.path)) {
					state.project = cfg;
					state.project.loaded = true;
					state.logger.add(LogLevel::Info,
									 "Saved project file: " +
											 state.inspectedData.path.filename()
													 .string());
				} else {
					state.logger.add(LogLevel::Error,
									 "Failed to save project file!");
				}
			}
		} else if (data.type == "npc") {
			ImGui::TextColored(ImVec4(0.35f, 0.75f, 0.95f, 1.0f),
							   "NPC Resource");
			char nameBuf[128];
			std::strncpy(nameBuf, state.inspectedData.name.c_str(),
						 sizeof(nameBuf) - 1);
			nameBuf[sizeof(nameBuf) - 1] = '\0';
			if (ImGui::InputText("Name", nameBuf, sizeof(nameBuf))) {
				state.inspectedData.name = nameBuf;
			}

			char glyphBuf[2] = {state.inspectedData.glyph, '\0'};
			if (ImGui::InputText("Glyph", glyphBuf, sizeof(glyphBuf))) {
				if (glyphBuf[0] != '\0') {
					state.inspectedData.glyph = glyphBuf[0];
				}
			}

			ImGui::Spacing();
			ImGui::Text("Dialogue Lines:");
			int deleteIdx = -1;
			for (size_t i = 0; i < state.inspectedData.dialogues.size(); ++i) {
				ImGui::PushID(static_cast<int>(i));
				char lineBuf[256];
				std::strncpy(lineBuf, state.inspectedData.dialogues[i].c_str(),
							 sizeof(lineBuf) - 1);
				lineBuf[sizeof(lineBuf) - 1] = '\0';
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 45);
				if (ImGui::InputText("##dlg", lineBuf, sizeof(lineBuf))) {
					state.inspectedData.dialogues[i] = lineBuf;
				}
				ImGui::SameLine();
				if (ImGui::Button("-")) {
					deleteIdx = static_cast<int>(i);
				}
				ImGui::PopID();
			}

			if (deleteIdx >= 0) {
				state.inspectedData.dialogues.erase(
						state.inspectedData.dialogues.begin() + deleteIdx);
			}

			ImGui::InputTextWithHint(
					"##addResDlg", "Add dialogue line...",
					state.inspectedData.newDialogueBuffer,
					sizeof(state.inspectedData.newDialogueBuffer));
			ImGui::SameLine();
			if (ImGui::Button("+ Add") &&
				state.inspectedData.newDialogueBuffer[0] != '\0') {
				state.inspectedData.dialogues.push_back(
						state.inspectedData.newDialogueBuffer);
				state.inspectedData.newDialogueBuffer[0] = '\0';
			}

			ImGui::Spacing();
			if (ImGui::Button("Save Resource File", ImVec2(-1, 0))) {
				evreghen::NPCResource res(state.inspectedData.name,
										  state.inspectedData.dialogues,
										  state.inspectedData.glyph);
				if (res.saveToFile(state.inspectedData.path)) {
					state.logger.add(LogLevel::Info,
									 "Saved NPC resource: " +
											 state.inspectedData.path.filename()
													 .string());
				} else {
					state.logger.add(LogLevel::Error,
									 "Failed to save NPC resource!");
				}
			}
		} else if (data.type == "enemy") {
			ImGui::TextColored(ImVec4(0.95f, 0.40f, 0.40f, 1.0f),
							   "Enemy Resource");
			char nameBuf[128];
			std::strncpy(nameBuf, state.inspectedData.name.c_str(),
						 sizeof(nameBuf) - 1);
			nameBuf[sizeof(nameBuf) - 1] = '\0';
			if (ImGui::InputText("Name", nameBuf, sizeof(nameBuf))) {
				state.inspectedData.name = nameBuf;
			}

			char glyphBuf[2] = {state.inspectedData.glyph, '\0'};
			if (ImGui::InputText("Glyph", glyphBuf, sizeof(glyphBuf))) {
				if (glyphBuf[0] != '\0') {
					state.inspectedData.glyph = glyphBuf[0];
				}
			}

			ImGui::SliderInt("HP", &state.inspectedData.hp, 1, 300);
			ImGui::SliderInt("Attack", &state.inspectedData.attack, 1, 100);

			ImGui::Spacing();
			if (ImGui::Button("Save Resource File", ImVec2(-1, 0))) {
				evreghen::EnemyResource res(
						state.inspectedData.name, state.inspectedData.hp,
						state.inspectedData.attack, state.inspectedData.glyph);
				if (res.saveToFile(state.inspectedData.path)) {
					state.logger.add(LogLevel::Info,
									 "Saved Enemy resource: " +
											 state.inspectedData.path.filename()
													 .string());
				} else {
					state.logger.add(LogLevel::Error,
									 "Failed to save Enemy resource!");
				}
			}
		} else if (data.type == "map") {
			ImGui::TextColored(ImVec4(0.4f, 0.9f, 0.6f, 1.0f), "Map / Scene");
			ImGui::Text("Dimensions: %d x %d", data.mapWidth, data.mapHeight);
			ImGui::Spacing();
			if (ImGui::Button("Open Scene in Editor", ImVec2(-1, 0))) {
				loadMapScene(state, state.inspectedData.path);
			}
		} else {
			ImGui::TextDisabled(
					"No specific inspector available for this file type.");
		}
	}

	ImGui::End();
}

// interactive 2d canvas for visual map editing and entity selection
void renderMapViewport(EditorState &state, ImGuiWindowFlags panelFlags)
{
	ImGui::Begin("Map Viewport", nullptr, panelFlags);

	int width = state.map.getWidth();
	int height = state.map.getHeight();

	if (width <= 0 || height <= 0) {
		ImGui::Text("Empty map.");
		ImGui::End();
		return;
	}

	float cellSize = 28.0f;
	ImVec2 canvasOrigin = ImGui::GetCursorScreenPos();
	ImDrawList *drawList = ImGui::GetWindowDrawList();

	// background grid
	ImVec2 canvasSize(width * cellSize, height * cellSize);
	drawList->AddRectFilled(canvasOrigin,
							ImVec2(canvasOrigin.x + canvasSize.x,
								   canvasOrigin.y + canvasSize.y),
							IM_COL32(22, 24, 30, 255));

	// draw tiles
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			uint32_t tid = state.map.getTile(x, y);
			const auto *def = state.registry.getTile(tid);

			ImVec2 pMin(canvasOrigin.x + x * cellSize,
						canvasOrigin.y + y * cellSize);
			ImVec2 pMax(pMin.x + cellSize, pMin.y + cellSize);

			ImU32 tileColor = IM_COL32(35, 40, 50, 255);
			if (def != nullptr) {
				if (def->solid) {
					tileColor = IM_COL32(60, 65, 78, 255);
				} else if (def->isTransition) {
					tileColor = IM_COL32(160, 130, 40, 255);
				} else {
					tileColor = IM_COL32(38, 44, 56, 255);
				}
			}

			drawList->AddRectFilled(pMin, pMax, tileColor);
			drawList->AddRect(pMin, pMax, IM_COL32(48, 54, 68, 80));

			if (def != nullptr && def->glyph != ' ') {
				char glyphStr[2] = {def->glyph, '\0'};
				drawList->AddText(ImVec2(pMin.x + 8.0f, pMin.y + 6.0f),
								  IM_COL32(140, 150, 170, 200), glyphStr);
			}
		}
	}

	// draw player
	const auto *player = state.map.getPlayer();
	if (player != nullptr &&
		state.map.inBounds(player->getX(), player->getY())) {
		ImVec2 pMin(canvasOrigin.x + player->getX() * cellSize,
					canvasOrigin.y + player->getY() * cellSize);
		ImVec2 pCenter(pMin.x + cellSize * 0.5f, pMin.y + cellSize * 0.5f);
		drawList->AddCircleFilled(pCenter, cellSize * 0.38f,
								  IM_COL32(60, 200, 90, 230));
		drawList->AddText(ImVec2(pMin.x + 9.0f, pMin.y + 6.0f),
						  IM_COL32(255, 255, 255, 255), "@");
	}

	// draw npcs
	const auto &npcs = state.map.getNpcs();
	for (size_t i = 0; i < npcs.size(); ++i) {
		const auto &npc = npcs[i];
		if (!npc || !state.map.inBounds(npc->getX(), npc->getY())) {
			continue;
		}
		ImVec2 pMin(canvasOrigin.x + npc->getX() * cellSize,
					canvasOrigin.y + npc->getY() * cellSize);
		ImVec2 pCenter(pMin.x + cellSize * 0.5f, pMin.y + cellSize * 0.5f);

		bool isSelected = (state.inspectorMode == InspectorMode::Entity &&
						   state.selectionType == SelectionType::NPC &&
						   state.selectedIndex == static_cast<int>(i));
		drawList->AddCircleFilled(pCenter, cellSize * 0.38f,
								  isSelected ? IM_COL32(60, 220, 255, 255)
											 : IM_COL32(50, 160, 220, 220));
		drawList->AddText(ImVec2(pMin.x + 9.0f, pMin.y + 6.0f),
						  IM_COL32(255, 255, 255, 255), "@");
	}

	// draw enemies
	const auto &enemies = state.map.getEnemies();
	for (size_t i = 0; i < enemies.size(); ++i) {
		const auto &enemy = enemies[i];
		if (!enemy || !state.map.inBounds(enemy->getX(), enemy->getY())) {
			continue;
		}
		ImVec2 pMin(canvasOrigin.x + enemy->getX() * cellSize,
					canvasOrigin.y + enemy->getY() * cellSize);
		ImVec2 pCenter(pMin.x + cellSize * 0.5f, pMin.y + cellSize * 0.5f);

		bool isSelected = (state.inspectorMode == InspectorMode::Entity &&
						   state.selectionType == SelectionType::Enemy &&
						   state.selectedIndex == static_cast<int>(i));
		drawList->AddCircleFilled(pCenter, cellSize * 0.38f,
								  isSelected ? IM_COL32(255, 80, 80, 255)
											 : IM_COL32(210, 50, 50, 220));
		drawList->AddText(ImVec2(pMin.x + 9.0f, pMin.y + 6.0f),
						  IM_COL32(255, 255, 255, 255), "E");
	}

	// input handling on canvas
	ImGui::InvisibleButton("canvas", canvasSize);
	if (ImGui::IsItemHovered()) {
		ImVec2 mousePos = ImGui::GetMousePos();
		int tx = static_cast<int>((mousePos.x - canvasOrigin.x) / cellSize);
		int ty = static_cast<int>((mousePos.y - canvasOrigin.y) / cellSize);

		if (state.map.inBounds(tx, ty)) {
			ImGui::SetTooltip("Coord: (%d, %d)", tx, ty);

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
				bool clicked = false;
				if (player != nullptr && player->getX() == tx &&
					player->getY() == ty) {
					state.inspectorMode = InspectorMode::Entity;
					state.selectionType = SelectionType::Player;
					state.selectedIndex = 0;
					clicked = true;
				}
				if (!clicked) {
					for (size_t i = 0; i < npcs.size(); ++i) {
						if (npcs[i] && npcs[i]->getX() == tx &&
							npcs[i]->getY() == ty) {
							state.inspectorMode = InspectorMode::Entity;
							state.selectionType = SelectionType::NPC;
							state.selectedIndex = static_cast<int>(i);
							clicked = true;
							break;
						}
					}
				}
				if (!clicked) {
					for (size_t i = 0; i < enemies.size(); ++i) {
						if (enemies[i] && enemies[i]->getX() == tx &&
							enemies[i]->getY() == ty) {
							state.inspectorMode = InspectorMode::Entity;
							state.selectionType = SelectionType::Enemy;
							state.selectedIndex = static_cast<int>(i);
							clicked = true;
							break;
						}
					}
				}
				if (!clicked) {
					state.inspectorMode = InspectorMode::None;
					state.selectionType = SelectionType::None;
					state.selectedIndex = -1;
				}
			}
		}
	}

	ImGui::End();
}

// bottom console panel displaying formatted log events with severity toggles
void renderOutput(EditorState &state, ImGuiWindowFlags panelFlags)
{
	ImGui::Begin("Output", nullptr, panelFlags);

	if (ImGui::Button("Clear")) {
		state.logger.clear();
	}
	ImGui::SameLine();
	ImGui::Checkbox("Info", &state.logger.showInfo);
	ImGui::SameLine();
	ImGui::Checkbox("Warnings", &state.logger.showWarning);
	ImGui::SameLine();
	ImGui::Checkbox("Errors", &state.logger.showError);
	ImGui::SameLine();
	ImGui::Checkbox("Auto-scroll", &state.logger.autoScroll);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(180);
	ImGui::InputTextWithHint("##logSearch", "Filter logs...",
							 state.logger.searchFilter,
							 sizeof(state.logger.searchFilter));

	ImGui::Separator();

	ImGui::BeginChild("LogScrollRegion", ImVec2(0, 0), false,
					  ImGuiWindowFlags_HorizontalScrollbar);

	std::string search = state.logger.searchFilter;
	std::transform(search.begin(), search.end(), search.begin(), ::tolower);

	for (const auto &entry : state.logger.entries) {
		if (entry.level == LogLevel::Info && !state.logger.showInfo) {
			continue;
		}
		if (entry.level == LogLevel::Warning && !state.logger.showWarning) {
			continue;
		}
		if (entry.level == LogLevel::Error && !state.logger.showError) {
			continue;
		}

		if (!search.empty()) {
			std::string msgLower = entry.message;
			std::transform(msgLower.begin(), msgLower.end(), msgLower.begin(),
						   ::tolower);
			if (msgLower.find(search) == std::string::npos) {
				continue;
			}
		}

		ImVec4 tagColor(0.40f, 0.80f, 0.95f, 1.0f);
		const char *tag = "[INFO]";
		if (entry.level == LogLevel::Warning) {
			tagColor = ImVec4(0.95f, 0.85f, 0.35f, 1.0f);
			tag = "[WARN]";
		} else if (entry.level == LogLevel::Error) {
			tagColor = ImVec4(0.95f, 0.40f, 0.40f, 1.0f);
			tag = "[ERROR]";
		}

		ImGui::TextDisabled("[%s]", entry.timestamp.c_str());
		ImGui::SameLine();
		ImGui::TextColored(tagColor, "%s", tag);
		ImGui::SameLine();
		ImGui::TextUnformatted(entry.message.c_str());
	}

	if (state.logger.autoScroll &&
		ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
		ImGui::SetScrollHereY(1.0f);
	}

	ImGui::EndChild();
	ImGui::End();
}

// popup modals for map creation, file dialogues, and editor configuration
void renderModals(EditorState &state)
{
	if (state.newMapModalOpen) {
		ImGui::OpenPopup("New Map File");
		state.newMapModalOpen = false;
	}
	if (ImGui::BeginPopupModal("New Map File", nullptr,
							   ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Enter map file name and dimensions:");
		ImGui::InputText("Filename", state.newMapPathBuffer,
						 sizeof(state.newMapPathBuffer));
		ImGui::SliderInt("Width", &state.newMapWidth, 4, 128);
		ImGui::SliderInt("Height", &state.newMapHeight, 4, 128);

		if (ImGui::Button("Create", ImVec2(100, 0))) {
			state.map = evreghen::Map(state.newMapWidth, state.newMapHeight);
			std::filesystem::path full =
					state.currentBrowserPath / state.newMapPathBuffer;
			if (evreghen::MapLoader::saveToFile(full.string(), state.map)) {
				state.currentFilePath = full.string();
				state.logger.add(LogLevel::Info,
								 "Created and saved new map: " + full.string());
				ImGui::CloseCurrentPopup();
			} else {
				state.logger.add(LogLevel::Error, "Failed to create map file!");
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(100, 0))) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	if (state.openMapModalOpen) {
		ImGui::OpenPopup("Open Map File");
		state.openMapModalOpen = false;
	}
	if (ImGui::BeginPopupModal("Open Map File", nullptr,
							   ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Enter path to map JSON file:");
		ImGui::InputText("##mapPath", state.filePathBuffer,
						 sizeof(state.filePathBuffer));
		if (ImGui::Button("Load", ImVec2(100, 0))) {
			if (loadMapScene(state, state.filePathBuffer)) {
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(100, 0))) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	if (state.saveMapModalOpen) {
		ImGui::OpenPopup("Save Map File");
		state.saveMapModalOpen = false;
	}
	if (ImGui::BeginPopupModal("Save Map File", nullptr,
							   ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Enter destination map JSON file path:");
		ImGui::InputText("##saveMapPath", state.filePathBuffer,
						 sizeof(state.filePathBuffer));
		if (ImGui::Button("Save", ImVec2(100, 0))) {
			if (evreghen::MapLoader::saveToFile(state.filePathBuffer,
												state.map)) {
				state.currentFilePath = state.filePathBuffer;
				state.logger.add(LogLevel::Info,
								 "Saved map to: " +
										 std::string(state.filePathBuffer));
				ImGui::CloseCurrentPopup();
			} else {
				state.logger.add(LogLevel::Error, "Failed to save map!");
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(100, 0))) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	if (state.resModalOpen) {
		ImGui::OpenPopup("Resource File");
		state.resModalOpen = false;
	}
	if (ImGui::BeginPopupModal("Resource File", nullptr,
							   ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text(state.isExportingResource
							? "Enter path to save resource JSON:"
							: "Enter path to load resource JSON:");
		ImGui::InputText("##resPath", state.resourcePathBuffer,
						 sizeof(state.resourcePathBuffer));

		if (ImGui::Button(state.isExportingResource ? "Export" : "Import",
						  ImVec2(100, 0))) {
			if (state.isExportingResource) {
				if (state.selectionType == SelectionType::NPC &&
					state.selectedIndex >= 0) {
					auto &npc = state.map.getNpcs()[state.selectedIndex];
					evreghen::NPCResource res(npc->getName(),
											  npc->getDialogues());
					if (res.saveToFile(state.resourcePathBuffer)) {
						state.logger.add(
								LogLevel::Info,
								"Exported NPC resource to " +
										std::string(state.resourcePathBuffer));
						ImGui::CloseCurrentPopup();
					}
				} else if (state.selectionType == SelectionType::Enemy &&
						   state.selectedIndex >= 0) {
					auto &enemy = state.map.getEnemies()[state.selectedIndex];
					evreghen::EnemyResource res(enemy->getName(),
												enemy->getHealth(),
												enemy->getAttackPower());
					if (res.saveToFile(state.resourcePathBuffer)) {
						state.logger.add(
								LogLevel::Info,
								"Exported Enemy resource to " +
										std::string(state.resourcePathBuffer));
						ImGui::CloseCurrentPopup();
					}
				}
			} else {
				auto res = evreghen::Resource::loadResource(
						state.resourcePathBuffer);
				if (res) {
					if (auto *npcRes = dynamic_cast<evreghen::NPCResource *>(
								res.get())) {
						if (state.selectionType == SelectionType::NPC &&
							state.selectedIndex >= 0) {
							auto &npc =
									state.map.getNpcs()[state.selectedIndex];
							int nx = npc->getX();
							int ny = npc->getY();
							npc = std::make_unique<evreghen::NPC>(
									npcRes->getName(), nx, ny,
									npcRes->getDialogues());
							state.logger.add(
									LogLevel::Info,
									"Imported NPC resource properties");
							ImGui::CloseCurrentPopup();
						}
					} else if (auto *enemyRes =
									   dynamic_cast<evreghen::EnemyResource *>(
											   res.get())) {
						if (state.selectionType == SelectionType::Enemy &&
							state.selectedIndex >= 0) {
							auto &enemy =
									state.map.getEnemies()[state.selectedIndex];
							int ex = enemy->getX();
							int ey = enemy->getY();
							enemy = std::make_unique<evreghen::Enemy>(
									enemyRes->getName(), ex, ey,
									enemyRes->getMaxHealth(),
									enemyRes->getAttackPower());
							state.logger.add(
									LogLevel::Info,
									"Imported Enemy resource properties");
							ImGui::CloseCurrentPopup();
						}
					}
				} else {
					state.logger.add(LogLevel::Error,
									 "Failed to load resource file!");
				}
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(100, 0))) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	if (state.editorSettingsModalOpen) {
		ImGui::OpenPopup("Editor Settings");
		state.editorSettingsModalOpen = false;
	}
	if (ImGui::BeginPopupModal("Editor Settings", nullptr,
							   ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Terminal Execution Configuration");
		ImGui::Separator();

		int mode = (state.settings.terminalMode == TerminalMode::AutoDetect)
						   ? 0
						   : 1;
		if (ImGui::RadioButton("Auto-detect terminal", &mode, 0)) {
			state.settings.terminalMode = TerminalMode::AutoDetect;
		}
		if (ImGui::RadioButton("Manual terminal command", &mode, 1)) {
			state.settings.terminalMode = TerminalMode::Manual;
		}

		if (state.settings.terminalMode == TerminalMode::AutoDetect) {
			ImGui::TextDisabled("Detected terminal: %s",
								state.settings.detectedTerminal.c_str());
		} else {
			ImGui::InputText("Command", state.settings.manualTerminalCmd,
							 sizeof(state.settings.manualTerminalCmd));
			ImGui::TextDisabled("Example: 'kitty -e' or 'alacritty -e'");
		}

		ImGui::Spacing();
		ImGui::InputText("CLI Binary Path", state.settings.cliBinaryPath,
						 sizeof(state.settings.cliBinaryPath));
		ImGui::TextDisabled(
				"Leave blank to automatically discover evreghen in build/path");

		ImGui::Spacing();
		if (ImGui::Button("Done", ImVec2(100, 0))) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	if (state.aboutModalOpen) {
		ImGui::OpenPopup("About Evreghen");
		state.aboutModalOpen = false;
	}
	if (ImGui::BeginPopupModal("About Evreghen", nullptr,
							   ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::TextColored(ImVec4(0.4f, 0.85f, 1.0f, 1.0f),
						   "Evreghen Game Engine");
		ImGui::Text("Version 1.0.0");
		ImGui::Spacing();
		ImGui::TextWrapped("A Cross-Platform Game Engine and Build System for "
						   "Terminal Games.");
		ImGui::Spacing();
		ImGui::Text("Authors: terra2o (M. Taha Kircali) "
					"<terra2o@protonmail.com>, Emir Baha "
					"Yildirim <jayshozie@gmail.com>");
		ImGui::Text("License: GNU General Public License v3 (GPL-3.0)");
		ImGui::Text("Repository: https://github.com/terra2o/evreghen");
		ImGui::Spacing();
		if (ImGui::Button("Close", ImVec2(100, 0))) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

} // namespace

int main(int argc, char **argv)
{
#if !defined(_WIN32)
	// auto-reap zombie child processes when external terminals close
	std::signal(SIGCHLD, SIG_IGN);
#endif

	if (!glfwInit()) {
		std::cerr << "failed to initialize GLFW\n";
		return 1;
	}

	const char *glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	GLFWwindow *window = glfwCreateWindow(1280, 720, "Evreghen Game Engine",
										  nullptr, nullptr);
	if (window == nullptr) {
		std::cerr << "failed to create GLFW window\n";
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	EditorState state;
	state.argv0 = (argc > 0 && argv[0]) ? argv[0] : "evreghen-gui";
	state.settings.detectedTerminal = detectTerminalCommand();

	std::filesystem::path currentDir = std::filesystem::current_path();
	state.projectRoot = detectProjectRoot(currentDir, state.project);
	state.currentBrowserPath = state.projectRoot;

	if (state.project.loaded) {
		state.logger.add(LogLevel::Info,
						 "Loaded project: " + state.project.name + " (" +
								 state.project.projectFilePath.string() + ")");
	} else {
		state.logger.add(LogLevel::Info,
						 "No .evreghen file found; project root set to: " +
								 state.projectRoot.string());
	}

	try {
		if (argc > 1) {
			state.map =
					evreghen::MapLoader::loadFromFile(argv[1], state.registry);
			state.currentFilePath = argv[1];
			state.logger.add(LogLevel::Info,
							 "Loaded map: " + std::string(argv[1]));
		} else {
			state.map = evreghen::MapLoader::loadFromJsonString(defaultMapJson,
																state.registry);
			state.logger.add(LogLevel::Info, "Loaded default scene preset");
		}
	} catch (const std::exception &ex) {
		state.logger.add(LogLevel::Error, "Error loading initial map: " +
												  std::string(ex.what()));
		state.map = evreghen::Map(16, 16);
	}

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		float menuHeight = 0.0f;
		if (ImGui::BeginMainMenuBar()) {
			menuHeight = ImGui::GetWindowSize().y;

			if (ImGui::BeginMenu("Files")) {
				if (ImGui::MenuItem("New Map (16x16)")) {
					state.map = evreghen::Map(16, 16);
					state.currentFilePath.clear();
					state.selectionType = SelectionType::None;
					state.selectedIndex = -1;
					state.inspectorMode = InspectorMode::None;
					state.logger.add(LogLevel::Info, "Created new 16x16 map");
				}
				if (ImGui::MenuItem("Open Map...")) {
					state.openMapModalOpen = true;
				}
				if (ImGui::MenuItem("Save Map", nullptr, false,
									!state.currentFilePath.empty())) {
					if (evreghen::MapLoader::saveToFile(state.currentFilePath,
														state.map)) {
						state.logger.add(LogLevel::Info,
										 "Saved to " + state.currentFilePath);
					} else {
						state.logger.add(LogLevel::Error,
										 "Failed to save map to " +
												 state.currentFilePath);
					}
				}
				if (ImGui::MenuItem("Save Map As...")) {
					state.saveMapModalOpen = true;
				}
				if (ImGui::MenuItem("Load Default Preset")) {
					state.map = evreghen::MapLoader::loadFromJsonString(
							defaultMapJson, state.registry);
					state.currentFilePath.clear();
					state.selectionType = SelectionType::None;
					state.selectedIndex = -1;
					state.inspectorMode = InspectorMode::None;
					state.logger.add(LogLevel::Info,
									 "Loaded default map preset");
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Exit")) {
					glfwSetWindowShouldClose(window, GLFW_TRUE);
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Project")) {
				if (ImGui::MenuItem("Run current scene (terminal)")) {
					std::filesystem::path runPath;
					if (!state.currentFilePath.empty()) {
						evreghen::MapLoader::saveToFile(state.currentFilePath,
														state.map);
						runPath = state.currentFilePath;
					} else {
						runPath = state.projectRoot / ".temp_scene.json";
						evreghen::MapLoader::saveToFile(runPath.string(),
														state.map);
						state.logger.add(
								LogLevel::Warning,
								"Saved in-memory scene to temporary file: " +
										runPath.filename().string());
					}

					std::string errorMsg;
					std::filesystem::path cliBin = findCliBinary(
							state.argv0.c_str(), state.settings.cliBinaryPath);
					bool success = launchTerminalCommand(
							state.settings, cliBin, {"run", runPath.string()},
							errorMsg);
					if (success) {
						state.logger.add(
								LogLevel::Info,
								"Launched current scene in terminal: " +
										runPath.string());
					} else {
						state.logger.add(LogLevel::Error,
										 "Failed to launch scene: " + errorMsg);
					}
				}

				if (ImGui::MenuItem(
							"Run game (from the beginning/root scene)")) {
					std::vector<std::string> args = {"run"};
					if (state.project.loaded &&
						!state.project.mainScene.empty()) {
						std::filesystem::path mainScenePath =
								state.projectRoot / state.project.mainScene;
						if (std::filesystem::exists(mainScenePath)) {
							args.push_back(mainScenePath.string());
							state.logger.add(
									LogLevel::Info,
									"Launching configured main scene: " +
											state.project.mainScene);
						} else {
							state.logger.add(
									LogLevel::Warning,
									"Main scene '" + state.project.mainScene +
											"' not found on disk; starting "
											"default campaign");
						}
					} else {
						state.logger.add(
								LogLevel::Info,
								"Launching default game campaign from root");
					}

					std::string errorMsg;
					std::filesystem::path cliBin = findCliBinary(
							state.argv0.c_str(), state.settings.cliBinaryPath);
					bool success = launchTerminalCommand(state.settings, cliBin,
														 args, errorMsg);
					if (success) {
						state.logger.add(LogLevel::Info,
										 "Launched game in terminal");
					} else {
						state.logger.add(LogLevel::Error,
										 "Failed to launch game: " + errorMsg);
					}
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Editor")) {
				if (ImGui::MenuItem("Settings...")) {
					state.editorSettingsModalOpen = true;
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help")) {
				if (ImGui::MenuItem("About")) {
					state.aboutModalOpen = true;
				}
				if (ImGui::MenuItem("Documentation")) {
					const char *docUrl = "https://github.com/terra2o/evreghen/"
										 "tree/main/docs";
#if defined(_WIN32)
					ShellExecuteA(nullptr, "open", docUrl, nullptr, nullptr,
								  SW_SHOWNORMAL);
#else
					std::string cmd = "xdg-open '" + std::string(docUrl) +
									  "' >/dev/null 2>&1 &";
					int res = std::system(cmd.c_str());
					(void)res;
#endif
					state.logger.add(LogLevel::Info,
									 "Opened documentation in browser: " +
											 std::string(docUrl));
				}
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		// tile layout sizing calculations
		float displayWidth = io.DisplaySize.x;
		float displayHeight = io.DisplaySize.y - menuHeight;
		float startY = menuHeight;

		float leftWidth = std::max(220.0f, displayWidth * 0.22f);
		float rightWidth = std::max(260.0f, displayWidth * 0.26f);
		float centerWidth = displayWidth - leftWidth - rightWidth;

		float outputHeight = std::max(140.0f, displayHeight * 0.25f);
		float upperHeight = displayHeight - outputHeight;

		float nodeTreeHeight = upperHeight * 0.5f;
		float fileSystemHeight = upperHeight - nodeTreeHeight;

		ImGuiWindowFlags panelFlags = ImGuiWindowFlags_NoCollapse |
									  ImGuiWindowFlags_NoMove |
									  ImGuiWindowFlags_NoResize |
									  ImGuiWindowFlags_NoBringToFrontOnFocus;

		// 1. Node Tree (top-left)
		ImGui::SetNextWindowPos(ImVec2(0.0f, startY), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(leftWidth, nodeTreeHeight),
								 ImGuiCond_Always);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		renderNodeTree(state, panelFlags);

		// 2. File System (bottom-left)
		ImGui::SetNextWindowPos(ImVec2(0.0f, startY + nodeTreeHeight),
								ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(leftWidth, fileSystemHeight),
								 ImGuiCond_Always);
		renderFileSystem(state, panelFlags);

		// 3. Map Viewport (center)
		ImGui::SetNextWindowPos(ImVec2(leftWidth, startY), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(centerWidth, upperHeight),
								 ImGuiCond_Always);
		renderMapViewport(state, panelFlags);

		// 4. Inspector (right)
		ImGui::SetNextWindowPos(ImVec2(leftWidth + centerWidth, startY),
								ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(rightWidth, upperHeight),
								 ImGuiCond_Always);
		renderInspector(state, panelFlags);

		// 5. Output (bottom)
		ImGui::SetNextWindowPos(ImVec2(0.0f, startY + upperHeight),
								ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(displayWidth, outputHeight),
								 ImGuiCond_Always);
		renderOutput(state, panelFlags);
		ImGui::PopStyleVar();

		renderModals(state);

		ImGui::Render();
		int display_w = 0;
		int display_h = 0;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(0.12f, 0.14f, 0.18f, 1.00f);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
