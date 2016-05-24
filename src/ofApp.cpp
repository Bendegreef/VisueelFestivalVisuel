#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
	ofSetVerticalSync(true);
	ofBackground(0);

	contour = false;
	arcScale = 0.30;


	ofSetBackgroundAuto(false);


	vidGrabber.listDevices();
	vidGrabber.setDeviceID(0);
	vidGrabber.setVerbose(true);
	vidGrabber.setup(320,180);



	bDebug = false;
	backgroundAuto = false;

	scale = 3;

	hasCameraStarted = false;
	
	gui.setup(); // most of the time you don't need a name
	gui.add(thresholdSlider.setup("threshold", 40, 1, 300));
	gui.add(minArea.setup("minArea", 40, 1, 30000));
	gui.add(maxArea.setup("maxArea", 540, 1, 100000));
	gui.add(learningTime.set("Learning Time", 30, 0, 30));
	gui.add(thresholdValue.set("Threshold Value", 10, 0, 255));
	gui.add(dyingTimeGui.setup("Dying Time", 40, 1, 3600));
	gui.loadFromFile("settings.xml");

	ofSetFrameRate(60);


	// wait for half a frame before forgetting something
	tracker.setPersistence(15);
	// an object can move up to 50 pixels per frame
	tracker.setMaximumDistance(50);
	title1.load("Title50.png");
	title2.load("backgroundAmericanTypewriter.png");
	title3.load("backgroundBigJohn.png");
	title4.load("backgroundComicSans.png");
	title5.load("backgroundFuturaBoldRound.png");
	title6.load("backgroundJaapokki.png");
	title7.load("backgroundMontserratBold.png");
	backgrounds.push_back(title1);
	backgrounds.push_back(title2); 
	backgrounds.push_back(title3);
	backgrounds.push_back(title4);
	backgrounds.push_back(title5);
	backgrounds.push_back(title6);
	backgrounds.push_back(title7);
	ofSetFullscreen(true);

	rgbaFboFloat.allocate(ofGetScreenWidth(), ofGetScreenHeight(), GL_RGBA32F_ARB);

	rgbaFboFloat.begin();
	ofClear(255, 255, 255, 0);
	rgbaFboFloat.end();
}

void ofApp::update() {


	vidGrabber.update();
	bool bNewFrame = false;
	bNewFrame = vidGrabber.isFrameNew();

	if (!hasCameraStarted && bNewFrame) {

		colorImg.allocate(vidGrabber.getWidth(), vidGrabber.getHeight());
		grayImage.allocate(vidGrabber.getWidth(), vidGrabber.getHeight());
		grayBg.allocate(vidGrabber.getWidth(), vidGrabber.getHeight());
		grayDiff.allocate(vidGrabber.getWidth(), vidGrabber.getHeight());



		hasCameraStarted = true;
	}
	if (bNewFrame && hasCameraStarted) {

		runningBackground.setLearningTime(learningTime);
		runningBackground.setThresholdValue(thresholdValue);
		runningBackground.update(vidGrabber, threshold);
		threshold.update();

		colorImg.setFromPixels(vidGrabber.getPixels());
		
		tracker.track(contourFinder.getBoundingRects());

		colorImg.setFromPixels(vidGrabber.getPixels());

		grayImage.setFromPixels(threshold.getPixels());
		
		contourFinder.setMaxArea(maxArea);
		contourFinder.setMinArea(minArea);
		contourFinder.findContours(grayImage);
	
		
	}

	rgbaFboFloat.begin();
	drawFboTest();
	rgbaFboFloat.end();
}

void ofApp::drawFboTest() {
	
	fadeAmnt = 10;
	ofFill();
	ofSetColor(0, 0, 0, fadeAmnt);
	ofDrawRectangle(0, 0, ofGetScreenWidth(), ofGetScreenHeight());



	
	ofNoFill();
	ofSetColor(255, 255, 255);

}

void ofApp::draw() {

	ofSetColor(255);
	vector<Glow>& followers = tracker.getFollowers();


	for (int i = 0; i < followers.size(); i++) {
		followers[i].vidGrabSize = ofVec2f(vidGrabber.getWidth(), vidGrabber.getHeight());
		followers[i].draw();
	}
	backgrounds[backgroundNumber].draw(0,0, 1920,1080);
	if (bDebug) {
		ofSetHexColor(0xffffff);
		grayImage.draw(360, 20, 320, 180);
		grayBg.draw(20, 20, 320, 180);
		grayDiff.draw(360, 280, 320, 180);
		threshold.draw(20, 360, 320, 180);
		contourFinder.draw();
		gui.draw();


	}
	ofSetLineWidth(4);
	if (contour) {
		ofSetColor(150);
		contourFinder.draw();
	}

	ofSetColor(255, 255, 255);
	rgbaFboFloat.draw(410, 0);


}



void ofApp::keyPressed(int key) {
	switch (key) {
	case ' ':
		//ref background nemen
		bLearnBakground = true;
		break;
	case 'd':
		//debug
		bDebug = !bDebug;
		break;
	case 'b':
		//backgroundauto aanzetten
		backgroundAuto = !backgroundAuto;
		ofSetBackgroundAuto(backgroundAuto);
		break;
	case 'c':
		//contour aanzetten
		contour = !contour;
		break;
	case '1':
		backgroundNumber = 0;
		break;
	case '2':
		backgroundNumber = 1;
		break; 
	case '3':
		backgroundNumber = 2;
		break; 
	case '4':
		backgroundNumber = 3;
		break;
	case '5':
		backgroundNumber = 4;
		break;
	case '6':
		backgroundNumber = 5;
		break;
	case '7':
		backgroundNumber = 6;
		break;
	}

}