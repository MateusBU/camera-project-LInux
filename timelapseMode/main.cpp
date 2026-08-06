/* ===========================
*          INCLUDES
* =========================== */

#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
#include <thread>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <cstdlib>

/* ===========================
 *     LOCAL VARIABLES
 * =========================== */
namespace fs = std::filesystem; //shortcut for filesystem (manipulate files and directories)

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

    cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER); //cv = OpenCV library namespace

    if(!cap.isOpened()) {
        std::cerr <<"Error opening the camera" << std::endl;
        return -1;
    }

    std::string outputDir = "timelapse_frames";
    fs::create_directory(outputDir);

    int intervalSeconds = 10;
    int totalPhotos = 24;

    cv::Mat frame;

    //discards 10 frames
    for(int i = 0; i < 10; i++) {
        cap >> frame;
    }

    for(int i = 0; i < totalPhotos; i++) {
        cap >> frame;

        if(frame.empty()) {
            std::cerr << "empty frame, skiping capture" << i << std::endl;
        }
        else {
            std::ostringstream filename;
            // Creates the file path/name in memory
            filename << outputDir << "/frame_" << std::setw(4) << std::setfill('0') << i << ".jpg";
        
            // OpenCV creates the .jpg file on disk
            cv::imwrite(filename.str(), frame);

            // Displays this message on the terminal
            std::cout << "Foto" << (i + 1) << "/" << totalPhotos << " save: " << filename.str() << std::endl;
        }

        if(i < totalPhotos - 1) {
            std::this_thread::sleep_for(std::chrono::seconds(intervalSeconds));
        }
    }

    cap.release();

    std::cout << "Capture complete! Compiling video with ffmeg..." << std::endl;

    std::string ffmpegCommand = 
        "ffmpeg -y -framerate 10 -i " + outputDir + "/frame_%04d.jpg "
        "-c:v libx264 -pix_fmt yuv420p timelapse.mp4";

    int result = system(ffmpegCommand.c_str());

    if(result == 0) {
        std::cout << "Timelapse compiled with success: timelapse.mp4" << std::endl;
    }
    else {
        std::cerr << "Error compiling video with ffmpeg." << std::endl; 
    }

    return 0;
}