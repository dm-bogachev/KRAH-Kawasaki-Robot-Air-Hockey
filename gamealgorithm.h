#ifndef GAMEALGORITHM_H
#define GAMEALGORITHM_H

#include <QDebug>

#include <puckdetector.h>
#include <puckpredictor.h>
#include <udpsender.h>
#include <settings.h>

#define ROBOT_STRIKER_POSITION programSettings.robotStrikerPosition
#define MAX_ROBOT_REACH (long long)programSettings.robotStrikerPosition - programSettings.robotMotionRange
#define TRIGGER_LINE (long long)(MAX_ROBOT_REACH) - programSettings.Kp*abs(puckDetector.puckAverageSpeed[2])/100
#define PUCK_SPEED_SLOW programSettings.puckSpeedSlow
#define PUCK_SPEED_FAST programSettings.puckSpeedFast

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
    void process(PuckDetector puckDetector, PuckPredictor puckPredictor, Settings programSettings, int frameHeight);
};

#endif // GAMEALGORITHM_H
