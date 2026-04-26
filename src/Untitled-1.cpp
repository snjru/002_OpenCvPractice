#include <windows.h>
#include <commdlg.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

// Windows APIを使ってファイル選択ダイアログを表示する関数
std::string openFileDialog() {
    char filename[MAX_PATH] = {0};
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = "Image Files (*.jpg;*.png)\0*.jpg;*.png\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

    if (GetOpenFileName(&ofn)) return std::string(filename);
    return "";
}

int main() {
    std::string path = openFileDialog();
    if (path.empty()) return 0;

    cv::Mat img = cv::imread(path);
    if (img.empty()) return -1;

    // モニターサイズに合わせて初期縮小（横幅1200ピクセル基準）
    int maxWidth = 1200;
    if (img.cols > maxWidth) {
        double scale = (double)maxWidth / img.cols;
        cv::resize(img, img, cv::Size(), scale, scale);
    }

    cv::namedWindow("Dice Viewer", cv::WINDOW_NORMAL);
    cv::imshow("Dice Viewer", img);

    std::cout << "画像を表示中。キーを押すと終了します。" << std::endl;
    cv::waitKey(0);

    return 0;
}