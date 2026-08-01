# CSI Camera Photo Capture (Raspberry Pi + OpenCV)

C++ program that captures a photo from a CSI camera (e.g. OV5647 Camera Module) on a Raspberry Pi, using OpenCV with a GStreamer + libcamera pipeline.

## Required packages

```bash
sudo apt update
sudo apt install -y \
    g++ \
    libopencv-dev \
    rpicam-apps \
    gstreamer1.0-tools \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-libcamera
```

| Package | Purpose |
|---|---|
| `g++` / `libopencv-dev` | Compiler and OpenCV library for C++ |
| `rpicam-apps` | Official Raspberry Pi camera tools |
| `gstreamer1.0-tools` | GStreamer command-line tools |
| `gstreamer1.0-plugins-base/good/bad` | Core and extended GStreamer elements (e.g. `videoconvert`) |
| `gstreamer1.0-libcamera` | Provides the `libcamerasrc` element, the bridge between GStreamer and libcamera |

## Compilation

```bash
g++ foto.cpp -o foto $(pkg-config --cflags --libs opencv4)
```

## What the code does

```cpp
std::string pipeline =
    "libcamerasrc ! "
    "video/x-raw,width=1296,height=972,format=NV12 ! "
    "videoconvert ! "
    "video/x-raw,format=BGR ! "
    "appsink drop=true";

cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER);
```
Opens the camera through a GStreamer pipeline instead of the plain V4L2 backend:
- `libcamerasrc` — connects to the camera via libcamera (required for CSI cameras).
- `video/x-raw,width=1296,height=972,format=NV12` — sets the resolution and native color format.
- `videoconvert` + `video/x-raw,format=BGR` — converts the frame to BGR, the format OpenCV works with.
- `appsink drop=true` — hands frames to the application, dropping old ones if not consumed fast enough.

```cpp
if (!cap.isOpened()) { ... }
```
Checks that the pipeline opened successfully; exits with an error message otherwise.

```cpp
cv::Mat frame;
for (int i = 0; i < 10; i++) {
    cap >> frame;
}
cap >> frame;
```
Discards the first 10 frames to let auto-exposure and auto white balance stabilize, then captures the final frame.

```cpp
if (!frame.empty()) {
    cv::imwrite("foto_success.jpg", frame);
    ...
}
cap.release();
```
Saves the frame to disk as `foto_success.jpg` if it's valid, then releases the camera.