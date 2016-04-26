#pragma once

#include "ofMain.h"
#include "ofxLibdc.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
using namespace cv;
using namespace ofxCv;

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    // ofxLibdc is a namespace, and Camera is the object
    ofxLibdc::Camera camera;
    
    // ofImage is where we store the current frame we grabbed from the Camera
    ofImage curFrame;
    
    // include these just to make sure they compile
    ofxLibdc::Grabber grabber;
    ofxLibdc::PointGrey pointGrey;
    
    ofxCvGrayscaleImage rawImage;
    ofxCvColorImage colorImage;
    ofImage processed;
    
    float brightness;
    
    ofImage bayer, rgb;
    int bayerType;
    string bayerName;
};
