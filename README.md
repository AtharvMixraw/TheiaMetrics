# TheiaMetrics

A professional toolkit for analyzing video compression quality. Automatically compress videos at multiple bitrates, measure quality degradation using industry-standard metrics, and visualize differences with interactive heatmaps.

<video controls src="demo/demo.mp4" title="Title"></video>

## Features

### Automated Compression Testing
- Generate compressed versions at 10 different bitrates (100k-5000k)
- Parallel processing for efficiency
- Batch analysis of multiple videos

### Quality Metrics
- PSNR (Peak Signal-to-Noise Ratio)
- SSIM (Structural Similarity Index)
- Frame-by-frame quality tracking
- Statistical analysis and reporting

### Interactive Visualization
- Side-by-side video comparison
- Real-time error heatmaps
- Synchronized frame navigation
- Multiple colormap options

### Analysis & Reporting
- CSV export with detailed metrics
- Visual graphs and charts
- Optimal bitrate recommendations
- Quality-to-size efficiency analysis

## Requirements

- C++11 compiler
- CMake 3.10 or higher
- OpenCV 4.x with GUI support
- FFmpeg
- Python 3.8+ (pandas, matplotlib)

## Installation

### System Dependencies

```bash
sudo apt update
sudo apt install -y build-essential cmake git
sudo apt install -y libopencv-dev libgtk-3-dev
sudo apt install -y ffmpeg
```

### Python Dependencies

```bash
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
```

### Build

```bash
mkdir build && cd build
cmake ..
make
cd ..
```

### Setup Shortcuts

```bash
cd scripts
chmod +x *.sh *.py

# Add convenient aliases
echo "alias vqdash='~/VideoMetrics/scripts/launch_dashboard.sh'" >> ~/.bashrc
echo "alias vqcompare='~/VideoMetrics/scripts/compare.sh'" >> ~/.bashrc
source ~/.bashrc
```

## Usage

### Quick Start

```bash
# 1. Compress video at multiple bitrates
./scripts/compress.sh data/input.mp4 data/compressed

# 2. Calculate metrics for all versions
./scripts/batch_eval.sh data/input.mp4 data/compressed results/metrics.csv

# 3. Generate visual reports
./scripts/generate_report.py results/metrics.csv results/graphs

# 4. Launch interactive dashboard
vqdash
```

### Command-Line Tools

**Batch Metrics Calculator**
```bash
./build/metrics <original_video> <compressed_video>
```

**Interactive Dashboard**
```bash
./build/dashboard <original_video> <compressed_video>
```

### Launcher Scripts

**Smart Launcher (remembers last comparison)**
```bash
vqdash                          # Use last comparison
vqdash video1.mp4 video2.mp4    # Specify videos
```

**Quick Compare (select bitrate)**
```bash
vqcompare           # Show menu
vqcompare 500       # Compare with 500k version
```

**Interactive Menu**
```bash
./scripts/launch_interactive.sh
```

### Dashboard Controls

| Key | Action |
|-----|--------|
| SPACE | Play/Pause |
| RIGHT ARROW | Next frame |
| LEFT ARROW | Previous frame |
| H | Toggle heatmap overlay |
| C | Change colormap |
| Q or ESC | Quit |

## Output

### Metrics CSV

```csv
filename,bitrate_kbps,file_size_mb,psnr_db,ssim
output_100k.mp4,100,0.51,28.45,0.8234
output_500k.mp4,500,1.63,50.25,0.9972
output_1000k.mp4,1000,2.84,53.12,0.9993
```

### Visual Reports

Generated in `results/graphs/`:
- `quality_vs_bitrate.png` - PSNR and SSIM curves
- `size_vs_quality.png` - File size vs quality tradeoff
- `compression_efficiency.png` - Quality per megabyte analysis
- `summary_table.png` - Formatted results table

### Interactive Dashboard

Four synchronized windows:
- Original video display
- Compressed video display
- Difference heatmap (blue = good, red = artifacts)
- Control panel with metrics and timeline

## Project Structure

