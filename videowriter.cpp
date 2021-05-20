#include "videowriter.h"

void VideoWriter::init(cv::Size frameSize)
{
    QString filename = folder;
    QDateTime current = QDateTime::currentDateTime();
    filename = filename.append("/rec").append(current.toString("dd.MM.yyyy_hh.mm.ss")).append(".avi");
    videoWriter = cv::VideoWriter(filename.toStdString(),
                                  cv::VideoWriter::fourcc('M','J','P','G'),
                                  30, frameSize, false);
    isInitialized = true;
}
VideoWriter::VideoWriter(QString saveFolderName)
{
    folder = saveFolderName;
    QDir().mkpath(folder);
    isInitialized = false;
}

void VideoWriter::write(cv::Mat frame)
{
    if (!isInitialized){init(cv::Size(frame.cols, frame.rows));}
    videoWriter.write(frame);
}

void VideoWriter::release()
{
    videoWriter.release();
    isInitialized = false;
}


