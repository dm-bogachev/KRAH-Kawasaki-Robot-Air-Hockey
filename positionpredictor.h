#ifndef POSITIONPREDICTOR_H
#define POSITIONPREDICTOR_H

#include <opencv2/core.hpp>

#include <puckdetector.h>
#include <settings.h>

#define ROBOT_STRIKER_POSITION programSettings.robotStrikerPosition
#define MAX_ROBOT_REACH programSettings.robotStrikerPosition - programSettings.robotMotionRange

class PositionPredictor
{
private:
    cv::Vec2f getExtrapolationCoefficients(const std::vector<cv::Point> &pointsVector);
    bool in_Range(int x, int a, int b);
    cv::Point predictPosition(std::vector<cv::Point> &puckTrajectoryPointsVector,
                              int predictionPointX,
                              cv::Vec2f extrapolationCoefficients,
                              cv::Size frameSize);

public:
    PositionPredictor();
    void predict(FrameGrabber frameGrabber, PuckDetector puckDetector, Settings programSettings);

    cv::Vec2f extrapolationCoefficients;
    cv::Point predictedPointRSP;
    cv::Point predictedPointMRR;
};

#endif // POSITIONPREDICTOR_H
