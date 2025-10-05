#!/bin/bash

# launch_dashboard.sh - Smart dashboard launcher with memory
# Usage: ./launch_dashboard.sh [original] [compressed]

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

CONFIG_FILE=".dashboard_config"

# Function to save config
save_config() {
    echo "LAST_ORIGINAL=$1" > "$CONFIG_FILE"
    echo "LAST_COMPRESSED=$2" >> "$CONFIG_FILE"
}

# Function to load config
load_config() {
    if [ -f "$CONFIG_FILE" ]; then
        source "$CONFIG_FILE"
    fi
}

# Check if dashboard exists
if [ ! -f "build/dashboard" ]; then
    echo "Error: Dashboard not built"
    echo "Run: cd build && cmake .. && make"
    exit 1
fi

# Determine videos to use
if [ $# -eq 2 ]; then
    # Arguments provided
    ORIGINAL="$1"
    COMPRESSED="$2"
elif [ $# -eq 0 ]; then
    # No arguments, try to use last used or defaults
    load_config
    
    if [ -n "$LAST_ORIGINAL" ] && [ -f "$LAST_ORIGINAL" ] && \
       [ -n "$LAST_COMPRESSED" ] && [ -f "$LAST_COMPRESSED" ]; then
        # Use last used videos
        ORIGINAL="$LAST_ORIGINAL"
        COMPRESSED="$LAST_COMPRESSED"
        echo "Using last comparison:"
        echo "  Original:   $ORIGINAL"
        echo "  Compressed: $COMPRESSED"
        echo ""
        read -p "Continue? (Y/n) " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Nn]$ ]]; then
            echo "Cancelled. Usage: $0 <original> <compressed>"
            exit 0
        fi
    else
        # Try defaults
        if [ -f "data/input.mp4" ]; then
            ORIGINAL="data/input.mp4"
            
            # Find newest compressed video
            if [ -d "data/compressed" ]; then
                COMPRESSED=$(ls -t data/compressed/*.mp4 2>/dev/null | head -1)
            fi
            
            if [ -z "$COMPRESSED" ]; then
                echo "No compressed videos found."
                echo "Usage: $0 <original> <compressed>"
                exit 1
            fi
            
            echo "Using default videos:"
            echo "  Original:   $ORIGINAL"
            echo "  Compressed: $COMPRESSED"
            echo ""
        else
            echo "Usage: $0 <original> <compressed>"
            exit 1
        fi
    fi
else
    echo "Usage: $0 [original] [compressed]"
    echo ""
    echo "With no arguments, uses last comparison or defaults"
    exit 1
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

# Save for next time
save_config "$ORIGINAL" "$COMPRESSED"

# Get file info
ORIG_SIZE=$(du -h "$ORIGINAL" | cut -f1)
COMP_SIZE=$(du -h "$COMPRESSED" | cut -f1)

echo "Starting Video Quality Dashboard..."
echo "Original:   $ORIGINAL ($ORIG_SIZE)"
echo "Compressed: $COMPRESSED ($COMP_SIZE)"
echo ""

# Launch dashboard
./build/dashboard "$ORIGINAL" "$COMPRESSED"