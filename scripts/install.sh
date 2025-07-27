#!/bin/bash
# FlexonDB Installation Script for Linux/macOS
# This script installs FlexonDB binaries to the system

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
DEFAULT_INSTALL_DIR="/usr/local/bin"
DEFAULT_LIB_DIR="/usr/local/lib"
DEFAULT_INCLUDE_DIR="/usr/local/include"

# Detect platform
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    PLATFORM="linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    PLATFORM="macos"
else
    echo -e "${RED}❌ Unsupported platform: $OSTYPE${NC}"
    exit 1
fi

# Check for command line options
INSTALL_DIR="$DEFAULT_INSTALL_DIR"
LIB_DIR="$DEFAULT_LIB_DIR"
INCLUDE_DIR="$DEFAULT_INCLUDE_DIR"
INSTALL_LIBS=false
FORCE=false

usage() {
    echo "FlexonDB Installation Script"
    echo ""
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -d, --dir DIR           Installation directory (default: $DEFAULT_INSTALL_DIR)"
    echo "  -l, --libs             Also install libraries and headers"
    echo "  --lib-dir DIR          Library installation directory (default: $DEFAULT_LIB_DIR)"
    echo "  --include-dir DIR      Header installation directory (default: $DEFAULT_INCLUDE_DIR)"
    echo "  -f, --force            Force installation without confirmation"
    echo "  -h, --help             Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0                     # Install to $DEFAULT_INSTALL_DIR"
    echo "  $0 -d \$HOME/bin        # Install to user's bin directory"
    echo "  $0 -l                  # Install binaries and libraries"
    echo "  $0 --help              # Show this help"
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -d|--dir)
            INSTALL_DIR="$2"
            shift 2
            ;;
        -l|--libs)
            INSTALL_LIBS=true
            shift
            ;;
        --lib-dir)
            LIB_DIR="$2"
            shift 2
            ;;
        --include-dir)
            INCLUDE_DIR="$2"
            shift 2
            ;;
        -f|--force)
            FORCE=true
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo -e "${RED}❌ Unknown option: $1${NC}"
            usage
            exit 1
            ;;
    esac
done

echo -e "${BLUE}🚀 FlexonDB Installation Script${NC}"
echo -e "${BLUE}================================${NC}"
echo ""

# Check if we're in the FlexonDB project directory
if [ ! -f "CMakeLists.txt" ] || [ ! -d "src" ]; then
    echo -e "${RED}❌ Error: This script must be run from the FlexonDB project root directory${NC}"
    exit 1
fi

# Check if build exists
DIST_DIR="dist/$PLATFORM"
if [ ! -d "$DIST_DIR" ]; then
    echo -e "${RED}❌ Error: FlexonDB has not been built for $PLATFORM${NC}"
    echo -e "${YELLOW}💡 Please build first with:${NC}"
    echo "   mkdir -p build/$PLATFORM"
    echo "   cd build/$PLATFORM"
    echo "   cmake ../.."
    echo "   make"
    exit 1
fi

# Check if binaries exist
MAIN_BINARY="$DIST_DIR/bin/flexon"
if [ ! -f "$MAIN_BINARY" ]; then
    echo -e "${RED}❌ Error: FlexonDB binary not found at $MAIN_BINARY${NC}"
    echo -e "${YELLOW}💡 Please build FlexonDB first${NC}"
    exit 1
fi

echo -e "${GREEN}✓${NC} Found FlexonDB build for $PLATFORM"
echo -e "${GREEN}✓${NC} Installation directory: $INSTALL_DIR"

if [ "$INSTALL_LIBS" = true ]; then
    echo -e "${GREEN}✓${NC} Library directory: $LIB_DIR"
    echo -e "${GREEN}✓${NC} Include directory: $INCLUDE_DIR"
fi

echo ""

# Check if we need sudo
NEED_SUDO=false
if [ ! -w "$INSTALL_DIR" ]; then
    NEED_SUDO=true
fi

if [ "$INSTALL_LIBS" = true ]; then
    if [ ! -w "$LIB_DIR" ] || [ ! -w "$INCLUDE_DIR" ]; then
        NEED_SUDO=true
    fi
fi

if [ "$NEED_SUDO" = true ]; then
    echo -e "${YELLOW}⚠️  This installation requires sudo privileges${NC}"
fi

