/* ===========================
*          INCLUDES
* =========================== */

#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>

/* ===========================
 *     LOCAL VARIABLES
 * =========================== */

/* ===========================
 *   GLOBAL FUNCTIONS
 * =========================== */

int main() {
    std::string pipeline = 
        "libcamerasrc ! "
        "video/x-raw,width=1296,height=972,format=NV12 ! "
        "videoconvert ! "
        "video/x-raw,format=BGR ! "
        "appsink drop=true";

    cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER);

    if(!cap.isOpened()) {
        std::cerr << "Error opening camera" << std::endl;
        return -1;
    }

    cv::QRCodeDetector qrDetector;
    cv::Mat frame;
    cv::Mat displayFrame;

    //Stabilizes exposure before starting
    for(int i = 0; i < 10; i++) {
        cap >> frame;
    }

    std::cout << "Detecting QRCode... (press 'q' to exit)" << std::endl;

    while(true) {
        cap >> frame;
        if(frame.empty()) {
            std::cerr << "Empty frame, skipping ..." << std::endl;
            continue;
        }

        //Read QRCode data
        std::string data = qrDetector.detectAndDecode(frame);
        if (!data.empty()) {
            std::cout << "QR Code detected: " << data << std::endl;
        }

        cv::resize(frame, displayFrame, cv::Size(), 0.5, 0.5);
        cv::imshow("Camera", displayFrame);
        if (cv::waitKey(1) == 'q') {
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}