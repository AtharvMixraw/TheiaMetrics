# Video Quality Dashboard

A comprehensive toolkit for analyzing video compression quality. Automatically compress videos at multiple bitrates and measure quality degradation using industry-standard metrics.

## Features

- **Automated Compression**: Generate compressed versions at 10 different bitrates (100k-5000k)
- **Quality Metrics**: Calculate PSNR and SSIM for each compressed video
- **Visual Analytics**: Generate graphs showing quality vs bitrate tradeoffs
- **Optimal Bitrate**: Automatically recommend best compression settings
- **Batch Processing**: Analyze multiple videos with a single command
- **CSV Export**: Export all metrics for further analysis

## Quick Start

```bash
# Setup
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt

# Build
mkdir build && cd build
cmake ..
make
cd ..

# Run
./scripts/run_pipeline.sh data/input.mp4
```

## Output

- **CSV file** with bitrate, file size, PSNR, and SSIM for each compression
- **4 visualization graphs**: quality vs bitrate, size vs quality, efficiency analysis, summary table
- **Optimal bitrate recommendation** based on quality-to-size ratio

## Requirements

- C++11 compiler
- CMake 3.10+
- OpenCV 4.x
- FFmpeg
- Python 3.8+ (pandas, matplotlib)

## Project Structure

```
├── src/              # C++ source files
├── include/          # Header files
├── scripts/          # Automation scripts
├── data/             # Input videos
├── results/          # Output metrics and graphs
└── build/            # Compiled binaries
```

## License

MIT