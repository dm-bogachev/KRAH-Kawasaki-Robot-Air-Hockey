#include "performance.h"

Performance::Performance()
{
    for (int i = 0; i < FPS_COUNTER_MAX_SIZE; i++) {FPSArray[i] = 0;}
}

void Performance::resetCounter()
{
    startingTime = cv::getTickCount();
}

unsigned int Performance::getAverageFPS() const
{
    return averageFPS;
}

void Performance::addFPSValue(double fps)
{
    for (int i = 1; i < FPS_COUNTER_MAX_SIZE; i++)
    {
        FPSArray[i-1] = FPSArray[i];
    }
    FPSArray[FPS_COUNTER_MAX_SIZE-1] = int(fps);
}

void Performance::calculateAverageFPS()
{
    unsigned int nonzero = 0;
    double sum = 0;
    for (int i = 0; i < FPS_COUNTER_MAX_SIZE; i++)
    {
        if (FPSArray[i] != 0)
        {
            sum += FPSArray[i];
            nonzero++;
        }
    }
    if (nonzero != 0)
    {
        averageFPS = (unsigned int)sum/nonzero;
    } else {
        averageFPS = 0;
    }
}

void Performance::stopCounter()
{
    double fps = cv::getTickFrequency() / (cv::getTickCount() - startingTime);
    addFPSValue(fps);
    calculateAverageFPS();
}
