#!/usr/bin/env python3
"""
generate_report.py - Create visual reports from metrics CSV
Usage: python3 generate_report.py <metrics.csv> [output_directory]
"""

import sys
import os
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib
matplotlib.use('Agg')  # Non-interactive backend

def load_data(csv_path):
    """Load metrics CSV file"""
    try:
        df = pd.read_csv(csv_path)
        print(f"✓ Loaded {len(df)} records from {csv_path}")
        return df
    except Exception as e:
        print(f"✗ Error loading CSV: {e}")
        sys.exit(1)

def create_quality_vs_bitrate_plot(df, output_dir):
    """Plot PSNR and SSIM vs Bitrate"""
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5))
    
    # PSNR vs Bitrate
    ax1.plot(df['bitrate_kbps'], df['psnr_db'], 'o-', linewidth=2, markersize=8, color='#2E86AB')
    ax1.set_xlabel('Bitrate (kbps)', fontsize=12)
    ax1.set_ylabel('PSNR (dB)', fontsize=12)
    ax1.set_title('Video Quality vs Bitrate (PSNR)', fontsize=14, fontweight='bold')
    ax1.grid(True, alpha=0.3)
    ax1.axhline(y=40, color='green', linestyle='--', alpha=0.5, label='Very Good (40 dB)')
    ax1.axhline(y=30, color='orange', linestyle='--', alpha=0.5, label='Acceptable (30 dB)')
    ax1.legend()
    
    # SSIM vs Bitrate
    ax2.plot(df['bitrate_kbps'], df['ssim'], 'o-', linewidth=2, markersize=8, color='#A23B72')
    ax2.set_xlabel('Bitrate (kbps)', fontsize=12)
    ax2.set_ylabel('SSIM', fontsize=12)
    ax2.set_title('Video Quality vs Bitrate (SSIM)', fontsize=14, fontweight='bold')
    ax2.grid(True, alpha=0.3)
    ax2.axhline(y=0.95, color='green', linestyle='--', alpha=0.5, label='Excellent (0.95)')
    ax2.axhline(y=0.90, color='orange', linestyle='--', alpha=0.5, label='Good (0.90)')
    ax2.legend()
    
    plt.tight_layout()
    output_path = os.path.join(output_dir, 'quality_vs_bitrate.png')
    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    print(f"✓ Saved: {output_path}")
    plt.close()

def create_size_vs_quality_plot(df, output_dir):
    """Plot File Size vs Quality metrics"""
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5))
    
    # File Size vs PSNR
    ax1.plot(df['file_size_mb'], df['psnr_db'], 'o-', linewidth=2, markersize=8, color='#F18F01')
    ax1.set_xlabel('File Size (MB)', fontsize=12)
    ax1.set_ylabel('PSNR (dB)', fontsize=12)
    ax1.set_title('Quality vs File Size (PSNR)', fontsize=14, fontweight='bold')
    ax1.grid(True, alpha=0.3)
    
    # File Size vs SSIM
    ax2.plot(df['file_size_mb'], df['ssim'], 'o-', linewidth=2, markersize=8, color='#C73E1D')
    ax2.set_xlabel('File Size (MB)', fontsize=12)
    ax2.set_ylabel('SSIM', fontsize=12)
    ax2.set_title('Quality vs File Size (SSIM)', fontsize=14, fontweight='bold')
    ax2.grid(True, alpha=0.3)
    
    plt.tight_layout()
    output_path = os.path.join(output_dir, 'size_vs_quality.png')
    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    print(f"✓ Saved: {output_path}")
    plt.close()

def create_efficiency_plot(df, output_dir):
    """Plot compression efficiency (Quality per MB)"""
    # Calculate efficiency metrics
    df['psnr_per_mb'] = df['psnr_db'] / df['file_size_mb']
    df['ssim_per_mb'] = df['ssim'] / df['file_size_mb']
    
    fig, ax = plt.subplots(figsize=(10, 6))
    
    # Dual y-axis plot
    ax1 = ax
    ax2 = ax.twinx()
    
    line1 = ax1.plot(df['bitrate_kbps'], df['psnr_per_mb'], 'o-', 
                     linewidth=2, markersize=8, color='#2E86AB', label='PSNR/MB')
    ax1.set_xlabel('Bitrate (kbps)', fontsize=12)
    ax1.set_ylabel('PSNR per MB', fontsize=12, color='#2E86AB')
    ax1.tick_params(axis='y', labelcolor='#2E86AB')
    
    line2 = ax2.plot(df['bitrate_kbps'], df['ssim_per_mb'], 's-', 
                     linewidth=2, markersize=8, color='#A23B72', label='SSIM/MB')
    ax2.set_ylabel('SSIM per MB', fontsize=12, color='#A23B72')
    ax2.tick_params(axis='y', labelcolor='#A23B72')
    
    ax1.set_title('Compression Efficiency (Quality per MB)', fontsize=14, fontweight='bold')
    ax1.grid(True, alpha=0.3)
    
    # Combined legend
    lines = line1 + line2
    labels = [line.get_label() for line in lines]
    ax1.legend(lines, labels, loc='best')
    
    plt.tight_layout()
    output_path = os.path.join(output_dir, 'compression_efficiency.png')
    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    print(f"✓ Saved: {output_path}")
    plt.close()

