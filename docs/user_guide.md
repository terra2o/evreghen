# Evreghen User Guide

Evreghen is a cross-platform game engine and build system designed specifically for terminal games. It provides both an interactive graphical editor ([`evreghen-gui`](file:///home/terra/Documents/cpp_projects/evreghen/gui/main.cpp)) and a terminal runtime and command-line utility ([`evreghen`](file:///home/terra/Documents/cpp_projects/evreghen/cli/main.cpp)).

---

## Table of Contents

1. [Quick Start](#quick-start)
2. [Project Configuration (`project.evreghen`)](#project-configuration-projectevreghen)
3. [Map & Scene Format](#map--scene-format)
4. [Resource Files (`.json`)](#resource-files-json)
5. [CLI Tool Reference](#cli-tool-reference)
6. [GUI Editor Manual](#gui-editor-manual)
7. [Gameplay & Controls](#gameplay--controls)
8. [Tile Registry & Default Tiles](#tile-registry--default-tiles)

---

## Quick Start

### 1. Launching the Built-in Campaign

If you launch the CLI without any arguments, it starts the built-in two-level sample dungeon:

```bash
./build/cli/evreghen
```

Alternatively, you can be explicit:

```bash
./build/cli/evreghen run
```

### 2. Launching Custom Scenes

To run one or more map scene files in sequence:

```bash
./build/cli/evreghen run path/to/level1.json path/to/level2.json
```

When you step onto stairs down (`>`), the engine transitions to the next level while preserving your current and maximum hit points.

### 3. Launching the GUI Editor

```bash
./build/gui/evreghen-gui
```

To directly open a scene file on startup:

```bash
./build/gui/evreghen-gui path/to/level.json
```

---

## Project Configuration (`project.evreghen`)

Evreghen projects are identified by a [`project.evreghen`](file:///home/terra/Documents/cpp_projects/evreghen/project.evreghen) file placed in the root directory of your game.

```json
{
    "name": "My Dungeon Crawler",
    "version": "1.0.0",
    "main_scene": "map.json"
}
```

### Fields

- `name`: string title of the game project.
- `version`: semantic version string of the game.
- `main_scene`: relative file path to the starting scene JSON file. When set, selecting **Project -> Run game (from the beginning/root scene)** in the GUI editor launches this scene.

When the GUI editor starts, it traverses parent directories from the current working directory to locate the nearest `project.evreghen` file and sets it as the active project root.

---

## Map & Scene Format

Evreghen maps use a JSON representation compatible with the [Tiled Map Editor](https://www.mapeditor.org/) format (version 1.10+).

### Scene Structure

A map contains dimensions, tile layout dimensions, and two primary layers:

```json
{
    "compressionlevel": -1,
    "width": 16,
    "height": 16,
    "tilewidth": 32,
    "tileheight": 32,
    "infinite": false,
    "orientation": "orthogonal",
    "renderorder": "right-down",
    "type": "map",
    "version": "1.10",
    "layers": [
        {
            "id": 1,
            "name": "Tile Layer 1",
            "type": "tilelayer",
            "x": 0,
            "y": 0,
            "width": 16,
            "height": 16,
            "opacity": 1,
            "visible": true,
            "data": [
                10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
                10,  2,  2,  2,  2,  2,  2, 10,  2,  2,  2,  2,  2,  2,  4, 10
            ]
        },
        {
            "id": 2,
            "name": "Objects",
            "type": "objectgroup",
            "opacity": 1,
            "visible": true,
            "objects": [
                {
                    "id": 1,
                    "name": "Hero",
                    "type": "player",
                    "visible": true,
                    "x": 32,
                    "y": 32
                },
                {
                    "id": 2,
                    "name": "Village Elder",
                    "type": "npc",
                    "visible": true,
                    "x": 96,
                    "y": 96,
                    "properties": [
                        {"name": "dialogue", "type": "string", "value": "Beware the dungeon depths."},
                        {"name": "dialogue", "type": "string", "value": "Take this advice and stay alive."}
                    ]
                },
                {
                    "id": 3,
                    "name": "Dungeon Goblin",
                    "type": "enemy",
                    "visible": true,
                    "x": 384,
                    "y": 256,
                    "properties": [
                        {"name": "hp", "type": "int", "value": 40},
                        {"name": "attack", "type": "int", "value": 12}
                    ]
                }
            ]
        }
    ]
}
```

### Coordinate System

- In `tilelayer`, coordinates are 0-indexed tile grid positions: `x = index % width`, `y = index / width`.
- In `objectgroup`, positions `x` and `y` are pixel-space coordinates. With standard 32x32 tiles, grid cell `(tx, ty)` maps to `x = tx * 32` and `y = ty * 32`.

---

## Resource Files (`.json`)

Resources allow you to define standalone templates for entities (such as NPCs and Enemies) that can be reused across multiple maps and scenes or edited via the GUI Inspector.

### NPC Resource (`npc`)

```json
{
    "type": "npc",
    "name": "Blacksmith",
    "glyph": "B",
    "dialogues": [
        "Need your armor patched?",
        "Sharp swords keep goblins away."
    ]
}
```

### Enemy Resource (`enemy`)

```json
{
    "type": "enemy",
    "name": "Orc Warrior",
    "glyph": "O",
    "maxHealth": 60,
    "attackPower": 18
}
```

---

## CLI Tool Reference

The [`evreghen`](file:///home/terra/Documents/cpp_projects/evreghen/cli/main.cpp) binary provides commands for running the game, inspecting maps, and manipulating standalone resources.

### 1. `run`

Launches the terminal curses engine runtime.

```bash
# run built-in campaign
evreghen run

# run custom map sequence
evreghen run level1.json level2.json level3.json

# direct shorthand
evreghen level1.json
```

### 2. `resource new`

Creates a new JSON resource template file.

```bash
# create an npc template
evreghen resource new \
  --type npc \
  --name "Merchant" \
  --glyph "M" \
  --dialogue "Care to view my wares?" \
  --dialogue "Gold buys peace of mind." \
  --out merchant.json

# create an enemy template
evreghen resource new \
  --type enemy \
  --name "Cave Troll" \
  --glyph "T" \
  --hp 80 \
  --attack 22 \
  --out troll.json
```

**Options:**
- `--type <npc|enemy>`: resource archetype (default: `npc`).
- `--name <name>`: display name (default: `Unnamed`).
- `--out <path>`: destination `.json` file path (**required**).
- `--glyph <char>`: visual ASCII glyph (e.g. `M`, `T`, `@`, `E`).
- `--dialogue <text>`: dialogue line (repeatable for multiple lines).
- `--hp <int>`: maximum hit points for enemy (default: `30`).
- `--attack <int>`: attack power for enemy (default: `10`).

### 3. `resource inspect`

Prints formatted attributes of an existing resource file:

```bash
evreghen resource inspect merchant.json
```

### 4. `map inspect`

Displays map dimensions, player starting coordinates, and every NPC and Enemy located on the map:

```bash
evreghen map inspect map.json
```

### 5. `map add-entity`

Appends an entity directly to an existing scene file on disk:

```bash
# add an npc manually
evreghen map add-entity map.json \
  --type npc \
  --name "Guard" \
  --x 4 \
  --y 6 \
  --dialogue "Halt! State your business."

# add an enemy using an external resource template
evreghen map add-entity map.json \
  --type enemy \
  --x 10 \
  --y 12 \
  --resource troll.json
```

---

## GUI Editor Manual

[`evreghen-gui`](file:///home/terra/Documents/cpp_projects/evreghen/gui/main.cpp) is an interactive OpenGL/Dear ImGui editor inspired by Godot's scene and node paradigm.

### Layout Overview

```
+-------------------------------------------------------------------------------+
| Main Menu Bar (Files | Project | Editor | Help)                               |
+-------------------+-----------------------------------+-----------------------+
| Node Tree         | Map Viewport                      | Inspector             |
| - Root (Map WxH)  | - Live tile grid visualization    | - Context-sensitive   |
|   - Player        | - Interactive entity dragging/pick|   properties for      |
|   - NPCs          | - Hover coordinate tooltips       |   selected nodes and  |
|   - Enemies       |                                   |   project/resource    |
+-------------------+                                   |   files               |
| File System       |                                   |                       |
| - Directory tree  |                                   |                       |
| - Quick-open      |                                   |                       |
+-------------------+-----------------------------------+-----------------------+
| Output Log Console (Filter, Level toggles, auto-scroll)                       |
+-------------------------------------------------------------------------------+
```

### Panels

1. **Node Tree** (top-left):
   - Displays the scene graph with the map root node.
   - Shows the active [`Player`](file:///home/terra/Documents/cpp_projects/evreghen/engine/include/evreghen/player.hpp#L24-L42), all [`NPC`](file:///home/terra/Documents/cpp_projects/evreghen/engine/include/evreghen/npc.hpp#L26-L40) nodes, and all [`Enemy`](file:///home/terra/Documents/cpp_projects/evreghen/engine/include/evreghen/enemy.hpp#L24-L44) nodes.
   - Click any item to inspect its properties.
   - Use **+ Add NPC** or **+ Add Enemy** to instantiate new entities in the scene.

2. **File System** (bottom-left):
   - Browses your project directory root.
   - Color-codes entries: `.evreghen` projects (gold), `.json` scenes/resources (green), other files (white).
   - Double-click a `.json` map to open it in the editor.
   - Double-click a `.evreghen` project to switch active project configurations.
   - Single-click any file to view and edit its metadata in the Inspector.
   - Use **+ New Map File** to generate a blank map with custom width and height.

3. **Map Viewport** (center):
   - Renders tiles and entity tokens with color-coded circles (`@` for Player, `@` blue for NPCs, `E` red for Enemies).
   - Left-click on any entity in the viewport to select it in the Inspector.
   - Hover over cells to see the tile coordinate tooltip `(x, y)`.

4. **Inspector** (right):
   - **Entity Mode**: Tune entity name, coordinates `(X, Y)`, NPC dialogues (add/edit/delete lines), or Enemy stats (`HP`, `Attack Power`).
   - **Resource Export/Import**: Export an entity's current configuration to an external `.json` resource, or load stats from an existing resource.
   - **File Inspector Mode**: Edit `.evreghen` project metadata (`name`, `version`, `main_scene`), or inspect/edit standalone `.json` NPC/Enemy resource files.

5. **Output Log Console** (bottom):
   - Displays timestamps and status messages from engine operations.
   - Toggle buttons for `Info`, `Warnings`, and `Errors`.
   - Real-time substring filter input.

### Menu Bar Reference

- **Files**:
  - `New Map (16x16)`: Resets viewport with a fresh 16x16 map.
  - `Open Map...`: Opens a dialogue to load a scene path.
  - `Save Map`: Writes the current scene back to its source file.
  - `Save Map As...`: Saves the current scene to a new path.
  - `Load Default Preset`: Reloads the built-in default dungeon template.
  - `Exit`: Closes the editor.
- **Project**:
  - `Run current scene (terminal)`: Saves the active scene (to a temporary file if unsaved) and spawns an external terminal running it.
  - `Run game (from the beginning/root scene)`: Launches the configured `main_scene` from `project.evreghen`.
- **Editor**:
  - `Settings...`: Configure terminal emulator selection (Auto-detect among `kitty`, `alacritty`, `gnome-terminal`, `konsole`, `xfce4-terminal`, `xterm`, or manual command like `kitty -e`), and customize the CLI binary path.
- **Help**:
  - `About`: Displays engine author and license information.
  - `Documentation`: Opens repository documentation.

---

## Gameplay & Controls

When running inside a terminal emulator:

| Key | Action | Description |
|---|---|---|
| `W` / `Up` / `K` | Move Up | Move 1 tile north |
| `S` / `Down` / `J` | Move Down | Move 1 tile south |
| `A` / `Left` / `H` | Move Left | Move 1 tile west |
| `D` / `Right` / `L` | Move Right | Move 1 tile east |
| `Space` / `E` | Interact | Talk to adjacent NPC or inspect tile |
| `Q` | Quit | Exit the game back to shell |

### Game Mechanics

- **Movement**: You cannot move through solid walls (`#`), water (`~`), or other living entities.
- **Interacting with NPCs**: Moving into an NPC or pressing `Space`/`E` while standing adjacent will trigger dialogue, cycling through their dialogue lines.
- **Combat**:
  - Bumping into an enemy initiates a turn where the enemy strikes and the player counter-attacks.
  - Walking adjacent to an enemy triggers an opportunity attack from the enemy.
  - Defeated enemies remain as non-blocking remnants on the ground.
- **Level Transitions**: Stepping onto stairs down (`>`) immediately advances the campaign to the next level in the queue. The player's current health carries over between levels.

---

## Tile Registry & Default Tiles

For editing maps, we use the [Tiled map editor](https://www.mapeditor.org/), and we use a template tileset for making the levels.

| Tile ID | Internal Name | Glyph | Solid | Entity | Transition | Description |
|---|---|---|---|---|---|---|
| `1` | `space` | `' '` | No | No | No | Empty void tile |
| `2` | `ground` | `.` | No | No | No | Standard walkable floor |
| `3` | `path` | `=` | No | No | No | Walkable pathway |
| `4` | `stairs_down`| `>` | No | No | Yes | Transition to next level |
| `5` | `stairs_up` | `<` | No | No | Yes | Upstairs marker |
| `9` | `water` | `~` | Yes | No | No | Impassable water tile |
| `10` | `wall` | `#` | Yes | No | No | Impassable solid wall |
| `17` | `player` | `@` | Yes | Yes | No | Spawn point for player |
| `18` | `enemy` | `E` | Yes | Yes | No | Spawn point for enemy |
| `19` | `npc` | `N` | Yes | Yes | No | Spawn point for NPC |
