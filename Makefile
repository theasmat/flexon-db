CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -O2
INCLUDES = -Iinclude
SRCDIR = src
BUILDDIR = build
EXAMPLEDIR = examples

# Source directories
CORE_SRCDIR = $(SRCDIR)/core
SHELL_SRCDIR = $(SRCDIR)/shell
CLI_SRCDIR = $(SRCDIR)/cli
COMMON_SRCDIR = $(SRCDIR)/common

# Create build directory
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Common utilities
$(BUILDDIR)/logo.o: $(COMMON_SRCDIR)/logo.c include/logo.h | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	
$(BUILDDIR)/welcome.o: $(COMMON_SRCDIR)/welcome.c include/welcome.h | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/error.o: $(COMMON_SRCDIR)/error.c include/error.h include/config.h include/types.h | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/utils.o: $(COMMON_SRCDIR)/utils.c include/utils.h include/error.h include/types.h include/config.h | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Core database modules
$(BUILDDIR)/schema.o: $(CORE_SRCDIR)/schema.c include/schema.h include/config.h | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/writer.o: $(CORE_SRCDIR)/writer.c include/writer.h include/schema.h include/config.h | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/reader.o: $(CORE_SRCDIR)/reader.c include/reader.h include/schema.h include/writer.h include/config.h | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Shell modules
$(BUILDDIR)/session.o: $(SHELL_SRCDIR)/session.c include/shell.h include/config.h | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/formatter.o: $(SHELL_SRCDIR)/formatter.c include/shell.h include/config.h | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/parser.o: $(SHELL_SRCDIR)/parser.c include/shell.h include/config.h | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/shell.o: $(SHELL_SRCDIR)/shell.c include/shell.h include/config.h | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# CLI main entry point
$(BUILDDIR)/main.o: $(CLI_SRCDIR)/main.c include/schema.h include/writer.h include/reader.h include/shell.h include/config.h | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Object groups
COMMON_OBJS = $(BUILDDIR)/error.o $(BUILDDIR)/utils.o $(BUILDDIR)/logo.o $(BUILDDIR)/welcome.o
CORE_OBJS = $(BUILDDIR)/schema.o $(BUILDDIR)/writer.o $(BUILDDIR)/reader.o
SHELL_OBJS = $(BUILDDIR)/session.o $(BUILDDIR)/formatter.o $(BUILDDIR)/parser.o $(BUILDDIR)/shell.o
CLI_OBJS = $(BUILDDIR)/main.o

# Main CLI tool
$(BUILDDIR)/flexon: $(CLI_OBJS) $(CORE_OBJS) $(SHELL_OBJS) $(COMMON_OBJS) | $(BUILDDIR)
	$(CC) $(CFLAGS) $^ -o $@

# Test programs
$(BUILDDIR)/test_schema: $(EXAMPLEDIR)/test_schema.c $(BUILDDIR)/schema.o | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

$(BUILDDIR)/test_writer: $(EXAMPLEDIR)/test_writer.c $(CORE_OBJS) | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

$(BUILDDIR)/test_reader: $(EXAMPLEDIR)/test_reader.c $(CORE_OBJS) | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

# Configuration demo
$(BUILDDIR)/test_config: test_config.c $(COMMON_OBJS) | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

# Build all
all: $(BUILDDIR)/flexon $(BUILDDIR)/test_schema $(BUILDDIR)/test_writer $(BUILDDIR)/test_reader $(BUILDDIR)/test_config

# Individual module builds
core: $(CORE_OBJS)
shell: $(SHELL_OBJS)
common: $(COMMON_OBJS)
cli: $(CLI_OBJS)

# Individual tests
test-schema: $(BUILDDIR)/test_schema
	./$(BUILDDIR)/test_schema

test-writer: $(BUILDDIR)/test_writer
	./$(BUILDDIR)/test_writer

test-reader: $(BUILDDIR)/test_reader
	./$(BUILDDIR)/test_reader

test-config: $(BUILDDIR)/test_config
	./$(BUILDDIR)/test_config

# Test all modules
test: test-schema test-writer test-reader

# Advanced CLI demo with directory support
demo-advanced: $(BUILDDIR)/flexon
	@echo "🚀 FlexonDB Advanced CLI Demo"
	@echo "=============================="
	@echo ""
	@echo "Creating databases in different directories..."
	mkdir -p demo_databases
	./$(BUILDDIR)/flexon create employees.fxdb --schema "name string, age int32, department string, salary float" -d demo_databases
	./$(BUILDDIR)/flexon create products.fxdb --schema "id int32, name string, price float, available bool"
	@echo ""
	@echo "Listing databases..."
	@echo "📂 Current directory:"
	./$(BUILDDIR)/flexon list
	@echo ""
	@echo "📂 demo_databases directory:"
	./$(BUILDDIR)/flexon list -d demo_databases
	@echo ""
	@echo "Database info:"
	./$(BUILDDIR)/flexon info employees.fxdb -d demo_databases
	@echo ""
	@echo "💡 Use test_writer to add data, then read with:"
	@echo "   ./build/flexon read <file.fxdb> [-d directory]"

# Install (copy to /usr/local/bin)
install: $(BUILDDIR)/flexon
	sudo cp $(BUILDDIR)/flexon /usr/local/bin/

# Clean targets
clean:
	rm -rf $(BUILDDIR) *.fxdb demo_databases test_databases

clean-core:
	rm -f $(CORE_OBJS)

clean-shell:
	rm -f $(SHELL_OBJS)

clean-common:
	rm -f $(COMMON_OBJS)

# Help target
help:
	@echo "FlexonDB Build System"
	@echo "====================="
	@echo ""
	@echo "Available targets:"
	@echo "  all           - Build everything"
	@echo "  core          - Build core database modules"
	@echo "  shell         - Build shell modules"
	@echo "  common        - Build common utilities"
	@echo "  cli           - Build CLI entry point"
	@echo "  test          - Run all tests"
	@echo "  test-schema   - Run schema tests"
	@echo "  test-writer   - Run writer tests"
	@echo "  test-reader   - Run reader tests"
	@echo "  test-config   - Run configuration demo"
	@echo "  demo-advanced - Run advanced demo"
	@echo "  clean         - Clean all build artifacts"
	@echo "  install       - Install to /usr/local/bin"
	@echo "  help          - Show this help"

.PHONY: all core shell common cli test test-schema test-writer test-reader test-config demo-advanced install clean clean-core clean-shell clean-common help