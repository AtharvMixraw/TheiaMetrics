#include <opencv2/opencv.hpp>
#include <iostream>
#include <iomanip>
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
        cout << "Error: Cannot open video files." << endl;
        return -1;
    }

    // Get total frame count for progress tracking
    int totalFrames = (int)refVideo.get(CAP_PROP_FRAME_COUNT);
    double fps = refVideo.get(CAP_PROP_FPS);
    
    cout << "Video info:" << endl;
    cout << "  Total frames: " << totalFrames << endl;
    cout << "  FPS: " << fps << endl;
    cout << "  Duration: " << totalFrames/fps << " seconds" << endl;
    cout << "\nProcessing..." << endl;

    Mat refFrame, distFrame;
    int frameCount = 0;
    double totalPSNR = 0.0;
    Scalar totalSSIM = Scalar(0,0,0,0);

    while (true) {
        bool refOk = refVideo.read(refFrame);
        bool distOk = distVideo.read(distFrame);
        if (!refOk || !distOk) break;

        // resize if sizes mismatch
        if (refFrame.size() != distFrame.size())
            resize(distFrame, distFrame, refFrame.size());

        double psnr = getPSNR(refFrame, distFrame);
        Scalar ssim = getMSSIM(refFrame, distFrame);

        totalPSNR += psnr;
        totalSSIM += ssim;
        frameCount++;
        
        // Progress update every 1% or 100 frames
        if (frameCount % 100 == 0) {
            double progress = (double)frameCount / totalFrames * 100.0;
            cout << "\rProgress: " << frameCount << "/" << totalFrames 
                 << " frames (" << fixed << setprecision(1) << progress << "%)  "
                 << "Current PSNR: " << setprecision(2) << psnr << " dB"
                 << flush;
        }
    }

    double avgPSNR = totalPSNR / frameCount;
    double avgSSIM = (totalSSIM[0] + totalSSIM[1] + totalSSIM[2]) / (3 * frameCount);

    cout << fixed << setprecision(4);
    cout << avgPSNR << "," << avgSSIM << endl;


    return 0;
}