#include <QDebug>
#include <QSize>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudawarping.hpp>

#include <settings.h>
#include <performance.h>
#include <sortcvpoints.h>
#include <framegrabber.h>
#include <trectdetector.h>
#include <puckdetector.h>
#include <positionpredictor.h>
#include <cvgui.h>

#define PUCK_SPEED_SLOW 20

int main()
{
    // Define all variables
    // General
    Settings programSettings;
    programSettings.Load();
    cvGUI gui(programSettings);
    Performance FPSCounter;
    FrameGrabber frameGrabber(programSettings);
    TRectDetector tableBorderDetector;
    PuckDetector puckDetector;
    PositionPredictor posPredictor;

    while (true) {
        FPSCounter.resetCounter();
        frameGrabber.grab();
        frameGrabber.warp(programSettings);
        frameGrabber.crop(programSettings);
        puckDetector.detect(frameGrabber, programSettings);
        if (puckDetector.isPuckDetected())
        {
            posPredictor.predict(frameGrabber, puckDetector, programSettings);
        }
        FPSCounter.stopCounter();
        gui.displayWindows(programSettings,
                           FPSCounter,
                           frameGrabber,
                           tableBorderDetector,
                           puckDetector,
                           posPredictor);
        if (gui.processKeyboard(programSettings,
                                frameGrabber,
                                tableBorderDetector))
        {return 0;}
    }
}



//cv::createTrackbar("p1", TRACKBARS_NAME, &p1, 255);
//cv::createTrackbar("p2", TRACKBARS_NAME, &p2, 255);
//cv::createTrackbar("min", TRACKBARS_NAME, &minRadius, 500);
//cv::createTrackbar("max", TRACKBARS_NAME, &maxRadius, 500);
//cv::createTrackbar("MINY",q TRACKBARS_NAME, &MINY, 2050);
//cv::createTrackbar("MAXY", TRACKBARS_NAME, &MAXY, 2050);
//cv::createTrackbar("ENDLINE", TRACKBARS_NAME, &ENDLINE, 2050);
//cv::createTrackbar("WORKING_DELTA", TRACKBARS_NAME, &WORKING_DELTA, 1024);
//cv::createTrackbar("GATE_DELTA", TRACKBARS_NAME, &GATE_DELTA, 1024);
//cv::createTrackbar("kx", TRACKBARS_NAME, &kx, 40000);
