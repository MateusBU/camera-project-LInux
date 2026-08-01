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

## Recording video

The same `cap` object (opened with the GStreamer/libcamera pipeline) can be used to record a video instead of a single photo. Note that CSI cameras have no microphone, so this produces video only, with no audio.

```cpp
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
```
Creates a `cv::VideoWriter` that will encode frames into `recorded_video.mp4` using the `mp4v` (MPEG-4) codec, at the given `fps` and with the same resolution as the captured frames. If the codec isn't available on the system, `writer.isOpened()` returns `false` and the program exits with an error — in that case, try the `MJPG` codec with a `.avi` file instead.

```cpp
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
    writer.write(frame);
}
```
Records for a fixed duration (10 seconds here) instead of a fixed number of frames, using `std::chrono` to track elapsed time. On each loop iteration it grabs a new frame from the camera and writes it to the video file; empty frames are skipped instead of being written, to avoid corrupting the output.

```cpp
std::cout << "Recording finished! Saved as 'recorded_video.mp4'." << std::endl;

cap.release();
writer.release();
```
Releases both the camera and the video writer once recording is done, flushing and finalizing the `.mp4` file.

## Stopping the recording with a button press (GPIO)
 
Instead of recording for a fixed duration, the loop condition can also check a digital input (e.g. a push button wired to `GPIO_BUTTON_1`) using the `bsp` module. The recording stops as soon as the button is pressed.
 
```cpp
bsp_init();
 
std::cout << "Recording... press the button to stop." << std::endl;
 
while (bsp_GetInputValue() != 0) {
    cap >> frame;
    if (frame.empty()) {
        std::cerr << "Empty frame, skiping..." << std::endl;
        continue;
    }
    writer.write(frame);
}
 
std::cout << "Button pressed! Recording finished. Saved as 'recorded_video.mp4'." << std::endl;
 
cap.release();
writer.release();
bsp_closeGPIOs();
```
 
- **`bsp_init()`** — configures the GPIO chip, requesting the button line as a digital input (with pull-up enabled), as covered in the `bsp` module.
- **`bsp_GetInputValue()`** — reads the current state of the button line on every loop iteration. With a pull-up resistor, the line reads `1` while the button is *not* pressed, and drops to `0` when it *is* pressed (assuming the button connects the pin to ground).
- **Loop condition `!= 0`** — keeps recording frame by frame as long as the button hasn't been pressed; the loop exits naturally the moment the button pulls the line low.
- **`bsp_closeGPIOs()`** — releases the GPIO lines and closes the chip once recording stops, alongside releasing the camera and video writer.
This replaces the fixed `durationSecond` timer from the previous section — recording length now depends entirely on when the button is pressed, rather than a hardcoded time value.
 
