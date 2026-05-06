
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

// Union-Find (Disjoint Set Union) structure to manage groups of eyes
struct UnionFind {
    // parent[i] stores the parent of element i
    std::vector<int> parent; 

    // Constructor: Initialize n elements, each as its own representative
    UnionFind(int n) {
        parent.resize(n);
        for (int i = 0; i < n; i++) {
            parent[i] = i; // Initially, every element is its own parent
        }
    }

    // Find the representative (root) of the set containing element i
    // Includes Path Compression for optimization
    int find(int i) {
        if (parent[i] == i) return i;
        // Recursively find the root and attach i directly to it
        return parent[i] = find(parent[i]); 
    }

    // Unite two sets containing elements i and j
    void unite(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);
        // If they belong to different sets, connect the roots
        if (root_i != root_j) {
            parent[root_i] = root_j;
        }
    }
};


using namespace cv;
using namespace std;


int main() {

    // Structure for the Open FIle Dialog
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
    
    Mat img;
    // Display the Open dialog box
    if (GetOpenFileName(&ofn) == TRUE) {
        cout << "Selected file path: " << ofn.lpstrFile << endl;
         img = imread(ofn.lpstrFile, IMREAD_COLOR);
    
        if (img.empty()) {
            cerr << "Error Could not open or find the Image!" << endl;
            return -1;
        }
    } else {
        // It's safer to exit if no file was selected
        return 0;
    }

    const int IMAGE_HEIGHT = 1000;   // Shown image height in pixel

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
            if (circularity > 0.4){
                final_eyes.push_back(filtered_contours[i]);
            }
        }
    }
    cout << "Number of filtered contours : " << filtered_contours.size() << endl;
    cout << "Number of final_eyes : " << final_eyes.size() << endl;

    vector<EyeData> allEyes;
    for (size_t i = 0; i < final_eyes.size(); i++){
        EyeData data;
        data.contour = final_eyes[i]; // Copy Contours
        minEnclosingCircle(final_eyes[i], data.center, data.radius);
    
        allEyes.push_back(data); // 行列（リスト）に追加
    }

    // Show filtered contours on image
    Mat result_img = img.clone();
    drawContours(result_img, filtered_contours, -1, Scalar(0, 255, 0), 2);
    drawContours(result_img, final_eyes, -1, Scalar(255, 0, 0), 2);
   

    putText(result_img, "Number of contours : " + to_string(filtered_contours.size()) , Point(100,120),
        FONT_HERSHEY_PLAIN, 5, Scalar(0, 255, 0), 5 );
    putText(result_img, "Number of eyes : " + to_string(final_eyes.size()) , Point(100,220),
        FONT_HERSHEY_PLAIN, 5, Scalar(255, 0, 0), 5 );

    
    for (const auto& eye : allEyes) {
        circle(result_img, eye.center, (int)eye.radius, Scalar(0, 0, 255), 3);
    
        int s = 10; // Size of Center Closs
        line(result_img, Point(eye.center.x - s, eye.center.y), 
                     Point(eye.center.x + s, eye.center.y), Scalar(0, 255, 0), 3);
        line(result_img, Point(eye.center.x, eye.center.y - s), 
                     Point(eye.center.x, eye.center.y + s), Scalar(0, 255, 0), 3);
    }

    // Get the actual width of the primary monitor
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int targetX = -1920; // Intended position (e.g., for sub-monitor)
    // Safety Check: If the target position is beyond the current screen width,
    // reset it to 0 (primary monitor) to avoid "losting" the window.
    if (targetX >= screenWidth) {
        targetX = 0;
    }
    // Create a Window with a specific name and manual sizing capability
    // WINDOW_NOMAL allows you to resize the window
    namedWindow("Filtered Image", WINDOW_NORMAL);
    // Move the window to the top-left corner (0,0)
    moveWindow("Filtered Image", targetX,0);
    // Set the desired window size (width, height)
    resizeWindow("Filtered Image", 800, 600);

    imshow("Filtered Image", result_img); 

    waitKey(0);

    return 0;

}