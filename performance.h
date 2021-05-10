#ifndef PERFORMANCE_H
#define PERFORMANCE_H

#include <opencv2/core.hpp>

#define FPS_COUNTER_MAX_SIZE 16

class Performance
{
private:
    unsigned long long startingTime;
    unsigned int averageFPS;
    int FPSArray[FPS_COUNTER_MAX_SIZE];
    void addFPSValue(double fps);
    void calculateAverageFPS();

public:
    Performance();
    void resetCounter();
    void stopCounter();
    unsigned int getAverageFPS() const;
};

#endif // PERFORMANCE_H
