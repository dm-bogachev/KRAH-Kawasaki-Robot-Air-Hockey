#include <QDebug>
#include <QSize>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/cudaimgproc.hpp>

#include <settings.h>

#define GPU_ACCELERATION programSettings.getGPUAcceleration()

#define VIDEO_WINDOW_NAME programSettings.getVideoWindowName().toStdString()
#define TRACKBARS_NAME programSettings.getTrackbarsWindowName().toStdString()

#define MAX_SKIPPED_FRAMES programSettings.getMaxSkippedFrames()
#define POINTS_VECTOR_LENGTH programSettings.getPointsVectorLength()

#define ROBOT_STRIKER_POSITION programSettings.getRobotStrikerPosition()
#define MAX_ROBOT_REACH programSettings.getRobotStrikerPosition() - programSettings.getRobotMotionRange()

#define pi 3.141592653589793238463

#define CV_COLOR_GREEN cv::Scalar(0,255,0)
#define CV_COLOR_RED cv::Scalar(255,0,0)
#define CV_COLOR_BLUE cv::Scalar(0,0,255)
#define CV_COLOR_WHITE cv::Scalar(255,255,255)
#define CV_COLOR_BLACK cv::Scalar(0,0,0)

struct pointsSortingStructure
{
    inline bool operator() (const cv::Point2f& struct1, const cv::Point2f& struct2)
    {
        return struct1.x*struct1.x + struct1.y*struct1.y < struct2.x*struct2.x + struct2.y*struct2.y;
    }
};

cv::Point2f getRectangleCenter(std::vector<cv::Point2f> rectangleCorners)
{
    cv::Point2f rectangleCenter(0.f, 0.f);
    for(int i = 0; i < 4; i++)
    {
        rectangleCenter += rectangleCorners[i];
    }
    return rectangleCenter /= 4.f;
}

void detectArucoMarkers(cv::Mat inputImage, std::vector<std::vector<cv::Point2f>> &markerCorners, std::vector<int> &markerIds)
{
    std::vector<std::vector<cv::Point2f>> rejectedCandidates;
    cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_250);

    // Invert image because markers are white
    cv::Mat invertedImage;
    cv::bitwise_not(inputImage, invertedImage);

    cv::aruco::detectMarkers(invertedImage, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);
    qDebug() << markerCorners.size() << "corners was detected";
}

std::vector<cv::Point> getArucoMarkersCenters(std::vector<std::vector<cv::Point2f>> markerCorners)
{
    std::vector<cv::Point> markerCenters;
    for (int i = 0; i < 4; i++){markerCenters.push_back(getRectangleCenter(markerCorners[i]));}
    std::sort(markerCenters.begin(), markerCenters.end(), pointsSortingStructure());
    return markerCenters;
}

double getImageRotationAngle(std::vector<cv::Point> markerCenters)
{
/*
    0-----------------2
    |                 |
    |                 |
    |                 |
    |                 |
    1-----------------3
*/
    double rotationAngleRad = atan(double(markerCenters[0].y - markerCenters[2].y)/double(markerCenters[0].x - markerCenters[2].x));
    return rotationAngleRad*180/pi;
}

void rotateMatByAngle(cv::Mat &mat, double angleDeg, cv::Point2f rotationPoint)
{
    // Get rotation matrix for rotating the image around its center in pixel coordinates
    cv::Point2f center((mat.cols-1)/2.0, (mat.rows-1)/2.0);
    cv::Mat rot = cv::getRotationMatrix2D(rotationPoint, angleDeg, 1.0);
    // Determine bounding rectangle, center not relevant
    cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), mat.size(), angleDeg).boundingRect2f();
    // Adjust transformation matrix
    rot.at<double>(0,2) += bbox.width/2.0 - mat.cols/2.0;
    rot.at<double>(1,2) += bbox.height/2.0 - mat.rows/2.0;
    cv::warpAffine(mat, mat, rot, bbox.size());
}

