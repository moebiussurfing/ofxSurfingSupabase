#pragma once

#include "ofMain.h"

#include "ofxGui.h"
#include "surfingSceneTesters.h"
#include "ofxSurfingHelpersParameters.h"

#include "ofxSurfingSupabase.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void exit();

private:
	SurfingSceneTesters scene;
	ofxPanel guiScene;
	ofParameter<void> vRandomize{"Randomize"};
	ofEventListener e_vRandomize;

	ofxSurfingSupabase db;
};
