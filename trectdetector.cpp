#include "trectdetector.h"

TRectDetector::TRectDetector(Settings &programSettings)
{
    this->programSettings = programSettings;
}

TRectDetector::TRectDetector()
{

}

void TRectDetector::detectTableBorders(FrameGrabber frameGrabber, Settings &programSettings)
{
    // For AruCo marker detection
    std::vector<std::vector<cv::Point2f>> markerCorners;
    std::vector<int> markerIds;
    std::vector<cv::Point> markerCenters;

    frameGrabber.frame = frameGrabber.pureFrame;

    detectArucoMarkers(frameGrabber.frame, markerCorners, markerIds);
    if (markerCorners.size() == 4)
    {
        markerCenters = getArucoMarkersCenters(markerCorners);
        programSettings.cvImageRotationAngle = getImageRotationAngle(markerCenters);
        programSettings.cvImageRotationPoint = markerCenters[0];
        frameGrabber.warp(programSettings);
        detectArucoMarkers(frameGrabber.frame, markerCorners, markerIds);
        if (markerCorners.size() == 4){
            markerCenters = getArucoMarkersCenters(markerCorners);
            programSettings.cvImageRotationPoint = markerCenters[0];
            programSettings.cvImageROIRect = cv::Rect(markerCenters[0], markerCenters[3]);
            return;
        }
    }
    programSettings.cvImageRotationAngle = 0;
    programSettings.cvImageRotationPoint = cv::Point(-1,-1);
    programSettings.cvImageROIRect = cv::Rect();
}

cv::Point2f TRectDetector::getRectangleCenter(std::vector<cv::Point2f> rectangleCorners)
{
    cv::Point2f rectangleCenter(0.f, 0.f);
    for(int i = 0; i < 4; i++)
    {
        rectangleCenter += rectangleCorners[i];
    }
    return rectangleCenter /= 4.f;
}

void TRectDetector::detectArucoMarkers(cv::Mat inputImage, std::vector<std::vector<cv::Point2f> > &markerCorners, std::vector<int> &markerIds)
{
    std::vector<std::vector<cv::Point2f>> rejectedCandidates;
    cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_250);

    // Invert image because markers are white
    cv::Mat invertedImage;
    cv::bitwise_not(inputImage, invertedImage);

    cv::aruco::detectMarkers(invertedImage, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);
    qDebug() << markerCorners.size() << "corners was detected";
}

std::vector<cv::Point> TRectDetector::getArucoMarkersCenters(std::vector<std::vector<cv::Point2f> > markerCorners)
{
    std::vector<cv::Point> markerCenters;
    for (int i = 0; i < 4; i++){markerCenters.push_back(getRectangleCenter(markerCorners[i]));}
    std::sort(markerCenters.begin(), markerCenters.end(), pointsSortingStructure());
    return markerCenters;
}

double TRectDetector::getImageRotationAngle(std::vector<cv::Point> markerCenters)
{
    /*
        0-----------------2
        |                 |
        |                 |
        |                 |
        |                 |
        1-----------------3
    */
    double rotationAngleRad = atan(double(markerCenters[0].y - markerCenters[2].y)/double(markerCenters[0].x - markerCenters[2].x));
    return rotationAngleRad*180/pi;
}
