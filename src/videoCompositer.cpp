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
#include <signal.h>
#include "transmitterReceiver.h"

std::mutex endProgram; //unlocked after the queue is fully emptied. Program will not end until this happens

//mutexes to ensure that the queues are safe
std::mutex queue1Lock;
std::mutex queue2Lock;

//mutexes to ensure that the images read from the cameras are thread safe
std::mutex camera1Lock;
std::mutex camera2Lock;
std::mutex camera3Lock;

std::atomic<bool> endThreads(false); //all threads will begin to end one this is set to true
std::atomic<int> camerasStarted; //checks how many cameras have begun to collect images. Will nott start compositing until this is equal to 3
std::ofstream timestamps("videostart.txt"); //to hold the timestamp of the first and last frame

//gets run when a sigint (ctrl c) is sent to the program. All of the threads should end after this is run
void signalHandler(int signum)
{
    endThreads = true;
}

//helper function for handleCamera
void lockCamera(int cameraNum, bool lock)
{
    switch(cameraNum)
    {
        case 1:
            if(lock)
                camera1Lock.lock();
            else
                camera1Lock.unlock();
            break;
        case 2:
            if(lock)
                camera2Lock.lock();
            else
                camera2Lock.unlock();
            break;
        case 3:
            if(lock)
                camera3Lock.lock();
            else
                camera3Lock.unlock();
            break;
        default:
            std::cout << "invalid camera number" << std::endl;
            exit(-10);
    }
}

//Thread to read in images from each of the cameras
void handleCamera(cv::VideoCapture* cam, cv::Mat* frame, int cameraNum, double fps)
{
    bool firstLoop = true;
    std::chrono::steady_clock::time_point timer; //to be used for the timer
    cv::Mat testFrame; //image gets copied from this if it successfully reads a frame from the camera
    
    //loops until endThreads is true or the camera breaks
    while(!endThreads)
    {
        timer = std::chrono::steady_clock::now() + std::chrono::milliseconds((int)(1000.0/fps)); //used to read in the images at a speed according to the given fps
        
        //makes sure that the camera successfully reads in a frame before sending it to the shared mat. Breaks the thread if this fails
        if(!cam->read(testFrame))
        {
            std::cout << "camera " << cameraNum << " is broken" << std::endl;
            
            if(firstLoop)
                endThreads = true;
            break; //this camera will no longer be updated. Even if it is seen again, its video file will likely be different
        }
        else
        {
            lockCamera(cameraNum, true);
            testFrame.copyTo(*frame);   
            lockCamera(cameraNum, false);
        }
        
        //updates the camerasStarted atomic integer for the compositer thread
        if(firstLoop)
        {
            camerasStarted++;
            firstLoop = false;
        }
        
        //pauses to ensure it is only reading in frames according to the given fps    
        if(std::chrono::steady_clock::now() <= timer)
            std::this_thread::sleep_until(timer);
        else
            std::cout << "Camera " << cameraNum << " went too slow" << std::endl;
    }
}

