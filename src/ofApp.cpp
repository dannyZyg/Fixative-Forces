/*
 
 Danny Keig 2018
 
 A program that explores forces through interaction. Uses computer vision (kinect depth image) to isolate a body in the image, facilitating communication
 between small particles/shapes and a human player. Different modes of behaviour are triggered after periods of interaction. After a certain number of
 large waves or strong motions (currently 10) the program will move to the next mode.

 Optical Flow from the video acts as another interactive force.
 
 Originally inspired by magnetic forces, although the program strays away from that a fair bit!
 
 Inspiration:
 
 https://www.youtube.com/watch?v=LMFAOLKaYd0
 https://www.openprocessing.org/sketch/171308
 https://vimeo.com/153944735
 
 
 Programming references and help:
 
 Optical Flow example from WCC class
 
 Transfering ofPolyline to ofPath = https://forum.openframeworks.cc/t/fill-polyline/10800
 
 Particle System help from the Kadenze course Creative Programming for Audiovisual Art (Memo Akten guest lecture)
 
 Parameter Smoother class - original code from https://www.youtube.com/watch?v=BdJRSqgEqPQ, adapted to accept ofPoint
 
 Daniel Shiffman's Nature of Code - Forces chapter
 
 
 */


#include "ofApp.h"

/*
    If you are struggling to get the device to connect ( especially Windows Users )
    please look at the ReadMe: in addons/ofxKinect/README.md
*/

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	// enable depth->video image calibration
	kinect.setRegistration(true);
    
	kinect.init();
	//kinect.init(true); // shows infrared instead of RGB video image
	//kinect.init(false, false); // disable video image (faster fps)
	
	kinect.open();		// opens first available kinect
	//kinect.open(1);	// open a kinect by id, starting with 0 (sorted by serial # lexicographically))
	//kinect.open("A00362A08602047A");	// open a kinect using it's unique serial #
	
    
    
	// print the intrinsic IR sensor values
	if(kinect.isConnected()) {
		ofLogNotice() << "sensor-emitter dist: " << kinect.getSensorEmitterDistance() << "cm";
		ofLogNotice() << "sensor-camera dist:  " << kinect.getSensorCameraDistance() << "cm";
		ofLogNotice() << "zero plane pixel size: " << kinect.getZeroPlanePixelSize() << "mm";
		ofLogNotice() << "zero plane dist: " << kinect.getZeroPlaneDistance() << "mm";
	}
	
#ifdef USE_TWO_KINECTS
	kinect2.init();
	kinect2.open();
#endif
	
	colorImg.allocate(kinect.width, kinect.height);
	grayImage.allocate(kinect.width, kinect.height);
	grayThreshNear.allocate(kinect.width, kinect.height);
	grayThreshFar.allocate(kinect.width, kinect.height);
	
	nearThreshold = 208;
	farThreshold = 160;
	bThreshWithOpenCV = true;
	ofSetFrameRate(60);
	
	// set the tilt on startup
	angle = 9;
	kinect.setCameraTiltAngle(angle);
	
    //setup particle system with 100 particles
    system.setup(100);

    debug = false;
    
    // set mode to 1 and report initial mode
    mode = 1;
    cout<<"MODE = "<< mode << endl;
    
    
}

