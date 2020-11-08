#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>

using namespace std;
using namespace cv;

int main() {
    std::cout << "Test!\n";
    Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(aruco::DICT_6X6_250);
    // create board
    Ptr<cv::aruco::GridBoard> board = cv::aruco::GridBoard::create(5, 7, 0.04, 0.01, dictionary);
    cv::Mat boardImage;
    board->draw(cv::Size(2100, 2800), boardImage, 10, 1);
    imwrite("aruco.png",boardImage);
    return 0;
}

