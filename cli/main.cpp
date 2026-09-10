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

#include "evreghen/level_manager.hpp"
#include "evreghen/map_loader.hpp"
#include "evreghen/renderer.hpp"
#include "evreghen/resource.hpp"

#if defined(_WIN32) || defined(_WIN64)
#include <curses.h>
#else
#include <ncurses.h>
#endif

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace {

constexpr const char *sampleLevel1 = R"json({
 "compressionlevel":-1,
 "height":20,
 "infinite":false,
 "layers":[
        {
         "data":[1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 10, 10, 10, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 10, 10, 10, 10, 10, 10, 2, 10, 10, 10, 10, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 2, 2, 2, 2, 3, 3, 2, 2, 2, 10, 10, 1, 1, 1,
            1, 1, 1, 1, 10, 10, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 18, 10, 1, 1, 1,
            1, 1, 1, 10, 10, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 2, 2, 2, 9, 9, 2, 3, 2, 2, 10, 1, 1, 1,
            1, 1, 1, 10, 10, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 9, 9, 2, 2, 3, 2, 2, 10, 1, 1, 1,
            1, 1, 1, 1, 10, 19, 2, 2, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 9, 9, 2, 2, 3, 3, 2, 10, 10, 1, 1, 1,
            1, 1, 1, 1, 10, 10, 2, 2, 3, 2, 2, 9, 9, 9, 9, 9, 9, 9, 9, 2, 3, 3, 3, 2, 2, 10, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 10, 10, 2, 3, 2, 9, 9, 2, 2, 2, 2, 2, 2, 2, 3, 3, 2, 2, 2, 2, 10, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 10, 2, 3, 3, 9, 2, 2, 2, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 10, 1, 1, 1, 1,
            1, 1, 1, 10, 10, 10, 10, 2, 2, 3, 2, 2, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 10, 10, 1, 1, 1, 1,
            1, 1, 1, 10, 2, 2, 2, 2, 17, 3, 3, 3, 3, 2, 2, 2, 10, 10, 10, 10, 10, 10, 10, 10, 10, 1, 1, 1, 1, 1,
            1, 1, 1, 10, 2, 2, 2, 2, 2, 2, 2, 2, 2, 10, 10, 10, 10, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
         "height":20,
         "id":1,
         "name":"Tile Layer 1",
         "opacity":1,
         "type":"tilelayer",
         "visible":true,
         "width":30,
         "x":0,
         "y":0
        }],
 "nextlayerid":2,
 "nextobjectid":1,
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
 "width":30
})json";

constexpr const char *sampleLevel2 = R"json({
 "compressionlevel":-1,
 "height":12,
 "infinite":false,
 "layers":[
        {
         "data":[
            10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
            10,  5,  2,  2,  2, 10,  2,  2,  2,  2,  2,  2,  2,  2, 10,  2,  2,  2,  4, 10,
            10, 17,  2,  2,  2, 10,  2, 10, 10, 10, 10, 10, 10,  2, 10,  2, 10, 10, 10, 10,
            10,  2,  2, 19,  2, 10,  2, 10,  9,  9,  9,  9, 10,  2, 10,  2,  2,  2,  2, 10,
            10,  2,  2,  2,  2,  2,  2, 10,  9,  9,  9,  9, 10,  2,  2,  2,  2, 18,  2, 10,
            10, 10, 10,  2, 10, 10, 10, 10,  9,  9,  9,  9, 10, 10, 10, 10,  2, 10, 10, 10,
            10,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 10,
            10,  2, 10, 10, 10, 10,  2, 10, 10, 10, 10, 10,  2, 10, 10, 10, 10, 10,  2, 10,
            10,  2,  2,  2,  2, 10,  2,  2,  2,  2,  2, 10,  2,  2,  2,  2,  2, 10,  2, 10,
            10, 10, 10, 10,  2, 10, 10, 10, 10, 10,  2, 10, 10, 10, 10, 10,  2, 10,  2, 10,
            10,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 10,
            10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10
         ],
         "height":12,
         "id":1,
         "name":"Tile Layer 1",
         "opacity":1,
         "type":"tilelayer",
         "visible":true,
         "width":20,
         "x":0,
         "y":0
        }],
 "nextlayerid":2,
 "nextobjectid":1,
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
 "width":20
})json";

