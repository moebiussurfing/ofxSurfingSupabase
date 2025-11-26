// file: src/ofApp.cpp

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetWindowTitle("ofxSurfingSupabase Example");
	
	// Setup scene parameters
	presetsManager.setup(scene.params);
	
	// Setup Supabase connection
	db.setup();
	
	// Connect presetsManager with Supabase for auto-sync
	db.syncWithPresetsManager(presetsManager);
	
	setupGui();
}

//--------------------------------------------------------------
void ofApp::setupGui() {
	bGui.set("Show GUI", true);
	gui.setup("Controls");
	gui.add(bGui);
	gui.setPosition(10, 10);
}

//--------------------------------------------------------------
void ofApp::update() {
	db.update();
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofBackground(30);
	
	// Draw scene
	scene.draw();

	// Draw presets manager GUI
	presetsManager.drawGui();
	
	// Listen to preset changes
	if (presetsManager.isChangedIndex()) {
		ofLogNotice("ofApp") << "Preset changed to index: " << presetsManager.index.get();
	}

	// Draw Supabase status and GUI
	db.draw();

	// Draw main controls GUI
	if (bGui) drawGui();
	
	// Draw instructions
	drawHelp();
}

//--------------------------------------------------------------
void ofApp::drawGui() {
	gui.draw();
}

//--------------------------------------------------------------
void ofApp::drawHelp() {
	int x = 10;
	int y = ofGetHeight() - 100;
	
	ofPushStyle();
	ofSetColor(255, 200);
	
	ofDrawBitmapString("KEYS:", x, y);
	y += 15;
	ofDrawBitmapString("G - Toggle GUI", x, y);
	y += 15;
	ofDrawBitmapString("S - Manual Sync (pull from Supabase)", x, y);
	y += 15;
	ofDrawBitmapString("P - Push current preset to Supabase", x, y);
	y += 15;
	
	ofSetColor(100, 255, 100);
	if (db.isConnected()) {
		ofDrawBitmapString("Status: CONNECTED to Supabase", x, y);
	} else {
		ofSetColor(255, 100, 100);
		ofDrawBitmapString("Status: DISCONNECTED - Check credentials.txt", x, y);
	}
	
	ofPopStyle();
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == 'g' || key == 'G') {
		bGui = !bGui;
	}
	else if (key == 's' || key == 'S') {
		// Manual sync
		db.syncNow();
		ofLogNotice("ofApp") << "Manual sync triggered";
	}
	else if (key == 'p' || key == 'P') {
		// Push current preset manually
		db.pushCurrentPreset();
		ofLogNotice("ofApp") << "Manual push triggered";
	}
	else if (key == ' ') {
		// Toggle Supabase debug
		// db.bShowDebug is private, controlled via GUI
	}
}

//--------------------------------------------------------------
void ofApp::exit() {
	db.exit();
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
}