# Confirmation prompt
if [ "$FORCE" = false ]; then
    echo -e "${YELLOW}📦 Ready to install FlexonDB${NC}"
    echo ""
    echo "Files to install:"
    echo "  • flexon          → $INSTALL_DIR/flexon"
    
    if [ -f "$DIST_DIR/bin/flexon-cli" ]; then
        echo "  • flexon-cli      → $INSTALL_DIR/flexon-cli"
    fi
    
    if [ -f "$DIST_DIR/bin/flexon-shell" ]; then
        echo "  • flexon-shell    → $INSTALL_DIR/flexon-shell"
    fi
    
    if [ "$INSTALL_LIBS" = true ]; then
        echo "  • Libraries       → $LIB_DIR/"
        echo "  • Headers         → $INCLUDE_DIR/"
    fi
    
    echo ""
    read -p "Continue with installation? [y/N] " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo -e "${YELLOW}📦 Installation cancelled${NC}"
        exit 0
    fi
fi

echo ""
echo -e "${BLUE}🔧 Installing FlexonDB...${NC}"

# Create directories
if [ "$NEED_SUDO" = true ]; then
    sudo mkdir -p "$INSTALL_DIR"
    if [ "$INSTALL_LIBS" = true ]; then
        sudo mkdir -p "$LIB_DIR"
        sudo mkdir -p "$INCLUDE_DIR"
    fi
else
    mkdir -p "$INSTALL_DIR"
    if [ "$INSTALL_LIBS" = true ]; then
        mkdir -p "$LIB_DIR"
        mkdir -p "$INCLUDE_DIR"
    fi
fi

# Install main binary
echo -e "${GREEN}  →${NC} Installing flexon..."
if [ "$NEED_SUDO" = true ]; then
    sudo cp "$DIST_DIR/bin/flexon" "$INSTALL_DIR/"
    sudo chmod +x "$INSTALL_DIR/flexon"
else
    cp "$DIST_DIR/bin/flexon" "$INSTALL_DIR/"
    chmod +x "$INSTALL_DIR/flexon"
fi

# Install additional binaries if they exist
for BINARY in flexon-cli flexon-shell; do
    if [ -f "$DIST_DIR/bin/$BINARY" ]; then
        echo -e "${GREEN}  →${NC} Installing $BINARY..."
        if [ "$NEED_SUDO" = true ]; then
            sudo cp "$DIST_DIR/bin/$BINARY" "$INSTALL_DIR/"
            sudo chmod +x "$INSTALL_DIR/$BINARY"
        else
            cp "$DIST_DIR/bin/$BINARY" "$INSTALL_DIR/"
            chmod +x "$INSTALL_DIR/$BINARY"
        fi
    fi
done

# Install libraries and headers if requested
if [ "$INSTALL_LIBS" = true ]; then
    if [ -d "$DIST_DIR/lib" ]; then
        echo -e "${GREEN}  →${NC} Installing libraries..."
        if [ "$NEED_SUDO" = true ]; then
            sudo cp -r "$DIST_DIR/lib/"* "$LIB_DIR/"
        else
            cp -r "$DIST_DIR/lib/"* "$LIB_DIR/"
        fi
    fi
    
    if [ -d "include" ]; then
        echo -e "${GREEN}  →${NC} Installing headers..."
        if [ "$NEED_SUDO" = true ]; then
            sudo cp -r include/* "$INCLUDE_DIR/"
        else
            cp -r include/* "$INCLUDE_DIR/"
        fi
    fi
fi

echo ""
echo -e "${GREEN}✅ FlexonDB installed successfully!${NC}"
echo ""
echo -e "${BLUE}🎉 You can now run:${NC}"
echo "   flexon --help"
echo ""

# Add to PATH reminder
if [[ ":$PATH:" != *":$INSTALL_DIR:"* ]]; then
    echo -e "${YELLOW}💡 Note: $INSTALL_DIR is not in your PATH${NC}"
    echo "   Add it to your PATH to use 'flexon' from anywhere:"
    echo "   export PATH=\"$INSTALL_DIR:\$PATH\""
    echo ""
fi

echo -e "${BLUE}📚 Documentation:${NC}"
echo "   • README.md for usage examples"
echo "   • docs/ directory for detailed documentation"
echo ""
echo -e "${GREEN}🚀 Happy database management with FlexonDB!${NC}"