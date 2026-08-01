#include <opencv2/opencv.hpp> //VideoCapture, MAt, imwrite, ...
#include <iostream>

int main() {
    // Pipeline GStreamer using libcamerasrc (required for CSI cameras in Bookworm)
    std::string pipeline =
        "libcamerasrc ! "    //GStreamer plugin
        "video/x-raw,width=1296,height=972,format=NV12 ! " //video with no compression, resolution, pixels format
        "videoconvert ! " // GStreame element that converts images formats
        "video/x-raw,format=BGR ! " //output as BGR
        "appsink drop=true";   //delivers each frame to C++ program, drops old frames if program is slow

    cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER); //objc cap

    if (!cap.isOpened()) {
        std::cerr << "Error opening the camera via GStreamer/libcamera!" << std::endl;
        return -1;
    }

    std::cout << "Camera opened successfully. Waiting for stabilization..." << std::endl;

    cv::Mat frame; //Mat means Matrix, it stores images
    // Discards the first few frames (auto-exposure/AWB still adjusting)
    for (int i = 0; i < 10; i++) {
        cap >> frame;   //same cap.read(frame);
    }

    
    int fps = 30;

    cv::VideoWriter writer(   //object writer, responsible for recording videos
        "recorded_video.mp4",
        cv::VideoWriter::fourcc('m', 'p', '4', 'v'), //defines the video codec, fourcc (Four Character Code)
        fps,
        cv::Size(frame.cols, frame.rows) //resoluction (1296x972)
    );

    if (!writer.isOpened()) {
        std::cerr << "Error opening the video file to write" << std::endl;
        return -1;
    }


    std::cout << "Recording for 10 seconds..." << std::endl;

    auto inicial = std::chrono::steady_clock::now();
    int durationSecond = 10;

    while(std::chrono::duration_cast<std::chrono::seconds>(
               std::chrono::steady_clock::now() - inicial).count() < durationSecond) {

        cap >> frame;
        if(frame.empty()) {
            std::cerr << "Empty frame, skiping..." <<std::endl;
            continue;
        }
        writer.write(frame); //gets de content of frame and add into the video
    }

    std::cout << "Recording finished! Saved as 'recorded_video.mp4'." << std::endl;

    cap.release();
    writer.release();
    return 0;
}