#include "dewarpVideo.h"
#include <math.h>

DewarpVideo::DewarpVideo(double cCoef)
{
    calibrationCoef = cCoef; 
}

cv::Mat DewarpVideo::dewarpFrame(cv::Mat frame)
{
    cv::Mat dewarpedFrame(frame.rows, frame.cols, CV_8UC3); //TODO:figure out the true size for the new image
    for(int i = 0; i < frame.cols; i++)
    {
        for(int j = 0; j < frame.rows; j++)
        {
            dewarpedFrmae.at<cv::Vec3b>(i,j) = getNewCoords(frame, i, j);
        }
    }
}

cv::Vec3b getNewCoords(cv::Mat frame, int x, int y)
{
    //convert to polar
    double r = sqrt(pow(x, 2) + pow(y,2))
    double theta = atan2(j, i)
    
    //multiply r by coef
    r *= calibrationCoef;
    
    //convert back and return point
    return frame.at<cv::Vec3b>(r*cos(theta), r*sin(theta));
}
