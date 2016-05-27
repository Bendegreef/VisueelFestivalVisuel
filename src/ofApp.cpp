#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

float dyingTime;



void Glow::setup(const cv::Rect& track) {
	color.setHsb(ofRandom(0, 128), 255, 255, 255);
	lineWidth = ofRandom(0.5, 1.5);
	cur = toOf(track).getCenter();
	smooth = cur;

	kalman.init(1 / 10000., 1 / 10.); // inverse of (smoothness, rapidness)

	speed = 0.f;
	frameBuffer = nullptr;
}

void Glow::update(const cv::Rect& track) {
	if (frameBuffer == nullptr) {
		return;
	}
	cur = toOf(track).getCenter();
	smooth.interpolate(cur, .5);
	//all.addVertex(smooth);


	if (smooth.x != 0 && smooth.y != 0) {
		//line.addVertex(smooth);
		kalman.update(smooth);

		point = kalman.getPrediction();
		if (point.x != 0 && point.y != 0) {
			//predicted.addVertex(point);
			estimated.addVertex(kalman.getEstimation());
		}

		speed = kalman.getVelocity().length();
	}

	if (estimated.size() > 8){
	}



}

void Glow::kill() {
	//float curTime = ofGetElapsedTimef();
	//if (startedDying == 0) {
	//	startedDying = curTime;
	//}
	//else if (curTime - startedDying > dyingTime) {
		dead = true;
	//}
}

void Glow::draw() {
	if (frameBuffer == nullptr) {
		return;
	}
	ofPushStyle();
	float size = 1;
	ofSetColor(255);
	ofSetLineWidth(lineWidth);
	if (startedDying) {
		ofSetColor(ofColor::red);
		size = ofMap(ofGetElapsedTimef() - startedDying, 0, dyingTime, size, 0, true);
	}
	ofSetColor(color);
	double scaleX = ofGetScreenWidth() / vidGrabSize.x;
	double scaleY = ofGetScreenHeight() / vidGrabSize.y;
	ofPushMatrix();
	ofScale(scaleX, scaleY, 1);
	ofDrawCircle(smooth, 7);
	ofPopMatrix();

	ofNoFill();
	ofSetColor(color);
	myPolylineDraw(estimated);
	ofSetColor(255);
	ofPopStyle();
}


void ofApp::setup() {
	ofSetVerticalSync(true);
	ofBackground(0);

	contour = true;

	ofSetBackgroundAuto(true);

	vidGrabber.listDevices();
	vidGrabber.setDeviceID(0);
	vidGrabber.setVerbose(true);
	vidGrabber.setup(320,180);

	rgbaFboFloat.allocate(ofGetScreenWidth(), ofGetScreenHeight(), GL_RGBA32F_ARB);
	rgbaFboFloat.begin();
	ofClear(255, 255, 255, 0);
	rgbaFboFloat.end();

	bDebug = false;
	backgroundAuto = false;

	scale = 3;

	hasCameraStarted = false;
	
	gui.setup(); // most of the time you don't need a name
	gui.add(minArea.setup("minArea", 40, 1, 3000));
	gui.add(maxArea.setup("maxArea", 540, 1, 10000));
	gui.add(learningTime.set("Learning Time", 30, 0, 60));
	gui.add(thresholdValue.set("Threshold Value", 10, 0, 255));
	gui.add(dyingTimeGui.setup("Dying Time", 40, 1, 3600));
	gui.loadFromFile("settings.xml");

	ofSetFrameRate(60);


	// wait for half a frame before forgetting something
	tracker.setPersistence(60);
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

	printData = false;
}

void ofApp::update() {
	vector<Glow>& followers = tracker.getFollowers();
	//followers = tracker.getFollowers();
	for (int i = 0; i < followers.size(); i++) {
		followers[i].frameBuffer = &rgbaFboFloat;
	}


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
		
		
		grayImage.setFromPixels(threshold.getPixels());
		grayImage.mirror(false, true);
		dyingTime = dyingTimeGui;
		contourFinder.setMaxArea(maxArea);
		contourFinder.setMinArea(minArea);
		contourFinder.findContours(grayImage);
		tracker.track(contourFinder.getBoundingRects());
	}
	if (ofGetFrameNum() % 50 == 0) {
		rgbaFboFloat.begin();
		ofSetColor(0, 0, 0, 1);
		ofDrawRectangle(0, 0, ofGetScreenWidth(), ofGetScreenHeight());
		rgbaFboFloat.end();
	}
}

void ofApp::draw() {

	rgbaFboFloat.draw(0,0);

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
		vidGrabber.draw(20, 20, 320, 180);
		grayDiff.draw(360, 280, 320, 180);
		threshold.draw(20, 360, 320, 180);
		gui.draw();


	}
	ofSetLineWidth(4);
	if (contour) {
		ofPushStyle();
		ofSetColor(150);
		double scaleX = ofGetScreenWidth() / vidGrabber.getWidth();
		double scaleY = ofGetScreenHeight() / vidGrabber.getHeight();
		ofPushMatrix();
		ofScale(scaleX, scaleY, 1);
		contourFinder.draw();
		ofPopMatrix();
		ofPopStyle();
		
	}
	if (printData) {
		for (int i = 0; i < followers.size(); i++) {
			cout << "Id " << followers[i].getLabel() << endl;
			for (int j = 0; j < followers[i].all.size(); j++) {
				cout << "X " << followers[i].all[j].x << " Y " << followers[i].all[j].y << endl;
			}
		}
		printData = false;
	}

	
}

void ofApp::drawFboTest() {

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
	case 'p':
		printData = true;
		break;
	}

}

void Glow::myPolylineDraw(ofPolyline line) {
	frameBuffer->begin();
	if (line.size() > 6) {

		float getHueAngle = color.getHueAngle();
		ofColor tmpColor = color;
		for (int i = line.size() - 2; i < line.size() - 1; i++) {
			getHueAngle += 1;
			tmpColor.setHueAngle(getHueAngle);
			ofSetColor(tmpColor);
			ofSetLineWidth(3);
			if (line[i].distance(line[i + 1]) > 0.2)  {
				
				ofDrawLine(translateToScreen(line[i]), translateToScreen(line[i + 1]));
				ofDrawLine(translateToScreen(berekenAlternatiefPunt(line[i], i, 2)), translateToScreen(berekenAlternatiefPunt(line[i + 1], i + 1, 2)));
				ofDrawLine(translateToScreen(berekenAlternatiefPunt(line[i], i, 4)), translateToScreen(berekenAlternatiefPunt(line[i + 1], i + 1, 4)));
				ofDrawLine(translateToScreen(berekenAlternatiefPunt(line[i], i, 6)), translateToScreen(berekenAlternatiefPunt(line[i + 1], i + 1, 6)));
				ofDrawLine(translateToScreen(berekenAlternatiefPunt(line[i], i, 8)), translateToScreen(berekenAlternatiefPunt(line[i + 1], i + 1, 8)));
			}
		}
	}
	frameBuffer->end();
}

ofVec2f Glow::translateToScreen(ofVec2f input) {
	input.x = ofMap(input.x, 0, vidGrabSize.x, 0, ofGetWindowWidth());
	input.y = ofMap(input.y, 0, vidGrabSize.y, 0, ofGetWindowHeight());
	return input;
}

ofVec2f Glow::berekenAlternatiefPunt(ofVec2f firstPoint, int index, int gap) {
	float offset = sin((float(index) / 15.0) + this->getLabel()) * gap;
	firstPoint.x += offset * cos(this->getLabel());
	firstPoint.y += offset * sin(this->getLabel());
	return firstPoint;
}

