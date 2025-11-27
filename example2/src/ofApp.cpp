// example2/src/ofApp.cpp

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetFrameRate(60);
	ofSetWindowTitle("Pure Remote Presets - No presetsLite");
	
	// Setup scene
	// scene.setup();
	
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
	// scene.update();
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
	int y = ofGetHeight() - 120;
	ofSetColor(255, 200);
	ofDrawBitmapString("PURE REMOTE MODE - No presetsLite", 20, y);
	y += 20;
	ofDrawBitmapString("1. Adjust sliders above", 20, y);
	y += 15;
	ofDrawBitmapString("2. Click 'Save Scene Direct' (right panel)", 20, y);
	y += 15;
	ofDrawBitmapString("3. Browse with < > arrows", 20, y);
	y += 15;
	ofDrawBitmapString("4. Click 'Load & Apply' to restore", 20, y);
	y += 20;
	ofDrawBitmapString("G: Toggle GUI", 20, y);
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
