# Contributing to Evreghen

Thank you for contributing to Evreghen! We welcome contributions ranging from bug fixes and documentation to new engine features and tools.

Before submitting a pull request, please review the requirements below.

---

## Developer Certificate of Origin (DCO)

Every commit submitted to the repository must be signed off to confirm that you have the right to submit the code under the project's GPLv3 license.

Use the `-s` flag when creating commits:

```bash
git commit -s -m "feat(scope): Short description"
```

This appends the standard trailer:

```
Signed-off-by: Your Name <your.email@example.com>
```

---

## Commit Guidelines

We follow the [Conventional Commits](https://www.conventionalcommits.org/) specification:

```
<type>(<scope>): <description>

[optional body]

Signed-off-by: Your Name <your.email@example.com>
```

### Types

- `feat`: New feature or capability
- `fix`: Bug fix
- `docs`: Documentation updates
- `style`: Code style or formatting changes without functional alteration
- `refactor`: Code refactoring without functional changes
- `test`: Adding or modifying tests
- `chore` / `build`: Build system or maintenance changes

---

## Code Style & Formatting

- All C++ code must adhere to the project's [`.clang-format`](./.clang-format) file (LLVM base, real tabs for indentation, 80-column limit).
- Run formatting before committing:
  ```bash
  find engine cli gui -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
  ```
- Write comments in a classic developer tone without capitalizing the first word.
- Only comment on non-obvious logic or deep architectural choices. Avoid comments that merely re-state syntax.

---

## Maintainer & Architecture Documentation

For a detailed walkthrough of the engine subsystems, node tree architecture, and extension guides, consult the [Maintainer Guide](./docs/maintainer_guide.md).
