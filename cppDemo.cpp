#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctime>

int main()
{
    const int WIDTH = 352; //352
    const int HEIGHT = 288; //288
    const int FPS = 20; //this does not change the actual fps. Need to look into.

    cv::VideoCapture cap1("/dev/video0");
    //checks if the camera is opened
    if(!cap1.isOpened())
    {
        std::cout << "Error: Cannot open camera 1" << std::endl;
        return -1;
    }
    cap1.set(cv::CAP_PROP_FRAME_WIDTH,WIDTH);
    cap1.set(cv::CAP_PROP_FRAME_HEIGHT,HEIGHT);
    cap1.set(cv::CAP_PROP_FPS, FPS);
    
    cv::VideoCapture cap2("/dev/video2");
    //checks if the camera is opened
    if(!cap2.isOpened())
    {
        std::cout << "Error: Cannot open camera 2" << std::endl;
        return -1;
    }
    cap2.set(cv::CAP_PROP_FRAME_WIDTH,WIDTH);
    cap2.set(cv::CAP_PROP_FRAME_HEIGHT,HEIGHT);
    cap2.set(cv::CAP_PROP_FPS, FPS);
    
    cv::VideoCapture cap3("/dev/video4");
    //checks if the camera is opened
    if(!cap2.isOpened())
    {
        std::cout << "Error: Cannot open camera 3" << std::endl;
        return -1;
    }
    cap3.set(cv::CAP_PROP_FRAME_WIDTH,WIDTH);
    cap3.set(cv::CAP_PROP_FRAME_HEIGHT,HEIGHT);
    cap3.set(cv::CAP_PROP_FPS, FPS);
    
    //creates the video writer.
    cv::Size frameSize(WIDTH*2,HEIGHT*2);
    bool color = true;
    cv::VideoWriter videoWriter("cppDemo.avi", cv::VideoWriter::fourcc('X','V','I','D'), FPS, frameSize, color);
    
    if (!videoWriter.isOpened()) 
    {
        std::cout << "Cannot save the video to file" << std::endl;
        return -2;
    }
    
    //creates a window
    std::string wname = "demo";
    cv::namedWindow(wname);
    
    cv::Mat fullImage(cv::Size(WIDTH*2, HEIGHT*2), CV_8UC3);
    
    std::time_t t;//holds the time
    std::string time; //the time string that will be printed
    
    cv::Mat frame1;
    cv::Mat frame2;
    cv::Mat frame3;
    
    while(true)
    {
        
        bool success = cap1.read(frame1) && cap2.read(frame2) && cap3.read(frame3);
        
        if(!success)
        {
            std::cout << "Camera is disconnected" << std::endl;
            break;
        }
        fullImage = cv::Mat(cv::Size(WIDTH*2, HEIGHT*2), CV_8UC3);
        frame1.copyTo(fullImage(cv::Rect(0,0,WIDTH,HEIGHT)));
        frame2.copyTo(fullImage(cv::Rect(WIDTH,0,WIDTH,HEIGHT)));
        frame3.copyTo(fullImage(cv::Rect(0,HEIGHT,WIDTH,HEIGHT)));
        
        t = std::time(0);
        time = std::ctime(&t); //remember that there's a ? if the window gets any bigger... Its because of unicode and ascii not playing nice
        cv::putText(fullImage, time, cv::Point(WIDTH+10,HEIGHT+20), cv::FONT_HERSHEY_COMPLEX, .8, cv::Scalar(255,255,255), 1, cv::LINE_AA);
        videoWriter.write(fullImage);
        
        cv::imshow(wname, fullImage);
        
        if(cv::waitKey(10) == 27)
        {
            break; //esc key is pressed. Why does this never work for me??
        }
    }
    
    videoWriter.release();
    
    return 0;
}
