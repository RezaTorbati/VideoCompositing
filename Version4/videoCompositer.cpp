#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <ctime>
#include <chrono>
#include <fstream>
#include <thread>
#include <queue> 
#include <mutex>
#include <atomic>

std::mutex endProgram;
std::mutex queueLock;
bool endThreads = false;
std::atomic<int> camerasStarted;
std::ofstream timestamps("videostart.txt"); //to hold the timestamp of the first and last frame

void handleCamera(cv::VideoCapture* cam, cv::Mat* frame, double fps)
{
    //int i=0; //helpful to decrease the debug spam
    
    bool firstLoop = true;
    std::chrono::steady_clock::time_point timer; //to be used for the timer

    while(!endThreads)
    {
        timer = std::chrono::steady_clock::now() + std::chrono::milliseconds((int)(1000.0/fps));
        cv::waitKey(1);
        if(!cam->read(*frame))
            std::cout << "a camera is broken" << std::endl;
        
        if(firstLoop)
        {
            camerasStarted++;
            firstLoop = false;
        }
            
        if(std::chrono::steady_clock::now() <= timer)
        {
            std::this_thread::sleep_until(timer);
     //       i=0;
        }
        else
        {
            //if(i%20==0)
                std::cout << "TOO FAST " << std::this_thread::get_id() << std::endl;
            //i++;
        }
    }
}

void imageCompositer(cv::Mat* main, cv::Mat* image1, cv::Mat* image2, cv::Mat* image3, double fps, int WIDTH, int HEIGHT, std::queue<cv::Mat>* imQueue)
{
    bool firstLoop=true;
  
    //creates a window
    //std::string wname = "main";
    //cv::namedWindow(wname);

    std::chrono::steady_clock::time_point timer; //to be used for the timer
    std::time_t t;//holds the local time for display purposes
    std::string time; //the time string that will be printed
    
    *main = cv::Mat(cv::Size(WIDTH*2 + 2, HEIGHT*2 + 2), CV_8UC3);
    
    while(camerasStarted < 3); //waits for three cameras to take their first pictures
    std::cout << "STARTED" << std::endl;
    
    std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
    int counter = 1; //to make sure that the timer writes the exact FPS given
    while(!endThreads)
    {
        timer = startTime + std::chrono::milliseconds((int)(counter*(1000.0/fps)));
	    ++counter;

        //writes a timestamp if this is the first loop that has run
	    if(firstLoop)
	    {
            auto us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
            timestamps << std::setprecision(15) << us.count() / 1000000.0 << std::endl;
	        firstLoop = false;
	    }
	    
        //resets fullimage and copies everything into it
        cv::rectangle(*main, cv::Point(WIDTH, HEIGHT), cv::Point(WIDTH*2, HEIGHT*2), cv::Scalar(0,0,0), cv::FILLED);
        image1->copyTo((*main)(cv::Rect(0,0,WIDTH,HEIGHT)));
        image2->copyTo((*main)(cv::Rect(WIDTH,0,WIDTH,HEIGHT)));
        image3->copyTo((*main)(cv::Rect(0,HEIGHT,WIDTH,HEIGHT)));
            
        //writes the date to the composited image.
        t = std::time(0);
        time = std::ctime(&t);
        time=time.substr(0,time.size()-1); //gets rid of the ?
        cv::putText(*main, time, cv::Point(WIDTH+10,HEIGHT+20), cv::FONT_HERSHEY_COMPLEX, .6, cv::Scalar(255,255,255), 1, cv::LINE_AA);
        
        //cv::imshow(wname, main->clone());
        
        imQueue->push(main->clone());
        cv::waitKey(1);
        
        if(imQueue->size() > 700)
        {
            std::cout << "\nOUT OF MEMORY\n" << std::endl;
            endThreads = true;
            auto us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
            timestamps << std::setprecision(15) << us.count() / 1000000.0 << std::endl;
        }
        
        /*if(counter % (int)fps == 0)
        {
            std::cout << "TIME: " << counter / fps << std::endl;
            std::cout << "FRAMES: " << counter << std::endl;
        }*/

        //waits before getting the next frame
        if(std::chrono::steady_clock::now() < timer)
            std::this_thread::sleep_until(timer);
        else
            std::cout << "TOO FAST 2" << std::endl;
    }
    std::cout << "FINAL COUNT: " << counter << std::endl;
}

void imageWriter(cv::VideoWriter* writer,  std::queue<cv::Mat>* imQueue)
{
    //int invalidCounter = 0;
    //cv::Mat lastValid;
    while(true)
    {
        //cv::waitKey(1);
        while(imQueue->size() < 100)
        {
            //std::cout << "WRITE COUNT: " << counter << std::endl;
            endProgram.unlock();
            std::cout << "queue empty" << std::endl;
	        cv::waitKey(50); //50 was pretty much chosen randomly
	        if(endThreads)
	        {
	            //std::cout << "Number of invalid frames: " << invalidCounter << std::endl;
                exit(1);
            }
        }
	    endProgram.try_lock();
	    
	    /*if(checkIfValid(imQueue->front()))
	    {
	        writer->write(imQueue->front());
	        lastValid = imQueue->front();
        }
        else
        {
            ++invalidCounter;
            std::cout << "invalid image" << std::endl;
            writer->write(lastValid);
        }*/
        
        writer->write(imQueue->front());
        imQueue->pop();
        //cv::waitKey(1);
        //++counter;
    }
}	

