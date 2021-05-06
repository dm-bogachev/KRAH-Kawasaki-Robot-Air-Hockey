#include <QDebug>
#include <QSize>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/cudaimgproc.hpp>

#include <settings.h>

#define GPU_ACCELERATION programSettings.getGPUAcceleration()

#define VIDEO_WINDOW_NAME programSettings.getVideoWindowName().toStdString()
#define TRACKBARS_NAME programSettings.getTrackbarsWindowName().toStdString()

#define MAX_SKIPPED_FRAMES programSettings.getMaxSkippedFrames()
#define POINTS_VECTOR_LENGTH programSettings.getPointsVectorLength()

#define CV_COLOR_RED cv::Scalar(255,0,0)


void setupTrackbarsWindow(Settings &programSettings)
{
    cv::namedWindow(TRACKBARS_NAME, cv::WINDOW_AUTOSIZE);
    cv::createTrackbar("houghParam1", TRACKBARS_NAME, &programSettings.houghParam1, 1024);
    cv::createTrackbar("houghParam2", TRACKBARS_NAME, &programSettings.houghParam2, 1024);
    cv::createTrackbar("puckMinRadius", TRACKBARS_NAME, &programSettings.puckMinRadius, 500);
    cv::createTrackbar("puckMaxRadius", TRACKBARS_NAME, &programSettings.puckMaxRadius, 500);
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
    camera.set(cv::CAP_PROP_EXPOSURE, 5000);
}

void getCameraFrame(cv::VideoCapture &camera, cv::Mat &frame)
{

    if (!camera.isOpened())
    {
        exit(-100);
    }
    camera.read(frame);
    cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
    cv::cvtColor(cv::imread("pic.jpg"), frame, cv::COLOR_BGR2GRAY);
}

cv::Point3f findPuckCPU(cv::Mat &frame, Settings programSettings)
{
    std::vector<cv::Vec3f> circles;
    HoughCircles(frame, circles, cv::HOUGH_GRADIENT, 1, frame.rows/2,
                 programSettings.houghParam1, programSettings.houghParam2,
                 programSettings.puckMinRadius, programSettings.puckMaxRadius);

    if (circles.size() != 0)
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
    //programSettings.getTrackbarsWindowName().toStdString()
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

std::vector<cv::Scalar> getSpeedVector(std::vector<cv::Point3f> &puckTrajectoryPointsVector, cv::Scalar &puckAverageSpeed)
{
    cv::Scalar speed;
    cv::Point3f sumSpeed;
    cv::Point3f temp;
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

//    int sumSpeed = 0;
//    int speed;
//    std::vector<int> Speed;
//    if (coord == 'y')
//    {
//        for (uint8_t i = 1; i < Points.size(); i++)
//        {
//            speed = Points[i].y - Points[i - 1].y;
//            Speed.push_back(speed);
//            sumSpeed += speed;
//        }
//    }
//    else if (coord == 'x')
//    {
//        for (uint8_t i = 1; i < Points.size(); i++)
//        {
//            speed = Points[i].x - Points[i - 1].x;
//            Speed.push_back(speed);
//            sumSpeed += speed;
//        }
//    }
//    averageSpeed = sumSpeed/(signed int)Speed.size();
//    return Speed;
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
    cv::cuda::GpuMat GPUFrame;
    cv::Mat copiedFrame;
    // For puck detection
    cv::Point3f puckPoint;
    // For trajectory extrapolation
    int skippedFramesNumber = 0;
    std::vector<cv::Point3f> puckTrajectoryPointsVector;
    std::vector<cv::Scalar> puckSpeedVector;
    cv::Scalar puckAverageSpeed;
    // For performance counter
    int fps;
    int64 startingTime;
    int averageFPS;
    int countedFPS[255];
    for (uint8_t i = 0; i < 255; i++) {countedFPS[i] = 0;}

    // For writing video
    //cv::VideoWriter cleanOutput = VideoWriter(clearFname.toStdString(), -1, camera->get(CAP_PROP_FPS), S, false);
    //cv::VideoWriter procOutput;

    // Load all required settings and initialize
    programSettings.Load();
    cv::namedWindow(VIDEO_WINDOW_NAME, cv::WINDOW_KEEPRATIO);
    setupTrackbarsWindow(programSettings);
    setupCamera(camera, programSettings.getCameraAddress(), programSettings.getCameraResolution());

    while (true) {
        startingTime = cv::getTickCount();
        getCameraFrame(camera, capturedFrame);
        capturedFrame.copyTo(copiedFrame);
        puckPoint = findPuck(capturedFrame, GPUFrame, programSettings);


        // If at least one puck was found
        if (puckPoint.x != -1){
            puckTrajectoryPointsVector.push_back(puckPoint);
            if (puckTrajectoryPointsVector.size() < 2) {continue;}
            puckSpeedVector = getSpeedVector(puckTrajectoryPointsVector, puckAverageSpeed);
            if (checkDirectionChange(puckSpeedVector)) {puckTrajectoryPointsVector.clear();}
            cv::circle(capturedFrame, cv::Point(puckPoint.x, puckPoint.y), puckPoint.z, cv::Scalar(255,255,255), 2);
            //

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
        cv::putText(capturedFrame, std::to_string(averageFPS), cv::Point(20,20), cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(255,0,0));

        cv::imshow(VIDEO_WINDOW_NAME, capturedFrame);
        char pressedKey = cv::waitKey(1);
        switch (pressedKey) {
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
//cv::createTrackbar("MINY", TRACKBARS_NAME, &MINY, 2050);
//cv::createTrackbar("MAXY", TRACKBARS_NAME, &MAXY, 2050);
//cv::createTrackbar("ENDLINE", TRACKBARS_NAME, &ENDLINE, 2050);
//cv::createTrackbar("WORKING_DELTA", TRACKBARS_NAME, &WORKING_DELTA, 1024);
//cv::createTrackbar("GATE_DELTA", TRACKBARS_NAME, &GATE_DELTA, 1024);
//cv::createTrackbar("kx", TRACKBARS_NAME, &kx, 40000);
