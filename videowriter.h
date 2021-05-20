#ifndef VIDEOWRITER_H
#define VIDEOWRITER_H

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <settings.h>
#include <QDir>
#include <QDateTime>

class VideoWriter
{
private:
    cv::VideoWriter videoWriter;
    bool isInitialized;
    QString folder;
    void init(cv::Size frameSize);
public:
    VideoWriter(QString saveFolderName);
    void write(cv::Mat frame);
    void release();
};

#endif // VIDEOWRITER_H
