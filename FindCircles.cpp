
#include <windows.h>
#include <shobjidl.h> // IFileOpenDialog
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

int main() {

    // Load the image
    const string FILE_NAME = "docs/test10.jpg";
    Mat img = imread(FILE_NAME);
    if (img.empty()) {
        cerr << "Could not open or find the image file." << endl;
        return -1;
    }

    // Convert to grayscale
    Mat gray;
    cvtColor( img, gray, COLOR_BGR2GRAY);

    // GaussianBlur medianBlur bilateralFilter
    GaussianBlur(gray, gray, Size(9, 9), 2, 2);
    //medianBlur(gray, gray, 9);
    //bilateralFilter(gray, gray, 11, 50, 100);

    // Apply Hough Circle Transform
    // This function detects circles in the binary image
    vector<Vec3f> circles;
    HoughCircles(gray, circles, HOUGH_GRADIENT_ALT, 
                1.5,      // Accumulator resolution
                90,     // Minimum distance between circle centers
                50,     // Canny edge detection high threshold
                0.9,     // Accumulator threshold (lower = more circles detected)
                50,     // Minimum radius
                90);    // Maximum radius

    // Draw the detected circles on the original image
    Mat result = img.clone();
        for (size_t i = 0; i < circles.size(); i++) {
            Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
            int radius = cvRound(circles[i][2]);
        
            // Draw the circle outline
            circle(result, center, radius, Scalar(0, 0, 255), 2);
            // Draw the circle center
            circle(result, center, 2, Scalar(0, 255, 0), 3);
        }
    
    putText(result, "Number of Circles :" + to_string(circles.size()),
            Point(30,120), FONT_HERSHEY_PLAIN, 10, Scalar(0, 255, 0), 3);
      
    // Print the data of circles on the terminal
    cout << "The number of detected circles :" << circles.size() << endl;
    for (size_t i =0; i < circles.size(); i++) {
        cout << "No" << i+1 << ": (" << circles[i][0] << "," << circles[i][1] << ") ," << circles[i][2] << endl; 
    }

  
    // Display results
    //imshow("GaussianBlur", gray);
    namedWindow(FILE_NAME, WINDOW_NORMAL);
    resizeWindow(FILE_NAME, 1920, 720);
    imshow(FILE_NAME, result);
    waitKey(0);

    return 0;

}