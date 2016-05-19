#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "arc.h"

class Glow : public ofxCv::RectFollower {
protected:
	ofColor color;
	ofVec2f cur, smooth;
	float startedDying;
	ofPolyline all;
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
};

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	//void mouseReleased(int x, int y, int button);

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
	//ofImage beeldOpslaan;
	//ofImage standaard;

	vector<arc*> arcs;
	void createNewArcs();
	int snapCounter;
	ofImage screenGrabber;
	bool bSnapshot;
	char snapString[255];
	float arcScale;

};
