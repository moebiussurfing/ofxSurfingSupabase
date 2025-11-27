// file: src/ofApp.h

#pragma once

#include "ofMain.h"

#include "ofxSurfingPresetsLiteOfxGui.h"
#include "surfingSceneTesters.h"

#include "ofxGui.h"
#include "ofxSurfingSupabase.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void windowResized(int w, int h);
	void exit();

	SurfingSceneTesters scene;
	SurfingPresetsLiteOfxGui presetsManager;

private:
	void setupGui();
	void drawGui();
	void drawHelp();
	
	ofxPanel gui;
	ofParameter<bool> bGui;

	ofxSurfingSupabase db;

	std::string sHelp = "";
};
