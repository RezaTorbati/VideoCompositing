#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

#include "dewarpVideo.h"

int main(int argc, char** argv)
{    
    if(argc < 6) 
    {
        std::cout << "Error, please pass the directory of calibrationMats, the calibration coefficient, the correction factor, the width and height and the fps as commandline arguments" << std::endl;
        return -1;
    } 
    
    //gets xMap and yMap from the calibration data
    std::ifstream calibrationIn((std::string(argv[1]) + "/calibrationMats.bin"), std::fstream::binary);
    
    //xMap info
    int calRows, calCols, calType;
    calibrationIn.read((char*)&calRows, sizeof(int));
    calibrationIn.read((char*)&calCols, sizeof(int));
    calibrationIn.read((char*)&calType, sizeof(int));
    cv::Mat xMap(calRows, calCols, calType);
    calibrationIn.read((char*)xMap.data, CV_ELEM_SIZE(calType) * calRows*calCols);
    
    //yMap info
    calibrationIn.read((char*)&calRows, sizeof(int));
    calibrationIn.read((char*)&calCols, sizeof(int));
    calibrationIn.read((char*)&calType, sizeof(int));
    cv::Mat yMap(calRows, calCols, calType);
    calibrationIn.read((char*)yMap.data, CV_ELEM_SIZE(calType) * calRows*calCols);
    
    //creates the dewarper object
    int width = std::stoi(argv[4]);
    int height = std::stoi(argv[5]);
    double correctionFactor = std::stod(argv[3]); 
    double calibrationCoef = std::stod(argv[2]);
    DewarpVideo dewarper(calibrationCoef, correctionFactor, width, height, CV_8UC3); 

    int count = 0;  
    cv::Mat fullImage(height*2, width*2, CV_8UC3);  
    cv::VideoWriter writer("correctedVideo.avi", cv::VideoWriter::fourcc('m','p','4','v'), std::stoi(argv[6]), cv::Size(width*2, height*2), true);
    cv::VideoCapture read("compressedVideo.avi"); //input file
    while(true)
    {
        read >> fullImage;
        if(!fullImage.data)
            break;
                
        dewarper.dewarpFrame(fullImage(cv::Rect(0,0,width,height)),xMap, yMap).copyTo(fullImage(cv::Rect(0,0,width,height))); //overwrite the top left quad with the corrected version of it
        dewarper.dewarpFrame(fullImage(cv::Rect(width,0,width,height)), xMap, yMap).copyTo(fullImage(cv::Rect(width,0,width,height))); //same but for the top right quad
        
        writer.write(fullImage); //write the corrected image to the file
        ++count; 
        if(count % 10 == 0 || count == 1)
            std::cout << "finished image " << count << std::endl;
    }
}