// main terminal gameplay loop driving rendering, input, and combat
int runGame(const std::vector<std::string> &mapFiles)
{
	evreghen::LevelManager levelManager;

	try {
		if (!mapFiles.empty()) {
			for (const auto &file : mapFiles) {
				levelManager.addLevelFromFile(file);
			}
		} else {
			levelManager.addLevelFromJson(sampleLevel1);
			levelManager.addLevelFromJson(sampleLevel2);
		}

		if (!levelManager.loadLevel(0)) {
			std::cerr << "failed to load initial level\n";
			return 1;
		}
	} catch (const std::exception &ex) {
		std::cerr << "error initializing level: " << ex.what() << '\n';
		return 1;
	}

	evreghen::CursesRenderer renderer;
	std::string statusMsg =
			"Level 1: Explore with WASD/Arrows. Find the stairs (>)!";

	bool running = true;
	while (running) {
		evreghen::Map &currentMap = levelManager.getCurrentMap();
		const evreghen::TileRegistry &registry = levelManager.getRegistry();

		renderer.render(currentMap, registry, statusMsg);

		int ch = renderer.getInput();
		if (ch == 'q' || ch == 'Q') {
			running = false;
			break;
		}

		evreghen::Player *player = currentMap.getPlayer();
		if (player == nullptr || !player->isAlive()) {
			statusMsg = "Player is down. Press Q to exit.";
			continue;
		}

		int dx = 0;
		int dy = 0;
		bool moveRequested = false;

		switch (ch) {
		case 'w':
		case 'W':
		case 'k':
		case 'K':
		case KEY_UP:
			dy = -1;
			moveRequested = true;
			break;
		case 's':
		case 'S':
		case 'j':
		case 'J':
		case KEY_DOWN:
			dy = 1;
			moveRequested = true;
			break;
		case 'a':
		case 'A':
		case 'h':
		case 'H':
		case KEY_LEFT:
			dx = -1;
			moveRequested = true;
			break;
		case 'd':
		case 'D':
		case 'l':
		case 'L':
		case KEY_RIGHT:
			dx = 1;
			moveRequested = true;
			break;
		case ' ':
		case 'e':
		case 'E': {
			evreghen::Entity *target = currentMap.findAdjacentInteractable(
					player->getX(), player->getY());
			if (target != nullptr) {
				statusMsg = target->interact(*player);
			} else {
				statusMsg = "Nothing nearby to interact with.";
			}
			break;
		}
		default:
			break;
		}

		if (moveRequested) {
			int targetX = player->getX() + dx;
			int targetY = player->getY() + dy;

			evreghen::Entity *obstacleEntity =
					currentMap.getEntityAt(targetX, targetY);
			if (obstacleEntity != nullptr && obstacleEntity != player) {
				statusMsg = obstacleEntity->interact(*player);
			} else if (currentMap.isWalkable(targetX, targetY, registry)) {
				player->setPosition(targetX, targetY);

				if (currentMap.isTransitionAt(targetX, targetY, registry)) {
					if (levelManager.hasNextLevel()) {
						levelManager.nextLevel();
						statusMsg =
								"Descended to Level " +
								std::to_string(
										levelManager.getCurrentLevelIndex() +
										1) +
								"! HP preserved.";
						continue;
					} else {
						statusMsg =
								"You reached the end of the dungeon! You win!";
					}
				}

				bool attacked = false;
				for (auto &enemy : currentMap.getEnemies()) {
					if (enemy && enemy->isAlive() &&
						enemy->isNear(*player, 1)) {
						statusMsg = enemy->attack(*player);
						attacked = true;
						break;
					}
				}
				if (!attacked &&
					!currentMap.isTransitionAt(targetX, targetY, registry)) {
					statusMsg.clear();
				}
			} else {
				statusMsg = "The path is blocked.";
			}
		}
	}

	return 0;
}

