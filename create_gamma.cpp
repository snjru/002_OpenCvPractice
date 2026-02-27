#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char* argv[]){
    
    // Check command line arguments
    // Expected: <executable> <gamma_value> <output_filename_without_extension>
    if (argc < 3){
        cerr << "Usage: " << argv[0] << " <gamma_vlue> <output_name>" << endl;
        cerr << "Example: " << argv[0] << " 2.2 my_gamma_setting" << endl;
        return 1;
    }

    // Parse arguments
    double gamma = stod(argv[1]);
    string base_name = argv[2];
    string csv_name = base_name + "_value_" + argv[1] + ".csv";
    string img_name = base_name + "_value_" + argv[1] + ".png";
    
    // Calculate LUT values and save to CSV
    vector<uchar> lut(256);
    ofstream ofs(csv_name);
    if(!ofs){
        cerr << "Error: Could not create file " << csv_name << endl;
        return 1;
    }

    // CSV Header
    ofs << csv_name << endl;
    ofs << "Gamma Value is " << argv[1] << endl;

    for( int i = 0; i < 256 ; i++){
        // Gamma Correction Formula: Output = (Input /  255)^gamma * 255
        double normalized = static_cast<double>(i) /  255.0;
        double corrected = pow(normalized, gamma) *  255.0;

        // Round and clamp to 0-255 using OpenCV's utility
        uchar result = saturate_cast<uchar>(round(corrected));

        lut[i] = result;
        ofs << i << "," << static_cast<int>(result) << endl;
    }
    ofs.close();

    // Draw the Tone Curve Graph (512*512 pixels)
    int canvas_size = 512;
    Mat graph(canvas_size, canvas_size, CV_8UC3, Scalar(255, 255, 255));
    // Draw reference line (identity mapping: y = x) in light gray
    line(graph, Point(0, canvas_size), Point(canvas_size, 0), Scalar(220, 220, 220), 1);

    // Plot the curve using line segments
    for (int i = 0; i < 255; ++i){
        // Map 0-255 range to canvas size
        // Note: Y-coordinate is inverted because image (0,0) is top-left
        Point p1(static_cast<int>(i * (canvas_size / 256.0)), 
                     static_cast<int>(canvas_size - (lut[i] * (canvas_size / 256.0))));
        Point p2(static_cast<int>((i + 1) * (canvas_size / 256.0)), 
                     static_cast<int>(canvas_size - (lut[i + 1] * (canvas_size / 256.0))));
        
        // Draw the curve in blue
        line(graph, p1, p2, Scalar(255, 100, 0), 2, LINE_AA);
    }

    // Add text info to the graph
    putText(graph, "Gamma: " + to_string(gamma), Point(20, 40),
            FONT_HERSHEY_SIMPLEX, 0.7, Scalar(50, 50, 50), 2);
    putText(graph, "Source: " + csv_name, Point(20, 70),
            FONT_HERSHEY_SIMPLEX, 0.5, Scalar(150, 150, 150), 1);

    // Save the graph as an image
    imwrite(img_name, graph);

    // Output summary and Display
    cout << "------------------------------------------" << endl;
    cout << "Process Complete!" << endl;
    cout << "CSV Data:     " << csv_name << endl;
    cout << "Graph Image:  " << img_name << endl;
    cout << "------------------------------------------" << endl;
    cout << "Opening window. Press any key to exit..." << endl;

    imshow(base_name, graph);
    waitKey(0);

    return 0;
}