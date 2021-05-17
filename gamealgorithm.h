#ifndef GAMEALGORITHM_H
#define GAMEALGORITHM_H

#include <QDebug>

#include <puckdetector.h>
#include <puckpredictor.h>
#include <udpsender.h>
#include <settings.h>

#define ROBOT_STRIKER_POSITION programSettings.robotStrikerPosition
#ifndef MAX_ROBOT_REACH
#define MAX_ROBOT_REACH (long long)programSettings.robotStrikerPosition - programSettings.robotMotionRange
#endif

#define TRIGGER_LINE (long long)(MAX_ROBOT_REACH) - programSettings.Kp*abs(puckDetector.puckAverageSpeed[2])/100
#define PUCK_SPEED_SLOW_F programSettings.puckSpeedSlowF
#define PUCK_SPEED_FAST_F programSettings.puckSpeedFastF
#define PUCK_SPEED_SLOW_B programSettings.puckSpeedSlowB
#define PUCK_SPEED_FAST_B programSettings.puckSpeedFastB

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
    void processBackward(PuckDetector puckDetector, PuckPredictor puckPredictor, Settings programSettings, int frameHeight);
    void processForward(PuckDetector puckDetector, PuckPredictor puckPredictor, Settings programSettings, int frameHeight);
};

#endif // GAMEALGORITHM_H
