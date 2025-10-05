#!/bin/bash

# compare.sh - Quick compare with specific bitrate
# Usage: ./compare.sh [bitrate]
# Example: ./compare.sh 500

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

if [ ! -f "build/dashboard" ]; then
    echo "Error: Dashboard not built"
    exit 1
fi

ORIGINAL="data/input.mp4"

if [ ! -f "$ORIGINAL" ]; then
    echo "Error: No input video at data/input.mp4"
    exit 1
fi

# If bitrate provided, use it
if [ $# -eq 1 ]; then
    BITRATE="$1"
    COMPRESSED="data/compressed/output_${BITRATE}k.mp4"
    
    if [ ! -f "$COMPRESSED" ]; then
        echo "Error: Compressed video not found: $COMPRESSED"
        echo ""
        echo "Available bitrates:"
        ls data/compressed/*.mp4 2>/dev/null | sed 's/.*output_//;s/k.mp4//' | sort -n
        exit 1
    fi
else
    # Show menu of available bitrates
    echo "Available compressed versions:"
    echo "------------------------------"
    
    if [ ! -d "data/compressed" ] || [ -z "$(ls data/compressed/*.mp4 2>/dev/null)" ]; then
        echo "No compressed videos found."
        echo "Run: ./scripts/compress.sh data/input.mp4 data/compressed"
        exit 1
    fi
    
    # Extract bitrates and sort
    BITRATES=($(ls data/compressed/output_*k.mp4 2>/dev/null | sed 's/.*output_//;s/k.mp4//' | sort -n))
    
    echo ""
    for i in "${!BITRATES[@]}"; do
        FILE="data/compressed/output_${BITRATES[$i]}k.mp4"
        SIZE=$(du -h "$FILE" | cut -f1)
        echo "$((i+1)). ${BITRATES[$i]}k ($SIZE)"
    done
    echo ""
    
    read -p "Select bitrate (1-${#BITRATES[@]}): " CHOICE
    
    if [ "$CHOICE" -ge 1 ] && [ "$CHOICE" -le "${#BITRATES[@]}" ]; then
        BITRATE="${BITRATES[$((CHOICE-1))]}"
        COMPRESSED="data/compressed/output_${BITRATE}k.mp4"
    else
        echo "Invalid choice"
        exit 1
    fi
fi

echo ""
echo "Comparing: Original vs ${BITRATE}k"
echo ""

./build/dashboard "$ORIGINAL" "$COMPRESSED"