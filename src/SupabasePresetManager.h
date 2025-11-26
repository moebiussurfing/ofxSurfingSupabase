// file: src/SupabasePresetManager.h

#pragma once
#include "ofMain.h"
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
	void drawGui(int x = 10, int y = 10);
	
	// Preset operations
	void refreshPresetList();
	void loadPreset(const string& name);
	void savePreset(const string& name, const ofJson& data);
	void deletePreset(const string& name);
	void renamePreset(const string& oldName, const string& newName);
	
	// Getters
	vector<PresetInfo>& getPresets() { return presets; }
	int getSelectedIndex() const { return selectedIndex; }
	bool isLoading() const { return bIsLoading; }
	
	// Events
	ofEvent<PresetInfo> onPresetLoaded;
	ofEvent<string> onPresetSaved;
	ofEvent<string> onPresetDeleted;
	ofEvent<string> onError;
	
private:
	SupabaseClient* client = nullptr;
	string userId;
	
	vector<PresetInfo> presets;
	int selectedIndex = -1;
	bool bIsLoading = false;
	
	// UI state
	string newPresetName = "";
	string renameBuffer = "";
	int renameIndex = -1;
	bool bShowSaveDialog = false;
	bool bShowRenameDialog = false;
	bool bShowDeleteConfirm = false;
	int deleteConfirmIndex = -1;
	
	void drawPresetList(int x, int y, int width);
	void drawControls(int x, int y, int width);
	void drawDialogs();
};
