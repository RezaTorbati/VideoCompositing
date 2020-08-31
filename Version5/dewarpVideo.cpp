#include "dewarpVideo.h"
#include <math.h>
#include <ctime>
#include <chrono>

DewarpVideo::DewarpVideo(double cCoef, double corFactor, int w, int h)
{
    calibrationCoef = cCoef; 
    correctionFactor = corFactor;
    width = w;
    height = h;
}

cv::Mat DewarpVideo::dewarpFrame(cv::Mat frame, cv::Mat xMap, cv::Mat yMap)
{
    frame.at<cv::Vec3b>(0,0) = 0; //sets this to zero so that I know of a black pixel that I can map to all of the outside pixels. Can change this to a different background color
    cv::resize(frame, frame, cv::Size(frame.cols * correctionFactor, frame.rows * correctionFactor));
    
    cv::remap(frame, frame, xMap, yMap, cv::INTER_LINEAR);
    cv::waitKey(1);
    
    //the mapping creates the smaller, corrected image in the top left corner of the picture so the rest needs cropped
    frame = frame(cv::Rect(0,0,getNewCoord(frame.cols, frame.rows,true), getNewCoord(frame.cols, frame.rows,false)));
    
    //goes back to the original size
    cv::resize(frame, frame, cv::Size(width, height));
    return frame;
}

cv::Mat DewarpVideo::getMap(bool xMap) //xMap is true to return map for x coords, false for y coords
{
    cv::Mat mappedImg(cv::Size(width * correctionFactor, height * correctionFactor), CV_32FC1, cv::Scalar(10000,10000,10000)); //10000 should be impossible normally so this is just so that I know where pixels that aren't being mapped to are so I can map them to the black pixel at 0,0
    
    int newXCoord, newYCoord;
    for(int i = 0; i < mappedImg.cols; i++)
    {
        for(int j = 0; j < mappedImg.rows; j++)
        {
            newXCoord = getNewCoord(i,j, true);
            newYCoord = getNewCoord(i,j, false);
            
            if(xMap)
                mappedImg.at<float>(newYCoord,newXCoord) = i;
            else
                mappedImg.at<float>(newYCoord,newXCoord) = j;
        } 
    }
    
    //changes all of the pixels at 10000 to be mapped to 0,0 which is black so the background is a solid black color
    for(int i = 0; i < mappedImg.cols; i++)
    {
        for(int j = 0; j < mappedImg.rows; j++)
        {
            if(mappedImg.at<float>(j,i) == 10000)
            {
                mappedImg.at<float>(j,i) = 0;
            }
        }
    }
    return mappedImg;
}

int DewarpVideo::getNewCoord(int x, int y, bool returnX)
{
    int cols = width * correctionFactor, rows = height * correctionFactor; //max cols and rows
    
    //convert to polar with the midpoint of the square being the origin
    x = x - (cols / 2); 
    y = y - (rows / 2);
    double r = (1 / correctionFactor) * sqrt(pow(x, 2) + pow(y,2));
    double theta = atan2(y, x);
    r = (r * (1.0 + calibrationCoef*r*r));

    //Finds the maximum r and theta so the program knows where to recenter the pixel when returning
    double maxR = (1 / correctionFactor) * sqrt(pow(cols/2, 2) + pow(rows/2,2));
    double maxTheta = atan2(rows/2, cols/2);
    maxR = (maxR  * (1.0 + calibrationCoef*maxR*maxR));

    //convert back, recenter the origin and return the point
    if(returnX)
        return round(r*cos(theta)) + round(maxR*cos(maxTheta));
    return round(r*sin(theta)) + round(maxR*sin(maxTheta));
}
