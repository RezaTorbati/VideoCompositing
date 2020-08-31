#include <iostream>
#include <fstream>

#include "dewarpVideo.h"

int main(int argc, char** argv)
{
    int width = 640;
    int height = 480;
    DewarpVideo dewarper(.00001, 4);

    if(argc < 2) 
    {
        std::cout << "Error, not enough args" << std::endl;
    }  
    
    std::string imageFile = argv[1];
    cv::Mat fullImage = cv::imread(imageFile);
    //imwrite("OLD"+imageFile, fullImage(cv::Rect(0,0,width,height)));
    //imwrite("NEW"+imageFile, dewarper.dewarpFrame(fullImage(cv::Rect(0,0,width,height))));
    dewarper.dewarpFrame(fullImage(cv::Rect(0,0,width,height))).copyTo(fullImage(cv::Rect(0,0,width,height)));
    dewarper.dewarpFrame(fullImage(cv::Rect(width,0,width,height))).copyTo(fullImage(cv::Rect(width,0,width,height)));
    
    imwrite(imageFile, fullImage);
}