int main(int argc, char* argv[])
{
    if(argc < 5)
    {
        std::cout << "ERROR, not enough args. Please make sure that config.txt is correct and that there are three cameras plugged in" << std::endl;
        return -3;
    }
        
    int WIDTH = 640; //352x288 was the best when using 3 genius cameras
    int HEIGHT = 480;
    const double FPS = std::stod(argv[4]);
    std::queue<cv::Mat>* images = new std::queue<cv::Mat>;
    
    std::string c1file(argv[1]);
    cv::VideoCapture cam1(c1file);
    cam1.set(cv::CAP_PROP_FRAME_WIDTH,WIDTH);
    cam1.set(cv::CAP_PROP_FRAME_HEIGHT,HEIGHT);
    cam1.set(cv::CAP_PROP_FOURCC ,cv::VideoWriter::fourcc('M','J','P','G') );
    //cam1.set(cv::CAP_PROP_FOURCC ,cv::VideoWriter::fourcc('Y','U','Y','V') ); //usb bus not big enough for this :(
    cam1.set(cv::CAP_PROP_FPS,30); //pretty sure this doesn't do anything
    if(!cam1.isOpened())
        std::cout << "Camera 1 not connected!" << std::endl;
     
    std::string c2file(argv[2]);
    cv::VideoCapture cam2(c2file);
    cam2.set(cv::CAP_PROP_FRAME_WIDTH,WIDTH);
    cam2.set(cv::CAP_PROP_FRAME_HEIGHT,HEIGHT);
    cam2.set(cv::CAP_PROP_FOURCC ,cv::VideoWriter::fourcc('M','J','P','G') );
    //cam2.set(cv::CAP_PROP_FOURCC ,cv::VideoWriter::fourcc('Y','U','Y','V') );
    cam2.set(cv::CAP_PROP_FPS,30);
    if(!cam2.isOpened())
        std::cout << "Camera 2 not connected!" << std::endl;
        
    std::string c3file(argv[3]);
    cv::VideoCapture cam3(c3file);
    cam3.set(cv::CAP_PROP_FRAME_WIDTH,WIDTH);
    cam3.set(cv::CAP_PROP_FRAME_HEIGHT,HEIGHT);
    //cam3.set(cv::CAP_PROP_FOURCC ,cv::VideoWriter::fourcc('M','J','P','G') );
    //cam2.set(cv::CAP_PROP_FOURCC ,cv::VideoWriter::fourcc('Y','U','Y','V') );
    cam3.set(cv::CAP_PROP_FPS,30);
    if(!cam3.isOpened())
        std::cout << "Camera 3 not connected!" << std::endl;
    
    //creates the video writer.
    cv::Size frameSize(WIDTH*2 + 2,HEIGHT*2 + 2);
    bool color = true;
    //cv::VideoWriter videoWriter("video.avi", cv::VideoWriter::fourcc('T','I','F','F'), FPS, frameSize, color); 
    cv::VideoWriter videoWriter("video.avi", 0, FPS, frameSize, color); //0 means raw video I think

    if (!videoWriter.isOpened()) 
    {
        std::cout << "Cannot save the video to file" << std::endl;
        return -2;
    }
    
    cv::Mat fullImage;//(cv::Size(WIDTH*2, HEIGHT*2), CV_8UC3); //the image that the others get composited to
    cv::Mat frame1;
    cv::Mat frame2;
    cv::Mat frame3;

    std::thread camera1(&handleCamera, &cam1, &frame1, FPS);
    std::cout << "Cam 1: " << camera1.get_id() << std::endl;
    std::thread camera2(&handleCamera, &cam2, &frame2, FPS);
    std::cout << "Cam 2: " << camera2.get_id() << std::endl;
    std::thread camera3(&handleCamera, &cam3, &frame3, FPS);
    std::cout << "Cam 3: " << camera3.get_id() << std::endl;

    std::thread compositer(&imageCompositer, &fullImage, &frame1, &frame2, &frame3, FPS, WIDTH, HEIGHT, images);
    compositer.detach();

    std::thread write(&imageWriter, &videoWriter, images);
    write.detach();

    std::string input;
    while(!endThreads)
    {
	    std::cin >> input;
	    if(input == "q")
	        endThreads = true;
    }
    
    auto us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
    timestamps << std::setprecision(15) << us.count() / 1000000.0 << std::endl;
    
    endProgram.lock(); //makes sure that the queue has been emptied before closing
    videoWriter.release();
    return 0;
}
