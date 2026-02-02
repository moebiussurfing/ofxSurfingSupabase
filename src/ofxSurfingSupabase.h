#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class ofxSurfingSupabase {
  // Lifecycle
private:
  void setup();
  
  // Setup scene parameters to manage
  void setupPresetParameters(ofParameterGroup& sceneParams);

public:
  void setup(ofParameterGroup & sceneParams);

  void update();
  void draw();
  void exit();

  //--
   
  // Remote operations
  void savePreset(const std::string& presetName);
  void loadPreset(const std::string& presetName);
  void deletePreset(const std::string& presetName);
  void refreshPresetList();
  void clearDatabase();
  
  // Browse presets
  void selectNext();
  void selectPrevious();
  
  // Direct scene operations (no local files)
  void sendSceneDirect();
  void loadAndApplyRemote();
  
  // UI Parameters
  ofParameter<bool> bConnected{"Connected", false};
  ofParameter<bool> bRemoteMode{"Remote Mode", false};
  ofParameter<bool> bAutoSync{"Auto Sync", false};
  ofParameter<bool> bShowDebug{"Show Debug", true};
  ofParameter<bool> bShowPresetManager{"Show Preset Manager", true};
  
  ofParameter<void> vReconnect{"Reconnect"};
  ofParameter<void> vSaveSceneDirect{"Save Scene Direct"};
  ofParameter<void> vLoadAndApply{"Load & Apply"};
  ofParameter<void> vSendToRemote{"Send to Remote"};
  ofParameter<void> vLoadFromRemote{"Load from Remote"};
  ofParameter<void> vRefreshList{"Refresh List"};
  ofParameter<void> vDeleteSelected{"Delete Selected"};
  ofParameter<void> vClearDatabase{"Clear Database"};
  ofParameter<void> vSelectNext{"Next >"};
  ofParameter<void> vSelectPrevious{"< Previous"};
  
  // Status
  std::string getConnectionStatus() const;
  bool isConnected() const { return bConnected; }
  
private:
  
  // Configuration
  struct SupabaseConfig {
    std::string authMode;
    std::string supabaseUrl;
    std::string supabaseAnonKey;
    std::string email;
    std::string password;
    bool isValid() const;
  };
  
  // HTTP Client
  struct HttpResponse {
    int statusCode;
    std::string body;
    bool success;
  };
  
  // Internal methods
  void setupParameters();
  void setupCallbacks();
  void setupGui();
  void startup();
  
  bool loadCredentials();
  bool authenticate();
  
  HttpResponse httpGet(const std::string& endpoint);
  HttpResponse httpPost(const std::string& endpoint, const std::string& jsonBody);
  HttpResponse httpDelete(const std::string& endpoint);
  
  std::string serializeSceneToJson();
  void deserializeJsonToScene(const std::string& jsonStr);
  
  std::string generateTimestampName();
  
  // State
  SupabaseConfig config_;
  std::string authToken_;
  std::string userId_;
  
  ofParameterGroup* sceneParams_;
  
  std::vector<std::string> presetNames_;
  int selectedPresetIndex_;
  
  // UI
  ofxPanel gui_;
  ofParameterGroup params_;
  ofParameterGroup paramsManager_;
  
  // Event listeners
  ofEventListener e_vReconnect;
  ofEventListener e_vSaveSceneDirect;
  ofEventListener e_vLoadAndApply;
  ofEventListener e_vSendToRemote;
  ofEventListener e_vLoadFromRemote;
  ofEventListener e_vRefreshList;
  ofEventListener e_vDeleteSelected;
  ofEventListener e_vClearDatabase;
  ofEventListener e_vSelectNext;
  ofEventListener e_vSelectPrevious;
  
  // Constants
  static const std::string CREDENTIALS_PATH;
  static const std::string TABLE_NAME;
};
