#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

void createHistogram(const Mat& src, Mat& dst){

    //1. Histogram calculation parameters
    Mat hist;
    int hdims[] = {256};
    float hranges[] = {0, 256};
    const float* ranges[] = {hranges};
    int channels[] = {0};

    //2. Excute calcHist
    calcHist(&src, 1, channels, Mat(), hist, 1, hdims, ranges);

    //3. Find the maximum value for normalization
    double hist_max;
    minMaxLoc(hist, nullptr, &hist_max);

    //4. Initialize the destination Mat
    dst = Mat::zeros(100, 256, CV_8UC1);

    //5.Render the histogram lines
    for(int i = 0; i < 256; i++){
        float bin_val = hist.at<float>(i);
        // Calculate the hight ratio (handle division by sero if hist_max is 0)
        double ratio = (hist_max > 0) ? (static_cast<double>(bin_val) / hist_max ) : 0.0;
        // Draw a vertical line from the bottom (img.rows) upwards
        line(dst,
             Point(i, dst.rows),
             Point(i, dst.rows - cvRound(dst.rows * ratio)),
             Scalar(255));
    }
}


int main(int argc, char* argv[])
{

    const int TARGET_HEIGHT = 400 ;
    const int HST_TARGET_HEIGHT = 200;

    if( argc != 2 ){
        cerr << "Usage: " << argv[0] << " < image_path > " << endl;
        return -1;
    }

    Mat image = imread( argv[1], IMREAD_UNCHANGED);
    if( image.empty() ){
        cerr << "Error: Could not load image" << endl;
        return -1;
    }

    // Calculate scale and determine new width
    double scale = (double)TARGET_HEIGHT / image.rows;
    int resized_width = cvRound(image.cols * scale); 

    Mat resized_img;
    Mat gray_img,resized_gray_img;
    Mat gray_eql_img, resized_gray_eql_img;
    cvtColor(image, gray_img, COLOR_BGR2GRAY);
    equalizeHist(gray_img, gray_eql_img);
    resize(image, resized_img, Size(resized_width, TARGET_HEIGHT));
    resize(gray_img, resized_gray_img, Size(resized_width, TARGET_HEIGHT));
    resize(gray_eql_img, resized_gray_eql_img,Size(resized_width, TARGET_HEIGHT));

    imshow("Original Image (Resized)", resized_img);
    moveWindow("Original Image (Resized)", 10,10);
    imshow("Grayscale Image (Resized)", resized_gray_img);
    moveWindow("Grayscale Image (Resized)", resized_width+20, 10);
    imshow("Grayscale equalized Image(Resized)", resized_gray_eql_img);
    moveWindow("Grayscale equalized Image(Resized)", (resized_width*2)+30, 10);
     
    //STEP Procces for Histograms
    Mat hst_img_gray, hst_img_gray_eql;
    createHistogram(gray_img, hst_img_gray);
    createHistogram(gray_eql_img, hst_img_gray_eql);
    
    // Resize Histograms
    Mat resized_hst_img_gray, resized_hst_img_gray_eql;
    double scale_hst_img_gray = HST_TARGET_HEIGHT / hst_img_gray.rows;
    double scale_hst_img_gray_eql = HST_TARGET_HEIGHT / hst_img_gray_eql.rows;
    int resized_img_hst_gray_width = cvRound( scale_hst_img_gray * hst_img_gray.cols );
    int resized_img_hst_gray_eql_width = cvRound( scale_hst_img_gray_eql * hst_img_gray_eql.cols );
    resize(hst_img_gray, resized_hst_img_gray, Size(resized_img_hst_gray_width, HST_TARGET_HEIGHT));
    resize(hst_img_gray_eql, resized_hst_img_gray_eql, Size(resized_img_hst_gray_eql_width, HST_TARGET_HEIGHT));

    // Show Histograms
    imshow("Histogram_gray", resized_hst_img_gray);
    moveWindow("Histogram_gray", resized_width+20, TARGET_HEIGHT + 150);
    imshow("Histogram_gray_eql", resized_hst_img_gray_eql);
    moveWindow("Histogram_gray_eql", (resized_width*2)+30, TARGET_HEIGHT + 150);
    
    waitKey(0);
    
    return 0;
}