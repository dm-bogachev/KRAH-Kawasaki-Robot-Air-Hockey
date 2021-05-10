#ifndef FRAME_H
#define FRAME_H

#include <QDebug>
#include <QSize>

#include <opencv2/videoio.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/cudawarping.hpp>

#include <settings.h>

#define GPU_ACCELERATION programSettings.GPUAcceleration

class FrameGrabber
{
private:
    cv::VideoCapture videoCapture;
    bool cameraOpened;

    void rotateMatByAngleCPU(cv::Mat &frame, Settings programSettings);
    void rotateMatByAngleGPU(cv::cuda::GpuMat &gpuMat, Settings programSettings);
    void rotateMatByAngle(cv::Mat &mat, Settings programSettings);

public:
    FrameGrabber(int cameraAddress, QSize cameraResolution);
    FrameGrabber(Settings programSettings);
    void grab();
    void warp(Settings programSettings);
    void crop(Settings programSettings);
    cv::Mat frame;
    cv::Mat pureFrame;
    int frameWidth;
    int frameHeight;
    //cv::Mat warpedFrame;
    bool isOpened();
};

#endif // FRAME_H
