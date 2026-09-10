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

#pragma once

// godot-style scene graph node hierarchy for terminal entities

#include "evreghen/resource.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace evreghen {

// base scene graph element with parent-child ownership and lifecycle hooks
class Node {
public:
	explicit Node(std::string name = "Node");
	virtual ~Node() = default;

	[[nodiscard]] const std::string &getName() const noexcept { return name_; }
	void setName(std::string name) { name_ = std::move(name); }

	Node *getParent() noexcept { return parent_; }
	[[nodiscard]] const Node *getParent() const noexcept { return parent_; }

	// reparents child and takes exclusive ownership
	void addChild(std::unique_ptr<Node> child);
	// unlinks child from the tree and transfers ownership back to caller
	std::unique_ptr<Node> removeChild(Node *child);
	[[nodiscard]] const std::vector<std::unique_ptr<Node>> &
	getChildren() const noexcept
	{
		return children_;
	}

	// recursive depth-first search by node name
	[[nodiscard]] Node *findChild(const std::string &name) const;

	// post-initialization hook, cascades to children
	virtual void ready();
	// frame update hook, dt is delta time in seconds
	virtual void process(float dt);

protected:
	std::string name_;
	Node *parent_{nullptr};
	std::vector<std::unique_ptr<Node>> children_;
};

// 2d spatial node with discrete integer grid coordinates
class Node2D : public Node {
public:
	Node2D(std::string name = "Node2D", int x = 0, int y = 0);

	[[nodiscard]] int getX() const noexcept { return x_; }
	[[nodiscard]] int getY() const noexcept { return y_; }
	void setPosition(int x, int y) noexcept
	{
		x_ = x;
		y_ = y;
	}

protected:
	int x_{0};
	int y_{0};
};

// spatial node rendered as an ascii glyph on the terminal grid
class EntityNode : public Node2D {
public:
	EntityNode(std::string name = "Entity", char glyph = '?', int x = 0,
	           int y = 0);

	[[nodiscard]] char getGlyph() const noexcept { return glyph_; }
	void setGlyph(char glyph) noexcept { glyph_ = glyph; }

protected:
	char glyph_{'?'};
};

// npc node with optional shared resource backing and dialogue cycling
class NPCNode : public EntityNode {
public:
	NPCNode(std::string name = "NPC", int x = 0, int y = 0,
	        std::vector<std::string> dialogues = {});

	void setResource(std::shared_ptr<NPCResource> res);
	[[nodiscard]] std::shared_ptr<NPCResource> getResource() const noexcept
	{
		return resource_;
	}

	[[nodiscard]] const std::vector<std::string> &getDialogues() const;
	void setDialogues(std::vector<std::string> dialogues);
	void addDialogue(std::string line);

	// returns current line and advances cursor, wrapping around
	[[nodiscard]] std::string getNextDialogue();

private:
	std::shared_ptr<NPCResource> resource_;
	std::vector<std::string> localDialogues_;
	size_t dialogueIdx_{0};
};

// combatant node with stats and backing resource binding
class EnemyNode : public EntityNode {
public:
	EnemyNode(std::string name = "Enemy", int x = 0, int y = 0,
	          int health = 30, int attackPower = 10);

	void setResource(std::shared_ptr<EnemyResource> res);
	[[nodiscard]] std::shared_ptr<EnemyResource> getResource() const noexcept
	{
		return resource_;
	}

	[[nodiscard]] int getHealth() const noexcept { return health_; }
	[[nodiscard]] int getMaxHealth() const noexcept { return maxHealth_; }
	[[nodiscard]] int getAttackPower() const noexcept { return attackPower_; }

	void setHealth(int hp) noexcept { health_ = hp; }
	void setMaxHealth(int hp) noexcept { maxHealth_ = hp; }
	void setAttackPower(int atk) noexcept { attackPower_ = atk; }

	[[nodiscard]] bool isAlive() const noexcept { return health_ > 0; }
	void takeDamage(int amount) noexcept;

private:
	std::shared_ptr<EnemyResource> resource_;
	int health_{30};
	int maxHealth_{30};
	int attackPower_{10};
};

} // namespace evreghen
