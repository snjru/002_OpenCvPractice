
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <windows.h>
#include <commdlg.h>


struct EyeData {
    cv::Point2f center;               // Center of Circle
    float radius;                     // Radius of Circle
    std::vector<cv::Point> contour;   // Original Cntoures
};

using namespace cv;
using namespace std;

int main() {

    
    //// Open File Preproces
    // Structure for the Open File Dialog
    OPENFILENAME ofn;
    char szFile[260];   // Buffer to store the selected file path
    // Initialize the OPENFILENAME structure
    ZeroMemory(&ofn, sizeof(ofn));
    ZeroMemory(szFile, sizeof(szFile));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Images\0*.jpg;*.jpeg;*.png;*.bmp\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL; // Initial directory (NULL uses the default)
    /*
     * Flags:
     * OFN_PATHMUSTEXIST: User can only type valid paths
     * OFN_FILEMUSTEXIST: User can only type valid file names
     * OFN_NOCHANGEDIR: Prevents the dialog from changing the current working directory
     */
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR; 
    
      
    //// Open File and Road
    Mat img_original;
    // Open File Diarog the Open dialog box
    if (GetOpenFileName(&ofn) == TRUE) {
        cout << "Selected file path: " << ofn.lpstrFile << endl;
        img_original = imread(ofn.lpstrFile, IMREAD_COLOR);
    
        if (img_original.empty()) {
            cerr << "Error Could not open or find the Image!" << endl;
            return -1;
        }
    } else {
        // It's safer to exit if no file was selected
        return 0;
    }
    

    //// Image Preprocessing
    // Convert to grayscale
    Mat img_gray;
    cvtColor(img_original, img_gray, COLOR_BGR2GRAY);
    // Gaussian Blur
    Mat img_blur;
    GaussianBlur(img_gray, img_blur, Size(9,9), 2, 2);
    // Convert to binary
    Mat img_bin;
    adaptiveThreshold( img_blur, img_bin, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 11 ,2);
    // Morphology
    Mat kernel3 = getStructuringElement(MORPH_ELLIPSE, Size(3,3));
    Mat img_morp;
    morphologyEx(img_bin, img_morp, MORPH_CLOSE, kernel3);
    //morphologyEx(img_morp, img_morp, MORPH_CLOSE, kernel5);
    // Dilation
    Mat img_dila;
    Mat kernel5 = getStructuringElement(MORPH_ELLIPSE, Size(5,5));
    dilate(img_morp, img_dila, kernel5);
    

    //// Extracting contours
    // Contour extraction after Threshold
    vector<vector<Point>> contours_bin;
    findContours(img_bin, contours_bin, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    cout << "Number of contours after Threshold : " << contours_bin.size() << endl;
    // Contour extraction after Morphology
    vector<vector<Point>> contours_morp;
    findContours(img_morp, contours_morp, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    cout << "Number of contours after Morphology : " << contours_morp.size() << endl;
    // Contour extraction after Dilate
    vector<vector<Point>> contours_dila;
    findContours(img_dila, contours_dila, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    cout << "Number of contours after Dilate : " << contours_dila.size() << endl;
    
    
    //// Filtering Contours
    // Filter contours with area
    vector<vector<Point>> contours_filtered_area;
    for(size_t i = 0; i < contours_dila.size(); i++){
        double area = contourArea(contours_dila[i]);
        if (area > 9000 && area < 30000) {
            contours_filtered_area.push_back(contours_dila[i]);
        }
    }
    cout << "Number of contours after filtered with Area : " << contours_filtered_area.size() << endl;
    
    // Filter contours with shape
    vector<vector<Point>> contours_filtered_shape;
    for (size_t i = 0; i < contours_filtered_area.size(); i++){
        double area = contourArea(contours_filtered_area[i]);
        double perimeter = arcLength(contours_filtered_area[i], true);
        if(perimeter > 0){
            double circularity = (4 * CV_PI * area) / (perimeter * perimeter);
            if (circularity > 0.4) {
                contours_filtered_shape.push_back(contours_filtered_area[i]);
            }
        }
    }
    cout << "Number of contours after filtered with shape : " << contours_filtered_shape.size() << endl;
    
      
    //// Make Circles array with EyeData structure
    vector<EyeData> allEyes;
    for (size_t i = 0; i < contours_filtered_shape.size(); i++){
        EyeData data;
        data.contour = contours_filtered_shape[i]; 
        minEnclosingCircle(contours_filtered_shape[i], data.center, data.radius); // Get the Final Circle
        allEyes.push_back(data); 
    }
   
    //// Draw Filtered Contours and Put the Number of Contours on image
    // Draw Contours
    Mat img_result = img_original.clone();
    //drawContours(img_result, contours_bin, -1, Scalar(0, 0, 255), 2); //bin
    //drawContours(img_result, contours_morp, -1, Scalar(0, 255, 255), 2);  //morp
    drawContours(img_result, contours_dila, -1, Scalar(255, 0, 0), 2); //Dilate
    drawContours(img_result, contours_filtered_area, -1, Scalar(0, 255, 0), 2); //Area
    drawContours(img_result, contours_filtered_shape, -1, Scalar(0, 0, 255), 2); //Shape

    // Put Texts on Image
    putText(img_result, "Number of contours after Dilate : " + to_string(contours_dila.size()) , Point(100,120),
        FONT_HERSHEY_PLAIN, 5, Scalar(255, 0, 0), 5 );
    putText(img_result, "Number of contours after filtered with area : " + to_string(contours_filtered_area.size()) , Point(100,220),
        FONT_HERSHEY_PLAIN, 5, Scalar(0, 255, 0), 5 );
    putText(img_result, "Number of eyes (aftere filtered with circularity) : " + to_string(contours_filtered_shape.size()) , Point(100,320),
        FONT_HERSHEY_PLAIN, 5, Scalar(0, 0, 255), 5 );
    
    /*// Draw Circle and Center Closs
    for (const auto& eye : allEyes) {
        circle(img_result, eye.center, (int)eye.radius, Scalar(0, 0, 255), 3);
    
        int s = 10; // Size of Center Closs
        line(img_result, Point(eye.center.x - s, eye.center.y), 
                     Point(eye.center.x + s, eye.center.y), Scalar(0, 255, 0), 3);
        line(img_result, Point(eye.center.x, eye.center.y - s), 
                     Point(eye.center.x, eye.center.y + s), Scalar(0, 255, 0), 3);
    }
    */

    // Get the actual width of the primary monitor
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int targetX = 0; // Intended position (e.g., for sub-monitor)
    // Safety Check: If the target position is beyond the current screen width,
    // reset it to 0 (primary monitor) to avoid "losting" the window.
    if (targetX >= screenWidth) {
        targetX = 0;
    }
    // Create a Window with a specific name and manual sizing capability
    // WINDOW_NOMAL allows you to resize the window
    string win1 = "After Dilate Image";
    string win2 = "Result Image";
    int width = 800;
    int height = 600;
    namedWindow(win1, WINDOW_NORMAL);
    namedWindow(win2, WINDOW_NORMAL);
    // Move the window to the top-left corner (0,0)
    moveWindow(win1, targetX,0);
    moveWindow(win2, width,0);
    // Set the desired window size (width, height)
    resizeWindow(win1, width, height);
    resizeWindow(win2, width, height);

    // Show Images
    imshow(win1, img_dila);
    imshow(win2, img_result); 

    waitKey(0);

    return 0;

}