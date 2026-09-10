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

#include "evreghen/map_loader.hpp"
#include "evreghen/resource.hpp"

#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace evreghen {

Map MapLoader::loadFromJsonString(const std::string &jsonStr,
                                  const TileRegistry &registry,
                                  std::unique_ptr<Player> existingPlayer,
                                  uint32_t defaultGroundTile)
{
	nlohmann::json root = nlohmann::json::parse(jsonStr);

	int width = root.at("width").get<int>();
	int height = root.at("height").get<int>();
	Map map(width, height);

	if (!root.contains("layers") || !root["layers"].is_array()) {
		throw std::runtime_error("missing or invalid layers array in map json");
	}

	for (const auto &layer : root["layers"]) {
		std::string type = layer.value("type", "");

		if (type == "tilelayer" && layer.contains("data")) {
			const auto &data = layer["data"];
			int layerWidth = layer.value("width", width);

			for (size_t i = 0; i < data.size(); ++i) {
				int x = static_cast<int>(i % layerWidth);
				int y = static_cast<int>(i / layerWidth);
				if (!map.inBounds(x, y)) {
					continue;
				}

				uint32_t tileId = data[i].get<uint32_t>();
				if (tileId == 0) {
					continue;
				}

				const TileDefinition *def = registry.getTile(tileId);
				if (def != nullptr && def->isEntity) {
					if (tileId == 17 || def->name == "player") {
						if (existingPlayer != nullptr) {
							existingPlayer->setPosition(x, y);
							map.setPlayer(std::move(existingPlayer));
						} else {
							map.setPlayer(std::make_unique<Player>(x, y));
						}
					} else if (tileId == 18 || def->name == "enemy") {
						map.addEnemy(std::make_unique<Enemy>("Goblin", x, y, 30,
						                                     10));
					} else if (tileId == 19 || def->name == "npc") {
						map.addNpc(std::make_unique<NPC>(
								"NPC", x, y,
								std::vector<std::string>{"Hello, traveler!"}));
					}
					// entities on tilelayer are replaced with ground
					map.setTile(x, y, defaultGroundTile);
				} else {
					map.setTile(x, y, tileId);
				}
			}
		} else if (type == "objectgroup" && layer.contains("objects")) {
			for (const auto &obj : layer["objects"]) {
				std::string objType = obj.value("type", "");
				std::string name = obj.value("name", "Entity");
				int ox = obj.value("x", 0) / templateTileWidth;
				int oy = obj.value("y", 0) / templateTileHeight;

				std::string resourcePath;
				std::vector<std::string> dialogues;
				int hp = 30;
				int atk = 10;
				bool hpSpecified = false;
				bool atkSpecified = false;

				if (obj.contains("properties") &&
				    obj["properties"].is_array()) {
					for (const auto &prop : obj["properties"]) {
						std::string propName = prop.value("name", "");
						if (propName == "resource") {
							resourcePath = prop.value("value", "");
						} else if (propName == "dialogue") {
							dialogues.push_back(prop.value("value", ""));
						} else if (propName == "hp") {
							hp = prop.value("value", 30);
							hpSpecified = true;
						} else if (propName == "attack") {
							atk = prop.value("value", 10);
							atkSpecified = true;
						}
					}
				}

				if (objType == "player") {
					if (existingPlayer != nullptr) {
						existingPlayer->setPosition(ox, oy);
						map.setPlayer(std::move(existingPlayer));
					} else {
						map.setPlayer(std::make_unique<Player>(ox, oy));
					}
				} else if (objType == "npc") {
					if (!resourcePath.empty()) {
						auto res = Resource::loadResource(resourcePath);
						auto *npcRes = dynamic_cast<NPCResource *>(res.get());
						if (npcRes != nullptr) {
							if (name == "Entity" || name == "NPC") {
								name = npcRes->getName();
							}
							if (dialogues.empty()) {
								dialogues = npcRes->getDialogues();
							}
						}
					}
					if (dialogues.empty()) {
						dialogues.push_back("Hello, traveler!");
					}
					// check if an npc was already spawned at ox, oy by the tilelayer
					bool updated = false;
					for (auto &existingNpc : map.getNpcs()) {
						if (existingNpc && existingNpc->getX() == ox &&
						    existingNpc->getY() == oy) {
							existingNpc = std::make_unique<NPC>(
									name, ox, oy, std::move(dialogues));
							updated = true;
							break;
						}
					}
					if (!updated) {
						map.addNpc(std::make_unique<NPC>(name, ox, oy,
						                                 std::move(dialogues)));
					}
				} else if (objType == "enemy") {
					if (!resourcePath.empty()) {
						auto res = Resource::loadResource(resourcePath);
						auto *enemyRes =
								dynamic_cast<EnemyResource *>(res.get());
						if (enemyRes != nullptr) {
							if (name == "Entity" || name == "Enemy") {
								name = enemyRes->getName();
							}
							if (!hpSpecified) {
								hp = enemyRes->getMaxHealth();
							}
							if (!atkSpecified) {
								atk = enemyRes->getAttackPower();
							}
						}
					}
					// check if an enemy was already spawned at ox, oy by the tilelayer
					bool updated = false;
					for (auto &existingEnemy : map.getEnemies()) {
						if (existingEnemy && existingEnemy->getX() == ox &&
						    existingEnemy->getY() == oy) {
							existingEnemy = std::make_unique<Enemy>(
									name, ox, oy, hp, atk);
							updated = true;
							break;
						}
					}
					if (!updated) {
						map.addEnemy(std::make_unique<Enemy>(name, ox, oy, hp,
						                                     atk));
					}
				}
			}
		}
	}

	return map;
}

