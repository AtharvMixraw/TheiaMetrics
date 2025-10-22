#include <opencv2/opencv.hpp>
#include <iostream>
#include <iomanip>
#include <chrono>
#include "metrics.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
    if (argc != 3) {
        cout << "Usage: ./metrics <original_video> <compressed_video>" << endl;
        return -1;
    }

    VideoCapture refVideo(argv[1]);
    VideoCapture distVideo(argv[2]);

    if (!refVideo.isOpened() || !distVideo.isOpened()) {
        cerr << "Error: Cannot open video files." << endl;
        return -1;
    }

    // Get video properties
    int totalFrames = (int)refVideo.get(CAP_PROP_FRAME_COUNT);
    double fps = refVideo.get(CAP_PROP_FPS);
    int width = (int)refVideo.get(CAP_PROP_FRAME_WIDTH);
    int height = (int)refVideo.get(CAP_PROP_FRAME_HEIGHT);
    
    cout << "Video info:" << endl;
    cout << "  Resolution: " << width << "x" << height << endl;
    cout << "  Total frames: " << totalFrames << endl;
    cout << "  FPS: " << fps << endl;
    cout << "  Duration: " << totalFrames/fps << " seconds" << endl;
    cout << endl;

    // For very long videos or high resolution, enable sampling
    int skipFrames = 1;
    if (totalFrames > 600 || width > 1920 || height > 1080) {
        if (totalFrames > 600) {
            skipFrames = max(1, totalFrames / 300);
            cout << "Long video detected - sampling every " << skipFrames << " frames" << endl;
        }
        if (width > 1920 || height > 1080) {
            cout << "High resolution detected - processing may be slower" << endl;
        }
        cout << endl;
    }

    cout << "Processing..." << endl;

    Mat refFrame, distFrame;
    int frameCount = 0;
    int processedFrames = 0;
    double totalPSNR = 0.0;
    Scalar totalSSIM = Scalar(0, 0, 0, 0);

    auto startTime = chrono::high_resolution_clock::now();

    while (true) {
        bool refOk = refVideo.read(refFrame);
        bool distOk = distVideo.read(distFrame);
        
        if (!refOk || !distOk) break;
        
        frameCount++;
        
        // Skip frames if sampling
        if ((frameCount - 1) % skipFrames != 0) {
            continue;
        }

        if (refFrame.empty() || distFrame.empty()) {
            cerr << "Warning: Empty frame at position " << frameCount << endl;
            break;
        }

        // Resize if sizes mismatch
        if (refFrame.size() != distFrame.size()) {
            resize(distFrame, distFrame, refFrame.size());
        }

        double psnr = getPSNR(refFrame, distFrame);
        Scalar ssim = getMSSIM(refFrame, distFrame);

        totalPSNR += psnr;
        totalSSIM += ssim;
        processedFrames++;
        
        // Progress update every 30 processed frames or every 10%
        if (processedFrames % 30 == 0 || processedFrames == 1) {
            auto currentTime = chrono::high_resolution_clock::now();
            auto elapsed = chrono::duration_cast<chrono::seconds>(currentTime - startTime).count();
            double framesPerSec = processedFrames / max(1.0, (double)elapsed);
            int estimatedTotal = (totalFrames + skipFrames - 1) / skipFrames;
            int remaining = max(0, int((estimatedTotal - processedFrames) / max(0.1, framesPerSec)));
            
            double progress = (double)frameCount / totalFrames * 100.0;
            cout << "\r  Frame " << frameCount << "/" << totalFrames 
                 << " (" << fixed << setprecision(1) << progress << "%)"
                 << " | Processed: " << processedFrames
                 << " | " << setprecision(1) << framesPerSec << " fps"
                 << " | Current PSNR: " << setprecision(2) << psnr << " dB"
                 << " | ETA: " << remaining << "s      " << flush;
        }
    }

    cout << endl << endl;

    refVideo.release();
    distVideo.release();

    if (processedFrames == 0) {
        cerr << "Error: No frames were processed!" << endl;
        return -1;
    }

    double avgPSNR = totalPSNR / processedFrames;
    double avgSSIM = (totalSSIM[0] + totalSSIM[1] + totalSSIM[2]) / (3 * processedFrames);

    // Output results in the format expected by batch_eval.sh
    cout << "Results:" << endl;
    cout << "  Frames processed: " << processedFrames << " of " << totalFrames << endl;
    cout << "  Average PSNR: " << fixed << setprecision(2) << avgPSNR << " dB" << endl;
    cout << "  Average SSIM: " << fixed << setprecision(4) << avgSSIM << endl;

    return 0;
}