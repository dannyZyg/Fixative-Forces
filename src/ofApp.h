#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ParticleSystem.hpp"
#include "Particle.hpp"
#include "Attractor.hpp"
#include "ParameterSmoother.hpp"


using namespace cv;

// Windows users:
// You MUST install the libfreenect kinect drivers in order to be able to use
// ofxKinect. Plug in the kinect and point your Windows Device Manager to the
// driver folder in:
//
//     ofxKinect/libs/libfreenect/platform/windows/inf
//
// This should install the Kinect camera, motor, & audio drivers.
//
// You CANNOT use this driver and the OpenNI driver with the same device. You
// will have to manually update the kinect device to use the libfreenect drivers
// and/or uninstall/reinstall it in Device Manager.
//
// No way around the Windows driver dance, sorry.

// uncomment this to read from two kinects simultaneously
//#define USE_TWO_KINECTS

class ofApp : public ofBaseApp {
public:
	
	void setup();
	void update();
	void draw();
	void exit();
    void kinectDraw();
	void drawPointCloud();
	
	void keyPressed(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
    void opticalFlowUpdate();
    void opticalFlowDraw();
    
    ParticleSystem system;
    ofxKinect kinect;
	
#ifdef USE_TWO_KINECTS
	ofxKinect kinect2;
#endif
	
	ofxCvColorImage colorImg;
	
	ofxCvGrayscaleImage grayImage; // grayscale depth image
	ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
	ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
	ofxCvContourFinder contourFinder;
    
    vector <ofxCvBlob> blobs;
    vector <ofPolyline> polylines;
    ofPolyline largestBlob;
    ofPath path;
	
	bool bThreshWithOpenCV;
	int nearThreshold;
	int farThreshold;
	int angle;
    bool calculatedFlow;
    
    ofxCvColorImage currentColor;        //First and second original images
    ofxCvGrayscaleImage gray1, gray2;    //Decimated grayscaled images
    ofxCvFloatImage flowX, flowY;        //Resulted optical flow in x and y axes
    
    float sumX, sumY, avgX, avgY;
    int numOfEntries;
    bool debug;
    int mode;
    ofColor blobFrom;
    ofColor blobTo;
    ofColor blobColor;
   
    
};