//This function is used as a thread to composit the images from the cameras together, combine them with the image from the shared memory and add them to the queue and publishes them
void imageCompositer(cv::Mat* image1, cv::Mat* image2, cv::Mat* image3, double fps, double publishFps, int WIDTH, int HEIGHT, double scaleFactor, std::queue<cv::Mat>* imQueue, std::queue<cv::Mat>* imQueue2, double rAlpha, double rBeta, double lAlpha, double lBeta)
{
    bool firstLoop=true;
    
    //sets up the shared memory
    TransmitterReceiver fourthQuad(HEIGHT, WIDTH, CV_8UC3, "/image4",true);
    TransmitterReceiver sendImage(HEIGHT*2*scaleFactor, WIDTH*2*scaleFactor, CV_8UC3, "/image_composite", false);
    
    //creates a window
    //std::string wname = "main";
    //cv::namedWindow(wname);

    std::chrono::steady_clock::time_point timer; //to be used for the timer
    //std::time_t t;//holds the local time for display purposes
    //std::string time; //the time string that will be printed
    
    //preallocates everything it needs
    cv::Mat* main;
    cv::Mat publishImage;
    cv::Mat quad(cv::Size(WIDTH, HEIGHT), CV_8UC3);
    cv::Rect quad1 = cv::Rect(0, 0, WIDTH, HEIGHT);
    cv::Rect quad2 = cv::Rect(WIDTH, 0, WIDTH, HEIGHT);
    cv::Rect quad3 = cv::Rect(0, HEIGHT, WIDTH, HEIGHT);
    cv::Rect quad4 = cv::Rect(WIDTH, HEIGHT, WIDTH, HEIGHT);    

    while(camerasStarted < 3); //waits for three cameras to take their first pictures
    std::cout << "STARTED" << std::endl;
    
    std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
    int counter = 1; //to make sure that the timer writes the exact FPS given
    while(!endThreads)
    {
        timer = startTime + std::chrono::milliseconds((int)(counter*(1000.0/fps))); //sets timer to the time that the loop should be at when finishing the frame
	    ++counter;
	    
	    //sets main to either a new matrix or reuses the memeory from an old frame that has already been written (which are stored in imQueue2)
	    queue2Lock.lock();
        if(imQueue2->size() >= 1)
        {
            *main = imQueue2->front();
            imQueue2->pop();
            queue2Lock.unlock();
        }
        else
        {
            queue2Lock.unlock();
            main = new cv::Mat(cv::Size(WIDTH*2, HEIGHT*2), CV_8UC3, cv::Scalar(0,0,0));
        }
        
        //writes a timestamp if this is the first loop that has run
        if(firstLoop)
        {
            auto us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
            timestamps << std::setprecision(15) << us.count() / 1000000.0 << std::endl;
            firstLoop = false;
        }
	    
        
        //changes brightness for the cameras and copies them to the main image
        camera1Lock.lock();
        image1->convertTo((*main)(quad1), -1, lAlpha, lBeta);
        camera1Lock.unlock();
        
        camera2Lock.lock();
        image2->convertTo((*main)(quad2), -1, rAlpha, rBeta);
	    camera2Lock.unlock();
	    
	    camera3Lock.lock();
        image3->copyTo((*main)(quad3));
        camera3Lock.unlock();
            
        //writes the date as a test fourth quad
        /*t = std::time(0);
        time = std::ctime(&t);
        time=time.substr(0,time.size()-1); //gets rid of the ?
        cv::putText(*main, time, cv::Point(WIDTH+10,HEIGHT+20), cv::FONT_HERSHEY_COMPLEX, .6, cv::Scalar(255,255,255), 1, cv::LINE_AA);
        */

        //gets the fourth quad from shared memory
        uchar quad_copied = fourthQuad.copy_data(quad);
        quad.copyTo((*main)(quad4));
        
        //send the full image, can test just by running the code, can see image in directory
        if (counter % (int)(fps / publishFps) == 0 || counter == 2) //count = 2 on the first run through the loop
        {
            cv::resize(*main, publishImage, cv::Size(), scaleFactor, scaleFactor);
            sendImage.copy_data(publishImage);
        }
        
        //cv::imshow(wname, main->clone());
        
        //if there is room in imQueue, adds the fully composited image to it to be written out later. Otherwise, does nothing with the image
        queue1Lock.lock();
        if(imQueue->size() >= 550)
        {
            if(imQueue->size() % 10 == 0)
                std::cout << "OUT OF MEMORY SO DROPPING FRAME" << std::endl;
            //endThreads = true;
            //break;
        }
        else
            imQueue->push(*main);
        queue1Lock.unlock();

        //waits before getting the next frame
        if(std::chrono::steady_clock::now() < timer)
            std::this_thread::sleep_until(timer);
        else
            std::cout << "Compositer " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - timer).count() << "ms off" << std::endl;
    }
}

//This thread write the images in imQueue as fast as possible. Because the images were added to the queue according to the given FPS, this thread does not have to worry about that
void imageWriter(std::queue<cv::Mat>* imQueue, std::queue<cv::Mat>* imQueue2)
{
    //used for debug
    bool printed = false;
    int lastCount = 0;
    int counter = 0;
    
    std::ofstream out("images.bin", std::fstream::binary); //writes the binary data of all images with this
    cv::Mat front; //used as a temporary image so don't have to lock queue1 for so long
    while(true)
    {
        while(imQueue->size() == 0)
        {
	        if(endThreads)
	        {
	            std::cout << "FINAL COUNT: " << counter << std::endl;
	            endProgram.unlock();
	            out.close();
                return;
            }
            endProgram.unlock();
            //std::cout << "queue empty" << std::endl;
	        cv::waitKey(50); //50 was pretty much chosen randomly
        }
	    endProgram.try_lock();

        //copies the front of the queue to the front mat and then removes it from the queue
        queue1Lock.lock();
        front = imQueue->front();
        imQueue->pop(); 
        queue1Lock.unlock();
        
        out.write(front.ptr<char>(0), front.rows*front.cols*CV_ELEM_SIZE(front.type())); //write the binary information of the image to a file
        
        //adds front to queue2 so the memory can be reused later without an expensive heap operation
        queue2Lock.lock();
        imQueue2->push(front);
        queue2Lock.unlock();

        //prints out helpful debug information
        ++counter;
        if(!printed && time(NULL) % 10 == 0)
        {
            std::cout << "Average FPS: " << ((float)(counter - lastCount) / 10.0) << std::endl;
            std::cout << "There are " << imQueue->size() << " images in the queue" << std::endl;
            lastCount = counter;
            printed = true;
        }
        else if(printed && time(NULL) % 10 != 0)
            printed=false;
    }
}	

