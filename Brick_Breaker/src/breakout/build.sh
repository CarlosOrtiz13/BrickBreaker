#!/bin/bash
#
# build.sh - Compile all Breakout game files
#
# This script compiles each .c file in the breakout module into .o object files
# Usage: ./build.sh from the breakout directory
#

# Color output for better readability
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo "========================================="
echo "Building Brick Breaker Ultimate"
echo "========================================="

# Compiler settings (adjust if needed)
CC="i686-elf-gcc"
CFLAGS="-I../../ -I../keyboard -I../graphics -I../timer -I../io -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -fno-builtin -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Wno-unused-function -Wno-unused-label -Wno-unused-parameter -Wno-unused-variable -Wno-cpp -Wno-implicit-function-declaration -std=gnu99"

# Output directory
BUILD_DIR="../../build/breakout"

# Create build directory if it doesn't exist
mkdir -p $BUILD_DIR

# List of source files to compile
FILES=(
    "breakout_audio.c"
    "breakout_graphics.c"
    "breakout_main.c"
    "breakout_menu.c"      # Title screen with player selection
    "breakout_particles.c"
    "breakout_physics.c"
    "breakout_powerups.c"
    "breakout_ui.c"
)

# Track if any compilation failed
ERRORS=0

# Compile each file
for file in "${FILES[@]}"; do
    echo -n "Compiling $file... "
    
    # Get output filename (replace .c with .o)
    output="${BUILD_DIR}/${file%.c}.o"
    
    # Compile
    if $CC $CFLAGS -c $file -o $output 2>&1 | grep -i "error"; then
        echo -e "${RED}FAILED${NC}"
        ERRORS=$((ERRORS + 1))
    else
        echo -e "${GREEN}OK${NC}"
    fi
done

# Summary
echo "========================================="
if [ $ERRORS -eq 0 ]; then
    echo -e "${GREEN}Build successful! All files compiled.${NC}"
    echo "Object files are in: $BUILD_DIR"
else
    echo -e "${RED}Build failed with $ERRORS error(s)${NC}"
    exit 1
fi
echo "========================================="