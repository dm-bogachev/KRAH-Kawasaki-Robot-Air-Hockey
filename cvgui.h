#ifndef CVGUI_H
#define CVGUI_H

#include <settings.h>
#include <framegrabber.h>
#include <trectdetector.h>
#include <puckdetector.h>
#include <performance.h>
#include <puckpredictor.h>
#include <videowriter.h>

#define VIDEO_WINDOW_NAME programSettings.videoWindowName.toStdString()
#define TRACKBARS_NAME programSettings.trackbarsWindowName.toStdString()
#define ROBOT_STRIKER_POSITION programSettings.robotStrikerPosition
#define MAX_ROBOT_REACH programSettings.robotStrikerPosition - programSettings.robotMotionRange

#define CV_COLOR_GREEN cv::Scalar(0,255,0)
#define CV_COLOR_RED cv::Scalar(0,0,255)
#define CV_COLOR_BLUE cv::Scalar(255,0,0)
#define CV_COLOR_WHITE cv::Scalar(255,255,255)
#define CV_COLOR_BLACK cv::Scalar(0,0,0)

class cvGUI
{
private:
    void setupTrackbarsWindow(Settings &programSettings);
    void showInfo(Settings programSettings);
    VideoWriter *videoWriter;
    bool isRecording;
    cv::Scalar FPSColor;


public:
    cvGUI(Settings &programSettings);
    bool processKeyboard(Settings &programSettings,
                         FrameGrabber frameGrabber,
                         TRectDetector tableBorderDetector);
    void displayWindows(Settings &programSettings,
                        Performance FPSCounter,
                        FrameGrabber frameGrabber,
                        TRectDetector tableBorderDetector,
                        PuckDetector puckDetector,
                        PuckPredictor posPredictor);
};

#endif // CVGUI_H
