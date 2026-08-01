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

    // Captures the final frame
    cap >> frame;

    if (!frame.empty()) {
        cv::imwrite("foto_success.jpg", frame);
        std::cout << "Success! Photo saved as 'foto_success.jpg'." << std::endl;
        std::cout << "Resolução: " << frame.cols << "x" << frame.rows << std::endl;
    } else {
        std::cerr << "Error: The captured frame is empty.!" << std::endl;
    }

    cap.release();
    return 0;
}