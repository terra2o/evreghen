# evreghen - A Cross-Platform Game Engine and Build System for Terminal Games

Evreghen is an open-source, cross-platform game engine and build system designed specifically for terminal games. It provides both a lightweight curses-based runtime for terminal gameplay and an interactive graphical editor powered by Dear ImGui, GLFW, and OpenGL 3 for designing scenes, inspecting resources, and editing entities live.

## "Evreghen"? What does that mean?

Evreghen means universe/dragon in Old Turkic.

---

## Features

- **Terminal Curses Engine**: High-performance character grid rendering powered by `ncurses` (Unix/macOS) and `PDCurses` (Windows).
- **Interactive GUI Scene Editor**: Desktop editor with a Godot-inspired node hierarchy, interactive 2D map viewport, entity inspector, integrated file system browser, and output logging console.
- **Scene & Asset Serialization**: Lossless JSON scene formats compatible with the Tiled Map Editor specification (version 1.10+).
- **Reusable Resource System**: Standalone JSON resource definitions for entity templates (NPCs and Enemies) with dialogue and stat configurations.
- **Multi-Level Campaign Support**: Sequential dungeon progression with state and health preservation between floors.
- **Headless Build Mode**: Flexible CMake configuration (`-DEVREGHEN_NO_GUI=ON`) allowing the engine to be compiled on servers and minimal environments as a headless build system.

---

## Documentation

- **[User Guide](./docs/user_guide.md)**: Manual for game creators covering project structure, map format, CLI commands, and GUI editor operations.
- **[Maintainer Guide](./docs/maintainer_guide.md)**: Deep dive into the engine's internal architecture, subsystems, and code quality standards.
- **[Build Instructions](./BUILD.md)**: Detailed compilation instructions for Linux, Windows, macOS, and *BSD.

---

## Contributing

Please check the [CONTRIBUTING.md](./CONTRIBUTING.md) file at the project root.

---

## License

This project, and every file therein, is licensed under the
[GNU General Public License v3](https://www.gnu.org/licenses/gpl-3.0.en.html).
For more information, please check the [LICENSE](./LICENSE) file located at the
root of the repository.
