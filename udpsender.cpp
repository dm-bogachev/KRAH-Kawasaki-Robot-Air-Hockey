#include "udpsender.h"

void UDPSender::UDPSend(QByteArray datagram)
{
    udpSocket.writeDatagram(datagram, QHostAddress::Broadcast, 49152);
}

UDPSender::UDPSender()
{

}

void UDPSender::moveTo(std::vector<cv::Scalar> points, double ylimit, double frameHeight)
{
    QString udpPackage;
    double pointPercent;
    udpPackage.push_back(QString::number(points.size()));
    udpPackage.push_back(":");
    for (int i = 0; i < points.size(); i++)
    {
        pointPercent = (double(points[i][0]) - ((frameHeight-2*ylimit)/2))/(2*ylimit);
        pointPercent *= 100;
        if (pointPercent > 100) {pointPercent = 100;}
        if (pointPercent < 0) {pointPercent = 0;}
        udpPackage.push_back(QString::number(pointPercent));
        udpPackage.push_back(";");
        udpPackage.push_back(QString::number(points[i][1]));
        if (i != points.size()-1) {udpPackage.push_back(":");}
    }
    UDPSend(udpPackage.toUtf8());
}
