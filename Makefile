CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -O2
INCLUDES = -Iinclude
SRCDIR = src
BUILDDIR = build
EXAMPLEDIR = examples

# Create build directory
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Object files
$(BUILDDIR)/schema.o: $(SRCDIR)/schema.c include/schema.h | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/writer.o: $(SRCDIR)/writer.c include/writer.h include/schema.h | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/reader.o: $(SRCDIR)/reader.c include/reader.h include/schema.h include/writer.h | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/session.o: $(SRCDIR)/session.c include/shell.h | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/formatter.o: $(SRCDIR)/formatter.c include/shell.h | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/parser.o: $(SRCDIR)/parser.c include/shell.h | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/shell.o: $(SRCDIR)/shell.c include/shell.h | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/main.o: $(SRCDIR)/main.c include/schema.h include/writer.h include/reader.h include/shell.h | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Core library
CORE_OBJS = $(BUILDDIR)/schema.o $(BUILDDIR)/writer.o $(BUILDDIR)/reader.o

# Shell library
SHELL_OBJS = $(BUILDDIR)/session.o $(BUILDDIR)/formatter.o $(BUILDDIR)/parser.o $(BUILDDIR)/shell.o

# Main CLI tool
$(BUILDDIR)/flexon: $(BUILDDIR)/main.o $(CORE_OBJS) $(SHELL_OBJS) | $(BUILDDIR)
	$(CC) $(CFLAGS) $^ -o $@

# Test programs
$(BUILDDIR)/test_schema: $(EXAMPLEDIR)/test_schema.c $(BUILDDIR)/schema.o | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

$(BUILDDIR)/test_writer: $(EXAMPLEDIR)/test_writer.c $(CORE_OBJS) | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

$(BUILDDIR)/test_reader: $(EXAMPLEDIR)/test_reader.c $(CORE_OBJS) | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

# Build all
all: $(BUILDDIR)/flexon $(BUILDDIR)/test_schema $(BUILDDIR)/test_writer $(BUILDDIR)/test_reader

# Individual tests
test-schema: $(BUILDDIR)/test_schema
	./$(BUILDDIR)/test_schema

test-writer: $(BUILDDIR)/test_writer
	./$(BUILDDIR)/test_writer

test-reader: $(BUILDDIR)/test_reader
	./$(BUILDDIR)/test_reader

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

# Clean
clean:
	rm -rf $(BUILDDIR) *.fxdb demo_databases test_databases

.PHONY: all test test-schema test-writer test-reader demo-advanced install clean