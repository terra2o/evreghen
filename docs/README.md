# Evreghen Documentation

Welcome to the documentation for Evreghen, a cross-platform game engine and build system for terminal games.

---

## Directory

- **[User Guide](./user_guide.md)**
  - [Quick Start](./user_guide.md#quick-start)
  - [Project Configuration (`project.evreghen`)](./user_guide.md#project-configuration-projectevreghen)
  - [Map & Scene Format](./user_guide.md#map--scene-format)
  - [Resource Files (`.json`)](./user_guide.md#resource-files-json)
  - [CLI Tool Reference](./user_guide.md#cli-tool-reference)
  - [GUI Editor Manual](./user_guide.md#gui-editor-manual)
  - [Gameplay & Controls](./user_guide.md#gameplay--controls)
  - [Tile Registry & Default Tiles](./user_guide.md#tile-registry--default-tiles)

- **[Maintainer & Architecture Guide](./maintainer_guide.md)**
  - [Architectural Overview](./maintainer_guide.md#architectural-overview)
  - [Target Structure & Build System](./maintainer_guide.md#target-structure--build-system)
  - [Core Subsystems](./maintainer_guide.md#core-subsystems)
    - [Node & Scene Graph System](./maintainer_guide.md#node--scene-graph-system)
    - [Entity Hierarchy](./maintainer_guide.md#entity-hierarchy)
    - [Resource System](./maintainer_guide.md#resource-system)
    - [Map & Serialization Pipeline](./maintainer_guide.md#map--serialization-pipeline)
    - [Level Progression & State Preservation](./maintainer_guide.md#level-progression--state-preservation)
    - [Tile Registry & GID Math](./maintainer_guide.md#tile-registry--gid-math)
    - [Curses Terminal Renderer](./maintainer_guide.md#curses-terminal-renderer)
    - [GUI Editor Architecture](./maintainer_guide.md#gui-editor-architecture)
  - [Extending the Engine](./maintainer_guide.md#extending-the-engine)
  - [Code Quality & Style Conventions](./maintainer_guide.md#code-quality--style-conventions)
  - [Commit Standards & Contribution Workflow](./maintainer_guide.md#commit-standards--contribution-workflow)

- **[Build Instructions](../BUILD.md)**
  - Prerequisites and build steps for Linux, Windows, macOS, and *BSD
  - Headless builds with `-DEVREGHEN_NO_GUI=ON`

- **[Contributing Guidelines](../CONTRIBUTING.md)**
  - Contribution workflow, DCO sign-off requirements, and code style
