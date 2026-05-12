#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <windows.h>
#include <commdlg.h>

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
    int value_C = 5;        
    // Convert to grayscale
    Mat img_gray;
    Mat img_thrs_gray;
    cvtColor(img_original, img_gray, COLOR_BGR2GRAY);
    adaptiveThreshold(img_gray, img_thrs_gray, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 11 ,value_C);
    
    //// Blurs
    // Blur
    Mat img_blur;
    Mat img_thrs_blur;
    blur(img_gray, img_blur, Size(5,5));    
    adaptiveThreshold(img_blur, img_thrs_blur, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 11, value_C);
    // GaussianBlur
    Mat img_gaussian_blur;
    Mat img_thrs_gaussian_blur;
    GaussianBlur(img_gray, img_gaussian_blur, Size(9,9), 10, 10);
    adaptiveThreshold(img_gaussian_blur, img_thrs_gaussian_blur, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 11, value_C);
    // medianBlur
    Mat img_median_blur;
    Mat img_thrs_median_blur;
    medianBlur(img_gray, img_median_blur, 5);
    adaptiveThreshold(img_median_blur, img_thrs_median_blur, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 11, value_C);
    // bilateralFilter
    Mat img_brt_filter;
    Mat img_thrs_brt_filter;
    bilateralFilter(img_gray, img_brt_filter, 0, 60.0, 60.0);
    adaptiveThreshold(img_brt_filter, img_thrs_brt_filter, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 11, value_C);

    // Create a Window with a specific name and manual sizing capability
    // WINDOW_NOMAL allows you to resize the window
    string win1 = "Original Image";
    string win2 = "Grayscale Image";
    string win2_t = "Tresholded Grayscale Image";
    string win3 = "Blur Image";
    string win3_t = "Tresholded Blur Image";
    string win4 = "GaussianBlur Image";
    string win4_t = "Thresholded GaussianBlur Image";
    string win5 = "MedianBlure Image";
    string win5_t = "Thresholded MedianBlure Image";
    string win6 = "BilateralFilter Image";
    string win6_t = "Thresholded BilateralFilter Image";
    
    int width = 380;
    int height = 280;
    namedWindow(win1, WINDOW_NORMAL);
    namedWindow(win2, WINDOW_NORMAL);
    namedWindow(win2_t, WINDOW_NORMAL);
    namedWindow(win3, WINDOW_NORMAL);
    namedWindow(win3_t, WINDOW_NORMAL);
    namedWindow(win4, WINDOW_NORMAL);
    namedWindow(win4_t, WINDOW_NORMAL);
    namedWindow(win5, WINDOW_NORMAL);
    namedWindow(win5_t, WINDOW_NORMAL);
    namedWindow(win6, WINDOW_NORMAL);
    namedWindow(win6_t, WINDOW_NORMAL);

    // Move the window to the top-left corner (0,0)
    int targetX = 0;
    int y_margin = 100;
    moveWindow(win1, targetX, 50);
    moveWindow(win2, targetX, height+y_margin);
    moveWindow(win2_t, targetX, (height*2)+y_margin);
    moveWindow(win3, targetX + width, height+y_margin);
    moveWindow(win3_t, targetX + width, (height*2)+y_margin);
    moveWindow(win4, targetX + (width*2), height+y_margin);
    moveWindow(win4_t, targetX + (width*2), (height*2)+y_margin);
    moveWindow(win5, targetX + (width*3), height+y_margin);
    moveWindow(win5_t, targetX + (width*3), (height*2)+y_margin);
    moveWindow(win6, targetX + (width*4), height+y_margin);
    moveWindow(win6_t, targetX + (width*4), (height*2)+y_margin);
   
    // Set the desired window size (width, height)
    resizeWindow(win1, width, height);
    resizeWindow(win2, width, height);
    resizeWindow(win2_t, width, height);
    resizeWindow(win3, width, height);
    resizeWindow(win3_t, width, height);
    resizeWindow(win4, width, height);
    resizeWindow(win4_t, width, height);
    resizeWindow(win5, width, height);
    resizeWindow(win5_t, width, height);
    resizeWindow(win6, width, height);
    resizeWindow(win6_t, width, height);

    // Show Images
    imshow(win1, img_original);
    imshow(win2, img_gray);
    imshow(win2_t, img_thrs_gray);
    imshow(win3, img_blur);
    imshow(win3_t, img_thrs_blur);
    imshow(win4, img_gaussian_blur);
    imshow(win4_t, img_thrs_gaussian_blur);
    imshow(win5, img_median_blur);
    imshow(win5_t, img_thrs_median_blur);
    imshow(win6, img_brt_filter);
    imshow(win6_t, img_thrs_brt_filter);

    waitKey(0);
    
    destroyAllWindows();

    return 0;

}