cv::Rect detectTableBorders(cv::Mat &frame, double &frameRotationAngle, cv::Point &frameRotationPoint)
{
    // For AruCo marker detection
    std::vector<std::vector<cv::Point2f>> markerCorners;
    std::vector<int> markerIds;
    std::vector<cv::Point> markerCenters;
    detectArucoMarkers(frame, markerCorners, markerIds);
    if (markerCorners.size() == 4)
    {
        markerCenters = getArucoMarkersCenters(markerCorners);
        frameRotationAngle = getImageRotationAngle(markerCenters);
        rotateMatByAngle(frame, frameRotationAngle, markerCenters[0]);
        detectArucoMarkers(frame, markerCorners, markerIds);
        if (markerCorners.size() == 4){
            markerCenters = getArucoMarkersCenters(markerCorners);
            frameRotationPoint = markerCenters[0];
            return cv::Rect(markerCenters[0], markerCenters[3]);
        }
    }
    frameRotationAngle = 0;
    frameRotationPoint = cv::Point(-1,-1);
    return cv::Rect(0,0,2048, 1088);
}

void setupTrackbarsWindow(Settings &programSettings)
{
    cv::namedWindow(TRACKBARS_NAME, cv::WINDOW_AUTOSIZE);
    cv::createTrackbar("houghParam1", TRACKBARS_NAME, &programSettings.houghParam1, 1024);
    cv::createTrackbar("houghParam2", TRACKBARS_NAME, &programSettings.houghParam2, 1024);
    cv::createTrackbar("puckMinRadius", TRACKBARS_NAME, &programSettings.puckMinRadius, 500);
    cv::createTrackbar("puckMaxRadius", TRACKBARS_NAME, &programSettings.puckMaxRadius, 500);
    cv::createTrackbar("robotStrikerPosition", TRACKBARS_NAME,
                       &programSettings.robotStrikerPosition,
                       programSettings.getCameraResolution().width());
    cv::createTrackbar("robotMotionRange", TRACKBARS_NAME,
                       &programSettings.robotMotionRange,
                       programSettings.getCameraResolution().width());
}

void showInfo(Settings programSettings)
{

    cv::Mat info = cv::Mat::ones(cv::Size(300,300), CV_8U);
    cv::putText(info, "Welcome to KRAH", cv::Point(5,20), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255,0,0));
    cv::imshow(TRACKBARS_NAME, info);
    cv::waitKey(1);
}

void setupCamera(cv::VideoCapture &camera, int cameraAddress, QSize cameraResolution)
{
    camera = cv::VideoCapture(cameraAddress, cv::CAP_DSHOW);

    if (!camera.isOpened())
    {
        qDebug() << "Error opening camera!";
        exit(-1);
    }
    camera.set(cv::CAP_PROP_FRAME_WIDTH, cameraResolution.width());
    camera.set(cv::CAP_PROP_FRAME_HEIGHT, cameraResolution.height());
    camera.set(cv::CAP_PROP_FPS, 90);
    //qcamera.set(cv::CAP_PROP_EXPOSURE, -5000);
}

void getCameraFrame(cv::VideoCapture &camera, cv::Mat &frame)
{

    if (!camera.isOpened())
    {
        exit(-100);
    }
    camera.read(frame);
    cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
}

cv::Point3f findPuckCPU(cv::Mat &frame, Settings programSettings)
{
    std::vector<cv::Vec3f> circles;
    HoughCircles(frame, circles, cv::HOUGH_GRADIENT, 1, frame.rows/2,
                 programSettings.houghParam1, programSettings.houghParam2,
                 programSettings.puckMinRadius, programSettings.puckMaxRadius);

    if (circles.size() == 0)
    {
        return cv::Point3f(-1, -1, -1);
    }

    cv::Point3f center(cvRound(circles[0][0]), cvRound(circles[0][1]), cvRound(circles[0][2]));
    qDebug() << "Detected puck with raduis: " << cvRound(circles[0][2]);

    return center;
}

cv::Point3f fingPuckGPU(cv::cuda::GpuMat frame, Settings programSettings)
{
    std::vector<cv::Vec3f> gpuCircles;
    cv::cuda::GpuMat gpuResult;
    cv::Ptr<cv::cuda::HoughCirclesDetector> gpuDetector = cv::cuda::createHoughCirclesDetector(1, frame.rows/2,
                                                                                               programSettings.houghParam1, programSettings.houghParam2,
                                                                                               programSettings.puckMinRadius, programSettings.puckMaxRadius);
    gpuDetector->detect(frame, gpuResult);
    gpuCircles.resize(gpuResult.size().width);
    if (!gpuCircles.empty())
        gpuResult.row(0).download(cv::Mat(gpuCircles).reshape(3, 1));

    if (gpuCircles.size() == 0)
    {
        return cv::Point3f(-1, -1, -1);
    }

    cv::Point3f center(cvRound(gpuCircles[0][0]), cvRound(gpuCircles[0][1]), cvRound(gpuCircles[0][2]));
    qDebug() << "Detected puck with raduis: " << cvRound(gpuCircles[0][2]);

    return center;
}