```
video-quality-dashboard/
├── CMakeLists.txt           # Build configuration
├── README.md                # This file
├── requirements.txt         # Python dependencies
├── src/                     # C++ source files
│   ├── main.cpp            # Metrics calculator
│   ├── main_dashboard.cpp  # Dashboard entry point
│   ├── metrics.cpp         # PSNR/SSIM implementation
│   ├── heatmap.cpp         # Error visualization
│   └── dashboard.cpp       # Interactive UI
├── include/                 # Header files
│   ├── metrics.h
│   ├── heatmap.h
│   └── dashboard.h
├── scripts/                 # Automation scripts
│   ├── compress.sh         # Multi-bitrate compression
│   ├── batch_eval.sh       # Batch metrics calculation
│   ├── generate_report.py  # Visual report generation
│   ├── launch_dashboard.sh # Smart launcher
│   ├── launch_interactive.sh # Menu-driven launcher
│   └── compare.sh          # Quick bitrate comparison
├── data/                    # Input videos
│   ├── input.mp4
│   └── compressed/         # Generated compressed videos
├── results/                 # Analysis output
│   ├── metrics.csv
│   └── graphs/
└── build/                   # Compiled binaries
    ├── metrics             # Command-line tool
    └── dashboard           # Interactive visualizer
```

## Examples

### Example 1: Optimize Single Video

```bash
# Compress at multiple bitrates
./scripts/compress.sh my_video.mp4 data/compressed

# Calculate metrics
./scripts/batch_eval.sh my_video.mp4 data/compressed results/metrics.csv

# Find optimal bitrate
./scripts/generate_report.py results/metrics.csv results/graphs

# Visual inspection of best option
vqcompare 500
```

### Example 2: Compare Two Specific Videos

```bash
# Direct comparison
./build/dashboard original.mp4 compressed.mp4

# Or using launcher
vqdash original.mp4 compressed.mp4
```

### Example 3: Batch Process Multiple Videos

```bash
for video in videos/*.mp4; do
    basename=$(basename "$video" .mp4)
    ./scripts/compress.sh "$video" "output/$basename"
    ./scripts/batch_eval.sh "$video" "output/$basename" "results/${basename}.csv"
done
```

## Interpreting Results

### PSNR (Peak Signal-to-Noise Ratio)

- 50+ dB: Excellent (nearly identical)
- 40-50 dB: Very good
- 30-40 dB: Good
- 20-30 dB: Acceptable
- Below 20 dB: Poor quality

### SSIM (Structural Similarity Index)

- 0.95-1.0: Excellent
- 0.90-0.95: Good
- 0.80-0.90: Fair
- Below 0.80: Poor

### Heatmap Colors

- Blue: Low error (good quality)
- Green-Yellow: Minor differences
- Orange-Red: Significant compression artifacts

## Performance

Typical processing times on mid-range hardware:

| Video Length | Frames | Compression | Metrics Calc | Dashboard Load |
|--------------|--------|-------------|--------------|----------------|
| 10 seconds   | 300    | 30s         | 5s           | 2s             |
| 30 seconds   | 900    | 90s         | 15s          | 5s             |
| 1 minute     | 1800   | 180s        | 30s          | 10s            |
| 5 minutes    | 9000   | 900s        | 150s         | 45s            |

## Troubleshooting

### Dashboard windows don't appear

Ensure X11 forwarding is enabled if using WSL:

```bash
export DISPLAY=:0
```

Windows 11 users: WSLg provides built-in GUI support.

### Slow performance

Reduce display resolution in `src/dashboard.cpp`:

```cpp
displayWidth_(480),   // Reduce from 640
displayHeight_(270),  // Reduce from 360
```

### Videos out of sync

Verify both videos have the same frame count:

```bash
ffprobe -v error -select_streams v:0 \
  -show_entries stream=nb_frames \
  -of csv=p=0 video.mp4
```

### Build errors

Check OpenCV installation:

```bash
pkg-config --modversion opencv4
pkg-config --cflags opencv4
```

## Advanced Configuration

### Custom Bitrates

Edit `scripts/compress.sh`:

```bash
BITRATES=(100 200 300 500 750 1000 1500 2000 3000 5000)
```

### Custom Colormap

Modify `src/dashboard.cpp`:

```cpp
colormapType_(cv::COLORMAP_HOT)  // Options: JET, HOT, COOL, RAINBOW
```

### Heatmap Sensitivity

Adjust threshold in `src/heatmap.cpp`:

```cpp
HeatmapGenerator::HeatmapGenerator() : threshold_(10.0) {}
```

## Contributing

Contributions are welcome. Please:

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

## License

MIT License. See LICENSE file for details.

## Acknowledgments

Built with OpenCV for computer vision and CMake for cross-platform building. Implements industry-standard PSNR and SSIM quality metrics.