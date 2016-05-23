#pragma once

#include "ofMain.h"

class arc{

public:
	float startAngle;
	float endAngle;
	
	float innerRadius;
	float outerRadius;
		
	bool clockwise;
	int hexColor;
	
	float colorBlueStartFactor;
	float colorBlueEndFactor;

	float colorGreenCorrection;
	// arc fill
	ofMesh mesh;
	// arc border
	ofMesh meshOutline;
	void draw(float rotation);
	void createMesh();
	
	private:

};