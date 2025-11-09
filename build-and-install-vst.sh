#!/bin/bash

# CR-717 VST3 Build and Install Script
# Builds the plugin and installs it to the system VST3 directory

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Detect OS
OS="$(uname -s)"
case "${OS}" in
    Darwin*)    PLATFORM="macOS";;
    Linux*)     PLATFORM="Linux";;
    MINGW*|MSYS*|CYGWIN*|MINGW64*) PLATFORM="Windows";;
    *)          PLATFORM="Unknown";;
esac

# Additional Windows detection for Git Bash and other environments
if [ "${PLATFORM}" = "Unknown" ]; then
    if [ -n "${WINDIR}" ] || [ -n "${windir}" ]; then
        PLATFORM="Windows"
    fi
fi

echo -e "${BLUE}=== CR-717 VST3 Build and Install ===${NC}"
echo -e "${BLUE}Platform: ${PLATFORM}${NC}"
echo ""

# Set paths
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
VST_SOURCE_DIR="${SCRIPT_DIR}/native/vst3/CR717"
BUILD_DIR="${VST_SOURCE_DIR}/build"

# Determine VST3 install location based on platform
if [ "${PLATFORM}" = "macOS" ]; then
    VST3_INSTALL_DIR="${HOME}/Library/Audio/Plug-Ins/VST3"
    VST3_NAME="Cherni CR-717.vst3"
elif [ "${PLATFORM}" = "Linux" ]; then
    # Check for common Linux VST3 directories, prioritize system-wide then user
    if [ -d "/usr/lib/vst3" ]; then
        VST3_INSTALL_DIR="/usr/lib/vst3"
    elif [ -d "/usr/local/lib/vst3" ]; then
        VST3_INSTALL_DIR="/usr/local/lib/vst3"
    elif [ -d "${HOME}/.vst3" ]; then
        VST3_INSTALL_DIR="${HOME}/.vst3"
    else
        # Create user VST3 directory if it doesn't exist
        VST3_INSTALL_DIR="${HOME}/.vst3"
        mkdir -p "${VST3_INSTALL_DIR}"
    fi
    VST3_NAME="Cherni CR-717.vst3"
elif [ "${PLATFORM}" = "Windows" ]; then
    # Try to determine the correct Program Files path for 32/64-bit systems
    if [ -d "/c/Program Files (x86)/Common Files/VST3" ]; then
        VST3_INSTALL_DIR="/c/Program Files (x86)/Common Files/VST3"
    elif [ -d "/c/Program Files/Common Files/VST3" ]; then
        VST3_INSTALL_DIR="/c/Program Files/Common Files/VST3"
    else
        # Fallback - user might need to run with admin privileges
        VST3_INSTALL_DIR="/c/Program Files/Common Files/VST3"
    fi
    VST3_NAME="Cherni CR-717.vst3"
else
    echo -e "${RED}Unsupported platform: ${PLATFORM}${NC}"
    exit 1
fi

echo -e "${YELLOW}Source directory: ${VST_SOURCE_DIR}${NC}"
echo -e "${YELLOW}Build directory: ${BUILD_DIR}${NC}"
echo -e "${YELLOW}Install directory: ${VST3_INSTALL_DIR}${NC}"
echo ""

# Check if source directory exists
if [ ! -d "${VST_SOURCE_DIR}" ]; then
    echo -e "${RED}Error: VST source directory not found: ${VST_SOURCE_DIR}${NC}"
    exit 1
fi

# Navigate to VST source directory
cd "${VST_SOURCE_DIR}"

# Clean previous build if requested
if [ "$1" = "clean" ] || [ "$1" = "--clean" ]; then
    echo -e "${YELLOW}Cleaning previous build...${NC}"
    if [ -d "${BUILD_DIR}" ]; then
        rm -rf "${BUILD_DIR}"
        echo -e "${GREEN}✓ Build directory cleaned${NC}"
    fi
    echo ""
fi

# Create build directory
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Configure with CMake
echo -e "${BLUE}Configuring CMake...${NC}"
cmake -DCMAKE_BUILD_TYPE=Release ..

if [ $? -ne 0 ]; then
    echo -e "${RED}✗ CMake configuration failed${NC}"
    exit 1
fi
echo -e "${GREEN}✓ CMake configuration successful${NC}"
echo ""

