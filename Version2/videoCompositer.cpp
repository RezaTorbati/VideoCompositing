#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <ctime>
#include <chrono>
#include <fstream>

int main(int argc, char* argv[])
{
    if(argc < 4)
    {
        std::cout << "ERROR, not enough args. Please make sure that config.txt is correct and that there are three cameras plugged in" << std::endl;
        return -3;
    }
        
    const int WIDTH = 352; //best resolution the cameras give that the pi can handle
    const int HEIGHT = 288;
    const int FPS = 20; //TODO: this is not perfect
    
    std::string c1file(argv[1]);
    cv::VideoCapture cam1(c1file);
    cam1.set(cv::CAP_PROP_FRAME_WIDTH,WIDTH);
    cam1.set(cv::CAP_PROP_FRAME_HEIGHT,HEIGHT);
    if(!cam1.isOpened())
        std::cout << "Camera 1 not connected!" << std::endl;
     
    std::string c2file(argv[2]);
    cv::VideoCapture cam2(c2file);
    cam2.set(cv::CAP_PROP_FRAME_WIDTH,WIDTH);
    cam2.set(cv::CAP_PROP_FRAME_HEIGHT,HEIGHT);
    if(!cam2.isOpened())
        std::cout << "Camera 2 not connected!" << std::endl;
        
    std::string c3file(argv[3]);
    cv::VideoCapture cam3(c3file);
    cam3.set(cv::CAP_PROP_FRAME_WIDTH,WIDTH);
    cam3.set(cv::CAP_PROP_FRAME_HEIGHT,HEIGHT);
    if(!cam3.isOpened())
        std::cout << "Camera 3 not connected!" << std::endl;
    
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
    bool tooFast = true;
    std::chrono::steady_clock::time_point timer; //to be used for the timer
    std::time_t t;//holds the local time for display purposes TODO: use timer
    std::string time; //the time string that will be printed TODO: not sure if needed
    
    cv::Mat frame1;
    cv::Mat frame2;
    cv::Mat frame3;
    
    while(true)
    {
        timer = std::chrono::steady_clock::now() + std::chrono::milliseconds(1000/FPS);
        
        //resets fullimage and copies everything into it
        fullImage = cv::Mat(cv::Size(WIDTH*2, HEIGHT*2), CV_8UC3);
        if(!cam1.read(frame1))  
            std::cout << "Camera 1 became disconnected!" << std::endl;
        else    
        {
            cv::rotate(frame1, frame1, cv::ROTATE_180);
            frame1.copyTo(fullImage(cv::Rect(0,0,WIDTH,HEIGHT)));
        }
        if(!cam2.read(frame2))  
            std::cout << "Camera 2 became disconnected!" << std::endl;
        else    
        {
            cv::rotate(frame2, frame2, cv::ROTATE_180);
            frame2.copyTo(fullImage(cv::Rect(WIDTH,0,WIDTH,HEIGHT)));
        }
        if(!cam3.read(frame3))  
            std::cout << "Camera 3 became disconnected!" << std::endl;
        else    
        {
            cv::rotate(frame3, frame3, cv::ROTATE_180);
            frame3.copyTo(fullImage(cv::Rect(0,HEIGHT,WIDTH,HEIGHT)));
        }
            
        //writes the date to the composited image. TODO: rewrite this to use timer
        t = std::time(0);
        time = std::ctime(&t);
        time=time.substr(0,time.size()-1); //gets rid of the ?
        cv::putText(fullImage, time, cv::Point(WIDTH+10,HEIGHT+20), cv::FONT_HERSHEY_COMPLEX, .6, cv::Scalar(255,255,255), 1, cv::LINE_AA);
        
        if(firstLoop)
        {
            auto us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
            timestamps << std::setprecision(15) << us.count() / 1000000.0 << std::endl;
            firstLoop=false;
        }
        
        videoWriter.write(fullImage);
        cv::imshow(wname, fullImage);
        
        cv::waitKey(1);
        
        //makes sure that the program waits 50ms between loops.
        while(std::chrono::steady_clock::now() <= timer)
            tooFast = false;
        if(tooFast)
            std::cout << "TOO FAST" << std::endl;
        tooFast = true;
    }
    videoWriter.release();
    return 0;
}
