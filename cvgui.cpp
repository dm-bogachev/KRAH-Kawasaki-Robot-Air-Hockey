#include "cvgui.h"

void cvGUI::showInfo(Settings programSettings)
{

    cv::Mat info = cv::Mat::ones(cv::Size(300,300), CV_8U);
    cv::putText(info, "Welcome to KRAH", cv::Point(5,20), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255,0,0));
    cv::imshow(TRACKBARS_NAME, info);
    cv::waitKey(1);
}

cvGUI::cvGUI(Settings &programSettings)
{
    setupTrackbarsWindow(programSettings);
}

bool cvGUI::processKeyboard(Settings &programSettings, FrameGrabber frameGrabber, TRectDetector tableBorderDetector)
{
    char pressedKey = cv::waitKey(1);
    switch (pressedKey) {
    case 'c':
        tableBorderDetector.detectTableBorders(frameGrabber, programSettings);
        break;
    case 'h':
        showInfo(programSettings);
        break;
    case 'q':
        return true;
    case 's':
        programSettings.Save();
        qDebug() << "Settings were successfully saved";
        break;
    case 'l':
        programSettings.Load();
        cv::destroyWindow(TRACKBARS_NAME);
        setupTrackbarsWindow(programSettings);
        qDebug() << "Settings were successfully loaded";
        break;
    }
    return false;
}

void cvGUI::displayWindows(Settings &programSettings,
                           Performance FPSCounter,
                           FrameGrabber frameGrabber,
                           TRectDetector tableBorderDetector,
                           PuckDetector puckDetector,
                           PositionPredictor posPredictor)
{
    cv::cvtColor(frameGrabber.frame, frameGrabber.frame, cv::COLOR_GRAY2RGB);
    if (puckDetector.currentPoint.x != -1){cv::circle(frameGrabber.frame,
                                                      cv::Point(puckDetector.currentPoint.x,
                                                                puckDetector.currentPoint.y),
                                                      puckDetector.currentPoint.z, CV_COLOR_GREEN, 2);}
    for (uint i = 0; i < puckDetector.puckTrajectoryPointsVector.size(); i++){cv::drawMarker(frameGrabber.frame, puckDetector.puckTrajectoryPointsVector[i], CV_COLOR_BLUE);}
    cv::line(frameGrabber.frame,
             cv::Point(ROBOT_STRIKER_POSITION,0),
             cv::Point(ROBOT_STRIKER_POSITION,frameGrabber.frameHeight),
             CV_COLOR_BLUE, 2);
    cv::line(frameGrabber.frame,
             cv::Point(MAX_ROBOT_REACH,0),
             cv::Point(MAX_ROBOT_REACH, frameGrabber.frameHeight),
             CV_COLOR_BLUE, 2);
    if (posPredictor.predictedPointRSP.x != -1){cv::circle(frameGrabber.frame, posPredictor.predictedPointRSP, 20, CV_COLOR_WHITE);}
//  cv::line(ROIFrame, cv::Point(0, posPredictor.extrapolationCoefficients[0]*0 + extrapolationCoefficients[1]),
//           cv::Point(ROIFrameSize.width, extrapolationCoefficients[0]*ROIFrameSize.width + extrapolationCoefficients[1]),
//           CV_COLOR_BLUE, 2);
//  cv::line(ROIFrame, cv::Point(puckPoint.x, puckPoint.y),
//           predictedPointRSP, CV_COLOR_BLUE, 2);
    cv::putText(frameGrabber.frame, std::to_string(FPSCounter.getAverageFPS()), cv::Point(80,80), cv::FONT_HERSHEY_SIMPLEX, 1, CV_COLOR_GREEN);
    cv::imshow(VIDEO_WINDOW_NAME, frameGrabber.frame);
}
