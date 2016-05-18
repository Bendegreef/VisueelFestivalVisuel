#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"

class Glow : public ofxCv::RectFollower {
protected:
	ofColor color;
	ofVec2f cur, smooth;
	float startedDying;
	ofPolyline all;
public:
	Glow()
		:startedDying(0) {
	}
	void setup(const cv::Rect& track);
	void update(const cv::Rect& track);
	void kill();
	void draw();
};

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();

	float scale;

	ofVideoPlayer movie;
	ofxCv::ContourFinder contourFinder;
	ofxCv::RectTrackerFollower<Glow> tracker;

	ofVideoGrabber vidGrabber;
	ofxCvColorImage colorImg;
	ofxCvGrayscaleImage grayImage, grayBg, grayDiff;

	bool bLearnBakground;
	bool                bDebug;
	bool backgroundAuto;

	ofxIntSlider        thresholdSlider;
	ofxFloatSlider        minArea;
	ofxFloatSlider        maxArea;
	ofxPanel            gui;
	void keyPressed(int key);

	ofImage title;
	ofImage slak;
};
