# Development Scripts

This directory contains helper scripts for local development and CI validation.

## CI Validation

### `ci-local.sh`
**Run all CI checks locally before pushing to GitHub**

```bash
./scripts/ci-local.sh
```

Runs all four checks that GitHub Actions CI performs:
1. Code formatting (clang-format)
2. Build with sanitizers
3. Run tests
4. Static analysis (clang-tidy)

**Exit code:** 0 if all checks pass, 1 if any fail

---

## Individual Checks

### `format-check.sh`
Check if code follows formatting rules

```bash
./scripts/format-check.sh
```

### `format-fix.sh`
Auto-format all code to match `.clang-format` rules

```bash
./scripts/format-fix.sh
```

### `build-all.sh`
Build all configurations (debug, release, sanitizers)

```bash
./scripts/build-all.sh
```

### `test-all.sh`
Run all tests (uses sanitizers build if available)

```bash
./scripts/test-all.sh
```

### `static-analysis.sh`
Run clang-tidy static analysis

```bash
./scripts/static-analysis.sh
```

---

## Recommended Workflow

### Before committing:
```bash
# Auto-format your code
./scripts/format-fix.sh

# Build and test
./scripts/build-all.sh
./scripts/test-all.sh
```

### Before pushing:
```bash
# Run full CI validation
./scripts/ci-local.sh
```

This ensures your changes will pass GitHub Actions CI.

---

## Requirements

**Required:**
- CMake 3.28+
- C++ compiler (GCC 13+, Clang 18+)
- Ninja build system

**Optional (for full CI validation):**
- `clang-format` - for formatting checks/fixes
- `clang-tidy` - for static analysis

Install on Ubuntu:
```bash
sudo apt-get install clang-format-18 clang-tidy-18
```
