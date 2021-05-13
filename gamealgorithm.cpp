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
    std::vector<cv::Scalar> robotTrajectory;
    if (puckDetector.puckTrajectoryPointsVector.size() < 2) {return;}
    if (puckDetector.isForwardDirection())
    {
        std::cout << puckDetector.puckAverageSpeed[2] << std::endl;
        if (abs(puckDetector.puckAverageSpeed[2]) > PUCK_SPEED_FAST)
        {
            if ((puckPredictor.predictedPointRSP.y > frameHeight / 2 - programSettings.robotGateYLimit) &&
                (puckPredictor.predictedPointRSP.y < frameHeight / 2 + programSettings.robotGateYLimit))
            {
                robotTrajectory.push_back(cv::Scalar(puckPredictor.predictedPointRSP.y, 0));
                if (!robotTrajectory.empty()) { udpSender.moveTo(robotTrajectory, programSettings.puckPositionYLimit, frameHeight); }
                return;
            }
        }
        else
        {
            if (puckDetector.currentPoint.x > TRIGGER_LINE)
            {
                robotTrajectory.push_back(cv::Scalar(puckPredictor.predictedPointRSP.y, 0));
                robotTrajectory.push_back(cv::Scalar(puckPredictor.predictedPointMRR.y, 100));
                robotTrajectory.push_back(cv::Scalar(frameHeight / 2, 0));
                if (!robotTrajectory.empty()) { udpSender.moveTo(robotTrajectory, programSettings.puckPositionYLimit, frameHeight); }
                return;
            }
            else
            {
                robotTrajectory.push_back(cv::Scalar(frameHeight / 2, 0));
                if (!robotTrajectory.empty()) { udpSender.moveTo(robotTrajectory, programSettings.puckPositionYLimit, frameHeight); }
                return;
            }

        }
        //if (puckDetector.currentPoint.x > TRIGGER_LINE)
        //{
        //    robotTrajectory.push_back(cv::Scalar(puckPredictor.predictedPointRSP.y, 0));
        //    robotTrajectory.push_back(cv::Scalar(puckPredictor.predictedPointMRR.y, 100));
        //    robotTrajectory.push_back(cv::Scalar(frameWidth/2,0));
        //    if (!robotTrajectory.empty()){udpSender.moveTo(robotTrajectory, programSettings.puckPositionYLimit, frameWidth);}
        //    qDebug() << "kiiiiiiiiiiiiiiiiiiiiiiii: " << puckDetector.currentPoint.y << 0 << 100 << 0 << 50;
        //    return;
        //}
    } else
    {
        robotTrajectory.push_back(cv::Scalar(frameHeight / 2, 0));
        if (!robotTrajectory.empty()) { udpSender.moveTo(robotTrajectory, programSettings.puckPositionYLimit, frameHeight); }
        return;
        if (puckDetector.currentPoint.x > MAX_ROBOT_REACH)
        {
            qDebug() << "Backward direction. Robot can reach puck";
            if (abs(puckDetector.puckAverageSpeed[0]) < PUCK_SPEED_SLOW)
            { // Puck is slow enough to hit
                qDebug() << "Puck is very slow, so we just hit it directly";
                robotTrajectory.push_back(cv::Scalar(puckDetector.currentPoint.y, 0));
                robotTrajectory.push_back(cv::Scalar(puckDetector.currentPoint.y, 100));
                robotTrajectory.push_back(cv::Scalar(puckDetector.currentPoint.y, 0));
                robotTrajectory.push_back(cv::Scalar(frameHeight/2,0));
                qDebug() << "Robot trajectory: " << puckDetector.currentPoint.y << 0 << 100 << 0 << 50;

            } else
            { // Puck is fast enough to hit
                robotTrajectory.push_back(cv::Scalar(puckPredictor.predictedPointRSP.y, 0));
                robotTrajectory.push_back(cv::Scalar(puckPredictor.predictedPointMRR.y, 100));
                robotTrajectory.push_back(cv::Scalar(frameHeight/2,0));
                qDebug() << puckPredictor.predictedPointRSP.y << puckPredictor.predictedPointMRR.y;
            }
        } else
        {
            robotTrajectory.push_back(cv::Scalar(frameHeight/2,0));
            //qDebug() << "Puck State 4" << "Robot will return in home position";
        }
    }
    if (!robotTrajectory.empty()){udpSender.moveTo(robotTrajectory, programSettings.puckPositionYLimit, frameHeight);}
}
