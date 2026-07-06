# How to Build Evreghen

Thank you for using Evreghen! If you haven't read the [README](./README.md),
please first read it, then come back.

## Building Evreghen WITH The GUI

<details>
    <summary>Linux</summary>

1. Clone the repo locally
```bash
~/ $ git clone https://github.com/jayshozie/evreghen
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

Aaand, it's done! @TODO: Check where the GUI binary is, and edit this line to tell the user.
</details>
<details>
    <summary>Windows</summary>

1. Clone the repo locally
```powershell
PS C:\Users\User> git clone https://github.com/jayshozie/evreghen
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

Aaand, it's done! @TODO: Check where the GUI binary is, and edit this line to tell the user.
</details>
<details>
    <summary>*BSD</summary>

1. Clone the repo locally
```bash
~/ $ git clone https://github.com/jayshozie/evreghen
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

Aaand, it's done! @TODO: Check where the GUI binary is, and edit this line to tell the user.
</details>
<details>
    <summary>macOS</summary>

1. Clone the repo locally
```bash
~ % git clone https://github.com/jayshozie/evreghen
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

Aaand, it's done! @TODO: Check where the GUI binary is, and edit this line to tell the user.
</details>

---

## Building Evreghen WITHOUT The GUI (as a build system for Evreghen games)

<details>
    <summary>Linux</summary>

1. Clone the repo locally
```bash
~/ $ git clone https://github.com/jayshozie/evreghen
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

Aaand, it's done! @TODO: Check where the GUI binary is, and edit this line to tell the user.
</details>
<details>
    <summary>Windows</summary>

1. Clone the repo locally:
```powershell
PS C:\Users\User> git clone https://github.com/jayshozie/evreghen
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

Aaand, it's done! @TODO: Check where the GUI binary is, and edit this line to tell the user.
</details>
<details>
    <summary>*BSD</summary>

1. Clone the repo locally:
```bash
~/ $ git clone https://github.com/jayshozie/evreghen
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

Aaand, it's done! @TODO: Check where the GUI binary is, and edit this line to tell the user.
</details>
<details>
    <summary>macOS</summary>

1. Clone the repo locally
```bash
~ % git clone https://github.com/jayshozie/evreghen
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

Aaand, it's done! @TODO: Check where the GUI binary is, and edit this line to tell the user.
</details>
