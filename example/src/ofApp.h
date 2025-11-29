#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxSurfingSupabase.h"
#include "surfingSceneTesters.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void exit();
	
private:
	// Scene with parameters
	SurfingSceneTesters scene;
	
	// Supabase integration
	ofxSurfingSupabase db;
	
	// UI
	ofParameter<bool> bGui{"GUI", true};

	ofxPanel gui;
};
