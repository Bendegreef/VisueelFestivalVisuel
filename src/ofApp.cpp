#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

const float dyingTime = 2;


//deel code kris Meeusen
int nrOfCircles = 1;
float rotation = 0;
float rotationSteps = 0.01;
float lastSwitch;
bool closing;
bool opening;

void Glow::setup(const cv::Rect& track) {
	color.setHsb(100, 250, 250);

	cur = toOf(track).getCenter();
	smooth = cur;
}

void Glow::update(const cv::Rect& track) {
	cur = toOf(track).getCenter();
	smooth.interpolate(cur, .5);
	all.addVertex(smooth);
}

void Glow::kill() {
	float curTime = ofGetElapsedTimef();
	if (startedDying == 0) {
		startedDying = curTime;
	}
	else if (curTime - startedDying > dyingTime) {
		//dead = true;
	}
}

void Glow::draw() {
	color.setHsb(static_cast<int>(ofGetElapsedTimef()) % 250, 250, 250);
	ofPushStyle();
	float size = 1;
	ofSetColor(255);
	ofSetLineWidth(2);
	if (startedDying) {
		ofSetColor(ofColor::red);
		size = ofMap(ofGetElapsedTimef() - startedDying, 0, dyingTime, size, 0, true);
	}
	ofNoFill();
	//ofDrawCircle(cur, size);
	ofSetColor(color);
	all.draw();
	ofSetColor(255);
	//ofDrawBitmapString(ofToString(label), cur);
	ofPopStyle();
}

void ofApp::setup() {
	createNewArcs();
	closing = false;
	opening = true;
	ofSetVerticalSync(true);
	ofBackground(0);

	lastSwitch = ofGetElapsedTimef();
	rotationSteps = 0.4;
	snapCounter = 0;
	bSnapshot = false;
	arcScale = 0;

	/*movie.load("video.mov");
	movie.play();*/

	ofSetBackgroundAuto(false);


	vidGrabber.listDevices();
	vidGrabber.setDeviceID(1);
	vidGrabber.setVerbose(true);
	vidGrabber.setup(1920, 1080);

	colorImg.allocate(1920, 1080);
	grayImage.allocate(1920, 1080);
	grayBg.allocate(1920, 1080);
	grayDiff.allocate(1920, 1080);

	bLearnBakground = true;
	bDebug = false;
	backgroundAuto = false;

	scale = 3;

	ofSetWindowShape(vidGrabber.getWidth(), vidGrabber.getHeight());

	
	gui.setup(); // most of the time you don't need a name
	gui.add(thresholdSlider.setup("threshold", 40, 1, 300));
	gui.add(minArea.setup("minArea", 40, 1, 30000));
	gui.add(maxArea.setup("maxArea", 540, 1, 100000));

	gui.loadFromFile("settings.xml");

	ofSetFrameRate(60);

	/*contourFinder.setMinAreaRadius(1);
	contourFinder.setMaxAreaRadius(100);
	contourFinder.setThreshold(15);*/

	// wait for half a frame before forgetting something
	tracker.setPersistence(15);
	// an object can move up to 50 pixels per frame
	tracker.setMaximumDistance(50);
	title.load("Title50.png");
	ofSetFullscreen(true);
	slak.load("cartoon-snail.png");
}

void ofApp::update() {
	//ofBackground(100, 100, 100);
	//movie.update();


	rotation += rotationSteps;

	if (ofGetElapsedTimef() - lastSwitch > 9 && !closing && !opening)
	{
		closing = true;
		opening = false;
	}

	if (closing) {
		arcScale -= (arcScale) / 8;

		if (arcScale < 0.1)
		{
			createNewArcs();
			opening = true;
			closing = false;
		}
	}

	if (opening) {
		arcScale += (1 - arcScale) / 8;
		if (scale > 0.99)
		{
			arcScale = 1;
			lastSwitch = ofGetElapsedTimef();
			opening = false;
		}
	}


	vidGrabber.update();
	bool bNewFrame = false;
	bNewFrame = vidGrabber.isFrameNew();
	/*if (movie.isFrameNew()) {
		blur(movie, 10);
		contourFinder.findContours(movie);
		tracker.track(contourFinder.getBoundingRects());
	}*/

	if (bNewFrame) {
		colorImg.setFromPixels(vidGrabber.getPixels());
		//colorImg.mirror(false, true);

		grayImage = colorImg;
		if (bLearnBakground == true) {
			grayBg = grayImage;		// the = sign copys the pixels from grayImage into grayBg (operator overloading)
			bLearnBakground = false;
		}

		// take the abs value of the difference between background and incoming and then threshold:
		grayDiff.absDiff(grayBg, grayImage);
		grayDiff.threshold(thresholdSlider);

		//contourFinder.findContours(grayDiff, minArea, maxArea, 4, false);
		contourFinder.setMaxArea(maxArea);
		contourFinder.setMinArea(minArea);
		contourFinder.findContours(grayDiff);
		tracker.track(contourFinder.getBoundingRects());
		
	}

}

