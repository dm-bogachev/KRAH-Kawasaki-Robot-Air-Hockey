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
    bool GPUAcceleration;
    QString videoWindowName;
    QString trackbarsWindowName;
    int cameraAddress;
    QSize cameraResolution;
    int maxSkippedFrames;
    int pointsVectorLength;

public:
    Settings();
    void Save();
    void Load();
    // Variables for UI
    int houghParam1;
    int houghParam2;
    int puckMinRadius;
    int puckMaxRadius;
    int robotStrikerPosition;
    int robotMotionRange;
    cv::Point cvImageRotationPoint;
    cv::Rect cvImageROIRect;
    double cvImageRotationAngle;
    // Getters and Setters
    QSize getCameraResolution() const;
    void setCameraResolution(const QSize &value);
    int getCameraAddress() const;
    void setCameraAddress(int value);
    int getHoughParam1() const;
    void setHoughParam1(int newHoughParam1);
    int getHoughParam2() const;
    void setHoughParam2(int newHoughParam2);
    int getPuckMinRadius() const;
    void setPuckMinRadius(int newPuckMinRadius);
    int getPuckMaxRadius() const;
    void setPuckMaxRadius(int newPuckMaxRadius);
    bool getGPUAcceleration() const;
    void setGPUAcceleration(bool newGPUAcceleration);
    const QString &getVideoWindowName() const;
    void setVideoWindowName(const QString &newVideoWindowName);
    const QString &getTrackbarsWindowName() const;
    void setTrackbarsWindowName(const QString &newTrackbarsWindowName);
    int getMaxSkippedFrames() const;
    void setMaxSkippedFrames(int newMaxSkippedFrame);
    int getPointsVectorLength() const;
    void setPointsVectorLength(int newPointsArrayLength);
    double getImageRotationAngle() const;
    void setImageRotationAngle(double newImageRotationAngle);
    int getRobotStrikerPosition() const;
    void setRobotStrikerPosition(int newRobotStrikerPosition);
    int getRobotMotionRange() const;
    void setRobotMotionRange(int newRobotMotionRange);
    cv::Point getImageRotationPoint() const;
    void setImageRotationPoint(cv::Point newImageRotationPoint);
};


#endif // SETTINGS_H
