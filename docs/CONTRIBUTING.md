# Contributing to FlexonDB

Thank you for your interest in contributing to FlexonDB! This document provides guidelines and information for contributors.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Setup](#development-setup)
- [Contributing Guidelines](#contributing-guidelines)
- [Coding Standards](#coding-standards)
- [Testing](#testing)
- [Documentation](#documentation)
- [Submitting Changes](#submitting-changes)

## Code of Conduct

FlexonDB is committed to providing a welcoming and inclusive environment for all contributors. Please be respectful and professional in all interactions.

### Our Standards

- Use welcoming and inclusive language
- Be respectful of differing viewpoints and experiences
- Gracefully accept constructive criticism
- Focus on what is best for the community
- Show empathy towards other community members

## Getting Started

### Prerequisites

- **C Compiler**: GCC 4.9+, Clang 3.5+, or MSVC 2015+
- **Make**: GNU Make 3.81+ or compatible
- **Git**: For version control
- **Text Editor**: Any editor with C syntax highlighting

### Repository Structure

```
flexon-db/
├── src/                    # Source code
│   ├── core/              # Core database engine
│   ├── cli/               # Command-line interface
│   ├── shell/             # Interactive shell
│   ├── common/            # Common utilities
│   └── compat/            # Cross-platform compatibility
├── include/               # Header files
├── examples/              # Example programs
├── docs/                  # Documentation
├── tests/                 # Test files
├── mobile/                # Mobile platform bindings
└── build/                 # Build artifacts (generated)
```

## Development Setup

### 1. Fork and Clone

```bash
# Fork the repository on GitHub, then clone your fork
git clone https://github.com/YOUR_USERNAME/flexon-db.git
cd flexon-db

# Add upstream remote
git remote add upstream https://github.com/theasmat/flexon-db.git
```

### 2. Build Development Environment

```bash
# Build everything
make clean && make all

# Run tests to ensure everything works
make test

# Install development tools (optional)
sudo apt install valgrind gdb    # Linux debugging tools
```

### 3. Create Development Branch

```bash
# Create feature branch
git checkout -b feature/your-feature-name

# Or bug fix branch
git checkout -b fix/issue-description
```

## Contributing Guidelines

### Types of Contributions

We welcome various types of contributions:

1. **Bug Reports** - Help us identify and fix issues
2. **Feature Requests** - Suggest new functionality
3. **Code Contributions** - Bug fixes, features, optimizations
4. **Documentation** - Improve or add documentation
5. **Testing** - Add test cases or improve test coverage
6. **Performance** - Optimizations and benchmarks

### Before You Start

1. **Check existing issues** - Avoid duplicate work
2. **Discuss major changes** - Create an issue first for large features
3. **Start small** - Begin with small, focused contributions
4. **Follow conventions** - Adhere to existing code style

### Issue Guidelines

When reporting bugs or requesting features:

```markdown
**Bug Report Template:**
- FlexonDB version
- Operating system and version
- Compiler and version
- Steps to reproduce
- Expected behavior
- Actual behavior
- Error messages (if any)

**Feature Request Template:**
- Use case description
- Proposed solution
- Alternative solutions considered
- Additional context
```

## Coding Standards

### C Code Style

FlexonDB follows these coding conventions:

#### Naming Conventions

```c
// Functions: lowercase with underscores
int parse_schema(const char* schema_str);

// Types: lowercase with _t suffix
typedef struct {
    uint32_t field_count;
} schema_t;

// Constants: UPPERCASE with underscores
#define MAX_FIELD_NAME_LENGTH 64

// Variables: lowercase with underscores
uint32_t row_count = 0;
const char* file_name = "test.fxdb";
```

#### Function Documentation

```c
/**
 * Parse a schema string into a schema structure
 * @param schema_str Schema definition string (e.g., "name string, age int32")
 * @return Parsed schema object, or NULL on error
 */
schema_t* parse_schema(const char* schema_str);
```

#### Error Handling

```c
// Always check return values
writer_t* writer = writer_create_default("test.fxdb", schema);
if (!writer) {
    fprintf(stderr, "Failed to create writer\n");
    return -1;
}

// Use consistent error codes
// 0 = success, -1 = error, NULL = error for pointers
```

#### Memory Management

```c
// Always pair malloc/free
char* buffer = malloc(size);
if (!buffer) {
    return -1;
}
// ... use buffer ...
free(buffer);

// Check parameters for NULL
if (!writer || !json_str) {
    return -1;
}
```

### Code Formatting

```c
// Indentation: 4 spaces (no tabs)
if (condition) {
    do_something();
    if (nested_condition) {
        do_nested_thing();
    }
}

// Braces: Opening brace on same line
for (int i = 0; i < count; i++) {
    process_item(i);
}

// Long lines: Break at 120 characters
int result = some_very_long_function_name(parameter1, parameter2,
                                         parameter3, parameter4);
```

### Header Files

```c
// Include guards
#ifndef HEADER_NAME_H
#define HEADER_NAME_H

// System includes first
#include <stdio.h>
#include <stdlib.h>

// Local includes second
#include "config.h"
#include "types.h"

// Function declarations
int function_name(int param1, const char* param2);

#endif // HEADER_NAME_H
```

## Testing

### Running Tests

```bash
# Run all tests
make test

# Run specific test suites
make test-schema
make test-writer
make test-reader

# Run with memory checking (if valgrind available)
valgrind --leak-check=full ./build/test_writer
```

### Writing Tests

Create test files following this pattern:

```c
// test_new_feature.c
#include "include/schema.h"
#include "include/writer.h"
#include <stdio.h>
#include <assert.h>

int test_basic_functionality() {
    printf("Testing basic functionality...\n");
    
    // Test setup
    schema_t* schema = parse_schema("name string, age int32");
    assert(schema != NULL);
    
    // Test operation
    writer_t* writer = writer_create_default("test.fxdb", schema);
    assert(writer != NULL);
    
    // Test cleanup
    writer_close(writer);
    writer_free(writer);
    free_schema(schema);
    
    printf("✓ Basic functionality test passed\n");
    return 0;
}

int main() {
    printf("=== New Feature Test Suite ===\n");
    
    if (test_basic_functionality() != 0) {
        return 1;
    }
    
    printf("=== All Tests Passed ===\n");
    return 0;
}
```

Add test to Makefile:
```makefile
$(BUILDDIR)/test_new_feature: test_new_feature.c $(CORE_OBJS) $(COMMON_OBJS) $(COMPAT_OBJS) | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

test-new-feature: $(BUILDDIR)/test_new_feature
	./$(BUILDDIR)/test_new_feature
```

### Test Coverage

Aim for comprehensive test coverage:

- **Unit Tests** - Test individual functions
- **Integration Tests** - Test component interactions
- **Error Handling** - Test failure conditions
- **Edge Cases** - Test boundary conditions
- **Cross-Platform** - Test on different systems

## Documentation

### Types of Documentation

1. **Code Comments** - Inline documentation
2. **API Documentation** - Function reference
3. **User Documentation** - Usage guides
4. **Developer Documentation** - Architecture and design

### Writing Documentation

```markdown
# Use clear, descriptive titles

## Structure content logically

### Include code examples

```c
// Example code should be complete and runnable
schema_t* schema = parse_schema("name string, age int32");
writer_t* writer = writer_create_default("test.fxdb", schema);
```

### Best Practices

- Use simple, clear language
- Include practical examples
- Keep examples up-to-date
- Cross-reference related topics
```

## Submitting Changes

### Preparation

```bash
# Ensure your branch is up-to-date
git fetch upstream
git rebase upstream/main

# Run tests
make clean && make all && make test

# Check for memory leaks (if valgrind available)
valgrind --leak-check=full ./build/test_writer

# Verify your changes don't break existing functionality
```

### Commit Guidelines

Write clear, descriptive commit messages:

```bash
# Good commit messages
git commit -m "Add JSON validation in writer_insert_json()"
git commit -m "Fix memory leak in schema parsing"
git commit -m "Improve error messages for CLI commands"

# Include issue references when applicable
git commit -m "Fix database corruption issue (#123)"
```

### Pull Request Process

1. **Create Pull Request**
   - Use descriptive title
   - Explain the change and why it's needed
   - Reference related issues
   - Include testing information

2. **Pull Request Template**
```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Documentation update
- [ ] Performance improvement
- [ ] Code refactoring

## Testing
- [ ] Tests pass locally
- [ ] Added new tests (if applicable)
- [ ] Manual testing completed

## Checklist
- [ ] Code follows style guidelines
- [ ] Self-review completed
- [ ] Documentation updated
- [ ] No breaking changes (or clearly documented)
```

3. **Review Process**
   - Maintainers will review your code
   - Address feedback promptly
   - Be open to suggestions
   - Update your branch as needed

### After Submission

- Respond to review comments
- Make requested changes
- Keep PR up-to-date with main branch
- Be patient during review process

## Development Workflow

### Typical Workflow

```bash
# 1. Start with latest code
git checkout main
git pull upstream main

# 2. Create feature branch
git checkout -b feature/awesome-feature

# 3. Make changes
# ... edit files ...

# 4. Test changes
make clean && make all && make test

# 5. Commit changes
git add .
git commit -m "Add awesome feature"

# 6. Push to your fork
git push origin feature/awesome-feature

# 7. Create pull request on GitHub
```

### Debugging Tips

```bash
# Build with debug symbols
make CFLAGS="-g -O0 -DDEBUG" all

# Use GDB for debugging
gdb ./build/flexon
(gdb) run create test.fxdb --schema "name string"
(gdb) bt

# Memory debugging with Valgrind
valgrind --tool=memcheck --leak-check=full ./build/flexon

# Static analysis (if available)
clang-analyzer-scan-build make
cppcheck src/
```

## Getting Help

### Resources

- **Documentation**: Check `docs/` directory
- **Examples**: Look at `examples/` directory
- **Issues**: Search existing GitHub issues
- **Discussions**: Use GitHub Discussions for questions

### Contact

- **GitHub Issues**: Bug reports and feature requests
- **GitHub Discussions**: General questions and discussions
- **Pull Requests**: Code contributions

## Recognition

Contributors will be recognized in:

- `CONTRIBUTORS.md` file
- Release notes for significant contributions
- GitHub contributors page

## License

By contributing to FlexonDB, you agree that your contributions will be licensed under the same MIT License that covers the project.

---

Thank you for contributing to FlexonDB! Your contributions help make FlexonDB better for everyone.

## Quick Reference

### Useful Commands

```bash
# Development setup
make clean && make all && make test

# Run specific tests
make test-schema test-writer test-reader

# Debug build
make CFLAGS="-g -O0 -DDEBUG" all

# Memory check
valgrind --leak-check=full ./build/test_writer

# Code formatting (if clang-format available)
find src -name "*.c" -o -name "*.h" | xargs clang-format -i
```

### File Locations

- **Core engine**: `src/core/`
- **CLI interface**: `src/cli/`
- **Shell interface**: `src/shell/`
- **Headers**: `include/`
- **Tests**: `examples/` and root directory test files
- **Documentation**: `docs/` and `README.md`