// cli handler for generating and inspecting standalone entity resources
int handleResource(int argc, char **argv)
{
	if (argc < 3) {
		std::cerr << "usage: evreghen resource <new|inspect> [options]\n";
		return 1;
	}

	std::string sub = argv[2];
	if (sub == "new") {
		std::string type = "npc";
		std::string name = "Unnamed";
		std::string outFile;
		std::vector<std::string> dialogues;
		int hp = 30;
		int atk = 10;
		char glyph = '@';

		for (int i = 3; i < argc; ++i) {
			std::string arg = argv[i];
			if (arg == "--type" && i + 1 < argc) {
				type = argv[++i];
			} else if (arg == "--name" && i + 1 < argc) {
				name = argv[++i];
			} else if (arg == "--out" && i + 1 < argc) {
				outFile = argv[++i];
			} else if (arg == "--dialogue" && i + 1 < argc) {
				dialogues.push_back(argv[++i]);
			} else if (arg == "--hp" && i + 1 < argc) {
				hp = std::stoi(argv[++i]);
			} else if (arg == "--attack" && i + 1 < argc) {
				atk = std::stoi(argv[++i]);
			} else if (arg == "--glyph" && i + 1 < argc) {
				std::string gStr = argv[++i];
				if (!gStr.empty()) {
					glyph = gStr[0];
				}
			}
		}

		if (outFile.empty()) {
			std::cerr << "error: --out <path> is required\n";
			return 1;
		}

		std::unique_ptr<evreghen::Resource> res;
		if (type == "npc") {
			res = std::make_unique<evreghen::NPCResource>(name, dialogues,
														  glyph);
		} else if (type == "enemy") {
			if (glyph == '@') {
				glyph = 'E';
			}
			res = std::make_unique<evreghen::EnemyResource>(name, hp, atk,
															glyph);
		} else {
			std::cerr << "unknown resource type: " << type << "\n";
			return 1;
		}

		if (!res->saveToFile(outFile)) {
			std::cerr << "failed to write resource to: " << outFile << "\n";
			return 1;
		}
		std::cout << "created " << type << " resource at: " << outFile << "\n";
		return 0;
	}

	if (sub == "inspect") {
		if (argc < 4) {
			std::cerr << "usage: evreghen resource inspect <file>\n";
			return 1;
		}
		std::string filePath = argv[3];
		auto res = evreghen::Resource::loadResource(filePath);
		if (!res) {
			std::cerr << "failed to load resource from: " << filePath << "\n";
			return 1;
		}
		std::cout << "resource type: " << res->getResourceType() << "\n";
		std::cout << "name: " << res->getName() << "\n";
		if (auto *npc = dynamic_cast<evreghen::NPCResource *>(res.get())) {
			std::cout << "glyph: " << npc->getGlyph() << "\n";
			std::cout << "dialogues (" << npc->getDialogues().size() << "):\n";
			for (size_t i = 0; i < npc->getDialogues().size(); ++i) {
				std::cout << "  [" << i + 1 << "] " << npc->getDialogues()[i]
						  << "\n";
			}
		} else if (auto *enemy =
						   dynamic_cast<evreghen::EnemyResource *>(res.get())) {
			std::cout << "glyph: " << enemy->getGlyph() << "\n";
			std::cout << "max hp: " << enemy->getMaxHealth() << "\n";
			std::cout << "attack: " << enemy->getAttackPower() << "\n";
		}
		return 0;
	}

	std::cerr << "unknown resource command: " << sub << "\n";
	return 1;
}

