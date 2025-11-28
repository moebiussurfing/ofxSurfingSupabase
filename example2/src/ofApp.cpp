// example2/src/ofApp.cpp

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetFrameRate(60);
	ofSetWindowTitle("Pure Remote Presets - No presetsLite");
	
	// Setup Supabase - PURE REMOTE MODE
	db.setup();
	db.setupSceneParams(scene.params); // Direct link
	
	// Enable Remote Mode by default
	db.bRemoteMode = true;
	
	setupGui();
	
	ofLogNotice("ofApp") << "✅ Pure Remote Mode - No local files";
}

//--------------------------------------------------------------
void ofApp::setupGui() {
	gui.setup("Scene Controls");
	gui.add(bGui);
	gui.add(scene.params);
}

//--------------------------------------------------------------
void ofApp::update() {
}

//--------------------------------------------------------------
void ofApp::draw() {
	scene.draw();
	
	if (bGui) {
		drawGui();
	}
}

//--------------------------------------------------------------
void ofApp::drawGui() {
	gui.draw();
	
	// Supabase UI
	db.draw();
	
	// Help text
	std::string s="";
	s+="PURE REMOTE MODE - No presetsLite\n";
	s+="1. Adjust sliders above\n";
	s+="2. Click 'Save Scene Direct' (right panel)\n";
	s+="3. Browse with < > arrows\n";
	s+="4. Click 'Load & Apply' to restore\n";
	s+="G: Toggle GUI\n";
	ofxSurfing::ofDrawBitmapStringBox(s, &gui);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == 'g' || key == 'G') {
		bGui = !bGui;
	}
}

//--------------------------------------------------------------
void ofApp::exit() {
	// Cleanup
}
