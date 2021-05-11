#include "settings.h"

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
    settings->setValue("debug_video_path", debugVideoPath);
    settings->endGroup();

    settings->beginGroup("Environment_options");
    settings->setValue("image_rotation_angle", cvImageRotationAngle);
    settings->setValue("image_roi_rect", QRect(cvImageROIRect.x, cvImageROIRect.y,
                                               cvImageROIRect.width, cvImageROIRect.height));
    settings->setValue("image_rotation_point", QPoint(cvImageRotationPoint.x, cvImageROIRect.y));
    settings->setValue("puck_position_y_limit", puckPositionYLimit);
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
    settings->setValue("puck_speed_slow", puckSpeedSlow);
    settings->setValue("k_p", Kp);
    settings->endGroup();

    settings->sync();
}

void Settings::Load()
{
    settings->beginGroup("General");
    GPUAcceleration = settings->value("gpu_acceleration", true).toBool();
    videoWindowName = settings->value("video_window_name", "KRAH VIDEO").toString();
    trackbarsWindowName = settings->value("trackbars_window_name", "KRAH TRACKBARS").toString();
    debugVideoPath = settings->value("debug_video_path", "").toString();
    settings->endGroup();

    settings->beginGroup("Environment_options");
    cvImageRotationAngle = settings->value("image_rotation_angle", 0).toFloat();

    QRect tmpRect = settings->value("image_roi_rect", QRect(0,0,1024,588)).toRect();
    cvImageROIRect = cv::Rect(tmpRect.x(), tmpRect.y(), tmpRect.width(), tmpRect.height());
    QPoint tmpPoint = settings->value("image_rotation_point", QPoint(-1,-1)).toPoint();
    cvImageRotationPoint = cv::Point(tmpPoint.x(), tmpPoint.y());
    //tmpPoint = settings->value("puck_position_y_limit", QPoint(0, tmpRect.height())).toPoint();
    puckPositionYLimit = settings->value("puck_position_y_limit", 0).toInt();

    settings->endGroup();

    settings->beginGroup("Camera_properties");
    cameraAddress = settings->value("camera_address", 0).toInt();
    cameraResolution = settings->value("camera_resolution", QSize(1024, 588)).toSize();
    settings->endGroup();

    settings->beginGroup("HoughCircle_parameters");
    houghParam1 = settings->value("hough_parameter_1", 200).toInt();
    houghParam2 = settings->value("hough_parameter_2", 26).toInt();
    puckMinRadius = settings->value("puck_min_radius", 13).toInt();
    puckMaxRadius = settings->value("puck_max_radius", 17).toInt();
    settings->endGroup();

    settings->beginGroup("Trajectory_extrapolation");
    maxSkippedFrames = settings->value("max_skipped_frames", 10).toInt();
    pointsVectorLength = settings->value("points_vector_length", 10).toInt();
    settings->endGroup();

    settings->beginGroup("Game_strategy");
    robotStrikerPosition = settings->value("robot_striker_position", 0).toInt();
    robotMotionRange = settings->value("robot_motion_range", 0).toInt();
    puckSpeedSlow = settings->value("puck_speed_slow", 20).toInt();
    Kp = settings->value("k_p", 10000).toInt();
    settings->endGroup();
}