Map MapLoader::loadFromFile(const std::filesystem::path &filePath,
                            const TileRegistry &registry,
                            std::unique_ptr<Player> existingPlayer,
                            uint32_t defaultGroundTile)
{
	std::ifstream file(filePath);
	if (!file.is_open()) {
		throw std::runtime_error("failed to open map file: " +
		                         filePath.string());
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	return loadFromJsonString(buffer.str(), registry, std::move(existingPlayer),
	                          defaultGroundTile);
}

std::string MapLoader::saveToJsonString(const Map &map)
{
	nlohmann::json root;
	int width = map.getWidth();
	int height = map.getHeight();

	root["compressionlevel"] = -1;
	root["infinite"] = false;
	root["width"] = width;
	root["height"] = height;
	root["tilewidth"] = templateTileWidth;
	root["tileheight"] = templateTileHeight;
	root["orientation"] = "orthogonal";
	root["renderorder"] = "right-down";
	root["type"] = "map";
	root["version"] = "1.10";

	std::vector<uint32_t> tileData;
	tileData.reserve(static_cast<size_t>(width * height));
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			tileData.push_back(map.getTile(x, y));
		}
	}

	nlohmann::json tileLayer;
	tileLayer["id"] = 1;
	tileLayer["name"] = "Tile Layer 1";
	tileLayer["type"] = "tilelayer";
	tileLayer["visible"] = true;
	tileLayer["opacity"] = 1;
	tileLayer["x"] = 0;
	tileLayer["y"] = 0;
	tileLayer["width"] = width;
	tileLayer["height"] = height;
	tileLayer["data"] = tileData;

	nlohmann::json objects = nlohmann::json::array();
	int objectId = 1;

	const Player *player = map.getPlayer();
	if (player != nullptr) {
		nlohmann::json pObj;
		pObj["id"] = objectId++;
		pObj["name"] = player->getName();
		pObj["type"] = "player";
		pObj["visible"] = true;
		pObj["x"] = player->getX() * templateTileWidth;
		pObj["y"] = player->getY() * templateTileHeight;
		objects.push_back(pObj);
	}

	for (const auto &npc : map.getNpcs()) {
		if (!npc) {
			continue;
		}
		nlohmann::json nObj;
		nObj["id"] = objectId++;
		nObj["name"] = npc->getName();
		nObj["type"] = "npc";
		nObj["visible"] = true;
		nObj["x"] = npc->getX() * templateTileWidth;
		nObj["y"] = npc->getY() * templateTileHeight;

		nlohmann::json props = nlohmann::json::array();
		for (const auto &line : npc->getDialogues()) {
			props.push_back({
					{"name", "dialogue"},
					{"type", "string"},
					{"value", line},
			});
		}
		nObj["properties"] = props;
		objects.push_back(nObj);
	}

	for (const auto &enemy : map.getEnemies()) {
		if (!enemy) {
			continue;
		}
		nlohmann::json eObj;
		eObj["id"] = objectId++;
		eObj["name"] = enemy->getName();
		eObj["type"] = "enemy";
		eObj["visible"] = true;
		eObj["x"] = enemy->getX() * templateTileWidth;
		eObj["y"] = enemy->getY() * templateTileHeight;

		eObj["properties"] = nlohmann::json::array({
				{{"name", "hp"}, {"type", "int"}, {"value", enemy->getHealth()}},
				{{"name", "attack"}, {"type", "int"}, {"value", enemy->getAttackPower()}},
		});
		objects.push_back(eObj);
	}

	nlohmann::json objectLayer;
	objectLayer["id"] = 2;
	objectLayer["name"] = "Objects";
	objectLayer["type"] = "objectgroup";
	objectLayer["visible"] = true;
	objectLayer["opacity"] = 1;
	objectLayer["objects"] = objects;

	root["layers"] = nlohmann::json::array({tileLayer, objectLayer});
	return root.dump(4);
}

bool MapLoader::saveToFile(const std::filesystem::path &filePath,
                           const Map &map)
{
	std::ofstream file(filePath);
	if (!file.is_open()) {
		return false;
	}
	file << saveToJsonString(map) << '\n';
	return true;
}

} // namespace evreghen
