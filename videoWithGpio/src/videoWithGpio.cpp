#include <opencv2/opencv.hpp> //VideoCapture, MAt, imwrite, ...
#include <iostream>

#include "bsp.hpp"

int main() {

    bsp_init();

    // Pipeline GStreamer using libcamerasrc (required for CSI cameras in Bookworm)
    std::string pipeline =
        "libcamerasrc ! " 
        "video/x-raw,width=1296,height=972,format=NV12 ! "
        "videoconvert ! "
        "video/x-raw,format=BGR ! "
        "appsink drop=true";

    cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER); //objc cap

    if (!cap.isOpened()) {
        std::cerr << "Error opening the camera via GStreamer/libcamera!" << std::endl;
        return -1;
    }

    std::cout << "Camera opened successfully. Waiting for stabilization..." << std::endl;

    cv::Mat frame;
    // Discards the first few frames (auto-exposure/AWB still adjusting)
    for (int i = 0; i < 10; i++) {
        cap >> frame;
    }

    
    int fps = 30;

    cv::VideoWriter writer(
        "recorded_video.mp4",
        cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
        fps,
        cv::Size(frame.cols, frame.rows)
    );

    if (!writer.isOpened()) {
        std::cerr << "Error opening the video file to write" << std::endl;
        return -1;
    }


    std::cout << "Recording for 10 seconds..." << std::endl;

    auto inicial = std::chrono::steady_clock::now();
    int durationSecond = 10;

    while(bsp_GetInputValue()) {

        cap >> frame;
        if(frame.empty()) {
            std::cerr << "Empty frame, skiping..." <<std::endl;
            continue;
        }
        writer.write(frame);
    }

    std::cout << "Recording finished! Saved as 'recorded_video.mp4'." << std::endl;

    cap.release();
    writer.release();
    return 0;
}