cv::Point3f findPuck(cv::Mat &capturedFrame, cv::cuda::GpuMat &GPUFrame, Settings programSettings)
{
    cv::Point3f puckPoint;
    if (GPU_ACCELERATION)
    {
        GPUFrame.upload(capturedFrame);
        puckPoint = fingPuckGPU(GPUFrame, programSettings);
        if (puckPoint.x != -1){
            GPUFrame.download(capturedFrame);
        }

    } else {
        puckPoint = findPuckCPU(capturedFrame,programSettings);
    }
    return puckPoint;
}

std::vector<cv::Scalar> getSpeedVector(std::vector<cv::Point> &puckTrajectoryPointsVector, cv::Scalar &puckAverageSpeed)
{
    cv::Scalar speed;
    cv::Point sumSpeed;
    cv::Point temp;
    std::vector<cv::Scalar> puckSpeedVector;

    for (int i = 1; i < puckTrajectoryPointsVector.size(); i++)
    {
        temp = puckTrajectoryPointsVector[i] - puckTrajectoryPointsVector[i-1];
        speed = cv::Scalar(temp.x, temp.y);
        puckSpeedVector.push_back(speed);
        sumSpeed += temp;
    }
    temp = sumSpeed/(signed int)puckSpeedVector.size();
    puckAverageSpeed = cv::Scalar(temp.x, temp.y);
    return puckSpeedVector;
}

bool checkDirectionChange(std::vector<cv::Scalar> puckSpeedVector)
{
    int positiveX = 0;
    int negativeX = 0;
    int positiveY = 0;
    int negativeY = 0;

    if (puckSpeedVector.size() <= 2)
        return false;

    for (uint8_t i = 0; i < puckSpeedVector.size(); i++)
    {
        if (puckSpeedVector[i][0] > 0)
        {
            positiveX++;
        }
        else if (puckSpeedVector[i][0] < 0)
        {
            negativeX++;
        }
        if (puckSpeedVector[i][1] > 0)
        {
            positiveY++;
        }
        else if (puckSpeedVector[i][1] < 0)
        {
            negativeY++;
        }
    }
    return !(positiveX == 0 || negativeX == 0) || !(positiveY == 0 || negativeY == 0);
}

cv::Vec2f getExtrapolationCoefficients(const std::vector<cv::Point> &pointsVector)
{
    int sumy = 0, sumx = 0, sumx2 = 0, sumxy = 0;
    float n = pointsVector.size(), a, b;

    for (int i = 0; i < n; i++)
    {
        sumy += pointsVector[i].y;
        sumx += pointsVector[i].x;
        sumx2 += pointsVector[i].x*pointsVector[i].x;
        sumxy += pointsVector[i].x*pointsVector[i].y;
    }

    a = (sumxy - (sumx*sumy/n))/(sumx2 - sumx*sumx/n);
    b = (sumy - a*sumx)/n;

    return cv::Vec2f(a, b);
}

bool in_Range(int x, int a, int b)
{
    return (x >= a)&&(x <= b);
}

cv::Point predictPosition(std::vector<cv::Point> &puckTrajectoryPointsVector,
                          int predictionPointX,
                          cv::Vec2f extrapolationCoefficients,
                          cv::Size frameSize)
{
    if (puckTrajectoryPointsVector.size() < 2){return cv::Point(-1, -1);}

    float a = extrapolationCoefficients[0], b = extrapolationCoefficients[1]; // for simplier using
    int predictionPointY, newx = predictionPointX, hitCount = 0;
    cv::Point cp = puckTrajectoryPointsVector[puckTrajectoryPointsVector.size()-1];
    cv::Point lp = puckTrajectoryPointsVector[puckTrajectoryPointsVector.size()-2];

    do
    {
        predictionPointY = a*predictionPointX + b;
        if (!in_Range(predictionPointY, 0, frameSize.height))
        {
            hitCount++;
             if (a > 0)
             {
                 newx = (frameSize.height-b)/a;
                 a = -a;
                 b = frameSize.height-a*newx;
             } else
             {
                 newx = (0-b)/a;
                 a = -a;
                 b = 0-a*newx;
             }
             lp = cp;
             cp = cv::Point(newx, predictionPointY);

             if (hitCount > 5){return cv::Point(-1, -1);}
        }
        else
        {
            if (hitCount >= 2){return cv::Point(-1, -1);}
            return cv::Point(predictionPointX, predictionPointY);
        }
    } while(true);
}

