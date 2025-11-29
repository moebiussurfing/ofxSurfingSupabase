#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetFrameRate(60);
	ofSetWindowTitle("ofxSurfingSupabase - Pure Remote Mode");
	
	gui.add(scene.params);
	gui.setPosition(10, 10);

	// Setup Supabase
	db.setup();
	db.setupSceneParams(scene.params);
	db.setShowGui(true);
}

//--------------------------------------------------------------
void ofApp::update() {
	db.update();
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofBackground(40);
	
	// Draw scene
	scene.draw();
	
	// Draw Supabase UI
	if (bGui) 
	{
		db.draw();
	}
	
	gui.draw();
	
	// Help text
	if (bGui) {
		std::string s = "";
		s += "PURE REMOTE MODE - No presetsLite\n";
		s += "\n";
		s += "1. Adjust sliders above\n";
		s += "2. Click 'Save Scene Direct' (right panel)\n";
		s += "3. Browse with < > arrows\n";
		s += "4. Click 'Load & Apply' to restore\n";
		s += "\n";
		s += "G: Toggle GUI\n";
		
		ofPushStyle();
		ofSetColor(0, 200);
		ofDrawRectRounded(10, ofGetHeight() - 160, 400, 150, 5);
		ofSetColor(255);
		ofDrawBitmapString(s, 20, ofGetHeight() - 140);
		ofPopStyle();
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == 'g' || key == 'G') {
		bGui = !bGui;
		db.setShowGui(bGui);
	}
}

//--------------------------------------------------------------
void ofApp::exit() {
	db.exit();
}