//--------------------------------------------------------------
void ofApp::update() {
    
    //function that recieves the 'mode' from ParticleSystem.
    mode = system.getMode();
  
    //update for kinect
    
	kinect.update();
	
	// there is a new frame and we are connected
	if(kinect.isFrameNew()) {
		
		// load grayscale depth image from the kinect source
		grayImage.setFromPixels(kinect.getDepthPixels());
        grayImage.mirror(false, true);
		
		// we do two thresholds - one for the far plane and one for the near plane
		// we then do a cvAnd to get the pixels which are a union of the two thresholds
		if(bThreshWithOpenCV) {
			grayThreshNear = grayImage;
			grayThreshFar = grayImage;
			grayThreshNear.threshold(nearThreshold, true);
			grayThreshFar.threshold(farThreshold);
			cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
		} else {
			
			// or we do it ourselves - show people how they can work with the pixels
			ofPixels & pix = grayImage.getPixels();
			int numPixels = pix.size();
			for(int i = 0; i < numPixels; i++) {
				if(pix[i] < nearThreshold && pix[i] > farThreshold) {
					pix[i] = 255;
				} else {
					pix[i] = 0;
				}
			}
		}
		
		// update the cv images
		grayImage.flagImageChanged();
        
		// find contours
		//find holes set to false
		contourFinder.findContours(grayImage, 2000, (kinect.width*kinect.height)/2, 20, false);
        //Store objects' centers
        blobs = contourFinder.blobs;
        
        //Fill vector of polylines with points from all of the detected blobs.
        polylines.clear();
        for (int i=0; i<blobs.size(); i++){
            vector <ofPoint> blobPoints = blobs[i].pts;
            ofPolyline tempPolyline;
            tempPolyline.addVertices(blobPoints);
            tempPolyline.getResampledBySpacing(50);
            polylines.push_back(tempPolyline);
        }
        
	}
	
#ifdef USE_TWO_KINECTS
	kinect2.update();
#endif
    
    //update particle system
    system.update();

    //send the polyline of the largest blob to Particle System class.
    system.receivePoints(largestBlob);

    //update optical flow calculations
    opticalFlowUpdate();
    
}

//--------------------------------------------------------------
void ofApp::draw() {
    
    
   
    //change background colour if in debug.
    if(debug){
        ofBackground(100, 100, 100);
    }
    if(!debug){
        ofBackgroundGradient(ofColor(40,11,86), ofColor( 15,1,38));
    }
    
    // Debug kinect images and text
	ofSetColor(255, 255, 255);
    if(debug){
		// draw from the live kinect
		kinect.drawDepth(10, 10, 400, 300);
        kinect.draw(420, 10, 400, 300);
		
		grayImage.draw(10, 320, 400, 300);
		contourFinder.draw(10, 320, 400, 300);
		
#ifdef USE_TWO_KINECTS
		kinect2.draw(420, 320, 400, 300);
#endif
	
	
	// draw instructions
	ofSetColor(255, 255, 255);
	stringstream reportStream;
        
    if(kinect.hasAccelControl()) {
        reportStream << "accel is: " << ofToString(kinect.getMksAccel().x, 2) << " / "
        << ofToString(kinect.getMksAccel().y, 2) << " / "
        << ofToString(kinect.getMksAccel().z, 2) << endl;
    } else {
        reportStream << "Note: this is a newer Xbox Kinect or Kinect For Windows device," << endl
		<< "motor / led / accel controls are not currently supported" << endl << endl;
    }
    
	reportStream << "press p to switch between images and point cloud, rotate the point cloud with the mouse" << endl
	<< "using opencv threshold = " << bThreshWithOpenCV <<" (press spacebar)" << endl
	<< "set near threshold " << nearThreshold << " (press: + -)" << endl
	<< "set far threshold " << farThreshold << " (press: < >) num blobs found " << contourFinder.nBlobs
	<< ", fps: " << ofGetFrameRate() << endl
	<< "press c to close the connection and o to open it again, connection is: " << kinect.isConnected() << endl;

    if(kinect.hasCamTiltControl()) {
    	reportStream << "press UP and DOWN to change the tilt angle: " << angle << " degrees" << endl
        << "press 1-5 & 0 to change the led mode" << endl;
    }
    
	ofDrawBitmapString(reportStream.str(), 20, 652);
        
    }

    
    //Need to find a polyline with a large perimeter.

    for(int i = 0; i < polylines.size(); i++){

        if(polylines[i].getPerimeter() > 300){
            
            //label that polyline as the largest blob!
            largestBlob = polylines[i].getSmoothed(10);
            largestBlob.setClosed(true);
            
            //Resample the whole line with a rough workaround to reposition the blob without ofTranslation issues between classes
            //A bit hacky, but works well
            vector<ofPoint> tempVert;
            int xAxisOffset = 180;
            int yAxisOffset = 50;

            for(int i = 0; i < largestBlob.size(); i ++){
            tempVert = largestBlob.getVertices();
            tempVert[i].x += xAxisOffset;
            tempVert[i].y += yAxisOffset;
            largestBlob = tempVert;
            }
        }
    }

    
//    ofScale(1.5, 1.5);
    
    
    // calculate ofPath to use for displaying a filled blob for the body
    // Incoming 'mode' changes different colour settings.

    ofPushStyle();
    ofFill();
    ofPath path;
    
    if(mode == 1){
        float lerpAmt = ofMap(avgX, -5, 5, 0., 1.);
        blobFrom = ofColor(11, 186, 221, 127);
        blobTo = ofColor(80, 21, 96, 127);
        blobColor = blobFrom.getLerped(blobTo, lerpAmt);
        path.setFillColor(blobColor);
    }
    if(mode == 2){
        float fillAlpha = ofMap(avgX, -5, 5, 40, 120, true);
        blobColor = ofColor(193, 25, 30, fillAlpha);
        path.setFillColor(blobColor);
        
    }
    if(mode == 3){
        float fillAlpha = ofMap(avgY, 0, 5, 0, 120, true);
        blobColor = ofColor(17, 115, 252, fillAlpha);
        path.setFillColor(blobColor);
    }
     if(mode == 4){
         float alpha = ofMap(ofNoise(ofGetFrameNum() * 0.04), 0, 1, 0, 255);
         float b = ofMap(ofNoise(ofGetFrameNum() * 0.04 + 500), 0, 1, 60, 120);
         float r = ofMap(avgX, -5, 5, 5, 65, true);
         blobColor = ofColor(r, 130, b, alpha);
         path.setFillColor(blobColor);
     }
    
    // transfer points from polyline to path
    
    for( int i = 0; i < largestBlob.getVertices().size(); i++) {
        if(i == 0) {
            path.newSubPath();
            path.moveTo(largestBlob.getVertices()[i] );
        } else {
            path.lineTo( largestBlob.getVertices()[i] );
        }
    }
    
    //display filled ofPath
    path.close();
    path.simplify();
    path.draw();
    ofPopStyle();

    //display particle system
    system.draw();
    
    //draw optical flow and display vectors (only in debug)
    opticalFlowDraw();
}



