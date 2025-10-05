#!/bin/bash

# compress.sh - Generate compressed versions of video at multiple bitrates
# Usage: ./compress.sh <input_video> [output_directory]

set -e  # Exit on error

# Check if input file is provided
if [ $# -lt 1 ]; then
    echo "Usage: $0 <input_video> [output_directory]"
    echo "Example: $0 ../data/input.mp4 ../data/compressed"
    exit 1
fi

INPUT_VIDEO="$1"
OUTPUT_DIR="${2:-../data/compressed}"

# Check if input file exists
if [ ! -f "$INPUT_VIDEO" ]; then
    echo "Error: Input file '$INPUT_VIDEO' not found!"
    exit 1
fi

# Create output directory if it doesn't exist
mkdir -p "$OUTPUT_DIR"

# Define bitrates to test (in kbps)
BITRATES=(100 200 300 500 750 1000 1500 2000 3000 5000)

echo "========================================="
echo "Video Compression Pipeline"
echo "========================================="
echo "Input: $INPUT_VIDEO"
echo "Output directory: $OUTPUT_DIR"
echo "Bitrates to test: ${BITRATES[@]} kbps"
echo ""

# Get input video info
echo "Analyzing input video..."
ffprobe -v error -select_streams v:0 \
    -show_entries stream=width,height,r_frame_rate,bit_rate,duration \
    -of default=noprint_wrappers=1 "$INPUT_VIDEO" 2>/dev/null || true

echo ""
echo "Starting compression..."
echo ""

# Loop through each bitrate
for bitrate in "${BITRATES[@]}"; do
    OUTPUT_FILE="$OUTPUT_DIR/output_${bitrate}k.mp4"
    
    echo "----------------------------------------"
    echo "Compressing at ${bitrate}k bitrate..."
    echo "Output: $OUTPUT_FILE"
    
    # Compress video with H.264 codec
    # -c:v libx264: Use H.264 codec
    # -b:v: Target video bitrate
    # -preset medium: Balance between speed and compression
    # -movflags +faststart: Optimize for streaming
    # -c:a aac -b:a 128k: Audio codec and bitrate
    ffmpeg -i "$INPUT_VIDEO" \
        -c:v libx264 \
        -b:v "${bitrate}k" \
        -preset medium \
        -movflags +faststart \
        -c:a aac \
        -b:a 128k \
        -y \
        "$OUTPUT_FILE" \
        2>&1 | grep -E "frame=|size=|time=|bitrate=" || true
    
    # Get output file size
    if [ -f "$OUTPUT_FILE" ]; then
        SIZE=$(du -h "$OUTPUT_FILE" | cut -f1)
        echo "✓ Complete! File size: $SIZE"
    else
        echo "✗ Failed to create $OUTPUT_FILE"
    fi
    echo ""
done

echo "========================================="
echo "Compression complete!"
echo "========================================="
echo "Generated files in: $OUTPUT_DIR"
ls -lh "$OUTPUT_DIR"/*.mp4 2>/dev/null || echo "No files found"
echo ""
echo "Next step: Run batch_eval.sh to calculate metrics"