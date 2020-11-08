#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>


using namespace std;
using namespace cv;
using namespace xfeatures2d;

void klick(int event, int x, int y, int flags, void* img);
void printMat(Mat mat);

struct viereck {
        int state = 0;
        int X1 = 0;  // first click
        int Y1 = 0;  // first click
        int X2 = 0;  // actual position
        int Y2 = 0;  // actual position
        Rect rect;
    } auswahl;
bool mousebutton_state = false;

int main() {
    cout << "This is M8-HA4-A1!\n";
    int state = 0;
    int state_init_flag = true;
    Mat img, img_gray, obj, img_out;
    Rect rect;
    int key; // brake capture loop
    int minHessian = 400;
    int minHessian2 = 400;
    Ptr<Feature2D> detector = SURF::create(minHessian);
    Ptr<Feature2D> detector2 = SURF::create(minHessian2);
    vector<KeyPoint> obj_keypoints; // Merkmalspunkte
    Mat obj_descriptors; // Merkmalsvektoren
    vector<KeyPoint> scene_keypoints; // Merkmalspunkte
    Mat scene_descriptors; // Merkmalsvektoren
    FlannBasedMatcher matcher;
    vector<DMatch> matches;
    vector<DMatch> good_matches;
    vector<Point> obj_goodPoints; // Merkmalspunkte
    vector<Point> scn_goodPoints; // Merkmalspunkte
    double max_dist = 0;
    double min_dist = numeric_limits<double>::max();
    double dist;
    vector<Point2f> object;
    vector<Point2f> scene;
    Mat H;
    vector<Point2f> obj_corners(4);
    vector<Point2f> scene_corners(4);

    VideoCapture videoCapture(0);
    videoCapture.set(CAP_PROP_FRAME_WIDTH, 320);
    videoCapture.set(CAP_PROP_FRAME_HEIGHT, 240);

    while(1) {
        videoCapture >> img;
        flip(img, img, +1);
        setMouseCallback("Florian Hofstetter", klick, NULL);
        key = waitKey(10);
        switch (state) {
            case 0:
                if (state_init_flag) {
                    #ifdef __linux__
                    cout << "\033[2J\033[1;1H";
                    #endif
                    cout << "State 0\n";
                    cout << "Press \"A\" to marke the Object\n";
                    state_init_flag = false;
                }
                if (key == 'a') {
                    state = 1;
                    state_init_flag = true;
                }
                break;

            case 1:
                if (state_init_flag) {
                    #ifdef __linux__
                    cout << "\033[2J\033[1;1H";
                    #endif
                    cout << "State 1\n";
                    cout << "Mark the Objekt and:\n";
                    cout << "    Press \"C\" to continue.\n";
                    cout << "    Press \"R\" to restart.\n";
                    state_init_flag = false;
                }

                rect.x=auswahl.X1;
                rect.y=auswahl.Y1;
                rect.width=auswahl.X2 - auswahl.X1;
                rect.height = auswahl.Y2 - auswahl.Y1;
                rectangle(img, rect, Scalar(0, 250, 0));
                if (key == 'R' || key == 'r') {
                    state = 0;
                    state_init_flag = true;
                }
                else if (key == 'C' || key == 'c'){
                    state = 2;
                    state_init_flag = true;
                }
                break;
            case 2:
                if (state_init_flag) {
                    #ifdef __linux__
                    cout << "\033[2J\033[1;1H";
                    #endif
                    cout << "State 2\n";
                    cout << "Serach for Feature Vectores\n";
                    cout << "    Press \"B\" to rearrange your selection.\n";
                    cout << "    Press \"C\" to continue.\n";
                    state_init_flag = false;
                }
                rectangle(img, rect, Scalar(0, 0, 250));
                img(rect).copyTo(obj);
                cvtColor(obj, obj, COLOR_BGR2GRAY);
                detector->detectAndCompute( obj, noArray(), obj_keypoints, obj_descriptors);
                if (key == 'b' || key == 'B') {
                    state = 1;
                    state_init_flag = true;
                }
                else if (key == 'c' || key == 'C') {
                    state = 3;
                    state_init_flag = true;
                }
                break;
            case 3:
                if (state_init_flag) {
                    #ifdef __linux__
                    cout << "\033[2J\033[1;1H";
                    #endif
                    cout << "State 3\n";
                    cout << "    Press \"B\" to search Feature Vectores.\n";
                    //cout << "    Press \"C\" to continue.\n";
                    state_init_flag = false;
                    //obj = img(rect);
                }
                //obj = img(rect);
                //rectangle(img, rect, Scalar(0, 0, 250));
                //imshow("Objekt", obj);
                //cvtColor(obj, obj, COLOR_BGR2GRAY);
                cvtColor(img, img_gray, COLOR_BGR2GRAY);
                //imshow("gray", img_gray);
                detector2->detectAndCompute( img_gray, noArray(), scene_keypoints, scene_descriptors);
                FlannBasedMatcher matcher;
                std::vector< DMatch > matches;
                matcher.match(obj_descriptors, scene_descriptors, matches);
                //matcher.match( img_descriptors, obj_descriptors, matches );
                cout << "found " << matches.size() << " matches" << std::endl;
                for (auto match : matches) {
                    dist = match.distance;
                    if (dist < min_dist) min_dist = dist;
                    if (dist > max_dist) max_dist = dist;
                }
                //----------
                good_matches.clear();
                obj_goodPoints.clear();
                scn_goodPoints.clear();
                for (auto match : matches) {
                    if (match.distance < 25 * min_dist) {
                        good_matches.push_back(match);
                        obj_goodPoints.push_back(obj_keypoints[match.queryIdx].pt);
                        scn_goodPoints.push_back(scene_keypoints[match.trainIdx].pt);
                    }
                }
                //----
                drawMatches(obj, obj_keypoints, img_gray, scene_keypoints, good_matches, img_out);
                scene.clear();
                object.clear();
                for (unsigned int i = 0; i < good_matches.size(); i++) {
                    //Get the keypoints from the good matches
                    object.push_back(obj_keypoints[good_matches[i].queryIdx].pt);
                    scene.push_back(scene_keypoints[good_matches[i].trainIdx].pt);
                }
                Mat H = findHomography(object, scene, RANSAC);
                if (!H.empty())
                {

                    obj_corners[0] = Point(0, 0);
                    obj_corners[1] = Point(obj.cols, 0);
                    obj_corners[2] = Point(obj.cols, obj.rows);
                    obj_corners[3] = Point(0, obj.rows);


                    perspectiveTransform(obj_corners, scene_corners, H);
                    printMat(H);
                }
                Point2f p0 = Point2f(obj.cols, 0);

                line(img_out, p0 + scene_corners[0], p0 + scene_corners[1], Scalar(0, 255, 0), 3);
                line(img_out, p0 + scene_corners[1], p0 + scene_corners[2], Scalar(0, 255, 0), 3);
                line(img_out, p0 + scene_corners[2], p0 + scene_corners[3], Scalar(0, 255, 0), 3);
                line(img_out, p0 + scene_corners[3], p0 + scene_corners[0], Scalar(0, 255, 0), 3);
                imshow("Image out", img_out);

                if (key == 'b' || key == 'B') {
                    state = 2;
                    state_init_flag = true;
                }
                break;
        }

        imshow("Florian Hofstetter", img);
        if (key=='q') {
            break;
        }

    }
    return 0;
}

void klick(int event, int x, int y, int flags, void* img) {
    if (event == EVENT_LBUTTONDOWN){
        mousebutton_state = true;
        // cout << "Mouse down\n";
        auswahl.X1 = x;
        auswahl.Y1 = y;
        auswahl.X2 = x;
        auswahl.Y2 = y;
        auswahl.state = 1;
    }
    else if (event == EVENT_MOUSEMOVE) {
        // cout << "Mouse move\n";
        if (mousebutton_state) {
            auswahl.X2 = x;
            auswahl.Y2 = y;
        }

    }
    else if (event == EVENT_LBUTTONUP) {
        mousebutton_state = 0;
        // cout << "Mouse up\n";
        auswahl.state = 0;
    }
}

void printMat(Mat mat)
{
    #ifdef __linux__
    cout << "\033[2J\033[1;1H";
    #endif
    for (int i = 0; i<mat.size().height; i++)
    {
        cout << "[";
        for (int j = 0; j<mat.size().width; j++)
        {
            cout << std::fixed << setw(10) << setprecision(2) << mat.at<double>(i, j);
            if (j != mat.size().width - 1)
                cout << ", ";
            else
                cout << "]" << endl;
        }
    }
}
