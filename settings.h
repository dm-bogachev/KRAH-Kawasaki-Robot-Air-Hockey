#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSize>
#include <QSettings>
#include <QFileInfo>

#define CONFIG_FILE_NAME "krah.ini"

class Settings
{
private:
    QSettings *settings;
    bool GPUAcceleration; // Not important
    QString videoWindowName;
    QString trackbarsWindowName;
    int cameraAddress;
    QSize cameraResolution;
    int maxSkippedFrames;
    int pointsVectorLength;
    int imageRotationAngle;
    QPoint tableROIPoint1;
    QPoint tableROIPoint2;

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
    int getImageRotationAngle() const;
    void setImageRotationAngle(int newImageRotationAngle);
    QPoint getTableROIPoint1() const;
    void setTableROIPoint1(QPoint newTableROIPoint1);
    QPoint getTableROIPoint2() const;
    void setTableROIPoint2(QPoint newTableROIPoint2);
    int getRobotStrikerPosition() const;
    void setRobotStrikerPosition(int newRobotStrikerPosition);
    int getRobotMotionRange() const;
    void setRobotMotionRange(int newRobotMotionRange);
};


#endif // SETTINGS_H
