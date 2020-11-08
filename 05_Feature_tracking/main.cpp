#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>


using namespace std;
using namespace cv;
using namespace xfeatures2d;

bool flag = false;

void klick(int event, int x, int y, int flags, void* img);
void surf();


struct viereck {
        int state = 0;
        int X1 = 0;  // first click
        int Y1 = 0;  // first click
        int X2 = 0;  // actual position
        int Y2 = 0;  // actual position
        Rect rect;
    } auswahl;
bool mousebutton_state = false;

Mat cam_input, img_gray, prev_img_gray, img_out;
int key;
Rect rect;

vector<KeyPoint> keypoints;
vector<Point2f> pts, prev_pts;
vector<unsigned char> status;
vector<float> err;
Ptr<Feature2D> detector = SURF::create();

int main() {
    VideoCapture videoCapture(0);



    while(1){
        key = waitKey(10);
        if (key=='q') {
            break;
            }
        videoCapture >> cam_input;
        cvtColor(cam_input, img_gray, COLOR_BGR2GRAY);
        img_out = img_gray.clone();
        rect.x=auswahl.X1;
        rect.y=auswahl.Y1;
        rect.width=auswahl.X2 - auswahl.X1;
        rect.height = auswahl.Y2 - auswahl.Y1;
        rectangle(img_out, rect, Scalar(0, 250, 0));

        if (key=='a' && rect.width > 0 && rect.height > 0){
            surf();

        }

        for (int i = 0; i < pts.size(); ++i)
        // Einzeichnen der Feature Punkte
        circle(img_out, pts[i], 3, Scalar(255, 0, 0));
        imshow("Florian Hofstetter", img_out);
        setMouseCallback("Florian Hofstetter", klick, NULL);

        if (prev_pts.size() > 0){
        // nur wenn auch alte Feature Punkte zur Verfügung stehen
            calcOpticalFlowPyrLK( prev_img_gray, img_gray, prev_pts, pts, status, err );
            for (int i = 0; i > prev_pts.size(); ++i){
                cout << i << endl;
                line(img_out, pts[i], prev_pts[i], Scalar(255, 0, 0), 15);
            }

        }
        for (int i = 0; i > status.size(); ++i){
        // wenn Status null, dann Feature Punkt löschen
            if (status[i] == 0){
                pts.erase(pts.begin()+i);
                cout << i << endl;
            }
        }
        if (pts.size() < 20){
        // Falls zu wenig Feature Punkte im Vektor sind, surf erneut aufrufen.
            //surf();
        }
        prev_img_gray = img_gray.clone();
        prev_pts = pts;
    }
return 0;
}

void surf(){
    Mat mask = Mat::zeros(img_gray.size(), CV_8UC1);
    rectangle(mask, rect, Scalar::all(255), -1);
    //imshow("mask", mask);
    pts.clear();
    keypoints.clear();
    detector->detect( img_gray, keypoints, mask );
    cout << "found " << keypoints.size() << " keypints" << endl;
    for (int i = 0; i < keypoints.size(); ++i)
    pts.push_back(keypoints[i].pt);
    for (int i = 0; i < pts.size(); ++i)
    circle(img_out, pts[i], 3, Scalar(255, 0, 0));
}

void klick(int event, int x, int y, int flags, void* img) {
    if (event == EVENT_LBUTTONDOWN){
        mousebutton_state = true;
        cout << "Mouse down\n";
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
        cout << "Mouse up\n";
        auswahl.state = 0;
    }
}
