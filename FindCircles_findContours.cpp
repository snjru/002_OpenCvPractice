
#include <windows.h>
#include <shobjidl.h> // IFileOpenDialog
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

int main() {

    // Difine consts
    const string FILE_NAME = "docs/test19.jpg"; // file name
    const int IMAGE_HEIGHT = 1000;   // Shown image height in pixel

    // Load the image
    Mat img = imread(FILE_NAME);
    if (img.empty()) {
        cerr << "Could not open or find the image file." << endl;
        return -1;
    }

    // Convert to grayscale
    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);
    // Gaussian Blur
    Mat blur;
    GaussianBlur(gray, blur, Size(9,9), 2, 2);
    // Convert to binary
    Mat bin;
    adaptiveThreshold( blur, bin, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 11 ,2);
    // Contour extraction after Threshold
    vector<vector<Point>> contours_b;
    findContours(bin, contours_b, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    cout << "Number of contours after Threshold : " << contours_b.size() << endl;
    
    // Morphology
    Mat kernel3 = getStructuringElement(MORPH_ELLIPSE, Size(3,3));
    Mat kernel5 = getStructuringElement(MORPH_ELLIPSE, Size(5,5));
    Mat kernel7 = getStructuringElement(MORPH_ELLIPSE, Size(7,7));
    Mat morp;
    morphologyEx(bin, morp, MORPH_CLOSE, kernel3);
    morphologyEx(morp, morp, MORPH_CLOSE, kernel5);
    // Dilate
    dilate(morp, morp, kernel7);
    // Contour extraction after Morphology
    vector<vector<Point>> contours_m;
    findContours(morp, contours_m, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    cout << "Number of contours after Morphology : " << contours_m.size() << endl;

    //imshow("Blur Image", blur);
    //imshow("Binary Image",bin);
    //imshow("Morphology Image", morp);

    // Filter contours with area
    vector<vector<Point>> filtered_contours;
    for(size_t i = 0; i < contours_m.size(); i++){
        double area = contourArea(contours_m[i]);
        if (area > 9000 && area < 30000) {
            filtered_contours.push_back(contours_m[i]);
        }
    }
    // Filter contours with shape
    vector<vector<Point>> final_eyes;
    for (size_t i = 0; i < filtered_contours.size(); i++){
        double area = contourArea(filtered_contours[i]);
        double perimeter = arcLength(filtered_contours[i], true);
        if(perimeter > 0){
            double circularity = (4 * CV_PI * area) / (perimeter * perimeter);
            if (circularity > 0.5){
                final_eyes.push_back(filtered_contours[i]);
            }
        }
    }
    cout << "Number of filtered contours : " << filtered_contours.size() << endl;
    cout << "Number of final_eyes : " << final_eyes.size() << endl;

    // Show filtered contours on image
    Mat result_img = img.clone();
    drawContours(result_img, filtered_contours, -1, Scalar(0, 255, 0), 2);
    drawContours(result_img, final_eyes, -1, Scalar(255, 0, 0), 2);
    putText(result_img, "Number of contours : " + to_string(filtered_contours.size()) , Point(100,120),
        FONT_HERSHEY_PLAIN, 5, Scalar(0, 255, 0), 5 );
    putText(result_img, "Number of eyes : " + to_string(final_eyes.size()) , Point(100,220),
        FONT_HERSHEY_PLAIN, 5, Scalar(255, 0, 0), 5 );
    
    imshow("Filtererd Contours", result_img); 

    waitKey(0);

    return 0;

}