// file: src/ofxSurfingSupabase.h

#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "SupabaseClient.h"
#include "SupabaseConfig.h"
#include "SupabasePresetSync.h"
#include "SupabasePresetManager.h"
#include "ofxSurfingPresetsLiteOfxGui.h"

class ofxSurfingSupabase {
public:
	void setup();
	void setupSceneParams(ofParameterGroup& sceneParams); // NEW
	void update();
	void draw();
	void exit();
	
	// Main integration method
	void syncWithPresetsManager(SurfingPresetsLiteOfxGui& pm);
	
	// Manual controls
	void syncNow();
	void forcePull();
	void pushCurrentPreset();
	void sendCurrentToRemote(); // From presetsLite file
	void sendSceneDirect(); // NEW: Direct from scene params
	void loadFromRemote(); // NEW: Load selected remote preset
	
	// Status
	bool isConnected() const { return client.isConfigured() && client.isAuthenticated(); }
	bool isSyncing() const { return sync.isSyncing(); }
	string getLastSyncTime() const { return sync.getLastSyncTime(); }
	int getPendingOperations() const { return sync.getPendingOperations(); }
	string getAuthMode() const { return config.authMode == AuthMode::API_KEY ? "API Key" : "Email/Password"; }
	
	// Direct access to preset manager
	SupabasePresetManager& getPresetManager() { return presetManager; }
	
	// Config
	SupabaseConfig config;
	
private:
	SupabaseClient client;
	SupabasePresetSync sync;
	SupabasePresetManager presetManager;
	
	ofParameter<bool> bAutoSync{"Auto Sync", false}; // OFF by default
	ofParameter<bool> bShowDebug{"Show Debug", true};
	ofParameter<bool> bShowPresetManager{"Show Preset Manager", true};
	ofParameter<void> btnSendToRemote{"Send to Remote"};
	ofParameter<void> btnSaveSceneDirect{"Save Scene Direct"}; // NEW
	ofParameter<void> btnLoadFromRemote{"Load from Remote"};
	ofParameterGroup params{"Supabase", bAutoSync, bShowDebug, bShowPresetManager, btnSendToRemote, btnSaveSceneDirect, btnLoadFromRemote};
	
	ofxPanel gui;
	// std::string sHelp = "";
	
	void onSyncComplete();
	void onSyncError(string& error);
	void setupAfterAuth(string& userId);
	void onBtnSendToRemote();
	void onBtnSaveSceneDirect(); // NEW
	void onBtnLoadFromRemote();
	void onRemotePresetLoaded(PresetInfo& info);
	
	SurfingPresetsLiteOfxGui* presetsManagerPtr = nullptr;
	ofParameterGroup* sceneParamsPtr = nullptr; // NEW: Direct scene access
};