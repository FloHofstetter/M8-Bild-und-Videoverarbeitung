#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>

using namespace std;
using namespace cv;

int main() {
    cout << "This is M8-HA2-A2!\n";
    vector<vector<Point2f>> corners;
    vector<int> ids;
    vector<Vec3d> rvecs;
    vector<Vec3d> tvecs;
    Mat img, img2, cameraMatrix, distCoeff;
    int q;      // key for bracke out loop
    int id;
    FileStorage fs("lifecam.yml", FileStorage::READ);
    if (!fs.isOpened()) {
        cerr << "failed to open camera.yml" << endl;
        return -1;
    }
    fs["camera_matrix"] >> cameraMatrix;
    fs["distortion_coefficients"] >> distCoeff;
    VideoCapture videoCapture(0);
    Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(aruco::DICT_6X6_250);
    Ptr<cv::aruco::GridBoard> board = cv::aruco::GridBoard::create(5, 7, 0.04, 0.01, dictionary);
    while(1) {
        videoCapture >> img;
        img.copyTo(img2);
        aruco::detectMarkers( img, dictionary, corners, ids );
        aruco::drawDetectedMarkers(img, corners, ids);
        aruco::estimatePoseSingleMarkers(corners, 1.0, cameraMatrix, distCoeff, rvecs, tvecs);
        for (id=0; id < ids.size(); id++) {
            aruco::drawAxis(img, cameraMatrix, distCoeff, rvecs[id], tvecs[id], 1 );
            cout << "Loop: " << id;
            cout << "\n";
            }
        q = waitKey(10);
        //flip(img, img, +1);
        imshow("Florian Hofstetter", img);
        if (q=='q') {
            break;
            }
        }
    return 0;
}
