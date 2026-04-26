#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main(int argc, char* argv[]){

// Declare and Initialization a hist array
Mat img_hst;
img_hst = Mat::zeros(100, 256, CV_8UC1)

const int hdims[] = {256};
const float hranges[] = {0, 256};
const float *ranges[] = {hranges};


// Calcurate the histogram of 1 channel data
Mat hist;
calcHist( &img_src, 1, 0, Mat(), hist, 1, hdims, ranges);

// Get the maximam value of the histogram
double hist_min, hist_max;
minMaxLoc( hist, &hist_min, &hist_max);

// Draw the histogram with whight lines
for(int i = 0; i <= 256; i++){
    int v = saturate_cast<int>(hist.at<float>(i));
    cout << i << " " << v << endl;
    line(img_hst, Point(i, img_hist.rows), Point(i, img_hst.rows-img_hst.rows * (v / hist_max)), Scalar(255, 255, 255));
}

}


