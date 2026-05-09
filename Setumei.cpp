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
        
    // Convert to grayscale
    Mat img_gray;
    cvtColor(img_original, img_gray, COLOR_BGR2GRAY);
    
    // Gaussian Blur
    Mat img_blur;
    GaussianBlur(img_gray, img_blur, Size(9,9), 10, 10);
    
    // bilateralFilter
    Mat img_brtFilter;
    bilateralFilter(img_gray, img_brtFilter, 0, 60.0, 60.0);

    // Convert to binary
    Mat img_bin_1;
    adaptiveThreshold( img_gray, img_bin_1, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 11 ,2);
    Mat img_bin_2;
    adaptiveThreshold( img_blur, img_bin_2, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 11 ,2);
    Mat img_bin_3;
    adaptiveThreshold( img_brtFilter, img_bin_3, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 11 ,2);


    // Get the actual width of the primary monitor
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int targetX = -1600; // Intended position (e.g., for sub-monitor)
    // Safety Check: If the target position is beyond the current screen width,
    // reset it to 0 (primary monitor) to avoid "losting" the window.
    if (targetX >= screenWidth) {
        targetX = 0;
    }
    // Create a Window with a specific name and manual sizing capability
    // WINDOW_NOMAL allows you to resize the window
    string win1 = "Original Image";
    string win2 = "Grayscale Image";
    string win3 = "Gaussian Image";
    string win4 = "bilateralFilter Image";
    string win5 = "Binary Image";
    string win6 = "Binary2 Image";
    string win7 = "Binary3 Image";
    
    int width = 450;
    int height = 300;
    namedWindow(win1, WINDOW_NORMAL);
    namedWindow(win2, WINDOW_NORMAL);
    namedWindow(win3, WINDOW_NORMAL);
    namedWindow(win4, WINDOW_NORMAL);
    namedWindow(win5, WINDOW_NORMAL);
    namedWindow(win6, WINDOW_NORMAL);
    namedWindow(win7, WINDOW_NORMAL);

    // Move the window to the top-left corner (0,0)
    moveWindow(win1, targetX,50);
    moveWindow(win2, targetX + width, 50);
    moveWindow(win3, targetX + (width*2), 50);
    moveWindow(win4, targetX + (width*3) , 50);
    moveWindow(win5, targetX + width, height+80);
    moveWindow(win6, targetX + (width*2), height+80);
    moveWindow(win7, targetX + (width*3), height+80);
   
    // Set the desired window size (width, height)
    resizeWindow(win1, width, height);
    resizeWindow(win2, width, height);
    resizeWindow(win3, width, height);
    resizeWindow(win4, width, height);
    resizeWindow(win5, width, height);
    resizeWindow(win6, width, height);
    resizeWindow(win7, width, height);

    // Show Images
    imshow(win1, img_original);
    imshow(win2, img_gray);
    imshow(win3, img_blur);
    imshow(win4, img_brtFilter);
    imshow(win5, img_bin_1);
    imshow(win6, img_bin_2);
    imshow(win7, img_bin_3);

    waitKey(0);

    return 0;

}