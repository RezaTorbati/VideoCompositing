#include <opencv2/opencv.hpp>
#include <iostream>

class DewarpVideo
{
    public:
        DewarpVideo(double cCoef, double corFactor, int w, int h);
        cv::Mat dewarpFrame(cv::Mat frame, cv::Mat xMap, cv::Mat yMap);
        cv::Mat getMap(bool xMap);
    private:
        //how far out the pixels move when being corrected
        double calibrationCoef;
        
        //this is what resize scale is before correcting the image. The bigger the less tearing but the slower the correction is.
        double correctionFactor;
        
        int width, height;
        
        //takes the x and y coords of the pixel to be corrected. If returnX is true, returns the x coord of the new pixel and returns the y coord if not
        int getNewCoord(int x, int y, bool returnX); 
};
