#include "puckdetector.h"

PuckDetector::PuckDetector()
{

}

void PuckDetector::detect(FrameGrabber frameGrabber, Settings programSettings)
{
    currentPoint = findPuck(frameGrabber.frame, programSettings);
    if (currentPoint.x != -1)
    {
        puckDetected = true;
        puckTrajectoryPointsVector.push_back(cv::Point(currentPoint.x, currentPoint.y));
        if (puckTrajectoryPointsVector.size() < 2) {return;}
        puckSpeedVector = getSpeedVector(puckTrajectoryPointsVector, puckAverageSpeed);
        if (checkDirectionChange(puckSpeedVector)) {puckTrajectoryPointsVector.clear();}
        skippedFramesNumber = 0;
    } else
    {
        puckDetected = false;
        skippedFramesNumber++;
        if (skippedFramesNumber > MAX_SKIPPED_FRAMES)
        {
            puckTrajectoryPointsVector.clear();
        }
    }
    if (puckTrajectoryPointsVector.size() == POINTS_VECTOR_LENGTH)
    {
        puckTrajectoryPointsVector.erase(puckTrajectoryPointsVector.begin());
    }
}

bool PuckDetector::isPuckDetected()
{
    return puckDetected;
}

bool PuckDetector::isForwardDirection()
{
    return puckSpeedVector[puckSpeedVector.size()-1][0] > 0;
}

cv::Point3f PuckDetector::findPuckCPU(cv::Mat frame, Settings programSettings)
{
    std::vector<cv::Vec3f> circles;
    HoughCircles(frame, circles, cv::HOUGH_GRADIENT, 1, frame.rows/2,
                 programSettings.houghParam1, programSettings.houghParam2,
                 programSettings.puckMinRadius, programSettings.puckMaxRadius);

    if (circles.size() == 0)
    {
        return cv::Point3f(-1, -1, -1);
    }

    cv::Point3f center(cvRound(circles[0][0]), cvRound(circles[0][1]), cvRound(circles[0][2]));
    //qDebug() << "Detected puck with raduis: " << cvRound(circles[0][2]);

    return center;
}

cv::Point3f PuckDetector::findPuckGPU(cv::cuda::GpuMat frame, Settings programSettings)
{
    std::vector<cv::Vec3f> gpuCircles;
    cv::cuda::GpuMat gpuResult;
    cv::Ptr<cv::cuda::HoughCirclesDetector> gpuDetector = cv::cuda::createHoughCirclesDetector(1, frame.rows/2,
                                                                                               programSettings.houghParam1, programSettings.houghParam2,
                                                                                               programSettings.puckMinRadius, programSettings.puckMaxRadius);
    gpuDetector->detect(frame, gpuResult);
    gpuCircles.resize(gpuResult.size().width);
    if (!gpuCircles.empty())
        gpuResult.row(0).download(cv::Mat(gpuCircles).reshape(3, 1));

    if (gpuCircles.size() == 0)
    {
        return cv::Point3f(-1, -1, -1);
    }

    cv::Point3f center(cvRound(gpuCircles[0][0]), cvRound(gpuCircles[0][1]), cvRound(gpuCircles[0][2]));
    //qDebug() << "Detected puck with raduis: " << cvRound(gpuCircles[0][2]);

    return center;
}

cv::Point3f PuckDetector::findPuck(cv::Mat &frame, Settings programSettings)
{
    cv::Point3f puckPoint;
    cv::cuda::GpuMat GPUMat;
    if (GPU_ACCELERATION)
    {
        GPUMat.upload(frame);
        puckPoint = findPuckGPU(GPUMat, programSettings);
        if (puckPoint.x != -1){
            GPUMat.download(frame);
        }

    } else {
        puckPoint = findPuckCPU(frame,programSettings);
    }
    return puckPoint;
}

std::vector<cv::Scalar> PuckDetector::getSpeedVector(std::vector<cv::Point> &puckTrajectoryPointsVector, cv::Scalar &puckAverageSpeed)
{
    cv::Scalar speed;
    cv::Point sumSpeed;
    cv::Point temp;
    std::vector<cv::Scalar> puckSpeedVector;

    for (int i = 1; i < puckTrajectoryPointsVector.size(); i++)
    {
        temp = puckTrajectoryPointsVector[i] - puckTrajectoryPointsVector[i-1];
        speed = cv::Scalar(temp.x, temp.y);
        puckSpeedVector.push_back(speed);
        sumSpeed += temp;
    }
    temp = sumSpeed/(signed int)puckSpeedVector.size();
    puckAverageSpeed = cv::Scalar(temp.x, temp.y);
    return puckSpeedVector;
}

bool PuckDetector::checkDirectionChange(std::vector<cv::Scalar> puckSpeedVector)
{
    int positiveX = 0;
    int negativeX = 0;
    int positiveY = 0;
    int negativeY = 0;

    if (puckSpeedVector.size() <= 2)
        return false;

    for (uint8_t i = 0; i < puckSpeedVector.size(); i++)
    {
        if (puckSpeedVector[i][0] > 0)
        {
            positiveX++;
        }
        else if (puckSpeedVector[i][0] < 0)
        {
            negativeX++;
        }
        if (puckSpeedVector[i][1] > 0)
        {
            positiveY++;
        }
        else if (puckSpeedVector[i][1] < 0)
        {
            negativeY++;
        }
    }
    return !(positiveX == 0 || negativeX == 0) || !(positiveY == 0 || negativeY == 0);
}
