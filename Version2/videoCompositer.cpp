#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <ctime>
#include <chrono>
#include <fstream>

int main()
{
    const int WIDTH = 352; //best resolution the cameras give that the pi can handle
    const int HEIGHT = 288;
    const int FPS = 20; //TODO: this is not perfect so make sure to ask
    
    std::vector<cv::VideoCapture> cams;
    cams.push_back(new cv::VideoCapture cap("/dev/video0"));
    if(!cams.back.isOpened())
        cams.erase(cams.back);
}