int main(int argc, char* argv[])
{
    if(argc < 13)
    {
        std::cout << "ERROR, not enough args. Please make sure that config.txt is correct and that there are three cameras plugged in. The args should be top left video file, top right video file, bottom left video file, fps, top right camera alpha, top right camera beta, top left camera alpta, top left camera beta, width, height, publish rate, publish scale factor" << std::endl;
        return -3;
    }
    
    signal(SIGINT, signalHandler);
        
    const int WIDTH = std::stoi(argv[9]); //352x288 was the best when using 3 genius cameras
    const int HEIGHT = std::stoi(argv[10]);
    const double FPS = std::stod(argv[4]);
    double rightAlpha = std::stod(argv[5]);
    double rightBeta = std::stod(argv[6]);
    double leftAlpha = std::stod(argv[7]);
    double leftBeta = std::stod(argv[8]);
    const double PUBLISH_FPS = std::stod(argv[11]);
    const double PUBLISH_SCALE_FACTOR = std::stod(argv[12]);
    
    //creates the queues
    std::queue<cv::Mat>* images = new std::queue<cv::Mat>;
    std::queue<cv::Mat>* usedImages = new std::queue<cv::Mat>;
    
    //creates the different VideoCapture objects that are later passed to the handleCamera threads
    std::string c1file(argv[1]);
    cv::VideoCapture cam1(c1file);
    cam1.set(cv::CAP_PROP_FRAME_WIDTH,WIDTH);
    cam1.set(cv::CAP_PROP_FRAME_HEIGHT,HEIGHT);
    cam1.set(cv::CAP_PROP_FOURCC ,cv::VideoWriter::fourcc('M','J','P','G') );
    //cam1.set(cv::CAP_PROP_FOURCC ,cv::VideoWriter::fourcc('Y','U','Y','V') ); //usb bus not big enough for this :(
    if(!cam1.isOpened())
        std::cout << "Camera 1 not connected!" << std::endl;
     
    std::string c2file(argv[2]);
    cv::VideoCapture cam2(c2file);
    cam2.set(cv::CAP_PROP_FRAME_WIDTH,WIDTH);
    cam2.set(cv::CAP_PROP_FRAME_HEIGHT,HEIGHT);
    cam2.set(cv::CAP_PROP_FOURCC ,cv::VideoWriter::fourcc('M','J','P','G') );
    //cam2.set(cv::CAP_PROP_FOURCC ,cv::VideoWriter::fourcc('Y','U','Y','V') );
    if(!cam2.isOpened())
        std::cout << "Camera 2 not connected!" << std::endl;
        
    std::string c3file(argv[3]);
    cv::VideoCapture cam3(c3file);
    cam3.set(cv::CAP_PROP_FRAME_WIDTH,WIDTH);
    cam3.set(cv::CAP_PROP_FRAME_HEIGHT,HEIGHT);
    //cam3.set(cv::CAP_PROP_FOURCC ,cv::VideoWriter::fourcc('M','J','P','G') );
    cam3.set(cv::CAP_PROP_FOURCC ,cv::VideoWriter::fourcc('Y','U','Y','V') );
    if(!cam3.isOpened())
        std::cout << "Camera 3 not connected!" << std::endl;
    
    //creates the three frames that will store the images from the cameras and creates the three handleCamera threads with them
    cv::Mat frame1;
    cv::Mat frame2;
    cv::Mat frame3;
    std::thread camera1(&handleCamera, &cam1, &frame1, 1, FPS);
    std::thread camera2(&handleCamera, &cam2, &frame2, 2, FPS);
    std::thread camera3(&handleCamera, &cam3, &frame3, 3, FPS);

    std::thread compositer(&imageCompositer, &frame1, &frame2, &frame3, FPS, PUBLISH_FPS, WIDTH, HEIGHT, PUBLISH_SCALE_FACTOR, images, usedImages, rightAlpha, rightBeta, leftAlpha, leftBeta);

    std::thread write(&imageWriter, images, usedImages);
    write.detach();

    //waits for the compositer to end which should happen after endThreads has been set to true which happens once the sigint signal is recieved
    compositer.join();
    
    //time stamp of end of program. Used by finishVideos
    auto us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
    timestamps << std::setprecision(15) << us.count() / 1000000.0 << std::endl; 
    
    //waits for the cameras to close
    camera1.join();
    camera2.join();
    camera3.join();
    
    endProgram.lock(); //makes sure that the queue has been emptied before closing    

    return 0;
}
