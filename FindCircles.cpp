
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

int main() {

    // Load the image
    Mat img = imread("docs/test14.jpg");
    if (img.empty()) {
        cerr << "Could not open or find the image file." << endl;
        return -1;
    }

    // Convert to grayscale
    Mat gray;
    cvtColor( img, gray, COLOR_BGR2GRAY);

    // GaussianBlur
    GaussianBlur(gray, gray, Size(9, 9), 2, 2);
    
    // Apply Hough Circle Transform
    // This function detects circles in the binary image
    vector<Vec3f> circles;
    HoughCircles(gray, circles, HOUGH_GRADIENT_ALT, 
                1,      // Accumulator resolution
                100,     // Minimum distance between circle centers
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
    
    // Print the data of circles on the terminal
    cout << "The number of detected circles :" << circles.size() << endl;
    for (size_t i =0; i < circles.size(); i++) {
        cout << "No" << i+1 << ": (" << circles[i][0] << "," << circles[i][1] << ") ," << circles[i][2] << endl; 
    }

    // Display results
    //imshow("GaussianBlur", gray);
    imshow("Result", result);
    waitKey(0);

    return 0;

}