//--------------------------------------------------------------
void ofApp::exit() {
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
	
#ifdef USE_TWO_KINECTS
	kinect2.close();
#endif
}

//--------------------------------------------------------------
void ofApp::opticalFlowDraw(){
    
    //calculate optical flow and find average optical flow values for X and Y.
    
    sumX = 0;
    sumY =0;
    avgX = 0;
    avgY = 0;
    numOfEntries = 0;
    if (calculatedFlow)
    {
        
        int w = gray1.width;
        int h = gray1.height;
        
        //1. Input images + optical flow
        ofPushMatrix();
        ofScale( 8, 8 );
        
        
        //Optical flow
        float *flowXPixels = flowX.getPixelsAsFloats();
        float *flowYPixels = flowY.getPixelsAsFloats();
        ofSetColor( 0, 0, 255 );
        for (int y = h/2 - 25; y < h/2 + 25; y+=5) {
            for (int x = w/2 - 25; x < w/2 + 25; x+=5) {
                
                
                float fx = flowXPixels[ x + w * y ];
                float fy = flowYPixels[ x + w * y ];
                //Draw only long vectors
                if ( fabs( fx ) + fabs( fy ) > 1 ) {
                    if(debug){
                        ofDrawRectangle( x-0.5, y-0.5, 1, 1 );
                        ofDrawLine( x, y, x + fx, y + fy );
                    }
                    sumX += fx;
                    sumY += fy;
                    
                    numOfEntries ++;
                    
                }
            }
        }
        ofPopMatrix();
    }
    
    // calculate averages
    if(numOfEntries > 0){
        avgX = sumX / numOfEntries;
        avgY = sumY / numOfEntries;
    }

}


