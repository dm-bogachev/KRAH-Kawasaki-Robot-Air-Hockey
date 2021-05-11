#ifndef GAMEALGORITHM_H
#define GAMEALGORITHM_H

#include <QDebug>

#include <puckdetector.h>
#include <puckpredictor.h>
#include <udpsender.h>
#include <settings.h>

#define ROBOT_STRIKER_POSITION programSettings.robotStrikerPosition
#define MAX_ROBOT_REACH programSettings.robotStrikerPosition - programSettings.robotMotionRange
#define TRIGGER_LINE MAX_ROBOT_REACH - programSettings.Kp*abs(puckDetector.puckAverageSpeed[0])/1000
#define PUCK_SPEED_SLOW programSettings.puckSpeedSlow

class GameAlgorithm
{
private:
    UDPSender udpSender;
    std::map<std::string, bool> stagesMap;
    void initStagesMap();
    void setState(std::string state);
    bool checkState(std::string state);
public:
    GameAlgorithm();
    void process(PuckDetector puckDetector, PuckPredictor puckPredictor, Settings programSettings, int frameWidth);
};

#endif // GAMEALGORITHM_H