void ofApp::draw() {
	
	ofSetColor(255);
	//vidGrabber.draw(vidGrabber.getWidth(), 0, -vidGrabber.getWidth(), vidGrabber.getHeight());
	//contourFinder.draw();
	vector<Glow>& followers = tracker.getFollowers();


	for (int i = 0; i < followers.size(); i++) {
		followers[i].draw();

	}
	title.draw(0,0, 1920, 1080);
	for (int i = 0; i < contourFinder.size(); i++) {
		ofDrawCircle(contourFinder.getCentroid(i).x, contourFinder.getCentroid(i).y, 20);
		//slak.draw(contourFinder.getCentroid(i).x, contourFinder.getCentroid(i).y, 50, 50);
	}
	if (bDebug) {
		ofSetHexColor(0xffffff);
		grayImage.draw(360, 20, 320, 180);
		grayBg.draw(20, 20, 320, 180);
		grayDiff.draw(360, 280, 320, 180);
		contourFinder.draw();
		/*for (int i = 0; i < contourFinder.nBlobs; i++) {
			contourFinder.blobs[i].draw(360, 280);
		}*/


	}
	//ofFill();
	ofSetLineWidth(4);
	if (ofGetElapsedTimef() < 10) {
		ofSetColor(40, 100, 100);

		char fpsStr[255]; // an array of chars

		sprintf(fpsStr, "press [f] to \ntoggle fullscreen\nmove mouse horizontaly\nfor speed");
		ofDrawBitmapString(fpsStr, ofGetWidth() / 2 - 82, ofGetHeight() / 2 - 30);
	}


	// uncomment to see the frame - currently framerate limited to 60fps
	/*
	char fpsStr[255]; // an array of chars
	sprintf(fpsStr, "frame rate: %f", ofGetFrameRate());
	ofDrawBitmapString(fpsStr, 10,10);
	*/

	glTranslatef(ofGetWidth() / 2, ofGetHeight() / 2, 0);

	ofScale(arcScale, arcScale, 1);

	for (int i = 0; i < nrOfCircles; i++)
	{
		arc* arcObject = arcs.at(i);
		arcObject->draw(rotation);
	}

	// create screenshot
	if (bSnapshot == true) {
		screenGrabber.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
		char fileName[255];
		sprintf(fileName, "snapshot_%3i.png", snapCounter++);
		screenGrabber.saveImage(fileName);
		bSnapshot = false;
	}

	gui.draw();

}

void ofApp::createNewArcs()
{
	for (int i = 0; i < arcs.size(); i++) {
		// not sure if cleans out everything...
		delete arcs[i];
	}
	arcs.clear();

	nrOfCircles = 4 + ofRandom(70);

	float innerRadius = 100;
	float angle = 0;
	float cirleAngle = 0;
	bool clockWise = true;
	float lineWidth = 10;
	float margin = 0;
	float circleStartAngle = 0;

	for (int i = 0; i < nrOfCircles; i++) {

		arc* arcObject = new arc();
		arcObject->innerRadius = innerRadius;
		arcObject->outerRadius = innerRadius + lineWidth;

		angle += ofRandom(0, 10) > 2 ? 0.1 : 0;
		arcObject->startAngle = angle;

		angle += ofRandom(0.2, 4.5);
		arcObject->endAngle = angle;

		arcObject->clockwise = clockWise;

		// using negative random values to avoid to much coloring
		arcObject->colorBlueStartFactor = ofClamp(ofRandom(-0.4, 1.0), 0, 1);
		arcObject->colorBlueEndFactor = ofClamp(ofRandom(-0.4, 1.0), 0, 1);
		arcObject->colorGreenCorrection = ofClamp(ofRandom(-0.4, 0.1), 0, 1);


		if (angle >= (TWO_PI + circleStartAngle)) {
			innerRadius += (margin + lineWidth);

			// do correction on last arc
			arcObject->endAngle = TWO_PI + circleStartAngle;

			angle = (angle - (TWO_PI));
			circleStartAngle = angle;
			clockWise = ofRandom(10) > 5;

			int lineWidthRandom = ofRandom(20);
			if (lineWidthRandom < 3) {
				lineWidth = 30;
			}
			else if (lineWidth <10) {
				lineWidth = 20;
			}
			else {
				lineWidth = 10;
			}

			//lineWidth = ofRandom(20) > 5 ? 10 : 20;

			int marginRandom = ofRandom(0, 20);

			if (marginRandom > 6) {
				margin = 0;
			}
			else if (marginRandom > 2) {
				margin = 10;
			}
			else
			{
				margin = 30;
			}
		}

		//cout << arcObject->startAngle << "  " << arcObject->endAngle << "   angle " << angle << "\n" ;

		arcObject->createMesh();
		arcs.push_back(arcObject);

	}
}

void ofApp::keyPressed(int key) {
	switch (key) {
	case ' ':
		bLearnBakground = true;
		break;
	case 'd':
		bDebug = !bDebug;
		break;
	case 'c':
		backgroundAuto = !backgroundAuto;
		ofSetBackgroundAuto(backgroundAuto);
		break;
	}

}

void ofApp::mouseReleased(int x, int y, int button) {
	closing = true;
	opening = false;
	lastSwitch = ofGetElapsedTimef();

}

