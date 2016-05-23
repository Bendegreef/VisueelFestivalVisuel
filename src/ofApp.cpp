#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

float dyingTime;


//deel code kris Meeusen
int nrOfCircles = 0;
float rotation = 0;
float rotationSteps = 0;
float lastSwitch;
//bool closing;
//bool opening;

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
	//color.setHsb(static_cast<int>(ofGetElapsedTimef()) % 250, 250, 250);
	ofPushStyle();
	float size = 1;
	ofSetColor(255);
	ofSetLineWidth(lineWidth);
	if (startedDying) {
		ofSetColor(ofColor::red);
		size = ofMap(ofGetElapsedTimef() - startedDying, 0, dyingTime, size, 0, true);
	}
	ofNoFill();
	//ofDrawCircle(cur, size);
	ofSetColor(color);
	myPolylineDraw(all);
	//all.
	ofSetColor(255);
	//ofDrawBitmapString(ofToString(label), cur);
	ofPopStyle();
}


void ofApp::setup() {
	createNewArcs();
	//closing = false;
	//opening = true;
	ofSetVerticalSync(true);
	ofBackground(0);

	lastSwitch = ofGetElapsedTimef();
	rotationSteps = 8;
	snapCounter = 0;
	bSnapshot = false;
	contour = false;
	arcScale = 0.30;

	/*movie.load("video.mov");
	movie.play();*/

	ofSetBackgroundAuto(false);


	vidGrabber.listDevices();
	vidGrabber.setDeviceID(0);
	vidGrabber.setVerbose(true);
	vidGrabber.setup(1920,1080);



	colorImg.allocate(1920,1080);
	grayImage.allocate(1920,1080);
	grayBg.allocate(1920,1080);
	grayDiff.allocate(1920,1080);
	//beeldOpslaan.allocate(1920, 1080, OF_IMAGE_COLOR);
	//standaard.allocate(1920, 1080, OF_IMAGE_COLOR);

	//standaard.load("standaard.jpg");
	//grayBg.setFromPixels(standaard.getPixels());

	bLearnBakground = false;
	bDebug = false;
	backgroundAuto = false;

	scale = 3;

	ofSetWindowShape(vidGrabber.getWidth(), vidGrabber.getHeight());

	
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
}

void ofApp::update() {

	rotation += rotationSteps;


	vidGrabber.update();
	bool bNewFrame = false;
	bNewFrame = vidGrabber.isFrameNew();

	if (bNewFrame) {

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
		followers[i].draw();
	}
	backgrounds[backgroundNumber].draw(0,0, 1920,1080);
	if (tron) {
		for (int i = 0; i < contourFinder.size(); i++) {
			ofPushMatrix();
			glTranslatef(contourFinder.getCentroid(i).x, contourFinder.getCentroid(i).y, 0);
			ofScale(arcScale, arcScale, 1);
			for (int i = 0; i < nrOfCircles; i++)
			{
				arc* arcObject = arcs.at(i);
				arcObject->draw(rotation);
			}

			ofPopMatrix();
		}
	}
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

}

void ofApp::createNewArcs()
{
	for (int i = 0; i < arcs.size(); i++) {
		// not sure if cleans out everything...
		delete arcs[i];
	}
	arcs.clear();

	nrOfCircles = 15;

	float innerRadius = 50;
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

		arcObject->createMesh();
		arcs.push_back(arcObject);

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
	case 't':
		//troncirckelsaanzetten
		tron = !tron;
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

void Glow::myPolylineDraw(ofPolyline line) {
	if (line.size() > 2) {
		float getHueAngle = color.getHueAngle();
		ofColor tmpColor = color;
		for (int i = 0; i < line.size() - 1; i++) {
			getHueAngle += 1;
			tmpColor.setHueAngle(getHueAngle);
			ofSetColor(tmpColor);
			ofDrawLine(line[i], line[i + 1]);
		}
	}
}
