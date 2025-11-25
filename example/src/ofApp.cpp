// file: src/ofApp.cpp

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	// Pass group of parameters to the presets manager
	presetsManager.setup(scene.params);

	db.setup();

	setupGui();
}

//--------------------------------------------------------------
void ofApp::setupGui() {

}

//--------------------------------------------------------------
void ofApp::update() {
	db.update();
}

//--------------------------------------------------------------
void ofApp::draw() {
	// Draw scene
	scene.draw();

	// Draw gui
	presetsManager.drawGui();
	// Simple callback to listen to index changes
	if (presetsManager.isChangedIndex()) ofLogNotice("ofApp") << "index changed to " << presetsManager.index.get();

	// Draw Supabase gui/info
	db.draw();

	if (bGui) drawGui();
}

//--------------------------------------------------------------
void ofApp::drawGui() {
	gui.draw();
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	
}

//--------------------------------------------------------------
void ofApp::exit() {
	db.exit();
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
}