void addFPSValue(int *FPSArray, int FPS)
{
    for (uint8_t i = 1; i < 255; i++)
    {
        FPSArray[i-1] = FPSArray[i];
    }
    FPSArray[254] = int(FPS);
}

int calculateAverageFPS(const int *countedFPS)//[255])
{
    uint8_t nonzero = 0;
    int sum = 0;
    for (uint8_t i = 0; i < 255; i++)
    {
        if (countedFPS[i] != 0)
        {
            sum += countedFPS[i];
            nonzero++;
        }
    }
    if (nonzero != 0) return sum/nonzero;
    return 0;
}

int main()
{
    // Define all variables
    // General
    Settings programSettings;
    // General Mat ang GpuMat variables
    cv::VideoCapture camera;
    cv::Mat capturedFrame;
    cv::Mat ROIFrame;
    cv::cuda::GpuMat GPUFrame;
    cv::Mat copiedFrame;
    cv::Size ROIFrameSize;
    // For table rotation and positioning
    cv::Rect ROI = cv::Rect(0, 0, 2048, 1088);
    double frameRotationAngle = 0;
    cv::Point frameRotationPoint = cv::Point(-1,-1);
    // For puck detection
    cv::Point3f puckPoint;
    // For trajectory extrapolation
    int skippedFramesNumber = 0;
    std::vector<cv::Point> puckTrajectoryPointsVector;
    std::vector<cv::Scalar> puckSpeedVector;
    cv::Scalar puckAverageSpeed;
    // For performance counter
    int fps;
    int64 startingTime;
    int averageFPS;
    int countedFPS[255];
    for (uint8_t i = 0; i < 255; i++) {countedFPS[i] = 0;}
    // For puck processing
    cv::Vec2f extrapolationCoefficients;
    cv::Point predictedPointRSP;
    cv::Point predictedPointMRR;
    // Load all required settings and initialize
    programSettings.Load();
    cv::namedWindow(VIDEO_WINDOW_NAME, cv::WINDOW_KEEPRATIO);
    setupTrackbarsWindow(programSettings);
    setupCamera(camera, programSettings.getCameraAddress(), programSettings.getCameraResolution());

    while (true) {
            startingTime = cv::getTickCount();
            getCameraFrame(camera, capturedFrame);
            capturedFrame.copyTo(copiedFrame);
            if (frameRotationPoint.x != -1)
            {
                rotateMatByAngle(capturedFrame, frameRotationAngle, frameRotationPoint);
                ROIFrame = cv::Mat(capturedFrame, ROI);
            }
            else
            {
                ROIFrame = capturedFrame;
            }
            ROIFrameSize = cv::Size(ROIFrame.cols, ROIFrame.rows);

            puckPoint = findPuck(ROIFrame, GPUFrame, programSettings);

            // If at least one puck was found
            if (puckPoint.x != -1){
                puckTrajectoryPointsVector.push_back(cv::Point(puckPoint.x, puckPoint.y));
                if (puckTrajectoryPointsVector.size() < 2) {continue;}
                puckSpeedVector = getSpeedVector(puckTrajectoryPointsVector, puckAverageSpeed);
                if (checkDirectionChange(puckSpeedVector)) {puckTrajectoryPointsVector.clear();}
                //predictedPointRSP;
                extrapolationCoefficients = getExtrapolationCoefficients(puckTrajectoryPointsVector);
                predictedPointRSP = predictPosition(puckTrajectoryPointsVector, MAX_ROBOT_REACH,
                                                    extrapolationCoefficients, ROIFrameSize);
                // Check puck direction
                if (puckSpeedVector[puckSpeedVector.size()-1][0] > 0)
                {
                    // Puck direction forward
                    if (puckPoint.x < MAX_ROBOT_REACH)
                    { // We need to reflect puck

                    } else
                    { // We need to attack puck

                    }
                } else
                {
                    // Puck direction backward
                    if (puckPoint.x < MAX_ROBOT_REACH)
                    { // We need to attack puck

                    } // No else, because we don't care if puck moves backward and we can't reach it
                }
                //
                skippedFramesNumber = 0;
            } else
            {
                skippedFramesNumber++;
                if (skippedFramesNumber > MAX_SKIPPED_FRAMES)
                {
                    //moveTo(Frame, HOMEPOINT);
                    puckTrajectoryPointsVector.clear();
                }
            }

            // Ring puckTrajectoryPointsVector buffer overflow
            if (puckTrajectoryPointsVector.size() == POINTS_VECTOR_LENGTH)
            {
                puckTrajectoryPointsVector.erase(puckTrajectoryPointsVector.begin());
            }

            fps = cv::getTickFrequency() / (cv::getTickCount() - startingTime);
            addFPSValue(countedFPS, fps);
            averageFPS = calculateAverageFPS(countedFPS);

            cv::cvtColor(ROIFrame, ROIFrame, cv::COLOR_GRAY2RGB);
            if (puckPoint.x != -1){cv::circle(ROIFrame, cv::Point(puckPoint.x, puckPoint.y), puckPoint.z, CV_COLOR_GREEN, 2);}
            for (uint i = 0; i < puckTrajectoryPointsVector.size(); i++){cv::drawMarker(ROIFrame, puckTrajectoryPointsVector[i], CV_COLOR_BLUE);}
            cv::line(ROIFrame,
                     cv::Point(ROBOT_STRIKER_POSITION,0),
                     cv::Point(ROBOT_STRIKER_POSITION,ROIFrameSize.height),
                     CV_COLOR_BLUE, 2);
            cv::line(ROIFrame,
                     cv::Point(MAX_ROBOT_REACH,0),
                     cv::Point(MAX_ROBOT_REACH, ROIFrameSize.height),
                     CV_COLOR_BLUE, 2);
            if (predictedPointRSP.x != -1){cv::circle(ROIFrame, predictedPointRSP, 20, CV_COLOR_WHITE);}
//            cv::line(ROIFrame, cv::Point(0, extrapolationCoefficients[0]*0 + extrapolationCoefficients[1]),
//                    cv::Point(ROIFrameSize.width, extrapolationCoefficients[0]*ROIFrameSize.width + extrapolationCoefficients[1]),
//                    CV_COLOR_BLUE, 2);
//            cv::line(ROIFrame, cv::Point(puckPoint.x, puckPoint.y),
//                    predictedPointRSP, CV_COLOR_BLUE, 2);
            cv::putText(ROIFrame, std::to_string(averageFPS), cv::Point(80,80), cv::FONT_HERSHEY_SIMPLEX, 4, CV_COLOR_GREEN);
            cv::imshow(VIDEO_WINDOW_NAME, ROIFrame);

            char pressedKey = cv::waitKey(1);
            switch (pressedKey) {
            case 'c':
                ROI = detectTableBorders(copiedFrame, frameRotationAngle, frameRotationPoint);
                break;
            case 'h':
                showInfo(programSettings);
                break;
            case 'q':
                camera.release();
                return 0;
            case 's':
                programSettings.Save();
                qDebug() << "Settings were successfully saved";
                break;
            case 'l':
                programSettings.Load();
                cv::destroyWindow(TRACKBARS_NAME);
                setupTrackbarsWindow(programSettings);
                qDebug() << "Settings were successfully loaded";
                break;
            }
    }
    return 0;
}



//cv::createTrackbar("p1", TRACKBARS_NAME, &p1, 255);
//cv::createTrackbar("p2", TRACKBARS_NAME, &p2, 255);
//cv::createTrackbar("min", TRACKBARS_NAME, &minRadius, 500);
//cv::createTrackbar("max", TRACKBARS_NAME, &maxRadius, 500);
//cv::createTrackbar("MINY",q TRACKBARS_NAME, &MINY, 2050);
//cv::createTrackbar("MAXY", TRACKBARS_NAME, &MAXY, 2050);
//cv::createTrackbar("ENDLINE", TRACKBARS_NAME, &ENDLINE, 2050);
//cv::createTrackbar("WORKING_DELTA", TRACKBARS_NAME, &WORKING_DELTA, 1024);
//cv::createTrackbar("GATE_DELTA", TRACKBARS_NAME, &GATE_DELTA, 1024);
//cv::createTrackbar("kx", TRACKBARS_NAME, &kx, 40000);
