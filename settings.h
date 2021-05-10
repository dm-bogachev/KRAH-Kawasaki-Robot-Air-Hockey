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
    int cameraAddress;
    QSize cameraResolution;
    int maxSkippedFrames;
    int pointsVectorLength;
    int houghParam1;
    int houghParam2;
    int puckMinRadius;
    int puckMaxRadius;
    int robotStrikerPosition;
    int robotMotionRange;
    cv::Point cvImageRotationPoint;
    cv::Rect cvImageROIRect;
    double cvImageRotationAngle;
};


#endif // SETTINGS_H
