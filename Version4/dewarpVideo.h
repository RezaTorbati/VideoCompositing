#include <opencv2/opencv.hpp>
#include <iostream>

class DewarpVideo
{
    public:
        DewarpVideo(double cCoef);
        cv::Mat dewarpFrame(cv::Mat frame);
    private:
        double calibrationCoef;
        cv::Vec3b getNewCoords(int x, int y);

};
