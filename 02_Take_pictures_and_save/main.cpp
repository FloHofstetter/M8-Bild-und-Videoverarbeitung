//**************************************************************************************************************
// Project           : M8-HA2-A1
//
// Program name      : main.cpp
//
// Author            : Florian Hofstetter
//
// Date created      : 04-Mai-2020
//
// Purpose           : Take pictures and save to disk.
//
// Revision History  :
//+--------------+---------------+----------+--------------------------------------------------------------------
// DATE          | AUTHOR        | VERSION  | COMMENT
//+--------------+---------------+----------+--------------------------------------------------------------------
//| 04-Mai-2020  | Hofstetter    | 1.0.0    | Initial Version
//+--------------+---------------+----------+--------------------------------------------------------------------
//
//***************************************************************************************************************


#include <iostream>
#include <opencv2/opencv.hpp>
int main() {
    using namespace std;
    using namespace cv;
    const int boardH = 9;
    const int boardW = 6;
    int q;      // key for bracke out loop
    bool found; // found chess board
    vector<Point2f> corners;
    cout << "This is M8-HA2-A1!" << endl;
    Mat cameraMatrix, distCoeffs, img, img0, out, birdView, H, rvec, tvec, rmat;
    VideoCapture videoCapture(0);
    FileStorage fs("lifecam.yml", FileStorage::READ);
    if (!fs.isOpened()) {
        cerr << "failed to open camera.yml" << endl;
        return -1;
    }
    fs["camera_matrix"] >> cameraMatrix;
    fs["distortion_coefficients"] >> distCoeffs;

    while(1) {
        #ifdef __linux__
        cout << "\033[2J\033[1;1H";
        #endif
        videoCapture >> img0;
        flip(img0, img0, +1);
        undistort(img0, img, cameraMatrix, distCoeffs);
        found = findChessboardCorners(img, Size(boardH, boardW),
                corners, CALIB_CB_ADAPTIVE_THRESH);
        out = img.clone();
        if(!found) {
            cout << "Corners not detected!" << endl;
            //cout << "Found!" << endl;
            out = Scalar(255, 255, 255) - out;
        }
        else {
            cout << "Corners detected!" << endl;
            Point2f imgPts[4];        // vektor for corner
            imgPts[0] = corners[0];   // upper left
            imgPts[1] = corners[8];   // upper right
            imgPts[2] = corners[45];  // bottom left
            imgPts[3] = corners[53];  // bottom right
            circle(out, imgPts[0], 9, Scalar(255, 0, 0), 3);    // BGR -> Red
            circle(out, imgPts[1], 9, Scalar(0, 255, 0), 3);    // BGR -> Green
            circle(out, imgPts[2], 9, Scalar(0, 0, 255), 3);    // BGR -> Red
            circle(out, imgPts[3], 9, Scalar(0, 255, 255), 3);  // BGR -> Yellow
            float length = 23.;    // one square in mm
            float X = length*(8.); // width chessboard corner rectangle in cm
            float Y = length*(5.); // height chessboard corner rectangle in cm
            Point2f objPts[4];
            objPts[0].x = 0 + 640/2; objPts[0].y = 0 + 480/2;
            objPts[1].x = X + 640/2; objPts[1].y = 0 + 480/2;
            objPts[2].x = 0 + 640/2; objPts[2].y = Y + 480/2;
            objPts[3].x = X + 640/2; objPts[3].y = Y + 480/2;
            H = getPerspectiveTransform(objPts, imgPts);
            cout << "homography matrix: " << endl << H << endl;
            warpPerspective(img, birdView, H, img.size(),
                            WARP_INVERSE_MAP + INTER_LINEAR, BORDER_CONSTANT,
                            Scalar::all(0)
            );
            imshow("bird", birdView);
            vector<Point2f> image_points;
            vector<Point3f> object_points;
            for (int i=0; i<4; ++i) {
                image_points.push_back(imgPts[i]);
                object_points.push_back(Point3f(objPts[i].x, objPts[i].y, (float)0));
            }
            bool solved = solvePnP(object_points, image_points,
                                   cameraMatrix, Mat(), rvec, tvec
                          );
            cout << "Image Points: " << endl << image_points << endl;
            cout << "Object Points: " << endl << object_points << endl;
        }
        imshow("Florian Hofstetter", out);
        q = waitKey(10);
        if (q=='q') {
            break;
        }
    }
    return 0;
}
