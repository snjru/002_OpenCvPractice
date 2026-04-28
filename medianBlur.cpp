
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;



int main() {

    // Max kernel size for medianBlur
    const int MAX_K = 9;
    const int ARY_SIZE = (MAX_K+1)/2;

    // Load the image
    Mat img = imread("docs/test14.jpg");
    if (img.empty()) {
        cerr << "Could not open or find the image file." << endl;
        return -1;
    }

    // Convert to grayscale
    Mat gray;
    cvtColor( img, gray, COLOR_BGR2GRAY);

    // medianBlur
    Mat mBlrAry[ARY_SIZE];
    int idx = 0;
    for ( int k = 1 ; k <= MAX_K; k +=2 ){
        medianBlur(gray, mBlrAry[idx], k);
        putText(mBlrAry[idx], "kernel size = " + to_string(k),
                Point(30,60), FONT_HERSHEY_SCRIPT_SIMPLEX, 1.5, Scalar(0, 255, 0), 3);
        idx++;
    }
    
    // Show images
    Mat canvas;
    hconcat(mBlrAry, ARY_SIZE, canvas);
    namedWindow("MedianBlur Comparison", WINDOW_NORMAL);
    setWindowProperty("MedianBlur Comparison", WND_PROP_ASPECT_RATIO, WINDOW_KEEPRATIO);
    resizeWindow("MedianBlur Comparison", 1280, 480);
    imshow("MedianBlur Comparison", canvas);
   
    waitKey(0);    
    return 0;

}