# Build
echo -e "${BLUE}Building plugin...${NC}"
if [ "${PLATFORM}" = "macOS" ]; then
    NUM_CORES=$(sysctl -n hw.ncpu 2>/dev/null || getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)
elif [ "${PLATFORM}" = "Linux" ]; then
    NUM_CORES=$(nproc 2>/dev/null || getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)
elif [ "${PLATFORM}" = "Windows" ]; then
    # In Windows Git Bash, try to get from Windows environment
    NUM_CORES=$(nproc 2>/dev/null || getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)
    # Fallback: try to get from Windows env variable
    if [ "$NUM_CORES" = "4" ] && [ -n "${NUMBER_OF_PROCESSORS}" ]; then
        NUM_CORES="${NUMBER_OF_PROCESSORS}"
    fi
else
    NUM_CORES=$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)
fi
cmake --build . --config Release -j${NUM_CORES}

if [ $? -ne 0 ]; then
    echo -e "${RED}✗ Build failed${NC}"
    exit 1
fi
echo -e "${GREEN}✓ Build successful${NC}"
echo ""

# Find the built VST3 - Windows uses different artifact path
if [ "${PLATFORM}" = "macOS" ]; then
    BUILT_VST="${BUILD_DIR}/CR717_artefacts/Release/VST3/${VST3_NAME}"
elif [ "${PLATFORM}" = "Linux" ]; then
    BUILT_VST="${BUILD_DIR}/CR717_artefacts/Release/VST3/${VST3_NAME}"
elif [ "${PLATFORM}" = "Windows" ]; then
    # Windows builds may have different artifact path, check multiple locations
    if [ -d "${BUILD_DIR}/CR717_artefacts/Release/VST3/${VST3_NAME}" ]; then
        BUILT_VST="${BUILD_DIR}/CR717_artefacts/Release/VST3/${VST3_NAME}"
    elif [ -d "${BUILD_DIR}/CR717_artefacts/VST3/${VST3_NAME}" ]; then
        BUILT_VST="${BUILD_DIR}/CR717_artefacts/VST3/${VST3_NAME}"
    else
        # Fallback for standard Windows build paths
        BUILT_VST="${BUILD_DIR}/CR717_artefacts/Release/VST3/${VST3_NAME}"
    fi
fi

# Check if built VST exists
if [ ! -e "${BUILT_VST}" ]; then
    echo -e "${RED}✗ Built VST3 not found at: ${BUILT_VST}${NC}"
    echo -e "${YELLOW}Searching for VST3 in build directory...${NC}"
    find "${BUILD_DIR}" -name "*.vst3" -type d -o -name "*.vst3" -type f
    exit 1
fi

echo -e "${GREEN}✓ Found built VST3: ${BUILT_VST}${NC}"
echo ""

# Create install directory if it doesn't exist
echo -e "${BLUE}Preparing installation...${NC}"

# On Windows, check if we need elevated privileges for system directories
if [ "${PLATFORM}" = "Windows" ] && ( [[ "${VST3_INSTALL_DIR}" == *"/Program Files/"* ]] || [[ "${VST3_INSTALL_DIR}" == *"/Program Files (x86)/"* ]]); then
    echo -e "${YELLOW}Note: Installing to system directory. Administrator privileges may be required.${NC}"
    # Try to create the directory - if it fails, we'll show an error later
    mkdir -p "${VST3_INSTALL_DIR}" 2>/dev/null || {
        echo -e "${YELLOW}Warning: Could not create directory ${VST3_INSTALL_DIR}${NC}"
        echo -e "${YELLOW}You may need to run this script as administrator or choose a user directory.${NC}"
    }
else
    mkdir -p "${VST3_INSTALL_DIR}"
fi

# Remove existing installation
INSTALLED_VST="${VST3_INSTALL_DIR}/${VST3_NAME}"
if [ -e "${INSTALLED_VST}" ]; then
    echo -e "${YELLOW}Removing existing installation...${NC}"
    # On Windows, try to handle potential permission issues
    if [ "${PLATFORM}" = "Windows" ]; then
        rm -rf "${INSTALLED_VST}" 2>/dev/null || {
            echo -e "${RED}Could not remove existing installation. Try running as administrator.${NC}"
            exit 1
        }
    else
        rm -rf "${INSTALLED_VST}"
    fi
    echo -e "${GREEN}✓ Existing installation removed${NC}"
