#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

float dyingTime;



void Glow::setup(const cv::Rect& track) {
	color.setHsb(ofRandom(255), 250, 250);
	lineWidth = ofRandom(1, 3);
	cur = toOf(track).getCenter();
	smooth = cur;
}

void Glow::update(const cv::Rect& track) {
	cur = toOf(track).getCenter();
	smooth.interpolate(cur, .5);
	all.addVertex(smooth);
	//cout << "id " << this->getLabel();
	//cout << "smooth " << this->smooth.x << " " <<  this->smooth.y << endl;
}

void Glow::kill() {
	float curTime = ofGetElapsedTimef();
	if (startedDying == 0) {
		startedDying = curTime;
	}
	else if (curTime - startedDying > dyingTime) {
		dead = true;
	}
}

void Glow::draw() {
	
	ofPushStyle();
	float size = 1;
	ofSetColor(255);
	ofSetLineWidth(lineWidth);
	if (startedDying) {
		ofSetColor(ofColor::red);
		size = ofMap(ofGetElapsedTimef() - startedDying, 0, dyingTime, size, 0, true);
	}
	ofNoFill();
	ofSetColor(color);
	myPolylineDraw(all);
	ofSetColor(255);
	ofPopStyle();
}


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
		
		dyingTime = dyingTimeGui;
		contourFinder.setMaxArea(maxArea);
		contourFinder.setMinArea(minArea);
		contourFinder.findContours(grayImage);
		tracker.track(contourFinder.getBoundingRects());
	
		
	}
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
		gui.draw();


	}
	ofSetLineWidth(4);
	if (contour) {
		ofSetColor(150);
		double scaleX = ofGetScreenWidth() / vidGrabber.getWidth();
		double scaleY = ofGetScreenHeight() / vidGrabber.getHeight();
		ofPushMatrix();
		ofScale(scaleX, scaleY, 1);
		contourFinder.draw();
		ofPopMatrix();
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
	if (line.size() > 2) {
		float getHueAngle = color.getHueAngle();
		ofColor tmpColor = color;
		for (int i = 0; i < line.size() - 1; i++) {
			getHueAngle += 1;
			tmpColor.setHueAngle(getHueAngle);
			ofSetColor(tmpColor);
			if (line[i].distance(line[i + 1]) > 0.2)  {
				
				ofDrawLine(translateToScreen(line[i]), translateToScreen(line[i + 1]));
				ofDrawLine(translateToScreen(berekenAlternatiefPunt(line[i], i, 2)), translateToScreen(berekenAlternatiefPunt(line[i + 1], i + 1, 2)));
				ofDrawLine(translateToScreen(berekenAlternatiefPunt(line[i], i, 4)), translateToScreen(berekenAlternatiefPunt(line[i + 1], i + 1, 4)));
				ofDrawLine(translateToScreen(berekenAlternatiefPunt(line[i], i, 6)), translateToScreen(berekenAlternatiefPunt(line[i + 1], i + 1, 6)));
				ofDrawLine(translateToScreen(berekenAlternatiefPunt(line[i], i, 8)), translateToScreen(berekenAlternatiefPunt(line[i + 1], i + 1, 8)));
			}
			else {
				//cout << "same " << endl;
			}
		}
	}
}

ofVec2f Glow::translateToScreen(ofVec2f input) {
	input.x = ofMap(input.x, 0, vidGrabSize.x, 0, ofGetWindowWidth());
	input.y = ofMap(input.y, 0, vidGrabSize.y, 0, ofGetWindowHeight());
	return input;
}

ofVec2f Glow::berekenAlternatiefPunt(ofVec2f firstPoint, int index, int gap) {
	//float angle = firstPoint.angle(secondPoint);
	//float offset = ofMap(angle, -180, 180, -400, 400);
	float offset = sin(float(index) / 15.0) * gap;
	//firstPoint.x += offset;
	firstPoint.y += offset;
	return firstPoint;
}

