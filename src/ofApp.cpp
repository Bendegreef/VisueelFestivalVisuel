#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

const float dyingTime = 2;

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
	ofSetVerticalSync(true);
	ofBackground(0);

	/*movie.load("video.mov");
	movie.play();*/

	ofSetBackgroundAuto(false);


	vidGrabber.listDevices();
	vidGrabber.setDeviceID(0);
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

	gui.draw();

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

