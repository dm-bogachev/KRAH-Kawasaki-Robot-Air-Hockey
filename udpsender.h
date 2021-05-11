#ifndef UDPSENDER_H
#define UDPSENDER_H

#include <QUdpSocket>
#include <opencv2/core.hpp>

#include <settings.h>
#define YLIMIT programSettings.puckPositionYLimit

class UDPSender
{
private:
    QUdpSocket udpSocket;

    void UDPSend(QByteArray datagram);

    public:
    UDPSender();
    void moveTo(std::vector<cv::Scalar> points, double ylimit, double frameHeight);

};

#endif // UDPSENDER_H
