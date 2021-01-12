//This program reads in the raw data collected by videoCompositer and compresses it into an avi, mp4v file.

#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

#include "dewarpVideo.h"

int main(int argc, char** argv)
{    
    if(argc < 3) 
    {
        std::cout << "Error, please pass the width, height and the fps as commandline arguments" << std::endl;
        return -1;
    } 

    int width = std::stoi(argv[1]);
    int height = std::stoi(argv[2]);

    int count = 0;  
    cv::Mat fullImage(height*2, width*2, CV_8UC3); 
    cv::VideoWriter writer2("compressedVideo.avi", cv::VideoWriter::fourcc('m','p','4','v'), std::stoi(argv[3]), cv::Size(width*2, height*2), true);
    std::ifstream in("images.bin", std::fstream::binary); //used to read in the binary information from the file
    while(in)
    {
        in.read((char*) fullImage.data, CV_ELEM_SIZE(CV_8UC3)*(width*2)*(height*2));

        writer2.write(fullImage);
        ++count; 
        if(count % 50 == 0 || count == 1)
            std::cout << "finished image " << count << std::endl;
    }
    in.close();
    return 0;
}
