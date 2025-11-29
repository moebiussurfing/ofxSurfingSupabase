// file: src/ofxSurfingSupabase.h
// PURE REMOTE MODE - No presetsLite dependencies

#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxSurfingHelpersLite.h"
#include "SupabaseClient.h"
#include "SupabaseConfig.h"
#include "SupabasePresetManager.h"

class ofxSurfingSupabase {
public:
	void setup();
	void setupSceneParams(ofParameterGroup& sceneParams);
	void update();
	void draw();
	void exit();
	
	// Core operations
	void saveSceneDirect();       // Serialize scene → Save to DB
	void loadAndApply();          // Load selected → Apply to scene
	void deleteSelected();        // Delete selected preset
	void clearDatabase();         // Clear all presets (DEBUG)
	void refreshList();           // Refresh preset list from DB
	
	// Navigation
	void selectNext();
	void selectPrevious();
	
	// Status
	bool isConnected() const { return client.isConfigured() && client.isAuthenticated(); }
	string getAuthMode() const { return config.authMode == AuthMode::API_KEY ? "API Key" : "Email/Password"; }
	
	// UI Control
	void setShowGui(bool show) { bShowGui = show; }
	
	SupabaseConfig config;
	
private:
	SupabaseClient client;
	SupabasePresetManager presetManager;
	ofParameterGroup* sceneParamsPtr = nullptr;
	
	// UI Parameters
	ofParameter<bool> bShowGui{"Supabase UI", true};
	ofParameter<bool> bAutoApply{"Auto Apply", false};
	
	ofParameter<void> btnSave{"Save Scene Direct"};
	ofParameter<void> btnLoad{"Load & Apply"};
	ofParameter<void> btnRefresh{"Refresh List"};
	ofParameter<void> btnPrev{"Prev"};
	ofParameter<void> btnNext{"Next"};
	ofParameter<void> btnDelete{"Delete Selected"};
	ofParameter<void> btnClear{"Clear DB (Debug)"};
	
	ofParameterGroup params{"SUPABASE REMOTE", bShowGui, bAutoApply, btnSave, btnLoad, btnPrev, btnNext, btnRefresh, btnDelete, btnClear};
	ofxPanel gui;
	
	// Event handlers
	void setupAfterAuth(string& userId);
	void onSaveScene();
	void onLoadAndApply();
	void onRefreshList();
	void onPrevPreset();
	void onNextPreset();
	void onDeleteSelected();
	void onClearDatabase();
	void onAutoApplyChanged(bool& value);
	
	// UI
	void drawPresetBrowser();
};