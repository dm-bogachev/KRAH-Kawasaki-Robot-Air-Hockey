#ifndef PUCKDETECTOR_H
#define PUCKDETECTOR_H

#include <opencv2/opencv.hpp>
#include <opencv2/cudaimgproc.hpp>

#include <framegrabber.h>
#include <settings.h>

#define GPU_ACCELERATION programSettings.GPUAcceleration
#define MAX_SKIPPED_FRAMES programSettings.maxSkippedFrames
#define POINTS_VECTOR_LENGTH programSettings.pointsVectorLength

class PuckDetector
{
private:
    int skippedFramesNumber = 0;

    std::vector<cv::Scalar> puckSpeedVector;
    cv::Scalar puckAverageSpeed;
    bool puckDetected;

    cv::Point3f findPuckCPU(cv::Mat frame, Settings programSettings);
    cv::Point3f findPuckGPU(cv::cuda::GpuMat frame, Settings programSettings);
    cv::Point3f findPuck(cv::Mat &frame, Settings programSettings);
    std::vector<cv::Scalar> getSpeedVector(std::vector<cv::Point> &puckTrajectoryPointsVector, cv::Scalar &puckAverageSpeed);
    bool checkDirectionChange(std::vector<cv::Scalar> puckSpeedVector);

public:
    PuckDetector();
    cv::Point3f currentPoint;
    std::vector<cv::Point> puckTrajectoryPointsVector;
    void detect(FrameGrabber frameGrabber, Settings programSettings);
    bool isPuckDetected();
    bool isForwardDirection();
};



#endif // PUCKDETECTOR_H