def create_summary_table(df, output_dir):
    """Create a summary table image"""
    fig, ax = plt.subplots(figsize=(12, len(df) * 0.5 + 1))
    ax.axis('tight')
    ax.axis('off')
    
    # Prepare table data
    table_data = df[['bitrate_kbps', 'file_size_mb', 'psnr_db', 'ssim']].copy()
    table_data.columns = ['Bitrate (kbps)', 'Size (MB)', 'PSNR (dB)', 'SSIM']
    table_data = table_data.round(2)
    
    # Add quality rating column
    def get_rating(row):
        if row['PSNR (dB)'] >= 40 and row['SSIM'] >= 0.95:
            return '⭐⭐⭐⭐⭐'
        elif row['PSNR (dB)'] >= 35 and row['SSIM'] >= 0.90:
            return '⭐⭐⭐⭐'
        elif row['PSNR (dB)'] >= 30 and row['SSIM'] >= 0.85:
            return '⭐⭐⭐'
        elif row['PSNR (dB)'] >= 25 and row['SSIM'] >= 0.80:
            return '⭐⭐'
        else:
            return '⭐'
    
    table_data['Quality'] = table_data.apply(get_rating, axis=1)
    
    # Create table
    table = ax.table(cellText=table_data.values,
                     colLabels=table_data.columns,
                     cellLoc='center',
                     loc='center',
                     colWidths=[0.15, 0.15, 0.15, 0.15, 0.15])
    
    table.auto_set_font_size(False)
    table.set_fontsize(10)
    table.scale(1, 2)
    
    # Style header
    for i in range(len(table_data.columns)):
        table[(0, i)].set_facecolor('#2E86AB')
        table[(0, i)].set_text_props(weight='bold', color='white')
    
    # Alternate row colors
    for i in range(1, len(table_data) + 1):
        for j in range(len(table_data.columns)):
            if i % 2 == 0:
                table[(i, j)].set_facecolor('#E8F4F8')
    
    plt.title('Video Quality Metrics Summary', fontsize=16, fontweight='bold', pad=20)
    
    output_path = os.path.join(output_dir, 'summary_table.png')
    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    print(f"✓ Saved: {output_path}")
    plt.close()

def find_optimal_bitrate(df):
    """Find the optimal bitrate (best quality-to-size ratio)"""
    # Weight SSIM more heavily as it correlates better with human perception
    df['quality_score'] = (df['psnr_db'] / 50) * 0.4 + df['ssim'] * 0.6
    df['efficiency'] = df['quality_score'] / df['file_size_mb']
    
    optimal_idx = df['efficiency'].idxmax()
    optimal = df.loc[optimal_idx]
    
    print("\n" + "="*50)
    print("OPTIMAL BITRATE RECOMMENDATION")
    print("="*50)
    print(f"Bitrate: {optimal['bitrate_kbps']} kbps")
    print(f"File Size: {optimal['file_size_mb']:.2f} MB")
    print(f"PSNR: {optimal['psnr_db']:.2f} dB")
    print(f"SSIM: {optimal['ssim']:.4f}")
    print(f"Efficiency Score: {optimal['efficiency']:.4f}")
    print("="*50)
    
    return optimal

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 generate_report.py <metrics.csv> [output_directory]")
        sys.exit(1)
    
    csv_path = sys.argv[1]
    output_dir = sys.argv[2] if len(sys.argv) > 2 else '../results/graphs'
    
    # Create output directory
    os.makedirs(output_dir, exist_ok=True)
    
    print("="*50)
    print("VIDEO QUALITY REPORT GENERATOR")
    print("="*50)
    
    # Load data
    df = load_data(csv_path)
    
    # Sort by bitrate
    df = df.sort_values('bitrate_kbps')
    
    print("\nGenerating visualizations...")
    
    # Generate plots
    create_quality_vs_bitrate_plot(df, output_dir)
    create_size_vs_quality_plot(df, output_dir)
    create_efficiency_plot(df, output_dir)
    create_summary_table(df, output_dir)
    
    # Find optimal bitrate
    find_optimal_bitrate(df)
    
    print("\n✓ All reports generated successfully!")
    print(f"✓ Output directory: {output_dir}")
    print("\nView the generated graphs:")
    for filename in os.listdir(output_dir):
        if filename.endswith('.png'):
            print(f"  - {os.path.join(output_dir, filename)}")

if __name__ == '__main__':
    main()