#!/bin/bash
# FlexonDB Build Automation Script
# Builds FlexonDB for multiple platforms

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Configuration
DEFAULT_PLATFORMS="host"
CLEAN_BUILD=false
VERBOSE=false
PARALLEL_JOBS=""
BUILD_TYPE="Release"

# Available platforms
AVAILABLE_PLATFORMS=("linux" "macos" "windows" "android" "ios" "host")

usage() {
    echo "FlexonDB Build Automation Script"
    echo ""
    echo "Usage: $0 [OPTIONS] [PLATFORMS...]"
    echo ""
    echo "Options:"
    echo "  -c, --clean            Clean build directories before building"
    echo "  -v, --verbose          Verbose build output"
    echo "  -j, --jobs N           Number of parallel build jobs"
    echo "  -t, --type TYPE        Build type: Debug, Release, RelWithDebInfo (default: Release)"
    echo "  -h, --help             Show this help message"
    echo ""
    echo "Platforms:"
    echo "  host                   Build for current host platform (default)"
    echo "  linux                  Build for Linux"
    echo "  macos                  Build for macOS"
    echo "  windows                Build for Windows (requires MinGW or cross-compiler)"
    echo "  android                Build for Android (requires Android NDK)"
    echo "  ios                    Build for iOS (requires Xcode, macOS only)"
    echo "  all                    Build for all available platforms"
    echo ""
    echo "Examples:"
    echo "  $0                     # Build for host platform"
    echo "  $0 linux macos         # Build for Linux and macOS"
    echo "  $0 --clean all         # Clean build and build for all platforms"
    echo "  $0 -j 4 android        # Build for Android with 4 parallel jobs"
    echo "  $0 --type Debug host   # Debug build for host platform"
}

# Parse command line arguments
PLATFORMS=()
while [[ $# -gt 0 ]]; do
    case $1 in
        -c|--clean)
            CLEAN_BUILD=true
            shift
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -j|--jobs)
            PARALLEL_JOBS="$2"
            shift 2
            ;;
        -t|--type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        all)
            PLATFORMS=("linux" "macos" "android" "ios")
            shift
            ;;
        host)
            PLATFORMS+=("host")
            shift
            ;;
        linux|macos|windows|android|ios)
            PLATFORMS+=("$1")
            shift
            ;;
        *)
            echo -e "${RED}❌ Unknown option or platform: $1${NC}"
            usage
            exit 1
            ;;
    esac
done

# Default to host platform if no platforms specified
if [ ${#PLATFORMS[@]} -eq 0 ]; then
    PLATFORMS=("host")
fi

# Detect host platform
detect_host_platform() {
    case "$OSTYPE" in
        linux-gnu*)   echo "linux" ;;
        darwin*)      echo "macos" ;;
        cygwin*)      echo "windows" ;;
        msys*)        echo "windows" ;;
        win32*)       echo "windows" ;;
        *)            echo "unknown" ;;
    esac
}

HOST_PLATFORM=$(detect_host_platform)

echo -e "${CYAN}🏗️  FlexonDB Build Automation${NC}"
echo -e "${CYAN}=============================${NC}"
echo ""
echo -e "${BLUE}Host platform:${NC} $HOST_PLATFORM"
echo -e "${BLUE}Build type:${NC} $BUILD_TYPE"
echo -e "${BLUE}Platforms to build:${NC} ${PLATFORMS[*]}"

if [ -n "$PARALLEL_JOBS" ]; then
    echo -e "${BLUE}Parallel jobs:${NC} $PARALLEL_JOBS"
fi

if [ "$CLEAN_BUILD" = true ]; then
    echo -e "${BLUE}Clean build:${NC} Yes"
fi

echo ""

# Check if we're in the FlexonDB project directory
if [ ! -f "CMakeLists.txt" ] || [ ! -d "src" ]; then
    echo -e "${RED}❌ Error: This script must be run from the FlexonDB project root directory${NC}"
    exit 1
fi

