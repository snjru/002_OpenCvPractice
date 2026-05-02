
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

int main() {

    // Difine consts
    const string FILE_NAME = "docs/test1.jpg"; // file name
    const int IMAGE_HEIGHT = 1000;   // Shown image height in pixel

    // Load the image
    Mat img_original = imread(FILE_NAME);
    if (img_original.empty()) {
        cerr << "Could not open or find the image file." << endl;
        return -1;
    }

    // Convert to grayscale
    Mat img_gray;
    cvtColor(img_original, img_gray, COLOR_BGR2GRAY);
    //Gaussian Blur
    Mat img_gaussian;
    GaussianBlur(img_gray, img_gaussian, Size(9, 9), 2, 2);
    // Canny
    Mat img_canny;
    Canny(img_gaussian, img_canny, 10, 30, 3, false);
    // Morphology
    Mat kernel_morp = getStructuringElement(MORPH_ELLIPSE, Size(11,11));
    Mat img_morp_close;
    morphologyEx(img_canny, img_morp_close, MORPH_CLOSE, kernel_morp);


    // Find Contours
    vector<vector<Point>> contours;
    findContours(img_morp_close, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    cout << "Number of contours after Morphology : " << contours.size() << endl;
    
    // Filter contours with area
    vector<vector<Point>> filtered_contours;
    for(size_t i = 0; i < contours.size(); i++){
        double area = contourArea(contours[i]);
        if (area > 9000 && area < 30000) {
            filtered_contours.push_back(contours[i]);
        }
    }
    cout << "Number of contours after Filter : " << filtered_contours.size() << endl;

    // Show filtered contours on original image
    Mat result_img = img_original.clone();
    drawContours(result_img, contours, -1, Scalar(255, 0, 0), 2);
    drawContours(result_img, filtered_contours, -1, Scalar(0, 255, 0), 2);
    //drawContours(result_img, final_eyes, -1, Scalar(255, 0, 0), 2);
    putText(result_img, "Number of contours : " + to_string(filtered_contours.size()) , Point(100,120),
        FONT_HERSHEY_PLAIN, 5, Scalar(0, 255, 0), 5 );
    //putText(result_img, "Number of eyes : " + to_string(final_eyes.size()) , Point(100,220),
    //    FONT_HERSHEY_PLAIN, 5, Scalar(255, 0, 0), 5 );
    

    //imshow("Gaussian Image", img_gaussian);
    //imshow("Canny Image", img_canny);
    imshow("Morphology Image", img_morp_close);
    imshow("Filtererd Contours", result_img); 
    
    waitKey(0);

    return 0;

}