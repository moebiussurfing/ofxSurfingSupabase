// example2/src/ofApp.h
// Pure Remote Presets - NO presetsLite

#pragma once

#include "ofMain.h"
#include "ofxSurfingSupabase.h"
#include "surfingSceneTesters.h"
#include "ofxGui.h"
#include "ofxSurfingHelpersLite.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void exit();
	
	SurfingSceneTesters scene;
	ofxSurfingSupabase db;
	
private:
	void setupGui();
	void drawGui();
	
	ofxPanel gui;
	ofParameter<bool> bGui{"Show GUI", true};
};
