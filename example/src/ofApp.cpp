#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
  ofLogNotice("ofApp") << "setup()";
  
  ofSetFrameRate(60);
  ofSetWindowTitle("ofxSurfingSupabase Example");

  // Move window
  //ofSetWindowPosition(-ofGetWidth()-100, 100);//left monitor
  //ofSetWindowPosition(3840+2160+100, 100);//right 4th FHD monitor

  // Setup scene
  scene.params.setName("Scene");
  
  // Setup database
  db.setup(scene.params);
  
  // Gui scene
  guiScene.setup(scene.params);
  guiScene.setPosition(ofGetWidth() - guiScene.getWidth() - 10, 10);
  guiScene.add(vRandomize);

  // Scene randomizer
  e_vRandomize = vRandomize.newListener([this]() {
    ofxSurfing::doRandomize(scene.params);
  });

  ofLogNotice("ofApp") << "setup(): Setup complete";
}

//--------------------------------------------------------------
void ofApp::update() {
  db.update();
}

//--------------------------------------------------------------
void ofApp::draw() {
  ofBackground(0);
  
  // Draw scene
  scene.draw();

  // UI Debug
  if(!db.bGui) return;

  // Scene UI
  guiScene.draw();
  
  // Draw database UI
  db.draw();
  
  // Draw instructions
  int y = ofGetHeight() - 140;
  ofDrawBitmapStringHighlight("CONTROLS:", 20, y);
  ofDrawBitmapStringHighlight("G: Toggle Gui", 20, y + 20);
  ofDrawBitmapStringHighlight(">: Next Preset", 20, y + 40);
  ofDrawBitmapStringHighlight("<: Previous Preset", 20, y + 60);
  ofDrawBitmapStringHighlight("L: Load", 20, y + 80);
  ofDrawBitmapStringHighlight("S: Save", 20, y + 100);
  ofDrawBitmapStringHighlight("R: Refresh", 20, y + 120);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
  if (key == 's') {
    db.vSaveToRemote.trigger();
  } else if (key == 'l' || key == 'L') {
    db.loadAndApplyRemote();
  }else if (key == 'g' || key == 'G') {
    db.bGui = !db.bGui;
  } else if (key == OF_KEY_LEFT) {
    db.selectPrevious();
  } else if (key == OF_KEY_RIGHT) {
    db.selectNext();
  } else if (key == 'r' || key == 'R') {
    db.refreshPresetList();
  } 
}

//--------------------------------------------------------------
void ofApp::exit() {
  ofLogNotice("ofApp") << "exit()";
  db.exit();
}
