#ifndef UDPSENDER_H
#define UDPSENDER_H

#include <QUdpSocket>
#include <opencv2/core.hpp>

#include <settings.h>

class UDPSender
{
private:
    QUdpSocket udpSocket;

    void UDPSend(QByteArray datagram);

    void moveTo(cv::Point point1, cv::Point point2, Settings programSettings, int frameHeight)
    {

//        QString package;
//        QString num;
//        double gateZoneLength = 2*GATE_DELTA;

//        double percent = (p.y - (frame.rows/2 - GATE_DELTA))/gateZoneLength;

//        percent = percent*100;

//        if (percent > 100)
//        {
//            percent = 100;
//        }
//        if (percent < 0)
//        {
//            percent = 0;
//        }

//        package.push_back(num.setNum(floor(percent)));
//        package.push_back(",");
//        num.clear();

//        if (p.x < ENDLINE)
//        {
//            package.push_back(num.setNum(100));
//        }
//        else
//        {
//            package.push_back(num.setNum(0));
//        }

//        UDPSend(package.toUtf8());
    }
public:
    UDPSender();
};

#endif // UDPSENDER_H
