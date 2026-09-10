# Evreghen Maintainer & Architecture Guide

This guide covers the internal architecture, design principles, extension workflows, and code quality standards for contributors and maintainers of Evreghen.

---

## Table of Contents

1. [Architectural Overview](#architectural-overview)
2. [Target Structure & Build System](#target-structure--build-system)
3. [Core Subsystems](#core-subsystems)
   - [Node & Scene Graph System](#node--scene-graph-system)
   - [Entity Hierarchy](#entity-hierarchy)
   - [Resource System](#resource-system)
   - [Map & Serialization Pipeline](#map--serialization-pipeline)
   - [Level Progression & State Preservation](#level-progression--state-preservation)
   - [Tile Registry & GID Math](#tile-registry--gid-math)
   - [Curses Terminal Renderer](#curses-terminal-renderer)
   - [GUI Editor Architecture](#gui-editor-architecture)
4. [Extending the Engine](#extending-the-engine)
   - [Adding a New Tile Type](#adding-a-new-tile-type)
   - [Adding a New Entity or Node Type](#adding-a-new-entity-or-node-type)
5. [Code Quality & Style Conventions](#code-quality--style-conventions)
6. [Commit Standards & Contribution Workflow](#commit-standards--contribution-workflow)

---

## Architectural Overview

Evreghen is organized into three distinct tiers:

```
+-----------------------------------------------------------+
|                       evreghen-gui                        |
|   (ImGui + GLFW + OpenGL 3 Desktop Map & Scene Editor)    |
+-----------------------------+-----------------------------+
                              | links
+-----------------------------v-----------------------------+
|                      evreghen CLI                         |
|   (Terminal Curses Game Runtime & Build/Asset Utility)    |
+-----------------------------+-----------------------------+
                              | links
+-----------------------------v-----------------------------+
|                     libengine.a                           |
|  - Node / Scene Graph       - Resource Serialization      |
|  - Entity Hierarchy         - Level Manager               |
|  - Map & Tiled I/O          - Curses Renderer Abstraction |
|  - Tile Registry            - nlohmann_json integration   |
+-----------------------------------------------------------+
```

1. **`engine/`**: The foundational static library ([`libengine.a`](file:///home/terra/Documents/cpp_projects/evreghen/engine/CMakeLists.txt#L41)). It contains all core domain models, math, JSON serialization, level management, and terminal curses rendering. It has no GUI dependencies.
2. **`cli/`**: The command-line binary ([`evreghen`](file:///home/terra/Documents/cpp_projects/evreghen/cli/main.cpp#L528)). It executes terminal games, loads multi-level campaigns, and manipulates maps/resources headlessly.
3. **`gui/`**: The graphical desktop editor ([`evreghen-gui`](file:///home/terra/Documents/cpp_projects/evreghen/gui/main.cpp#L1590)). Built using Dear ImGui, GLFW, and OpenGL 3.

---

## Target Structure & Build System

The build is coordinated via top-level [`CMakeLists.txt`](file:///home/terra/Documents/cpp_projects/evreghen/CMakeLists.txt).

### Build Flags

- `EVREGHEN_NO_GUI`: (default `OFF`)
  - When `OFF`, builds the core `engine` library, `cli/evreghen`, and `gui/evreghen-gui`.
  - When `ON`, skips `gui/` entirely. This mode is used on headless build runners, servers, or minimal environments lacking OpenGL/X11/Wayland headers.

### Dependencies

- **nlohmann_json** (`v3.11.3`): Managed via `FetchContent` in [`engine/CMakeLists.txt`](file:///home/terra/Documents/cpp_projects/evreghen/engine/CMakeLists.txt#L20-L26).
- **Curses**: `find_package(Curses REQUIRED)` on Linux/macOS linking against `libncurses`. On Windows, discovers `PDCurses` or `PDCursesMod`.
- **GLFW** (`3.4`) & **Dear ImGui** (`v1.91.8`): Fetched via `FetchContent` in [`gui/CMakeLists.txt`](file:///home/terra/Documents/cpp_projects/evreghen/gui/CMakeLists.txt#L21-L43) and compiled statically into `imgui_lib`.
- **OpenGL**: Discovered via `find_package(OpenGL REQUIRED)`.

---

## Core Subsystems

### Node & Scene Graph System

The node architecture is located in [`include/evreghen/node.hpp`](file:///home/terra/Documents/cpp_projects/evreghen/engine/include/evreghen/node.hpp) and [`src/node.cpp`](file:///home/terra/Documents/cpp_projects/evreghen/engine/src/node.cpp). It mirrors the node tree hierarchy popularized by Godot 4:

- [`Node`](file:///home/terra/Documents/cpp_projects/evreghen/engine/include/evreghen/node.hpp#L29): Abstract base with hierarchical ownership via `std::vector<std::unique_ptr<Node>> children_` and a raw non-owning `Node *parent_` pointer. Provides:
  - `ready()`: Cascades initialization down the tree.
  - `process(float dt)`: Propagates per-frame tick updates.
  - `addChild(std::unique_ptr<Node>)`, `removeChild(Node *)`, `findChild(const std::string &)`.
- [`Node2D`](file:///home/terra/Documents/cpp_projects/evreghen/engine/include/evreghen/node.hpp#L59): Adds discrete 2D spatial coordinates `x_` and `y_`.
- [`EntityNode`](file:///home/terra/Documents/cpp_projects/evreghen/engine/include/evreghen/node.hpp#L76): Associates a display ASCII `glyph_` with the 2D coordinate.
- [`NPCNode`](file:///home/terra/Documents/cpp_projects/evreghen/engine/include/evreghen/node.hpp#L88): Supports Godot-style resource binding via `std::shared_ptr<NPCResource>`. Can derive its dialogue list from either local instance overrides or its linked shared resource.
- [`EnemyNode`](file:///home/terra/Documents/cpp_projects/evreghen/engine/include/evreghen/node.hpp#L111): Supports binding via `std::shared_ptr<EnemyResource>`, synchronizing `health_`, `maxHealth_`, and `attackPower_`.

### Entity Hierarchy

The runtime entities live in [`include/evreghen/entity.hpp`](file:///home/terra/Documents/cpp_projects/evreghen/engine/include/evreghen/entity.hpp):

- [`Entity`](file:///home/terra/Documents/cpp_projects/evreghen/engine/include/evreghen/entity.hpp#L26): Base class with grid coordinates `(x, y)`, an ASCII glyph, and Chebyshev proximity checks via `isNear(int targetX, int targetY, int distance)`.
- [`Player`](file:///home/terra/Documents/cpp_projects/evreghen/engine/include/evreghen/player.hpp#L24): Glyph `'@'`. Tracks `health_`, `maxHealth_`, and `attackPower_`. Implements `takeDamage()` and `heal()`.
- [`NPC`](file:///home/terra/Documents/cpp_projects/evreghen/engine/include/evreghen/npc.hpp#L26): Glyph `'N'` (or custom). Stores a vector of dialogue strings and advances dialogue sequentially on each call to `interact()`.
- [`Enemy`](file:///home/terra/Documents/cpp_projects/evreghen/engine/include/evreghen/enemy.hpp#L24): Glyph `'E'`. Executes turn-based damage exchange via `attack(Player &)` and `interact(Player &)`.

### Resource System

Resources are data templates stored on disk as JSON, declared in [`include/evreghen/resource.hpp`](file:///home/terra/Documents/cpp_projects/evreghen/engine/include/evreghen/resource.hpp):

- [`Resource`](file:///home/terra/Documents/cpp_projects/evreghen/engine/include/evreghen/resource.hpp#L29): Base class with virtual `toJson()` and `fromJson()`, plus file I/O methods `saveToFile()` and `loadFromFile()`.
- [`Resource::loadResource`](file:///home/terra/Documents/cpp_projects/evreghen/engine/include/evreghen/resource.hpp#L44): Factory method inspecting the `"type"` field to instantiate either an [`NPCResource`](file:///home/terra/Documents/cpp_projects/evreghen/engine/include/evreghen/resource.hpp#L51) or an [`EnemyResource`](file:///home/terra/Documents/cpp_projects/evreghen/engine/include/evreghen/resource.hpp#L80).

### Map & Serialization Pipeline

[`MapLoader`](file:///home/terra/Documents/cpp_projects/evreghen/engine/include/evreghen/map_loader.hpp#L29) provides lossless conversion between [`Map`](file:///home/terra/Documents/cpp_projects/evreghen/engine/include/evreghen/map.hpp#L30) objects and Tiled JSON:

1. **Reading**:
   - `tilelayer`: Populates `tiles_` array in row-major order. If a tile matches a definition marked `isEntity` (such as legacy tile IDs 17, 18, 19), an entity is instantiated and the background tile is substituted with `defaultGroundTile` (tile ID 2).
   - `objectgroup`: Scans objects for `type == "player"`, `"npc"`, or `"enemy"`. Properties such as `"hp"`, `"attack"`, `"dialogue"`, and `"resource"` are parsed. If a `"resource"` attribute exists, the file is loaded to initialize default stats before explicit object overrides are applied.
2. **Writing**:
   - `tilelayer` (ID 1): Dumps raw integer IDs from `map.getTile(x, y)`.
   - `objectgroup` (ID 2): Serializes all active entities into JSON objects with pixel coordinates `(x * 32, y * 32)` and properties.

### Level Progression & State Preservation

[`LevelManager`](file:///home/terra/Documents/cpp_projects/evreghen/engine/include/evreghen/level_manager.hpp#L29) orchestrates multi-scene campaigns:

- Stores an index of level sources (`LevelSourceType::File` or `LevelSourceType::JsonString`).
- When switching levels via `loadLevel(index)` or `nextLevel()`, the manager calls `currentMap_.releasePlayer()`.
- The extracted `std::unique_ptr<Player>` is passed into `MapLoader::loadFromFile()`.
- The new map adopts this existing player instance, preserving their current hit points and stats across dungeon floors while updating position `(x, y)` to the new map's spawn coordinates.

### Tile Registry & GID Math

[`TileRegistry`](file:///home/terra/Documents/cpp_projects/evreghen/engine/include/evreghen/tile_registry.hpp#L42) decouples global tile IDs (GIDs) from hardcoded values.

Given a tileset image of dimensions $W \times H$ with cell size $t_w \times t_h$, the number of columns is:
$$\text{columns} = \frac{W}{t_w} = \frac{256}{32} = 8$$

Mapping between 0-indexed grid cell $(t_x, t_y)$ and 1-based GID:
$$\text{id} = t_y \cdot \text{columns} + t_x + \text{firstGid}$$
$$t_x = (\text{id} - \text{firstGid}) \pmod{\text{columns}}, \quad t_y = \left\lfloor\frac{\text{id} - \text{firstGid}}{\text{columns}}\right\rfloor$$

These calculations are implemented as `constexpr` functions:
- [`TileRegistry::coordsToId`](file:///home/terra/Documents/cpp_projects/evreghen/engine/include/evreghen/tile_registry.hpp#L47)
- [`TileRegistry::idToCoords`](file:///home/terra/Documents/cpp_projects/evreghen/engine/include/evreghen/tile_registry.hpp#L54)

### Curses Terminal Renderer

[`CursesRenderer`](file:///home/terra/Documents/cpp_projects/evreghen/engine/include/evreghen/renderer.hpp#L27) manages terminal mode initialization via RAII:

- Constructor invokes `initscr()`, `cbreak()`, `noecho()`, `keypad(stdscr, TRUE)`, and `curs_set(0)`.
- Destructor ensures `endwin()` is called to restore normal terminal state upon exit or exception.
- Uses `mvaddch` to draw tiles followed by entities, and renders a HUD status line below the map grid.

### GUI Editor Architecture

The GUI editor ([`gui/main.cpp`](file:///home/terra/Documents/cpp_projects/evreghen/gui/main.cpp)) is built on Dear ImGui:

- **Isolated Process Spawning**: Because curses and an OpenGL/GLFW window cannot share standard I/O in the same process without terminal corruption, running a scene from the GUI launches the external CLI in an independent terminal emulator.
  - On POSIX platforms, [`launchTerminalCommand`](file:///home/terra/Documents/cpp_projects/evreghen/gui/main.cpp#L355) uses `fork()` and `execvp()`.
  - Signal handler `std::signal(SIGCHLD, SIG_IGN)` automatically reaps child processes upon exit, preventing zombies.
  - Automatic terminal detection inspects `$PATH` for popular emulators (`kitty`, `alacritty`, `gnome-terminal`, `konsole`, `xfce4-terminal`, `xterm`).
- **Interactive Viewport Canvas**: Renders custom ImGui primitives using `ImDrawList`: tiles as filled rectangles with grid borders, and entities as colored circles with centered text glyphs.

---

## Extending the Engine

### Adding a New Tile Type

To register a new tile in the engine:

1. Open [`engine/src/tile_registry.cpp`](file:///home/terra/Documents/cpp_projects/evreghen/engine/src/tile_registry.cpp).
2. Inside [`TileRegistry::registerDefaults()`](file:///home/terra/Documents/cpp_projects/evreghen/engine/src/tile_registry.cpp#L74), add your tile using grid coordinates:

```cpp
// register a lava hazard tile at column 2, row 1
registerTile(2, 1, "lava", '^', true, false, false);
```

3. If the tile has special interaction logic (e.g., damaging the player upon entry), inspect the tile in the movement check inside [`cli/main.cpp`](file:///home/terra/Documents/cpp_projects/evreghen/cli/main.cpp#L243):

```cpp
uint32_t targetTile = currentMap.getTile(targetX, targetY);
const auto *def = registry.getTile(targetTile);
if (def != nullptr && def->name == "lava") {
    player->takeDamage(10);
    statusMsg = "You stepped on searing lava!";
}
```

### Adding a New Entity or Node Type

1. **Create Class Header and Implementation**:
   - Derive from [`Entity`](file:///home/terra/Documents/cpp_projects/evreghen/engine/include/evreghen/entity.hpp#L26) for gameplay instances, or [`EntityNode`](file:///home/terra/Documents/cpp_projects/evreghen/engine/include/evreghen/node.hpp#L76) for scene-graph nodes.
   - Implement `interact(Player &)` to handle bumps and interaction keys.
2. **Update Map Storage**:
   - Add storage in [`Map`](file:///home/terra/Documents/cpp_projects/evreghen/engine/include/evreghen/map.hpp#L30) (e.g., `std::vector<std::unique_ptr<Chest>> chests_`).
   - Extend `getEntityAt()` and `findAdjacentInteractable()` in [`engine/src/map.cpp`](file:///home/terra/Documents/cpp_projects/evreghen/engine/src/map.cpp).
3. **Serialization**:
   - In [`MapLoader::loadFromJsonString`](file:///home/terra/Documents/cpp_projects/evreghen/engine/src/map_loader.cpp#L29), parse objects where `type == "your_type"`.
   - In [`MapLoader::saveToJsonString`](file:///home/terra/Documents/cpp_projects/evreghen/engine/src/map_loader.cpp#L213), iterate over your entities and add their JSON dictionary to `objects`.
4. **GUI Editor Integration**:
   - In [`gui/main.cpp`](file:///home/terra/Documents/cpp_projects/evreghen/gui/main.cpp), extend [`renderNodeTree`](file:///home/terra/Documents/cpp_projects/evreghen/gui/main.cpp#L530) to list instances in the hierarchy.
   - Extend [`renderInspector`](file:///home/terra/Documents/cpp_projects/evreghen/gui/main.cpp#L736) to render input controls for their properties.
   - Extend [`renderMapViewport`](file:///home/terra/Documents/cpp_projects/evreghen/gui/main.cpp#L1111) to render the token on the 2D canvas.

---

## Code Quality & Style Conventions

### Formatting Rules

All C++ code must strictly adhere to the project's [`.clang-format`](file:///home/terra/Documents/cpp_projects/evreghen/.clang-format) file:

- **Indentation**: Real tabs for C++ source and headers (`tab_width = 4`, `indent_width = 4`). Spaces for CMake (`4`), JSON (`2`), and YAML (`2`).
- **Column Limit**: 80 columns maximum.
- **Braces**: Functions have their opening brace on a new line (`AfterFunction: true`). All control statements (`if`, `for`, `while`) have their opening brace on the same line.
- **Run Clang-Format**:
  ```bash
  find engine cli gui -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
  ```

### Commenting Standards

- Do not capitalize the first word of code comments. Comments should read naturally, matching classic Unix C/C++ style.
- Never write comments for obvious code (e.g. do not write `// set x coordinate` above `x = 5`).
- Only write comments for non-obvious logic, architectural rationale, or hardware/OS workarounds.

---

## Commit Standards & Contribution Workflow

### Signed Commits

Every commit must include a Signed-off-by trailer confirming agreement with the Developer Certificate of Origin (DCO):

```bash
git commit -s -m "feat(map): Add support for directional stairs"
```

### Commit Message Format

Commit messages follow the Conventional Commits specification:

```
<type>(<scope>): <subject>

<body>

Signed-off-by: Full Name <email@example.com>
```

- **Types**: `feat`, `fix`, `docs`, `refactor`, `chore`, `test`, `build`.
- **Scopes**: `engine`, `cli`, `gui`, `map`, `renderer`, `build`, `readme`.
