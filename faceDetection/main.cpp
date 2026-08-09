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
    }

    //Load classifier of pre-trained face
    cv::CascadeClassifier faceCascade;
    std::string cascadePath = "/usr/share/opencv4/haarcascades/haarcascade_frontalface_default.xml";

    if(!faceCascade.load(cascadePath)) {
        std::cerr << "Error loading classifier Haar Cascade!" << std::endl;
        return -1;
    }
    cv::Mat frame, gray;
    cv::Mat displayFrame;

    //Stabilizes exposure before starting
    for(int i = 0; i < 10; i++) {
        cap >> frame;
    }

    std::cout << "Detecting faces... (Ctrl + c to exit)" << std::endl;

    while(true) {
        cap >> frame;
        if(frame.empty()) {
            std::cerr << "Empty frame, skipping ..." << std::endl;
            continue;
        }

        //Convert to grayscale (improve detection performance and precision)
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        //improve contrast
        cv::equalizeHist(gray, gray);

        // Dynamic list to store bounding box rectangles (x, y, width, height) of detected faces
        std::vector<cv::Rect> faces;

        faceCascade.detectMultiScale(
            gray,
            faces,
            1.1, //scaleFactor: how much the image is reduced at each scale
            5,   //minNeighbors: how many neighboring detections confirm a face
            0,
            cv::Size(60, 60) //minimum size to detect face (pixels)
        );

        // Draws a rectangle around each detected face.
        for(const auto& face : faces) {
            cv::rectangle(frame, face, cv::Scalar(0, 255, 0), 2);
        }

        //resize to display the camera view
        cv::resize(frame, displayFrame, cv::Size(), 0.5, 0.5);
        cv::imshow("Faces Detection", displayFrame);

        if (cv::waitKey(1) == 'q') {
            break;
        }

        std::cout << "Detected faces: " << faces.size() << std::endl;

        if(faces.size()) {
            cv::imwrite("last_detected_frame.jpg", frame);
        }
    }

    cap.release();
    return 0;
}