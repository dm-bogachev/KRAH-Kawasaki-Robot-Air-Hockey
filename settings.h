#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSize>
#include <QRect>
#include <QSettings>
#include <QFileInfo>

#include <opencv2/core.hpp>

#define CONFIG_FILE_NAME "krah.ini"

class Settings
{
private:
    QSettings *settings;
public:
    Settings();
    void Save();
    void Load();
    bool GPUAcceleration;
    QString videoWindowName;
    QString trackbarsWindowName;
    QString debugVideoPath;
    int cameraAddress;
    QSize cameraResolution;
    cv::Point cvImageRotationPoint;
    cv::Rect cvImageROIRect;
    double cvImageRotationAngle;
    int houghParam1;
    int houghParam2;
    int puckMinRadius;
    int puckMaxRadius;
    int maxSkippedFrames;
    int pointsVectorLength;
    int robotStrikerPosition;
    int robotMotionRange;
    int puckSpeedSlowF;
    int puckSpeedFastF;
    int puckSpeedSlowB;
    int puckSpeedFastB;
    int Kp;
    int puckPositionYLimit;
    int robotGateYLimit; // Добавить
};


#endif // SETTINGS_H
