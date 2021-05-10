#include "framegrabber.h"

bool FrameGrabber::isOpened()
{
    return cameraOpened;
}

void FrameGrabber::rotateMatByAngleCPU(cv::Mat &frame, Settings programSettings)
{
    // Get rotation matrix for rotating the image around its center in pixel coordinates
    cv::Point2f center((frame.cols-1)/2.0, (frame.rows-1)/2.0);
    cv::Mat rot = cv::getRotationMatrix2D(programSettings.cvImageRotationPoint,
                                          programSettings.cvImageRotationAngle, 1.0);
    // Determine bounding rectangle, center not relevant
    cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), frame.size(),
                                      programSettings.cvImageRotationAngle).boundingRect2f();
    // Adjust transformation matrix
    rot.at<double>(0,2) += bbox.width/2.0 - frame.cols/2.0;
    rot.at<double>(1,2) += bbox.height/2.0 - frame.rows/2.0;
    cv::warpAffine(frame, frame, rot, bbox.size());
}

void FrameGrabber::rotateMatByAngleGPU(cv::cuda::GpuMat &gpuMat, Settings programSettings)
{
    // Get rotation matrix for rotating the image around its center in pixel coordinates
    cv::Point2f center((gpuMat.cols-1)/2.0, (gpuMat.rows-1)/2.0);
    cv::Mat rot = cv::getRotationMatrix2D(programSettings.cvImageRotationPoint,
                                          programSettings.cvImageRotationAngle, 1.0);
    // Determine bounding rectangle, center not relevant
    cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), gpuMat.size(),
                                      programSettings.cvImageRotationAngle).boundingRect2f();
    // Adjust transformation matrix
    rot.at<double>(0,2) += bbox.width/2.0 - gpuMat.cols/2.0;
    rot.at<double>(1,2) += bbox.height/2.0 - gpuMat.rows/2.0;
    cv::cuda::warpAffine(gpuMat, gpuMat, rot, bbox.size());
}

void FrameGrabber::rotateMatByAngle(cv::Mat &mat, Settings programSettings)
{
    cv::cuda::GpuMat gpuMat;
    if (GPU_ACCELERATION)
    {
        gpuMat.upload(mat);
        rotateMatByAngleGPU(gpuMat, programSettings);
        gpuMat.download(mat);
    } else {
        rotateMatByAngleCPU(mat, programSettings);
    }
}

FrameGrabber::FrameGrabber(int cameraAddress, QSize cameraResolution)
{
    videoCapture = cv::VideoCapture(cameraAddress, cv::CAP_DSHOW);
    cameraOpened = true;
    if (!videoCapture.isOpened())
    {
        qDebug() << "Frame: Error opening camera!";
        cameraOpened = false;
    }
    videoCapture.set(cv::CAP_PROP_FRAME_WIDTH, cameraResolution.width());
    videoCapture.set(cv::CAP_PROP_FRAME_HEIGHT, cameraResolution.height());
    videoCapture.set(cv::CAP_PROP_FPS, 90);
}

FrameGrabber::FrameGrabber(Settings programSettings)
{
    videoCapture = cv::VideoCapture(programSettings.cameraAddress, cv::CAP_DSHOW);
    cameraOpened = true;
    if (!videoCapture.isOpened())
    {
        qDebug() << "Frame: Error opening camera!";
        cameraOpened = false;
    }
    videoCapture.set(cv::CAP_PROP_FRAME_WIDTH, programSettings.cameraResolution.width());
    videoCapture.set(cv::CAP_PROP_FRAME_HEIGHT, programSettings.cameraResolution.height());
    videoCapture.set(cv::CAP_PROP_FPS, 90);
}

void FrameGrabber::grab()
{

    if (!videoCapture.isOpened())
    {
        frame = cv::Mat();
    }
    videoCapture.read(frame);
    cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
    frame.copyTo(pureFrame);
    frameWidth = frame.size().width;
    frameHeight = frame.size().height;
}

void FrameGrabber::warp(Settings programSettings)
{
    if (programSettings.cvImageRotationPoint.x != -1)
    {
        rotateMatByAngle(frame, programSettings);
    }

    frameWidth = frame.size().width;
    frameHeight = frame.size().height;
}

void FrameGrabber::crop(Settings programSettings)
{
    if (programSettings.cvImageRotationPoint.x != -1)
    {
        frame = cv::Mat(frame, programSettings.cvImageROIRect);
    }

    frameWidth = frame.size().width;
    frameHeight = frame.size().height;
}
