#include "dewarpVideo.h"
#include <math.h>
#include <ctime>
#include <chrono>

DewarpVideo::DewarpVideo(double cCoef, double corFactor, int w, int h, int type)
{
    calibrationCoef = cCoef; 
    correctionFactor = corFactor;
    width = w;
    height = h;
    largeFrame = cv::Mat(w * corFactor, h * corFactor, type);
    fixedLargeFrame = cv::Mat(w*corFactor, h*corFactor, type);
    croppedFrame = cv::Mat(getNewCoord(largeFrame.cols, largeFrame.rows,true), getNewCoord(largeFrame.cols, largeFrame.rows,false), type);
}

cv::Mat DewarpVideo::dewarpFrame(cv::Mat frame, cv::Mat xMap, cv::Mat yMap)
{
    frame.at<cv::Vec3b>(0,0) = 0; //sets this to zero so that I know of a black pixel that I can map to all of the outside pixels. Can change this for a different background color
    
    //std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
    //resizes the image which will get rid of tearing
    cv::resize(frame, largeFrame, cv::Size(frame.cols * correctionFactor, frame.rows * correctionFactor));
    
    //std::chrono::steady_clock::time_point time1 = std::chrono::steady_clock::now();
    //remaps the pixels of the image according to the x and y maps
    cv::remap(largeFrame, fixedLargeFrame, xMap, yMap, cv::INTER_LINEAR); 
    
    //std::chrono::steady_clock::time_point time2 = std::chrono::steady_clock::now();
    //the mapping creates the smaller, corrected image in the top left corner of the picture so the rest needs cropped
    croppedFrame = fixedLargeFrame(cv::Rect(0,0,getNewCoord(largeFrame.cols, largeFrame.rows,true), getNewCoord(largeFrame.cols, largeFrame.rows,false)));
    
    //std::chrono::steady_clock::time_point time3 = std::chrono::steady_clock::now();
    //goes back to the original size which gets rid of the tearing
    cv::resize(croppedFrame, frame, cv::Size(width, height));
    
    /*
    std::chrono::steady_clock::time_point time4 = std::chrono::steady_clock::now();
    
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(time1 - startTime).count() << "ms for first resize" << std::endl;
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1).count() << "ms for remap" << std::endl;
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(time3 - time2).count() << "ms for crop" << std::endl;
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(time4 - time3).count() << "ms for second resize" << std::endl << std::endl;*/
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
