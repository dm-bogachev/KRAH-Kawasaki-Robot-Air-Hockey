#include "gamealgorithm.h"

GameAlgorithm::GameAlgorithm()
{

}

void GameAlgorithm::process(PuckDetector puckDetector, PuckPredictor puckPredictor, Settings programSettings)
{
    if (puckDetector.isForwardDirection())
    {
        if (puckDetector.currentPoint.x > MAX_ROBOT_REACH)
        {
            if (puckDetector.currentPoint.x > TRIGGER_LINE)
            {
                qDebug() << "Puck Stage 1.1" << "Attack normally";
            }
        } else
        {

        }

    } else
    {
        if (puckDetector.currentPoint.x > MAX_ROBOT_REACH)
        {
            qDebug() << "Average speed" << puckDetector.puckAverageSpeed[0];
            if (abs(puckDetector.puckAverageSpeed[0]) < PUCK_SPEED_SLOW)
            { // Puck is slow enough to hit
                qDebug() << "Puck Stage 3.1" << "Robot will attack and return home";
            } else
            { // Puck is fast enough to hit
                qDebug() << "Puck Stage 3.2" << "Robot will return in home position safe";;
            }
        } else
        {
            qDebug() << "Puck State 4" << "Robot will return in home position";
        }
    }
}
