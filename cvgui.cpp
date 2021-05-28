#include "cvgui.h"

void cvGUI::setupTrackbarsWindow(Settings& programSettings)
{
    cv::namedWindow(TRACKBARS_NAME, cv::WINDOW_KEEPRATIO);
    cv::resizeWindow(TRACKBARS_NAME, 2000, 2000);

    cv::createTrackbar("houghParam1", TRACKBARS_NAME, &programSettings.houghParam1, 1024);
    cv::createTrackbar("houghParam2", TRACKBARS_NAME, &programSettings.houghParam2, 1024);
    cv::createTrackbar("puckMinRadius", TRACKBARS_NAME, &programSettings.puckMinRadius, 500);
    cv::createTrackbar("puckMaxRadius", TRACKBARS_NAME, &programSettings.puckMaxRadius, 500);
    cv::createTrackbar("robotStrikerPosition", TRACKBARS_NAME,
        &programSettings.robotStrikerPosition,
        programSettings.cameraResolution.width());
    cv::createTrackbar("robotMotionRange", TRACKBARS_NAME,
        &programSettings.robotMotionRange,
        programSettings.cameraResolution.width());
    cv::createTrackbar("puckPositionYLimit", TRACKBARS_NAME,
        &programSettings.puckPositionYLimit,
        programSettings.cameraResolution.width());
    cv::createTrackbar("robotGateYLinit", TRACKBARS_NAME,
        &programSettings.robotGateYLimit,
        programSettings.cameraResolution.width());
    cv::createTrackbar("playerZoneMargin", TRACKBARS_NAME,
        &programSettings.playerZoneMargin,
        programSettings.cameraResolution.width());
    cv::createTrackbar("puckSpeedSlowF", TRACKBARS_NAME,
        &programSettings.puckSpeedSlowF, 100);
    cv::createTrackbar("puckSpeedFastF", TRACKBARS_NAME,
        &programSettings.puckSpeedFastF, 100);
    cv::createTrackbar("puckSpeedSlowB", TRACKBARS_NAME,
        &programSettings.puckSpeedSlowB, 100);
    cv::createTrackbar("puckSpeedFastB", TRACKBARS_NAME,
        &programSettings.puckSpeedFastB, 100);
    cv::createTrackbar("Kp", TRACKBARS_NAME,
        &programSettings.Kp, 20000);
}

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
    videoWriter = new VideoWriter(programSettings.videoFolderPath);
    FPSColor = CV_COLOR_GREEN;
    isRecording = false;
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
    case 'r':
        if (isRecording)
        {
            isRecording = false;
            FPSColor = CV_COLOR_GREEN;
            videoWriter->release();
            qDebug() << "Recording was stopped";
        } else
        {
            isRecording = true;
            FPSColor = CV_COLOR_RED;
            qDebug() << "Recording was started";
        }

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
                           PuckPredictor posPredictor)
{
    if (GPU_ACCELERATION)
    {
        cv::cuda::cvtColor(frameGrabber.gpuFrame, frameGrabber.gpuFrame, cv::COLOR_GRAY2RGB);
        frameGrabber.gpuFrame.download(frameGrabber.frame);
    } else
    {
        cv::cvtColor(frameGrabber.frame, frameGrabber.frame, cv::COLOR_GRAY2RGB);
    }
    if (isRecording)
    {
        videoWriter->write(frameGrabber.recordFrame);
    }
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
             cv::Point(MAX_ROBOT_REACH - programSettings.Kp*abs(puckDetector.puckAverageSpeed[2])/100,0),
             cv::Point(MAX_ROBOT_REACH - programSettings.Kp*abs(puckDetector.puckAverageSpeed[2])/100, frameGrabber.frameHeight),
             CV_COLOR_BLUE, 2);
    cv::line(frameGrabber.frame,
             cv::Point(MAX_ROBOT_REACH,0),
             cv::Point(MAX_ROBOT_REACH, frameGrabber.frameHeight),
             CV_COLOR_RED, 2);
    cv::line(frameGrabber.frame,
             cv::Point(PLAYER_ZONE,0),
             cv::Point(PLAYER_ZONE, frameGrabber.frameHeight),
             CV_COLOR_RED, 2);
    //
    cv::line(frameGrabber.frame,
             cv::Point(0, frameGrabber.frameHeight/2 - programSettings.puckPositionYLimit),
             cv::Point(frameGrabber.frameWidth, frameGrabber.frameHeight/2 - programSettings.puckPositionYLimit),
             CV_COLOR_RED, 2);
    cv::line(frameGrabber.frame,
             cv::Point(0, frameGrabber.frameHeight/2 + programSettings.puckPositionYLimit),
             cv::Point(frameGrabber.frameWidth, frameGrabber.frameHeight/2 + programSettings.puckPositionYLimit),
            CV_COLOR_RED, 2);
    //
    cv::line(frameGrabber.frame,
        cv::Point(0, frameGrabber.frameHeight / 2 - programSettings.robotGateYLimit),
        cv::Point(frameGrabber.frameWidth, frameGrabber.frameHeight / 2 - programSettings.robotGateYLimit),
        CV_COLOR_GREEN, 2);
    cv::line(frameGrabber.frame,
        cv::Point(0, frameGrabber.frameHeight / 2 + programSettings.robotGateYLimit),
        cv::Point(frameGrabber.frameWidth, frameGrabber.frameHeight / 2 + programSettings.robotGateYLimit),
        CV_COLOR_GREEN, 2);
    //
    if (posPredictor.predictedPointRSP.x != -1){cv::circle(frameGrabber.frame, posPredictor.predictedPointRSP, 20, CV_COLOR_WHITE);}
    if (posPredictor.predictedPointMRR.x != -1){cv::circle(frameGrabber.frame, posPredictor.predictedPointMRR, 20, CV_COLOR_WHITE);}
    //
    cv::putText(frameGrabber.frame, std::to_string(FPSCounter.getAverageFPS()), cv::Point(80,80), cv::FONT_HERSHEY_SIMPLEX, 1, FPSColor);
    cv::imshow(VIDEO_WINDOW_NAME, frameGrabber.frame);
}
