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

void GameAlgorithm::process(PuckDetector puckDetector, PuckPredictor puckPredictor, Settings programSettings, int frameWidth)
{
    std::vector<cv::Scalar> robotTrajectory;
    if (puckDetector.puckTrajectoryPointsVector.size() < 2) {return;}
    if (puckDetector.isForwardDirection())
    {
        robotTrajectory.push_back(cv::Scalar(puckDetector.currentPoint.y, 0));
        /*
        if (puckDetector.currentPoint.x > MAX_ROBOT_REACH)
        {
            if (puckDetector.currentPoint.x > TRIGGER_LINE)
            {
                robotTrajectory.push_back(cv::Scalar((puckPredictor.predictedPointMRR.y), 0));
                robotTrajectory.push_back(cv::Scalar((puckPredictor.predictedPointRSP.y), 100));
                robotTrajectory.push_back(cv::Scalar(puckDetector.currentPoint.y, 0));
                robotTrajectory.push_back(cv::Scalar(frameWidth/2,0));
                qDebug() << "Puck Stage 1.1" << "Attack normally";
            }
        } else
        {
            if (false || abs(puckDetector.puckAverageSpeed[0]) < PUCK_SPEED_SLOW)
            { // Puck is slow enough to hit
                robotTrajectory.push_back(cv::Scalar(puckDetector.currentPoint.y, 0));
                robotTrajectory.push_back(cv::Scalar(puckDetector.currentPoint.y, 100));
                robotTrajectory.push_back(cv::Scalar(puckDetector.currentPoint.y, 0));
                robotTrajectory.push_back(cv::Scalar(frameWidth/2,0));
                udpSender.moveTo(robotTrajectory, programSettings.puckPositionYLimit, frameWidth);
                qDebug() << "Puck Stage 3.1" << "Robot will attack and return home";
            } else
            { // Puck is fast enough to hit
                qDebug() << "Puck Stage 3.2" << "Robot will return in home position safe";;
            }
        }
        */
    } else
    {
        if (puckDetector.currentPoint.x > MAX_ROBOT_REACH)
        {
            qDebug() << "Backward direction. Robot can reach puck";
            if (abs(puckDetector.puckAverageSpeed[0]) < PUCK_SPEED_SLOW)
            { // Puck is slow enough to hit
                setState("BIS");

                qDebug() << "Puck is very slow, so we just hit it directly";
                robotTrajectory.push_back(cv::Scalar(puckDetector.currentPoint.y, 0));
                robotTrajectory.push_back(cv::Scalar(puckDetector.currentPoint.y, 100));
                robotTrajectory.push_back(cv::Scalar(puckDetector.currentPoint.y, 0));
                robotTrajectory.push_back(cv::Scalar(frameWidth/2,0));
                qDebug() << "Robot trajectory: " << puckDetector.currentPoint.y << 0 << 100 << 0 << 50;

            } else
            { // Puck is fast enough to hit
                qDebug() << puckPredictor.predictedPointRSP.y << puckPredictor.predictedPointMRR.y;
            }
        } else
        {
            //qDebug() << "Puck State 4" << "Robot will return in home position";
        }
    }
    udpSender.moveTo(robotTrajectory, programSettings.puckPositionYLimit, frameWidth);
}
