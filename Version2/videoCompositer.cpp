#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <ctime>
#include <chrono>
#include <fstream>

int main(int argc, char* argv[])
{
    for(int i = 0; i < argc; i++)
        std::cout<<argv[i]<<std::endl;
        
    const int WIDTH = 352; //best resolution the cameras give that the pi can handle
    const int HEIGHT = 288;
    const int FPS = 20; //TODO: this is not perfect
    
    std::vector<cv::VideoCapture> cams;
    cams.push_back(cv::VideoCapture("/dev/video0")); //TODO: add config for this
    if(!cams.back().isOpened())
    {
        cams.erase(cams.end());
        std::cout << "Camera 1 not connected" << std::endl;
    }
    
    cams.push_back(cv::VideoCapture("/dev/video2")); //TODO: add config for this
    if(!cams.back().isOpened())
    {
        cams.erase(cams.end());
        std::cout << "Camera 2 not connected" << std::endl;
    }
    
    cams.push_back(cv::VideoCapture("/dev/video4"));
    if(!cams.back().isOpened())
    {
        cams.erase(cams.end());
        std::cout << "Camera 3 not connected" << std::endl;
    }
    
    for(int i = 0; i < cams.size(); i++)
    {
        cams[i].set(cv::CAP_PROP_FRAME_WIDTH,WIDTH);
        cams[i].set(cv::CAP_PROP_FRAME_HEIGHT,HEIGHT);
    }
    
    //creates the video writer.
    cv::Size frameSize(WIDTH*2,HEIGHT*2);
    bool color = true;
    cv::VideoWriter videoWriter("video.avi", cv::VideoWriter::fourcc('X','V','I','D'), FPS, frameSize, color);
    if (!videoWriter.isOpened()) 
    {
        std::cout << "Cannot save the video to file" << std::endl;
        return -2;
    }
    
    //creates a window
    std::string wname = "main";
    cv::namedWindow(wname);
    
    cv::Mat fullImage(cv::Size(WIDTH*2, HEIGHT*2), CV_8UC3); //the image that the others get composited to
    
    bool firstLoop=true;
    std::ofstream timestamps("videostart.txt"); //to hold the timestamp of the first frame
    std::chrono::steady_clock::time_point timer; //to be used for the timer
    std::time_t t;//holds the local time for display purposes TODO: use timer
    std::string time; //the time string that will be printed TODO: not sure if needed
    
    std::vector<cv::Mat> frames;
    for(int i = 0; i < cams.size(); i++) frames.push_back(cv::Mat());
    
    while(true) //TODO: add in kill condition that doesn't need ^C
    {
        timer = std::chrono::steady_clock::now() + std::chrono::milliseconds(50);
        
        //read from each camera
        for(int i = 0; i < cams.size(); i++)
        {
            if(!(cams[i].read(frames[i])))
                std::cout << "Camera " << i << " diconnected" << std::endl;
        }
        
        //resets fullimage and copies everything into it
        fullImage = cv::Mat(cv::Size(WIDTH*2, HEIGHT*2), CV_8UC3);
        if(frames.size() >= 1)
            frames[0].copyTo(fullImage(cv::Rect(0,0,WIDTH,HEIGHT)));
        if(frames.size() >= 2)
            frames[1].copyTo(fullImage(cv::Rect(WIDTH,0,WIDTH,HEIGHT)));
        if(frames.size() >= 3)
            frames[2].copyTo(fullImage(cv::Rect(0,HEIGHT,WIDTH,HEIGHT)));
            
        //writes the date to the composited image. TODO: rewrite this to use timer
        t = std::time(0);
        time = std::ctime(&t); //remember that there's a ? if the window gets any bigger... Its because of unicode and ascii not playing nice
        cv::putText(fullImage, time, cv::Point(WIDTH+10,HEIGHT+20), cv::FONT_HERSHEY_COMPLEX, .8, cv::Scalar(255,255,255), 1, cv::LINE_AA);
        
        if(firstLoop)
        {
            auto us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
            timestamps << std::setprecision(15) << us.count() / 1000000.0 << std::endl;
            firstLoop=false;
        }
        
        videoWriter.write(fullImage);
        cv::imshow(wname, fullImage);
        
        cv::waitKey(1);
        /*std::cout << (key & 0xFF) << std::endl;
        if((key & 0xFF) == 27) //supposed to end when esc is pressed. waitkey is acting super spooky and is not usable. Had to do this via bash instead
        {
            break;
        }*/
        
        //makes sure that the program waits 50ms between loops.
        while(std::chrono::steady_clock::now() <= timer);
    }
    videoWriter.release();
    return 0;
}
