#include <iostream>
#include <fstream>
#include <vector>

#include "dewarpVideo.h"

int main(int argc, char** argv)
{
    int width = 640;
    int height = 480;
    double correctionFactor = 4.5; 
    
    DewarpVideo dewarper(.00001, correctionFactor, width, height);
    
    //creates the maps. This takes awhile
    cv::Mat xMap;
    cv::Mat yMap;
    xMap = dewarper.getMap(true);
    yMap = dewarper.getMap(false);
    
    if(argc < 2) 
    {
        std::cout << "Error, please pass number of images to correct as commandline argument" << std::endl;
        return -1;
    }  
    
    //gets all of the files from stdin
    std::vector<std::string> images;
    std::string curFile;
    for(int i = 0; i < std::stoi(argv[1]); i++)
    {
        std::getline(std::cin, curFile);
        images.push_back(curFile);
    }
     
    for(int i = 0; i < images.size(); i++)
    {   
        std::string imageFile = images[i];
        cv::Mat fullImage = cv::imread(imageFile);
        
        dewarper.dewarpFrame(fullImage(cv::Rect(0,0,width,height)),xMap, yMap).copyTo(fullImage(cv::Rect(0,0,width,height)));
        
        dewarper.dewarpFrame(fullImage(cv::Rect(width,0,width,height)), xMap, yMap).copyTo(fullImage(cv::Rect(width,0,width,height)));
        
        imwrite("../correctedImages/" + imageFile, fullImage);
        std::cout << "finished image " << imageFile << std::endl;
    }
}
