#ifndef SORTCVPOINTS_H
#define SORTCVPOINTS_H

#include <opencv2/core.hpp>

struct pointsSortingStructure
{
    inline bool operator() (const cv::Point2f& struct1, const cv::Point2f& struct2)
    {
        return struct1.x*struct1.x + struct1.y*struct1.y < struct2.x*struct2.x + struct2.y*struct2.y;
    }
};

#endif // SORTCVPOINTS_H
