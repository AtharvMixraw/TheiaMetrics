#!/bin/bash

# batch_eval.sh - Run metrics on multiple compressed videos
# Usage: ./batch_eval.sh <original_video> <compressed_directory> [output_csv]

set -e

# Detect if virtual environment should be used
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
if [ -d "$PROJECT_ROOT/venv" ] && [ -z "$VIRTUAL_ENV" ]; then
    echo "📦 Detected virtual environment, activating..."
    source "$PROJECT_ROOT/venv/bin/activate"
    VENV_ACTIVATED=1
fi

# Check arguments
if [ $# -lt 2 ]; then
    echo "Usage: $0 <original_video> <compressed_directory> [output_csv]"
    echo "Example: $0 ../data/input.mp4 ../data/compressed ../results/metrics.csv"
    exit 1
fi

ORIGINAL_VIDEO="$1"
COMPRESSED_DIR="$2"
OUTPUT_CSV="${3:-../results/metrics.csv}"

# Check if original video exists
if [ ! -f "$ORIGINAL_VIDEO" ]; then
    echo "Error: Original video '$ORIGINAL_VIDEO' not found!"
    exit 1
fi

# Check if compressed directory exists
if [ ! -d "$COMPRESSED_DIR" ]; then
    echo "Error: Compressed directory '$COMPRESSED_DIR' not found!"
    exit 1
fi

# Create results directory
RESULTS_DIR=$(dirname "$OUTPUT_CSV")
mkdir -p "$RESULTS_DIR"

# Path to metrics executable
METRICS_BIN="../build/metrics"

if [ ! -f "$METRICS_BIN" ]; then
    echo "Error: Metrics executable not found at $METRICS_BIN"
    echo "Please build the project first:"
    echo "  cd ../build && cmake .. && make"
    exit 1
fi

echo "========================================="
echo "Batch Video Quality Evaluation"
echo "========================================="
echo "Original: $ORIGINAL_VIDEO"
echo "Compressed dir: $COMPRESSED_DIR"
echo "Output CSV: $OUTPUT_CSV"
echo ""

# Write CSV header
echo "filename,bitrate_kbps,file_size_mb,psnr_db,ssim" > "$OUTPUT_CSV"

# Get original file size for comparison
ORIGINAL_SIZE=$(stat -f%z "$ORIGINAL_VIDEO" 2>/dev/null || stat -c%s "$ORIGINAL_VIDEO" 2>/dev/null)
ORIGINAL_SIZE_MB=$(echo "scale=2; $ORIGINAL_SIZE / 1024 / 1024" | bc)

echo "Original file size: ${ORIGINAL_SIZE_MB} MB"
echo ""
echo "Processing compressed videos..."
echo ""

# Counter for processed files
COUNT=0

# Process each compressed video
for compressed_video in "$COMPRESSED_DIR"/*.mp4; do
    if [ ! -f "$compressed_video" ]; then
        continue
    fi
    
    FILENAME=$(basename "$compressed_video")
    
    # Extract bitrate from filename (e.g., output_500k.mp4 -> 500)
    BITRATE=$(echo "$FILENAME" | grep -oE '[0-9]+' | head -1)
    
    if [ -z "$BITRATE" ]; then
        echo "⚠ Warning: Could not extract bitrate from $FILENAME, skipping..."
        continue
    fi
    
    # Get file size
    FILE_SIZE=$(stat -f%z "$compressed_video" 2>/dev/null || stat -c%s "$compressed_video" 2>/dev/null)
    FILE_SIZE_MB=$(echo "scale=2; $FILE_SIZE / 1024 / 1024" | bc)
    
    # Calculate compression ratio
    COMPRESSION_RATIO=$(echo "scale=2; ($ORIGINAL_SIZE - $FILE_SIZE) / $ORIGINAL_SIZE * 100" | bc)
    
    echo "----------------------------------------"
    echo "File: $FILENAME"
    echo "Bitrate: ${BITRATE}k"
    echo "Size: ${FILE_SIZE_MB} MB (${COMPRESSION_RATIO}% reduction)"
    
    # Run metrics calculation
    echo "Calculating metrics..."
    METRICS_OUTPUT=$($METRICS_BIN "$ORIGINAL_VIDEO" "$compressed_video" 2>/dev/null)
    
    # Extract PSNR and SSIM from output
    PSNR=$(echo "$METRICS_OUTPUT" | grep "Average PSNR:" | grep -oE '[0-9]+\.[0-9]+')
    SSIM=$(echo "$METRICS_OUTPUT" | grep "Average SSIM:" | grep -oE '[0-9]+\.[0-9]+')
    
    if [ -z "$PSNR" ] || [ -z "$SSIM" ]; then
        echo "✗ Error: Failed to extract metrics"
        continue
    fi
    
    echo "PSNR: ${PSNR} dB"
    echo "SSIM: ${SSIM}"
    
    # Append to CSV
    echo "$FILENAME,$BITRATE,$FILE_SIZE_MB,$PSNR,$SSIM" >> "$OUTPUT_CSV"
    
    COUNT=$((COUNT + 1))
    echo "✓ Processed successfully"
    echo ""
done

echo "========================================="
echo "Evaluation Complete!"
echo "========================================="
echo "Processed $COUNT videos"
echo "Results saved to: $OUTPUT_CSV"
echo ""
echo "Summary:"
cat "$OUTPUT_CSV"
echo ""
echo "To visualize results, you can:"
echo "  - Open $OUTPUT_CSV in Excel/LibreOffice"
echo "  - Use Python/R for plotting"
echo "  - Run generate_report.py (if available)"