#pragma once

#include "ofMain.h"
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
  SurfingSceneTesters scene;
  ofxSurfingSupabase db;
};
