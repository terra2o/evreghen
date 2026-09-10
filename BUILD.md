# How to Build Evreghen

Thank you for using Evreghen! If you haven't read the [README](./README.md),
please first read it, then come back.

## Prerequisites

Before building, make sure you have the following installed:
- CMake 3.28 or later
- A C++20 compliant compiler (GCC 11+, Clang 13+, or MSVC 2019+)
- Terminal curses development libraries:
  - **Linux (Debian/Ubuntu)**: `sudo apt install libncurses-dev`
  - **Linux (Fedora/RHEL)**: `sudo dnf install ncurses-devel`
  - **Linux (Arch)**: `sudo pacman -S ncurses`
  - **macOS**: ncurses comes with the system or via Homebrew (`brew install ncurses`)
  - **Windows**: PDCurses or PDCursesMod (via vcpkg or MSYS2)

---

## Building Evreghen WITH The GUI

<details>
    <summary>Linux</summary>

1. Clone the repo locally
```bash
~/ $ git clone https://github.com/terra2o/evreghen
~/ $ cd evreghen
~/evreghen $
```

2. Create a build directory
```bash
~/evreghen $ mkdir build
~/evreghen $ cd build
~/evreghen/build $
```

3. Build the project
```bash
~/evreghen/build $ cmake ../
~/evreghen/build $ cmake --build .
```

The GUI binary will be located at `./gui/evreghen-gui` and the CLI binary will be at `./cli/evreghen`.
</details>
<details>
    <summary>Windows</summary>

1. Clone the repo locally
```powershell
PS C:\Users\User> git clone https://github.com/terra2o/evreghen
PS C:\Users\User> cd evreghen
PS C:\Users\User\evreghen>
```

2. Create a build directory
```powershell
PS C:\Users\User\evreghen> mkdir build
PS C:\Users\User\evreghen> cd build
PS C:\Users\User\evreghen\build>
```

3. Build the project
```powershell
PS C:\Users\Username\evreghen\build> cmake ..
PS C:\Users\Username\evreghen\build> cmake --build .
```

The GUI binary will be located at `.\gui\evreghen-gui.exe` (or `.\gui\Debug\evreghen-gui.exe` / `.\gui\Release\evreghen-gui.exe` depending on your generator), and the CLI binary will be at `.\cli\evreghen.exe`.
</details>
<details>
    <summary>*BSD</summary>

1. Clone the repo locally
```bash
~/ $ git clone https://github.com/terra2o/evreghen
~/ $ cd evreghen
~/evreghen $
```

2. Create a build directory
```bash
~/evreghen $ mkdir build
~/evreghen $ cd build
~/evreghen/build $
```

3. Build the project
```bash
~/evreghen/build $ cmake ../
~/evreghen/build $ cmake --build .
```

The GUI binary will be located at `./gui/evreghen-gui` and the CLI binary will be at `./cli/evreghen`.
</details>
<details>
    <summary>macOS</summary>

1. Clone the repo locally
```bash
~ % git clone https://github.com/terra2o/evreghen
~ % cd evreghen
~/evreghen %
```

2. Create a build directory
```bash
~/evreghen % mkdir build
~/evreghen % cd build
~/evreghen/build %
```

3. Build the project
```bash
~/evreghen/build % cmake ..
~/evreghen/build % cmake --build .
```

The GUI binary will be located at `./gui/evreghen-gui` and the CLI binary will be at `./cli/evreghen`.
</details>

---

## Building Evreghen WITHOUT The GUI (as a build system for Evreghen games)

<details>
    <summary>Linux</summary>

1. Clone the repo locally
```bash
~/ $ git clone https://github.com/terra2o/evreghen
~/ $ cd evreghen
~/evreghen $
```

2. Create a build directory
```bash
~/evreghen $ mkdir build
~/evreghen $ cd build
~/evreghen/build $
```

3. Build the project
```bash
~/evreghen/build $ cmake -DEVREGHEN_NO_GUI=ON ../
~/evreghen/build $ cmake --build .
```

The CLI engine binary will be located at `./cli/evreghen`.
</details>
<details>
    <summary>Windows</summary>

1. Clone the repo locally
```powershell
PS C:\Users\User> git clone https://github.com/terra2o/evreghen
PS C:\Users\User> cd evreghen
PS C:\Users\User\evreghen>
```

2. Create a build directory
```powershell
PS C:\Users\User\evreghen> mkdir build
PS C:\Users\User\evreghen> cd build
PS C:\Users\User\evreghen\build>
```

3. Build the project
```powershell
PS C:\Users\Username\evreghen\build> cmake -DEVREGHEN_NO_GUI=ON ..
PS C:\Users\Username\evreghen\build> cmake --build .
```

The CLI engine binary will be located at `.\cli\evreghen.exe` (or `.\cli\Debug\evreghen.exe` / `.\cli\Release\evreghen.exe` depending on your generator).
</details>
<details>
    <summary>*BSD</summary>

1. Clone the repo locally
```bash
~/ $ git clone https://github.com/terra2o/evreghen
~/ $ cd evreghen
~/evreghen $
```

2. Create a build directory
```bash
~/evreghen $ mkdir build
~/evreghen $ cd build
~/evreghen/build $
```

3. Build the project
```bash
~/evreghen/build $ cmake -DEVREGHEN_NO_GUI=ON ../
~/evreghen/build $ cmake --build .
```

The CLI engine binary will be located at `./cli/evreghen`.
</details>
<details>
    <summary>macOS</summary>

1. Clone the repo locally
```bash
~ % git clone https://github.com/terra2o/evreghen
~ % cd evreghen
~/evreghen %
```

2. Create a build directory
```bash
~/evreghen % mkdir build
~/evreghen % cd build
~/evreghen/build %
```

3. Build the project
```bash
~/evreghen/build % cmake -DEVREGHEN_NO_GUI=ON ..
~/evreghen/build % cmake --build .
```

The CLI engine binary will be located at `./cli/evreghen`.
</details>
