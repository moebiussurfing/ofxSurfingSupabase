// file: src/SupabasePresetSync.h

#pragma once
#include "ofMain.h"
#include "SupabaseClient.h"
#include "ofxSurfingPresetsLiteOfxGui.h"

class SupabasePresetSync {
public:
	void setup(SupabaseClient* client, const string& userId);
	void syncWithPresetsManager(SurfingPresetsLiteOfxGui& pm);
	
	// Manual sync operations
	void pullAllPresets();
	void pushPreset(const string& name, const ofJson& data);
	void pushCurrentPreset();
	void deleteRemotePreset(const string& name);
	
	// Status
	bool isSyncing() const { return bIsSyncing; }
	string getLastSyncTime() const { return lastSyncTime; }
	int getPendingOperations() const { return pendingOps; }
	
	ofEvent<void> onSyncComplete;
	ofEvent<string> onSyncError;
	
private:
	SupabaseClient* client = nullptr;
	string userId;
	SurfingPresetsLiteOfxGui* presetsManager = nullptr;
	
	bool bIsSyncing = false;
	string lastSyncTime;
	int pendingOps = 0;
	
	// Listeners
	void onPresetSaved(int& index);
	void onPresetDeleted(int& index);
	
	// Helpers
	string getCurrentPresetName();
	ofJson getCurrentPresetData();
	void savePresetToLocal(const string& name, const ofJson& data);
};
