#!/bin/bash

# launch_interactive.sh - Interactive dashboard launcher
# Usage: ./launch_interactive.sh

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

echo "========================================="
echo "Video Quality Dashboard Launcher"
echo "========================================="
echo ""

# Check if dashboard exists
if [ ! -f "build/dashboard" ]; then
    echo "Error: Dashboard not built"
    echo "Run: cd build && cmake .. && make"
    exit 1
fi

# Find input video
if [ ! -f "data/input.mp4" ]; then
    echo "No input video found at data/input.mp4"
    read -p "Enter path to original video: " ORIGINAL
else
    ORIGINAL="data/input.mp4"
    echo "Original video: $ORIGINAL"
fi

# List compressed videos
echo ""
echo "Available compressed videos:"
echo "----------------------------"

if [ -d "data/compressed" ] && [ "$(ls -A data/compressed/*.mp4 2>/dev/null)" ]; then
    select COMPRESSED in data/compressed/*.mp4 "Enter custom path" "Quit"; do
        case $COMPRESSED in
            "Enter custom path")
                read -p "Enter path to compressed video: " COMPRESSED
                break
                ;;
            "Quit")
                echo "Cancelled."
                exit 0
                ;;
            *)
                if [ -n "$COMPRESSED" ]; then
                    break
                fi
                ;;
        esac
    done
else
    echo "No compressed videos found in data/compressed/"
    read -p "Enter path to compressed video: " COMPRESSED
fi

# Verify files exist
if [ ! -f "$ORIGINAL" ]; then
    echo "Error: Original video not found: $ORIGINAL"
    exit 1
fi

if [ ! -f "$COMPRESSED" ]; then
    echo "Error: Compressed video not found: $COMPRESSED"
    exit 1
fi

# Get file info
ORIG_SIZE=$(du -h "$ORIGINAL" | cut -f1)
COMP_SIZE=$(du -h "$COMPRESSED" | cut -f1)

echo ""
echo "========================================="
echo "Starting Dashboard"
echo "========================================="
echo "Original:   $ORIGINAL ($ORIG_SIZE)"
echo "Compressed: $COMPRESSED ($COMP_SIZE)"
echo ""
echo "Controls:"
echo "  SPACE: Play/Pause"
echo "  Arrow Keys: Navigate frames"
echo "  H: Toggle heatmap"
echo "  C: Change colormap"
echo "  Q/ESC: Quit"
echo ""

# Launch dashboard
./build/dashboard "$ORIGINAL" "$COMPRESSED"