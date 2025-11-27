// file: src/SupabasePresetManager.h

#pragma once
#include "ofMain.h"
#include "ofxGui.h"
#include "SupabaseClient.h"

struct PresetInfo {
	string name;
	string id;
	string createdAt;
	string updatedAt;
	ofJson data;
};

class SupabasePresetManager {
public:
	void setup(SupabaseClient* client, const string& userId);
	void drawGui();
	
	// Preset operations
	void refreshPresetList();
	void loadPreset(const string& name);
	void savePreset(const string& name, const ofJson& data);
	void deletePreset(const string& name);
	void clearDatabase();
	
	// Navigation
	void selectNext();
	void selectPrev();
	int getSelectedIndex() const { return selectedIndex; }
	string getSelectedPresetName() const;
	
	// Getters
	vector<PresetInfo>& getPresets() { return presets; }
	bool isLoading() const { return bIsLoading; }
	
	// Events
	ofEvent<PresetInfo> onPresetLoaded;
	ofEvent<string> onPresetSaved;
	ofEvent<string> onPresetDeleted;
	ofEvent<string> onError;
	
	// GUI Parameters
	ofParameterGroup params{"Remote Presets"};
	ofParameter<void> btnRefresh{"Refresh List"};
	ofParameter<void> btnLoadSelected{"Load Selected"};
	ofParameter<void> btnDeleteSelected{"Delete Selected"};
	ofParameter<void> btnClearDatabase{"Clear Database (Debug)"};
	ofParameter<void> btnPrev{"◀ Prev"};
	ofParameter<void> btnNext{"Next ▶"};
	ofParameter<string> selectedPresetName{"Selected", "none"};
	
private:
	SupabaseClient* client = nullptr;
	string userId;
	
	vector<PresetInfo> presets;
	int selectedIndex = -1;
	bool bIsLoading = false;
	
	ofxPanel gui;
	
	// Callbacks
	void onBtnRefresh();
	void onBtnLoad();
	void onBtnDelete();
	void onBtnClear();
	void onBtnPrev();
	void onBtnNext();
	
	void updateSelectedName();
};