// cli handler for inspecting and appending entities to map json files
int handleMap(int argc, char **argv)
{
	if (argc < 3) {
		std::cerr << "usage: evreghen map <inspect|add-entity> [options]\n";
		return 1;
	}

	std::string sub = argv[2];
	evreghen::TileRegistry registry;

	if (sub == "inspect") {
		if (argc < 4) {
			std::cerr << "usage: evreghen map inspect <map_file>\n";
			return 1;
		}
		std::string filePath = argv[3];
		try {
			evreghen::Map map =
					evreghen::MapLoader::loadFromFile(filePath, registry);
			std::cout << "map file: " << filePath << "\n";
			std::cout << "dimensions: " << map.getWidth() << "x"
					  << map.getHeight() << "\n";
			if (map.getPlayer() != nullptr) {
				std::cout << "player: " << map.getPlayer()->getName() << " at ("
						  << map.getPlayer()->getX() << ", "
						  << map.getPlayer()->getY() << ")\n";
			}
			std::cout << "npcs (" << map.getNpcs().size() << "):\n";
			for (const auto &npc : map.getNpcs()) {
				std::cout << "  - " << npc->getName() << " at (" << npc->getX()
						  << ", " << npc->getY() << ") with "
						  << npc->getDialogues().size() << " dialogues\n";
			}
			std::cout << "enemies (" << map.getEnemies().size() << "):\n";
			for (const auto &enemy : map.getEnemies()) {
				std::cout << "  - " << enemy->getName() << " at ("
						  << enemy->getX() << ", " << enemy->getY()
						  << ") [HP: " << enemy->getHealth()
						  << ", ATK: " << enemy->getAttackPower() << "]\n";
			}
			return 0;
		} catch (const std::exception &ex) {
			std::cerr << "error inspecting map: " << ex.what() << "\n";
			return 1;
		}
	}

	if (sub == "add-entity") {
		if (argc < 4) {
			std::cerr
					<< "usage: evreghen map add-entity <map_file> --type "
					   "<npc|enemy> --name <name> --x <x> --y <y> [options]\n";
			return 1;
		}
		std::string filePath = argv[3];
		std::string type = "npc";
		std::string name = "Entity";
		int x = 0;
		int y = 0;
		std::vector<std::string> dialogues;
		int hp = 30;
		int atk = 10;
		std::string resPath;

		for (int i = 4; i < argc; ++i) {
			std::string arg = argv[i];
			if (arg == "--type" && i + 1 < argc) {
				type = argv[++i];
			} else if (arg == "--name" && i + 1 < argc) {
				name = argv[++i];
			} else if (arg == "--x" && i + 1 < argc) {
				x = std::stoi(argv[++i]);
			} else if (arg == "--y" && i + 1 < argc) {
				y = std::stoi(argv[++i]);
			} else if (arg == "--dialogue" && i + 1 < argc) {
				dialogues.push_back(argv[++i]);
			} else if (arg == "--hp" && i + 1 < argc) {
				hp = std::stoi(argv[++i]);
			} else if (arg == "--attack" && i + 1 < argc) {
				atk = std::stoi(argv[++i]);
			} else if (arg == "--resource" && i + 1 < argc) {
				resPath = argv[++i];
			}
		}

		try {
			evreghen::Map map =
					evreghen::MapLoader::loadFromFile(filePath, registry);

			if (!resPath.empty()) {
				auto res = evreghen::Resource::loadResource(resPath);
				if (auto *npcRes =
							dynamic_cast<evreghen::NPCResource *>(res.get())) {
					if (name == "Entity") {
						name = npcRes->getName();
					}
					if (dialogues.empty()) {
						dialogues = npcRes->getDialogues();
					}
				} else if (auto *enemyRes =
								   dynamic_cast<evreghen::EnemyResource *>(
										   res.get())) {
					if (name == "Entity") {
						name = enemyRes->getName();
					}
					hp = enemyRes->getMaxHealth();
					atk = enemyRes->getAttackPower();
				}
			}

			if (type == "npc") {
				if (dialogues.empty()) {
					dialogues.push_back("...");
				}
				map.addNpc(std::make_unique<evreghen::NPC>(
						name, x, y, std::move(dialogues)));
			} else if (type == "enemy") {
				map.addEnemy(
						std::make_unique<evreghen::Enemy>(name, x, y, hp, atk));
			} else {
				std::cerr << "unknown entity type: " << type << "\n";
				return 1;
			}

			if (!evreghen::MapLoader::saveToFile(filePath, map)) {
				std::cerr << "failed to save updated map to: " << filePath
						  << "\n";
				return 1;
			}
			std::cout << "added " << type << " '" << name << "' to " << filePath
					  << "\n";
			return 0;
		} catch (const std::exception &ex) {
			std::cerr << "error updating map: " << ex.what() << "\n";
			return 1;
		}
	}

	std::cerr << "unknown map command: " << sub << "\n";
	return 1;
}

} // namespace

int main(int argc, char **argv)
{
	if (argc > 1) {
		std::string cmd = argv[1];
		if (cmd == "resource") {
			return handleResource(argc, argv);
		}
		if (cmd == "map") {
			return handleMap(argc, argv);
		}
		if (cmd == "run") {
			std::vector<std::string> mapFiles;
			for (int i = 2; i < argc; ++i) {
				mapFiles.push_back(argv[i]);
			}
			return runGame(mapFiles);
		}
		if (cmd == "-h" || cmd == "--help") {
			std::cout
					<< "evreghen - terminal game engine and build system\n\n"
					<< "commands:\n"
					<< "  run [map_files...]           launch terminal game\n"
					<< "  resource new [options]       create entity resource\n"
					<< "  resource inspect <file>      inspect entity "
					   "resource\n"
					<< "  map inspect <file>           inspect map file\n"
					<< "  map add-entity <file> [opts] add entity to map\n";
			return 0;
		}
		// if argument is a map file path directly
		std::vector<std::string> mapFiles;
		for (int i = 1; i < argc; ++i) {
			mapFiles.push_back(argv[i]);
		}
		return runGame(mapFiles);
	}

	return runGame({});
}