# Build function
build_platform() {
    local platform=$1
    local actual_platform=$platform
    
    # Handle "host" platform
    if [ "$platform" = "host" ]; then
        actual_platform=$HOST_PLATFORM
    fi
    
    echo -e "${PURPLE}🔨 Building for $platform${NC}"
    echo -e "${PURPLE}================================${NC}"
    
    local build_dir="build/$actual_platform"
    local dist_dir="dist/$actual_platform"
    
    # Clean if requested
    if [ "$CLEAN_BUILD" = true ]; then
        echo -e "${YELLOW}  → Cleaning build directory${NC}"
        rm -rf "$build_dir"
        rm -rf "$dist_dir"
    fi
    
    # Create build directory
    mkdir -p "$build_dir"
    
    # Configure build
    echo -e "${GREEN}  → Configuring build${NC}"
    
    local cmake_args=("-DCMAKE_BUILD_TYPE=$BUILD_TYPE")
    local toolchain_file=""
    
    case $actual_platform in
        linux|macos|windows)
            # Native or cross-compilation build
            cmake_args+=("-DBUILD_PLATFORM=$actual_platform")
            ;;
        android)
            # Check for Android NDK
            if [ -z "$ANDROID_NDK" ] && [ -z "$ANDROID_NDK_ROOT" ]; then
                echo -e "${YELLOW}  ⚠️  Warning: Android NDK not found in environment${NC}"
                echo -e "${YELLOW}      Please set ANDROID_NDK or ANDROID_NDK_ROOT environment variable${NC}"
                echo -e "${YELLOW}      Attempting to use toolchain file anyway...${NC}"
            fi
            
            toolchain_file="toolchains/android.cmake"
            if [ ! -f "$toolchain_file" ]; then
                echo -e "${RED}  ❌ Android toolchain file not found: $toolchain_file${NC}"
                return 1
            fi
            cmake_args+=("-DCMAKE_TOOLCHAIN_FILE=$toolchain_file")
            ;;
        ios)
            # Check if we're on macOS
            if [ "$HOST_PLATFORM" != "macos" ]; then
                echo -e "${RED}  ❌ iOS builds are only supported on macOS${NC}"
                return 1
            fi
            
            toolchain_file="toolchains/ios.cmake"
            if [ ! -f "$toolchain_file" ]; then
                echo -e "${RED}  ❌ iOS toolchain file not found: $toolchain_file${NC}"
                return 1
            fi
            cmake_args+=("-DCMAKE_TOOLCHAIN_FILE=$toolchain_file")
            ;;
        *)
            echo -e "${RED}  ❌ Unsupported platform: $actual_platform${NC}"
            return 1
            ;;
    esac
    
    # Run CMake configuration
    cd "$build_dir"
    
    if [ "$VERBOSE" = true ]; then
        cmake "${cmake_args[@]}" ../..
    else
        cmake "${cmake_args[@]}" ../.. > /dev/null
    fi
    
    cd - > /dev/null
    
    # Build
    echo -e "${GREEN}  → Building${NC}"
    
    local build_args=("--build" "$build_dir" "--config" "$BUILD_TYPE")
    
    if [ -n "$PARALLEL_JOBS" ]; then
        build_args+=("--parallel" "$PARALLEL_JOBS")
    fi
    
    if [ "$VERBOSE" = true ]; then
        cmake "${build_args[@]}"
    else
        cmake "${build_args[@]}" > /dev/null
    fi
    
    # Install/copy to dist
    echo -e "${GREEN}  → Installing to dist directory${NC}"
    
    # The CMake build system is already configured to output to the correct dist directory
    # We just need to make sure it exists and is built
    if [ ! -d "../../dist/$actual_platform" ]; then
        echo -e "${YELLOW}    Warning: dist directory not found, CMake should have created it${NC}"
    fi
    
    echo -e "${GREEN}  ✅ $platform build complete${NC}"
    echo -e "${GREEN}     Binaries: $dist_dir/bin/${NC}"
    echo -e "${GREEN}     Libraries: $dist_dir/lib/${NC}"
    echo ""
}

# Build summary
SUCCESSFUL_BUILDS=()
FAILED_BUILDS=()

# Build each platform
for platform in "${PLATFORMS[@]}"; do
    if build_platform "$platform"; then
        SUCCESSFUL_BUILDS+=("$platform")
    else
        FAILED_BUILDS+=("$platform")
        echo -e "${RED}❌ Failed to build for $platform${NC}"
        echo ""
    fi
done

# Final summary
echo -e "${CYAN}📊 Build Summary${NC}"
echo -e "${CYAN}================${NC}"
echo ""

if [ ${#SUCCESSFUL_BUILDS[@]} -gt 0 ]; then
    echo -e "${GREEN}✅ Successful builds (${#SUCCESSFUL_BUILDS[@]}):${NC}"
    for platform in "${SUCCESSFUL_BUILDS[@]}"; do
        echo -e "${GREEN}   • $platform${NC}"
    done
    echo ""
fi

if [ ${#FAILED_BUILDS[@]} -gt 0 ]; then
    echo -e "${RED}❌ Failed builds (${#FAILED_BUILDS[@]}):${NC}"
    for platform in "${FAILED_BUILDS[@]}"; do
        echo -e "${RED}   • $platform${NC}"
    done
    echo ""
fi

echo -e "${BLUE}📦 Output directories:${NC}"
echo -e "${BLUE}   • Binaries: dist/{platform}/bin/${NC}"
echo -e "${BLUE}   • Libraries: dist/{platform}/lib/${NC}"
echo ""

if [ ${#SUCCESSFUL_BUILDS[@]} -gt 0 ]; then
    echo -e "${GREEN}🎉 Build automation complete!${NC}"
    
    # Show installation instructions
    echo ""
    echo -e "${BLUE}💡 Installation:${NC}"
    echo -e "${BLUE}   • Linux/macOS: ./scripts/install.sh${NC}"
    echo -e "${BLUE}   • Windows: .\\scripts\\install.ps1${NC}"
    echo ""
    
    exit 0
else
    echo -e "${RED}💥 All builds failed!${NC}"
    exit 1
fi