#include "settings.h"

QSize Settings::getCameraResolution() const
{
    return cameraResolution;
}

void Settings::setCameraResolution(const QSize &value)
{
    cameraResolution = value;
}

int Settings::getCameraAddress() const
{
    return cameraAddress;
}

void Settings::setCameraAddress(int value)
{
    cameraAddress = value;
}

int Settings::getHoughParam1() const
{
    return houghParam1;
}

void Settings::setHoughParam1(int newHoughParam1)
{
    houghParam1 = newHoughParam1;
}

int Settings::getHoughParam2() const
{
    return houghParam2;
}

void Settings::setHoughParam2(int newHoughParam2)
{
    houghParam2 = newHoughParam2;
}

int Settings::getPuckMinRadius() const
{
    return puckMinRadius;
}

void Settings::setPuckMinRadius(int newPuckMinRadius)
{
    puckMinRadius = newPuckMinRadius;
}

int Settings::getPuckMaxRadius() const
{
    return puckMaxRadius;
}

void Settings::setPuckMaxRadius(int newPuckMaxRadius)
{
    puckMaxRadius = newPuckMaxRadius;
}

bool Settings::getGPUAcceleration() const
{
    return GPUAcceleration;
}

void Settings::setGPUAcceleration(bool newGPUAcceleration)
{
    GPUAcceleration = newGPUAcceleration;
}

const QString &Settings::getVideoWindowName() const
{
    return videoWindowName;
}

void Settings::setVideoWindowName(const QString &newVideoWindowName)
{
    videoWindowName = newVideoWindowName;
}

const QString &Settings::getTrackbarsWindowName() const
{
    return trackbarsWindowName;
}

void Settings::setTrackbarsWindowName(const QString &newTrackbarsWindowName)
{
    trackbarsWindowName = newTrackbarsWindowName;
}

int Settings::getMaxSkippedFrames() const
{
    return maxSkippedFrames;
}

void Settings::setMaxSkippedFrames(int newMaxSkippedFrame)
{
    maxSkippedFrames = newMaxSkippedFrame;
}

int Settings::getPointsVectorLength() const
{
    return pointsVectorLength;
}

void Settings::setPointsVectorLength(int newPointsArrayLength)
{
    pointsVectorLength = newPointsArrayLength;
}

double Settings::getImageRotationAngle() const
{
    return cvImageRotationAngle;
}

void Settings::setImageRotationAngle(double newImageRotationAngle)
{
    cvImageRotationAngle = newImageRotationAngle;
}

Settings::Settings()
{
    this->settings = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    this->Load();
    if (!(QFileInfo::exists(CONFIG_FILE_NAME) && QFileInfo(CONFIG_FILE_NAME).isFile())) Save();

}

void Settings::Save()
{
    settings->beginGroup("General");
    settings->setValue("gpu_acceleration", GPUAcceleration);
    settings->setValue("video_window_name", videoWindowName);
    settings->setValue("trackbars_window_name", trackbarsWindowName);
    settings->endGroup();

    settings->beginGroup("Environment_options");
    settings->setValue("image_rotation_angle", cvImageRotationAngle);
    settings->setValue("image_roi_rect", QRect(cvImageROIRect.x, cvImageROIRect.y,
                                               cvImageROIRect.width, cvImageROIRect.height));
    settings->setValue("image_rotation_point", QPoint(cvImageRotationPoint.x, cvImageROIRect.y));
    settings->endGroup();

    settings->beginGroup("Camera_properties");
    settings->setValue("camera_address", cameraAddress);
    settings->setValue("camera_resolution", cameraResolution);
    settings->endGroup();

    settings->beginGroup("HoughCircle_parameters");
    settings->setValue("hough_parameter_1", houghParam1);
    settings->setValue("hough_parameter_2", houghParam2);
    settings->setValue("puck_min_radius", puckMinRadius);
    settings->setValue("puck_max_radius", puckMaxRadius);
    settings->endGroup();

    settings->beginGroup("Trajectory_extrapolation");
    settings->setValue("max_skipped_frames", maxSkippedFrames);
    settings->setValue("points_vector_length", pointsVectorLength);
    settings->endGroup();

    settings->beginGroup("Game_strategy");
    settings->setValue("robot_striker_position", robotStrikerPosition);
    settings->setValue("robot_motion_range", robotMotionRange);
    settings->endGroup();

    settings->sync();
}

void Settings::Load()
{
    settings->beginGroup("General");
    setGPUAcceleration(settings->value("gpu_acceleration", true).toBool());
    setVideoWindowName(settings->value("video_window_name", "KRAH VIDEO").toString());
    setTrackbarsWindowName(settings->value("trackbars_window_name", "KRAH TRACKBARS").toString());
    settings->endGroup();

    settings->beginGroup("Environment_options");
    setImageRotationAngle(settings->value("image_rotation_angle", 0).toFloat());
    QRect tmpRect = settings->value("image_roi_rect", QRect(0,0,1024,588)).toRect();
    cvImageROIRect = cv::Rect(tmpRect.x(), tmpRect.y(), tmpRect.width(), tmpRect.height());
    QPoint tmpPoint = settings->value("image_rotation_point", QPoint(-1,-1)).toPoint();
    cvImageRotationPoint = cv::Point(tmpPoint.x(), tmpPoint.y());
    settings->endGroup();

    settings->beginGroup("Camera_properties");
    setCameraAddress(settings->value("camera_address", 0).toInt());
    setCameraResolution(settings->value("camera_resolution", QSize(1024, 588)).toSize());
    settings->endGroup();

    settings->beginGroup("HoughCircle_parameters");
    setHoughParam1(settings->value("hough_parameter_1", 200).toInt());
    setHoughParam2(settings->value("hough_parameter_2", 26).toInt());
    setPuckMinRadius(settings->value("puck_min_radius", 13).toInt());
    setPuckMaxRadius(settings->value("puck_max_radius", 17).toInt());
    settings->endGroup();

    settings->beginGroup("Trajectory_extrapolation");
    setMaxSkippedFrames(settings->value("max_skipped_frames", 10).toInt());
    setPointsVectorLength(settings->value("points_vector_length", 10).toInt());
    settings->endGroup();

    settings->beginGroup("Game_strategy");
    setRobotStrikerPosition(settings->value("robot_striker_position", 0).toInt());
    setRobotMotionRange(settings->value("robot_motion_range", 0).toInt());
    settings->endGroup();
}

int Settings::getRobotMotionRange() const
{
    return robotMotionRange;
}

void Settings::setRobotMotionRange(int newRobotMotionRange)
{
    robotMotionRange = newRobotMotionRange;
}

int Settings::getRobotStrikerPosition() const
{
    return robotStrikerPosition;
}

void Settings::setRobotStrikerPosition(int newRobotStrikerPosition)
{
    robotStrikerPosition = newRobotStrikerPosition;
}
