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
#include <puckpredictor.h>
#include <gamealgorithm.h>
#include <cvgui.h>

int main()
{
//    cv::Scalar p1 = cv::Scalar(35, 0);
//    cv::Scalar p2 = cv::Scalar(50, 100);
//    std::vector<cv::Scalar> po;
//    po.push_back(p1);
//    po.push_back(p2);
//    po.push_back(cv::Scalar(100,100));
//    po.push_back(cv::Scalar(100,0));
//    po.push_back(cv::Scalar(50,0));

//    while (true)
//        sender.moveTo(po, 45, 100);

    // Define all variables
    // General
    Settings programSettings;
    programSettings.Load();
    cvGUI gui(programSettings);
    Performance FPSCounter;
    FrameGrabber frameGrabber(programSettings);
    TRectDetector tableBorderDetector;
    PuckDetector puckDetector;
    PuckPredictor puckPredictor;
    GameAlgorithm game;
    while (true) {
        FPSCounter.resetCounter();
        frameGrabber.grab();
        frameGrabber.warp(programSettings);
        frameGrabber.crop(programSettings);
        puckDetector.detect(frameGrabber, programSettings);
        if (puckDetector.isPuckDetected())
        {
            puckPredictor.predict(frameGrabber, puckDetector, programSettings);
            game.process(puckDetector, puckPredictor, programSettings, frameGrabber.frameHeight);
        }
        FPSCounter.stopCounter();
        gui.displayWindows(programSettings,
                           FPSCounter,
                           frameGrabber,
                           tableBorderDetector,
                           puckDetector,
                           puckPredictor);
        if (gui.processKeyboard(programSettings,
                                frameGrabber,
                                tableBorderDetector))
        {return 0;}
    }
}
