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
	ofPolyline all2;
	float lineWidth;
public:
	Glow()
		:startedDying(0) {
	}
	void setup(const cv::Rect& track);
	void update(const cv::Rect& track);
	void kill();
	void draw();
	void myPolylineDraw(ofPolyline line);
	void myPolylineMultiDraw(ofPolyline line);
};

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);

	ofVideoPlayer movie;
	ofxCv::ContourFinder contourFinder;
	ofxCv::RectTrackerFollower<Glow> tracker;

	ofVideoGrabber vidGrabber;
	ofxCvColorImage colorImg;
	ofxCvGrayscaleImage grayImage, grayBg, grayDiff;

	bool bLearnBakground;
	bool bDebug;
	bool backgroundAuto;
	bool contour;
	bool tron;

	ofxIntSlider thresholdSlider;
	ofxFloatSlider minArea;
	ofxFloatSlider maxArea;
	ofxPanel gui;

	ofImage title;
	ofImage screenGrabber;

	float scale;
	float arcScale;

	char snapString[255];
};
