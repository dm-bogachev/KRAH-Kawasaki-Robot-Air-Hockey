#ifndef TRECTDETECTOR_H
#define TRECTDETECTOR_H

#include <opencv2/aruco.hpp>

#include <settings.h>
#include <framegrabber.h>
#include <sortcvpoints.h>

#define _PI 3.141592653589793238463

class TRectDetector
{
private:
    Settings programSettings;
    cv::Point2f getRectangleCenter(std::vector<cv::Point2f> rectangleCorners);
    void detectArucoMarkers(cv::Mat inputImage, std::vector<std::vector<cv::Point2f>> &markerCorners, std::vector<int> &markerIds);
    std::vector<cv::Point> getArucoMarkersCenters(std::vector<std::vector<cv::Point2f>> markerCorners);
    double getImageRotationAngle(std::vector<cv::Point> markerCenters);
public:
    TRectDetector(Settings &programSettings);
    TRectDetector();
    void detectTableBorders(FrameGrabber frameGrabber, Settings &programSettings);
};

#endif // TRECTDETECTOR_H
