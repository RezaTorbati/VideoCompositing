#include <iostream>
#include <fstream>
#include <vector>

#include "dewarpVideo.h"

int main(int argc, char** argv)
{
    if(argc < 6)
    {
        std::cout << "Error: please pass write folder, width, height, correctionFactor and calibrationCoefficiant as commandline args" << std::endl;
    }

    int width = std::stoi(argv[2]);
    int height = std::stoi(argv[3]);
    double correctionFactor = std::stod(argv[4]); 
    double calibrationCoef = std::stod(argv[5]);

    DewarpVideo dewarper(calibrationCoef, correctionFactor, width, height, CV_8UC3); //type doesn't matter for this one. It is 32FC1 for the calibration mats regardless
    
    //creates the maps. This takes a bit
    cv::Mat xMap;
    cv::Mat yMap;
    xMap = dewarper.getMap(true);
    yMap = dewarper.getMap(false);
    
    std::ofstream out(std::string(argv[1]) + "/calibrationMats.bin", std::fstream::binary); //used to write the binary information of the maps to a file
    
    //writes the x map binary information
    int xType = xMap.type();
    out.write((char*)&xMap.rows, sizeof(int));
    out.write((char*)&xMap.cols, sizeof(int));
    out.write((char*)&xType, sizeof(int));
    out.write(xMap.ptr<char>(0), xMap.rows*xMap.cols*CV_ELEM_SIZE(xMap.type()));
        
    //writes the y map binary information
    int yType = yMap.type();
    out.write((char*)&yMap.rows, sizeof(int));
    out.write((char*)&yMap.cols, sizeof(int));
    out.write((char*)&yType, sizeof(int));
    out.write(yMap.ptr<char>(0), yMap.rows*yMap.cols*CV_ELEM_SIZE(yMap.type()));

    out.close();
}
