#include "puckpredictor.h"

cv::Vec2f PuckPredictor::getExtrapolationCoefficients(const std::vector<cv::Point> &pointsVector)
{
    int sumy = 0, sumx = 0, sumx2 = 0, sumxy = 0;
    float n = pointsVector.size(), a, b;

    for (int i = 0; i < n; i++)
    {
        sumy += pointsVector[i].y;
        sumx += pointsVector[i].x;
        sumx2 += pointsVector[i].x*pointsVector[i].x;
        sumxy += pointsVector[i].x*pointsVector[i].y;
    }

    a = (sumxy - (sumx*sumy/n))/(sumx2 - sumx*sumx/n);
    b = (sumy - a*sumx)/n;

    return cv::Vec2f(a, b);
}

bool PuckPredictor::in_Range(int x, int a, int b)
{
    return (x >= a)&&(x <= b);
}

cv::Point PuckPredictor::predictPosition(std::vector<cv::Point> &puckTrajectoryPointsVector, int predictionPointX, cv::Vec2f extrapolationCoefficients, cv::Size frameSize) {

    if (puckTrajectoryPointsVector.size() < 2){return cv::Point(-1, -1);}

    float a = extrapolationCoefficients[0], b = extrapolationCoefficients[1]; // for simplier using
    int predictionPointY, newx = predictionPointX, hitCount = 0;
    cv::Point cp = puckTrajectoryPointsVector[puckTrajectoryPointsVector.size()-1];
    cv::Point lp = puckTrajectoryPointsVector[puckTrajectoryPointsVector.size()-2];

    do
    {
        predictionPointY = a*predictionPointX + b;
        if (!in_Range(predictionPointY, 0, frameSize.height))
        {
            hitCount++;
            if (a > 0)
            {
                newx = (frameSize.height-b)/a;
                a = -a;
                b = frameSize.height-a*newx;
            } else
            {
                newx = (0-b)/a;
                a = -a;
                b = 0-a*newx;
            }
            lp = cp;
            cp = cv::Point(newx, predictionPointY);

            if (hitCount > 5){return cv::Point(-1, -1);}
        }
        else
        {
            if (hitCount >= 2){return cv::Point(-1, -1);}
            return cv::Point(predictionPointX, predictionPointY);
        }
    } while(true);
}

PuckPredictor::PuckPredictor()
{

}

void PuckPredictor::predict(FrameGrabber frameGrabber, PuckDetector puckDetector, Settings programSettings)
{
    extrapolationCoefficients = getExtrapolationCoefficients(puckDetector.puckTrajectoryPointsVector);
    predictedPointMRR = predictPosition(puckDetector.puckTrajectoryPointsVector, MAX_ROBOT_REACH,
                                        extrapolationCoefficients, cv::Size(frameGrabber.frameWidth,
                                                                            frameGrabber.frameHeight));
    predictedPointRSP = predictPosition(puckDetector.puckTrajectoryPointsVector, ROBOT_STRIKER_POSITION,
                                        extrapolationCoefficients, cv::Size(frameGrabber.frameWidth,
                                                                            frameGrabber.frameHeight));
}
