#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "arc.h"
#include <sstream>

class Glow : public ofxCv::RectFollower {
protected:
	ofColor color;
	ofVec2f cur, smooth;
	float startedDying;
	
	float lineWidth;
public:
	ofPolyline all;
	Glow()
		:startedDying(0) {
	}
	void setup(const cv::Rect& track);
	void update(const cv::Rect& track);
	void kill();
	void draw();
	void myPolylineDraw(ofPolyline line);
	ofVec2f translateToScreen(ofVec2f input);
	ofVec2f vidGrabSize;
	ofVec2f berekenAlternatiefPunt(ofVec2f eerstePunt, int index, int gap);
	ofxCv::KalmanPosition kalman;

	ofPolyline predicted, line, estimated;
	ofVec2f point;
	float speed;


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
	ofParameter<float> learningTime, thresholdValue;
	ofxFloatSlider dyingTimeGui;
	ofxPanel            gui;
	void keyPressed(int key);

	ofImage title1;
	ofImage title2;
	ofImage title3;
	ofImage title4;
	ofImage title5;
	ofImage title6;
	ofImage title7;

	ofImage slak;

	int snapCounter;
	ofImage screenGrabber;
	bool bSnapshot;
	bool contour;
	char snapString[255];
	float arcScale;
	bool tron;
	ofxCv::RunningBackground runningBackground;
	ofImage threshold;
	vector<ofImage> backgrounds;

	int backgroundNumber;

	bool hasCameraStarted;
	bool printData;
	ofImage screenshot;
	int photoNr;
	std::stringstream ss;
};
