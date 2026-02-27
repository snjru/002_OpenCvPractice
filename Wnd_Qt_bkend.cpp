#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

void onButtonPress(int state, void* userdate){
    cout << "The buttom is pushed!" << endl;
}

int main(){

    // Make a empty window
    string winName = "Image Viewer";
    namedWindow( winName, WINDOW_AUTOSIZE);

    // Make a button
    // argument: (name, function, data, kind of button)
    createButton("Open Image", onButtonPress, nullptr, QT_PUSH_BUTTON, false);

    cout << "Window displayed." << endl;
    cout << "Any key to exit." << endl;

    // Wait to keep display
    Mat dummy = Mat::zeros(400, 600, CV_8UC3);
    putText(dummy, "Please push 'Open Image' button", Point(50, 200),
            FONT_HERSHEY_SIMPLEX, 0.8, Scalar(255, 255, 255), 2);

    imshow(winName, dummy);
    waitKey(0);

    return 0;

}