//--------------------------------------------------------------

void ofApp::opticalFlowUpdate(){
 
    //Image manipulation to find the optical flow
    
    if(kinect.isFrameNew()) {
        if ( gray1.bAllocated ) {
            gray2 = gray1;
            calculatedFlow = true;
        }
        
        //Convert to ofxCv images
        ofPixels & pixels = kinect.getPixels();
        currentColor.setFromPixels( pixels );
        
        float decimate = 0.25;              //Decimate images to 25% (makes calculations faster + works like a blurr too)
        ofxCvColorImage imageDecimated1;
        imageDecimated1.allocate( currentColor.width * decimate, currentColor.height * decimate );
        imageDecimated1.scaleIntoMe( currentColor, CV_INTER_AREA );             //High-quality resize
        gray1 = imageDecimated1;
        
        if ( gray2.bAllocated ) {
            Mat img1( gray1.getCvImage() );  //Create OpenCV images
            Mat img2( gray2.getCvImage() );
            Mat flow;                        //Image for flow
            //Computing optical flow (visit https://goo.gl/jm1Vfr for explanation of parameters)
            calcOpticalFlowFarneback( img1, img2, flow, 0.7, 3, 11, 5, 5, 1.1, 0 );
            //Split flow into separate images
            vector<Mat> flowPlanes;
            split( flow, flowPlanes );
            //Copy float planes to ofxCv images flowX and flowY
            //we call this to convert back from native openCV to ofxOpenCV data types
            IplImage iplX( flowPlanes[0] );
            flowX = &iplX;
            IplImage iplY( flowPlanes[1] );
            flowY = &iplY;
        }
    }

    // send optical flow values to particle system
    system.receiveFlow(avgX, avgY);
    
}

//--------------------------------------------------------------

// Keyboard inputs for kinect calibration and debugging ONLY :-)
void ofApp::keyPressed (int key) {
	switch (key) {
            
        case 'd' :
            debug = !debug;
            
        case '9':
            system.modeSwitch();
            break;
            
		case ' ':
			bThreshWithOpenCV = !bThreshWithOpenCV;
			break;
			
			
		case '>':
		case '.':
			farThreshold ++;
			if (farThreshold > 255) farThreshold = 255;
			break;
			
		case '<':
		case ',':
			farThreshold --;
			if (farThreshold < 0) farThreshold = 0;
			break;
			
		case '+':
		case '=':
			nearThreshold ++;
			if (nearThreshold > 255) nearThreshold = 255;
			break;
			
		case '-':
			nearThreshold --;
			if (nearThreshold < 0) nearThreshold = 0;
			break;
			
		case 'w':
			kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
			break;
			
		case 'o':
			kinect.setCameraTiltAngle(angle); // go back to prev tilt
			kinect.open();
			break;
			
		case 'c':
			kinect.setCameraTiltAngle(0); // zero the tilt
			kinect.close();
			break;
			
		case '1':
			kinect.setLed(ofxKinect::LED_GREEN);
			break;
			
		case '2':
			kinect.setLed(ofxKinect::LED_YELLOW);
			break;
			
		case '3':
			kinect.setLed(ofxKinect::LED_RED);
			break;
			
		case '4':
			kinect.setLed(ofxKinect::LED_BLINK_GREEN);
			break;
			
		case '5':
			kinect.setLed(ofxKinect::LED_BLINK_YELLOW_RED);
			break;
			
		case '0':
			kinect.setLed(ofxKinect::LED_OFF);
			break;
			
		case OF_KEY_UP:
			angle++;
			if(angle>30) angle=30;
			kinect.setCameraTiltAngle(angle);
			break;
			
		case OF_KEY_DOWN:
			angle--;
			if(angle<-30) angle=-30;
			kinect.setCameraTiltAngle(angle);
			break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
	
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
    }

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}





