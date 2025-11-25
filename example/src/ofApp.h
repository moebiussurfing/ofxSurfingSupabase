// file: src/ofApp.h

#pragma once

#include "ofMain.h"
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

private:
	ofxPanel gui;

	ofxSurfingSupabase db;
};