fi

# Install (copy) the VST3
echo -e "${BLUE}Installing plugin...${NC}"

# On Windows, we might need to handle permissions differently
if [ "${PLATFORM}" = "Windows" ]; then
    # Ensure destination directory exists and handle Windows path conversion
    mkdir -p "${VST3_INSTALL_DIR}" 2>/dev/null || {
        echo -e "${YELLOW}Warning: Could not create directory, attempting install anyway...${NC}"
    }
    # Use rsync for better Windows compatibility, fallback to cp
    if command -v rsync >/dev/null 2>&1; then
        rsync -av "${BUILT_VST}" "${VST3_INSTALL_DIR}/"
    else
        cp -R "${BUILT_VST}" "${VST3_INSTALL_DIR}/"
    fi
else
    cp -R "${BUILT_VST}" "${VST3_INSTALL_DIR}/"
fi

if [ $? -ne 0 ]; then
    echo -e "${RED}✗ Installation failed${NC}"
    exit 1
fi

echo -e "${GREEN}✓ Plugin installed successfully${NC}"
echo ""

# Verify installation
if [ -e "${INSTALLED_VST}" ]; then
    echo -e "${GREEN}=== Installation Complete ===${NC}"
    echo -e "${GREEN}Plugin location: ${INSTALLED_VST}${NC}"
    echo ""
    echo -e "${BLUE}Next steps:${NC}"
    echo -e "  1. Restart your DAW (Ableton Live, Reaper, etc.)"
    echo -e "  2. Rescan plugins if necessary"
    echo -e "  3. Look for 'Cherni CR-717' in your instrument list"
    echo ""
    
    # macOS specific: check code signing
    if [ "${PLATFORM}" = "macOS" ]; then
        echo -e "${YELLOW}Note: For distribution, you should code sign the plugin:${NC}"
        echo -e "  codesign --deep --force --sign \"Developer ID\" \"${INSTALLED_VST}\""
        echo ""
    fi
elif [ "${PLATFORM}" = "Windows" ] && ( [[ "${VST3_INSTALL_DIR}" == *"/Program Files/"* ]] || [[ "${VST3_INSTALL_DIR}" == *"/Program Files (x86)/"* ]]); then
    # Suggest user directory on Windows if system installation failed
    USER_VST_DIR="${HOME}/.vst3"
    echo -e "${YELLOW}Installation to system directory failed.${NC}"
    echo -e "${YELLOW}Consider installing to user directory instead:${NC}"
    echo -e "${YELLOW}  mkdir -p \"${USER_VST_DIR}\"${NC}"
    echo -e "${YELLOW}  Then run this script again or copy the plugin manually.${NC}"
    echo -e "${RED}✗ Installation verification failed${NC}"
    exit 1
else
    echo -e "${RED}✗ Installation verification failed${NC}"
    exit 1
fi

# Optional: Run pluginval if available
if command -v pluginval &> /dev/null; then
    echo -e "${BLUE}pluginval found. Run validation? (y/n)${NC}"
    read -r response
    if [ "$response" = "y" ] || [ "$response" = "Y" ]; then
        echo -e "${BLUE}Running pluginval...${NC}"
        # On Windows, we might need to handle path differently
        if [ "${PLATFORM}" = "Windows" ]; then
            # Convert path for Windows if needed
            WIN_INSTALLED_VST=$(cygpath -w "${INSTALLED_VST}" 2>/dev/null || echo "${INSTALLED_VST}")
            pluginval --strictness-level 5 --validate "${WIN_INSTALLED_VST}"
        else
            pluginval --strictness-level 5 --validate "${INSTALLED_VST}"
        fi
    fi
else
    echo -e "${YELLOW}Tip: Install pluginval for automated testing:${NC}"
    if [ "${PLATFORM}" = "macOS" ]; then
        echo -e "  brew install pluginval (or download from: https://github.com/Tracktion/pluginval)${NC}"
    elif [ "${PLATFORM}" = "Linux" ]; then
        echo -e "  Download from: https://github.com/Tracktion/pluginval${NC}"
    elif [ "${PLATFORM}" = "Windows" ]; then
        echo -e "  Download from: https://github.com/Tracktion/pluginval${NC}"
    fi
fi

echo ""
echo -e "${GREEN}Done!${NC}"
