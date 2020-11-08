//***************************************************************************************************************
// Project           : M8-HA1-A4
//
// Program name      : main.cpp
//
// Author            : Florian Hofstetter
//
// Date created      : 26-Apr-2020
//
// Purpose           : Record distored and undestord images.
//
// Revision History  :
//+--------------+---------------+----------+--------------------------------------------------------------------
// DATE          | AUTHOR        | VERSION  | COMMENT
//+--------------+---------------+----------+--------------------------------------------------------------------
//| 26-Apr-2020  | Hofstetter    | 1.0.0    | Initial Version
//+--------------+---------------+----------+--------------------------------------------------------------------
//
//***************************************************************************************************************

#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>

int main() {
    std::ofstream logfile;
    logfile.open("M8-HA1-A4.log");
    unsigned int imgcounter = 0;  // counting saved images
    bool undistored_flag = false;  // false = distored, true = undistored
    std::cout << "Hello OpenCV" << std::endl;
    logfile << "Hello OpenCV" << std::endl;
    cv::namedWindow("Hello", 0);
    cv::VideoWriter videoWriter;
    cv::VideoCapture videoCapture(0);
    // videoCapture.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    // videoCapture.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    while (1) {
        cv::Mat img;  // for image Input
        cv::Mat img2;  // for undistored picture
        cv::Mat cameraMatrix = (cv::Mat_<double>(3,3) <<  654.210, 0., 318.557, 0., 658.899, 229.735, 0., 0., 1.);
        cv::Mat distCoeffs = (cv::Mat_<double>(1,5) << 0.0103, -0.267, -0.006, 0.002, 0.0);
        videoCapture >> img;  // Cature device to matrice
        if ( img.empty() ) {
            // exit if no image
            std::cout << "Can't read image...\nexit now\n";
            logfile << "Can't read image...\nexit now\n";
            return -1;
            }
        if (undistored_flag) {
            // only if user sets undistorsion
            undistort(img, img2, cameraMatrix , distCoeffs);
            img = img2;
        }
        cv::imshow("Hello", img);
        int c = cv::waitKey(10);
        if (c == 's') {
            cv::imwrite("bild_" + std::to_string(imgcounter) + ".png", img);
            std::cout << "Saved image " + std::to_string(imgcounter) + "\n";
            logfile << "Saved image " + std::to_string(imgcounter) + "\n";
            if (imgcounter < 2e64 - 1) {
                imgcounter++;
            }
            else {
                // Image counter overflow
                std::cout << "To many images taken, exit now\n";
                logfile << "To many images taken, exit now\n";
                break;
            }
        }
        else if (c=='x') {
            // User wants to exit
            std::cout << "Bye!\n";
            logfile << "Programm terminated by user request\n";
            return -1;
        }
        else if (c=='u') {
            // User wants undistored picture
            undistored_flag = !undistored_flag;
        }
    }
    logfile.close();

    return -1;
}
