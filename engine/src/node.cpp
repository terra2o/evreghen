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

#include "evreghen/node.hpp"

#include <algorithm>

namespace evreghen {

Node::Node(std::string name) : name_(std::move(name))
{
}

void Node::addChild(std::unique_ptr<Node> child)
{
	if (!child) {
		return;
	}
	child->parent_ = this;
	children_.push_back(std::move(child));
}

std::unique_ptr<Node> Node::removeChild(Node *child)
{
	auto it = std::find_if(
			children_.begin(), children_.end(),
			[child](const std::unique_ptr<Node> &ptr) { return ptr.get() == child; });
	if (it == children_.end()) {
		return nullptr;
	}

	std::unique_ptr<Node> found = std::move(*it);
	found->parent_ = nullptr;
	children_.erase(it);
	return found;
}

Node *Node::findChild(const std::string &name) const
{
	for (const auto &child : children_) {
		if (child->getName() == name) {
			return child.get();
		}
		Node *nested = child->findChild(name);
		if (nested != nullptr) {
			return nested;
		}
	}
	return nullptr;
}

void Node::ready()
{
	for (auto &child : children_) {
		child->ready();
	}
}

void Node::process(float dt)
{
	for (auto &child : children_) {
		child->process(dt);
	}
}

Node2D::Node2D(std::string name, int x, int y) : Node(std::move(name)), x_(x), y_(y)
{
}

EntityNode::EntityNode(std::string name, char glyph, int x, int y)
	: Node2D(std::move(name), x, y), glyph_(glyph)
{
}

NPCNode::NPCNode(std::string name, int x, int y,
                 std::vector<std::string> dialogues)
	: EntityNode(std::move(name), '@', x, y),
	  localDialogues_(std::move(dialogues))
{
}

void NPCNode::setResource(std::shared_ptr<NPCResource> res)
{
	resource_ = std::move(res);
	if (resource_) {
		name_ = resource_->getName();
		glyph_ = resource_->getGlyph();
	}
}

const std::vector<std::string> &NPCNode::getDialogues() const
{
	if (!localDialogues_.empty()) {
		return localDialogues_;
	}
	if (resource_) {
		return resource_->getDialogues();
	}
	static const std::vector<std::string> emptyList;
	return emptyList;
}

void NPCNode::setDialogues(std::vector<std::string> dialogues)
{
	localDialogues_ = std::move(dialogues);
	dialogueIdx_ = 0;
}

void NPCNode::addDialogue(std::string line)
{
	localDialogues_.push_back(std::move(line));
}

std::string NPCNode::getNextDialogue()
{
	const auto &lines = getDialogues();
	if (lines.empty()) {
		return "...";
	}
	const std::string &line = lines[dialogueIdx_ % lines.size()];
	dialogueIdx_ = (dialogueIdx_ + 1) % lines.size();
	return line;
}

EnemyNode::EnemyNode(std::string name, int x, int y, int health,
                     int attackPower)
	: EntityNode(std::move(name), 'E', x, y),
	  health_(health),
	  maxHealth_(health),
	  attackPower_(attackPower)
{
}

void EnemyNode::setResource(std::shared_ptr<EnemyResource> res)
{
	resource_ = std::move(res);
	if (resource_) {
		name_ = resource_->getName();
		glyph_ = resource_->getGlyph();
		maxHealth_ = resource_->getMaxHealth();
		health_ = maxHealth_;
		attackPower_ = resource_->getAttackPower();
	}
}

void EnemyNode::takeDamage(int amount) noexcept
{
	health_ = std::max(0, health_ - amount);
}

} // namespace evreghen
