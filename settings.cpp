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

int Settings::getImageRotationAngle() const
{
    return imageRotationAngle;
}

void Settings::setImageRotationAngle(int newImageRotationAngle)
{
    imageRotationAngle = newImageRotationAngle;
}

QPoint Settings::getTableROIPoint1() const
{
    return tableROIPoint1;
}

void Settings::setTableROIPoint1(QPoint newTableROIPoint1)
{
    tableROIPoint1 = newTableROIPoint1;
}

QPoint Settings::getTableROIPoint2() const
{
    return tableROIPoint2;
}

void Settings::setTableROIPoint2(QPoint newTableROIPoint2)
{
    tableROIPoint2 = newTableROIPoint2;
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
    settings->setValue("image_rotation_angle", imageRotationAngle);
    settings->setValue("table_roi_point_1", tableROIPoint1);
    settings->setValue("table_roi_point_2", tableROIPoint2);
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
    setImageRotationAngle(settings->value("image_rotation_angle", 0).toInt());
    setTableROIPoint1(settings->value("table_roi_point_1", QPoint(0,0)).toPoint());
    setTableROIPoint2(settings->value("table_roi_point_2", QPoint(cameraResolution.width(), cameraResolution.height())).toPoint());
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
    setTableROIPoint2(QPoint(cameraResolution.width(), cameraResolution.height()));

}
//int param1, int param2, int minRadius, int maxRadius
