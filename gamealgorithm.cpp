#include "gamealgorithm.h"

void GameAlgorithm::initStagesMap()
{
    /*
     XYZ:
     X: F - forward, B - backward
     Y: I - Inside robotReach, O - outside robotReach, H - inside trigger
     Z: S - slow speed, F - fast speed
    */
    stagesMap.insert(std::pair<std::string,bool>("FIS",false));
    stagesMap.insert(std::pair<std::string,bool>("FIF",false));
    stagesMap.insert(std::pair<std::string,bool>("BIS",false));
    stagesMap.insert(std::pair<std::string,bool>("BIF",false));
    stagesMap.insert(std::pair<std::string,bool>("FOS",false));
    stagesMap.insert(std::pair<std::string,bool>("FOF",false));
}

void GameAlgorithm::setState(std::string state)
{
    initStagesMap();
    stagesMap[state] = true;
}

bool GameAlgorithm::checkState(std::string state)
{
    return stagesMap[state];
}

GameAlgorithm::GameAlgorithm()
{

}

void GameAlgorithm::process(PuckDetector puckDetector, PuckPredictor puckPredictor, Settings programSettings, int frameHeight)
{
    if (puckDetector.puckTrajectoryPointsVector.size() < 2) {return;}
    if (puckDetector.isForwardDirection())
    {
        processForward(puckDetector, puckPredictor, programSettings, frameHeight);
    }
    else
    {
        processBackward(puckDetector, puckPredictor, programSettings, frameHeight);
    }
}

void GameAlgorithm::processBackward(PuckDetector puckDetector, PuckPredictor puckPredictor, Settings programSettings, int frameHeight)
{
    std::vector<cv::Scalar> robotTrajectory;
    if (puckDetector.currentPoint.x > MAX_ROBOT_REACH)
    {
        // Stage BR
        if (abs(puckDetector.puckAverageSpeed[0]) < PUCK_SPEED_SLOW_B)
        { // Puck is slow enough to hit
            // Stage BRS
            robotTrajectory.push_back(cv::Scalar(puckDetector.currentPoint.y, 0));
            robotTrajectory.push_back(cv::Scalar(puckDetector.currentPoint.y +
                                                 puckDetector.puckAverageSpeed[1], 100)); // Maybe will be removed
            robotTrajectory.push_back(cv::Scalar(puckDetector.currentPoint.y, 0));
            robotTrajectory.push_back(cv::Scalar(frameHeight/2,0));
        } else if ((abs(puckDetector.puckAverageSpeed[0]) > PUCK_SPEED_SLOW_B) &&
                   (abs(puckDetector.puckAverageSpeed[0]) < PUCK_SPEED_FAST_B))
        {
            // Puck is fast enough
            // Stage BRF
            robotTrajectory.push_back(cv::Scalar(puckPredictor.predictedPointRSP.y, 0));
            robotTrajectory.push_back(cv::Scalar(puckPredictor.predictedPointMRR.y, 100));
            robotTrajectory.push_back(cv::Scalar(frameHeight/2,0));
        }
    } else
    {
        // Stage BO
        robotTrajectory.push_back(cv::Scalar(frameHeight/2,0));
    }
    if (!robotTrajectory.empty()) { udpSender.moveTo(robotTrajectory, programSettings.puckPositionYLimit, frameHeight); }
}

void GameAlgorithm::processForward(PuckDetector puckDetector, PuckPredictor puckPredictor, Settings programSettings, int frameHeight)
{
    std::vector<cv::Scalar> robotTrajectory;
    if (abs(puckDetector.puckAverageSpeed[0]) > PUCK_SPEED_FAST_F)
    {
        // Stage FXF
        if ((puckPredictor.predictedPointRSP.y > frameHeight / 2 - programSettings.robotGateYLimit) &&
                (puckPredictor.predictedPointRSP.y < frameHeight / 2 + programSettings.robotGateYLimit))
        {
            robotTrajectory.push_back(cv::Scalar(puckPredictor.predictedPointRSP.y, 0));
        }
    }
    else if (abs(puckDetector.puckAverageSpeed[0]) < PUCK_SPEED_SLOW_F)
    {
        // Stage FXS
        //
        /*
        robotTrajectory.push_back(cv::Scalar(puckDetector.currentPoint.y, 0));
        robotTrajectory.push_back(cv::Scalar(puckDetector.currentPoint.y +
                                             puckDetector.puckAverageSpeed[1], 100)); // Maybe will be removed
        robotTrajectory.push_back(cv::Scalar(puckDetector.currentPoint.y, 0));
        robotTrajectory.push_back(cv::Scalar(frameHeight/2,0));
        */
        ;
    } else
    {
        // Stage FX
        if (puckDetector.currentPoint.x > TRIGGER_LINE)
        {
            // Stage FTN
            robotTrajectory.push_back(cv::Scalar(puckPredictor.predictedPointRSP.y, 0));
            robotTrajectory.push_back(cv::Scalar(puckPredictor.predictedPointMRR.y, 100));
            robotTrajectory.push_back(cv::Scalar(frameHeight / 2, 0));
        }
        else
        {
            // Stage FN
            robotTrajectory.push_back(cv::Scalar(frameHeight / 2, 0));
            //if (!robotTrajectory.empty()) { udpSender.moveTo(robotTrajectory, programSettings.puckPositionYLimit, frameHeight); }
            //return;
        }
    }
    if (!robotTrajectory.empty()) { udpSender.moveTo(robotTrajectory, programSettings.puckPositionYLimit, frameHeight); }
}
