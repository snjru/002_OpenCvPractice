#include <iostream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include "include/lut_loader.h"
#include "include/graph_utils.h"

using namespace std;
using namespace cv;
using namespace Mat;


/**
 * Loads a Gamma LUT from a CSV file.
 * Expected format; Index, Value(0-255)
 */
bool loadLUTfromCSV(const string& path, Mat& lut){
    ifstream file(path);
    if(!file.is_open()){
        cerr << "Error: Could not open file: " << path << endl;
        return false;
    }

    string line;
    // Skip the header line
    if (!getline(file, line)) return false;

    int rowCount = 0;
    while (getline(file, line)){
        stringstream ss(line);
        string item;

        try{
            // Read Index
            if(!getline(ss, item, ',')) continue;
            int idx = stoi(item);

            // Read Value
            if(!getline(ss, item, ',')) continue;
            int val = stoi(item);

            // Range validation
            if(idx < 0 || idx > 255 || val < 0 || val > 255){
                // Report the specific invalid value to the user
                cerr << "Error: Value out of range [0-255] detected." << endl;
                cerr << "At Row" << (rowcount + 2) << " -> Index: " << idx << ", Value: " << val << endl;
                return false;
            }

            // Assign value to the OpenCV LUT Matrix
            lut.at<uchar>(idx) = static_cast<uchar>(val);
            rowCount++;
        } catch(const exception& e) {
            cerr << "Error: Numerical conversion failed at row " << (rowCount + 2) << endl;
            cerr << "Details: " << e.what() << endl;
            return false;
        }
    }

}

int main(int argc, char* argv[]){
    
    // Check command line arguments
    // Expected: <executable> <conv_filename.csv> <output_filename_without_extension>
    if (argc < 3){
        cerr << "Usage: " << argv[0] << " <cnv_file name(csv)> <output_image file name>" << endl;
        cerr << "Example: " << argv[0] << " a(csv) b(png)" << endl;
        return 1;
    }

    string csv_path = argv[1];
    string img_path = argv[2];

    // 1. LUT & Image Load
    Mat lut = Mat::zeros(1, 256, CV_8U);
    if (!loadLUTfromCSV(csv_path, lut)) return -1;

    Mat src = imread(img_path);
    if (src.empty()) {
        cerr << "Error: Could not open image." << endl;
        return -1;
    }

    // 2. Apply LUT
    Mat dst;
    cv::LUT(src, lut, dst);

    // 3. Create Graph (Height: 512px)
    int h = 512;
    Mat graph = Mat::ones(h, h, CV_8UC3) * 255;
    drawToneCurve(graph, lut, Scalar(255, 100, 0)); // Blue line

    // 4. Resize to match Graph Height (Keeping Aspect Ratio)
    Mat src_res, dst_res;
    double scale = static_cast<double>(h) / src.rows;
    resize(src, src_res, Size(), scale, scale, INTER_AREA);
    resize(dst, dst_res, Size(), scale, scale, INTER_AREA);

    // Ensure all images are 3-channel BGR for concatenation
    if (src_res.channels() == 1) cvtColor(src_res, src_res, COLOR_GRAY2BGR);
    if (dst_res.channels() == 1) cvtColor(dst_res, dst_res, COLOR_GRAY2BGR);
    // graph は既に CV_8UC3 (3チャンネル) で作成しているので変換不要

    // 5. Concatenate Images in Order: [Original | Graph | Processed]
    // 左端を元の画像にするために、src_res を先頭に配置します
    Mat combined;
    vector<Mat> imgs = { src_res, graph, dst_res };
    hconcat(imgs, combined);

    // 6. Save & Display
    size_t lastdot = img_path.find_last_of(".");
    string base = (lastdot == string::npos) ? img_path : img_path.substr(0, lastdot);
    
    //imwrite(base + "_processed.png", dst);
    imwrite(base + "_comparison.jpg", combined);

    // ウィンドウ表示
    namedWindow("Result: Original | Graph | Processed", WINDOW_AUTOSIZE);
    imshow("Result: Original | Graph | Processed", combined);
    
    cout << "------------------------------------------" << endl;
    cout << "Layout Order: [Original] -> [Graph] -> [Processed]" << endl;
    cout << "Saved comparison to: " << base + "_comparison.jpg" << endl;
    cout << "------------------------------------------" << endl;

    waitKey(0);
    return 0;
}