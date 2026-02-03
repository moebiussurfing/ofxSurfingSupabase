#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class ofxSurfingSupabase {
  // Lifecycle
private:
  // Setup scene parameters to manage. Auto calls setup()
  void setupPresetParameters(ofParameterGroup& sceneParams);

public:
  void setup(); // Only for using with a external presets manager
  void setup(ofParameterGroup & sceneParams); // Main setup passing target param group

  void update();
  void draw();	
  void exit();
  void keyPressed(int key);

  //--
   
  // Remote operations
  void savePreset(const std::string& presetName);
  void loadPreset(const std::string& presetName);
  void deletePresetRemote(const std::string& presetName);
  void refreshPresetListRemote();
  void clearDatabase();
  
  // Browse presets
  void selectNextRemote();
  void selectPreviousRemote();
  void selectedIndexRemoteUpdate();
  
  // Direct scene operations (no local files)
  // void sendSceneDirect();
  void loadAndApplyRemote();
  
  // UI Parameters
  ofParameter<bool> bConnected{"Connected", false};
  ofParameter<bool> bRemoteMode{"Remote Mode", true};
  ofParameter<bool> bAutoSave{"Auto Save", false};
  ofParameter<bool> bAutoLoad{"Auto Load", true};
  ofParameter<bool> bGui{"Show Gui", true};
  ofParameter<bool> bKeys{"Keys", true};
  ofParameter<bool> bDebug{"Debug", true};
  ofParameter<void> vReconnect{"Reconnect"};
  ofParameter<void> vSaveToRemote{"Save to Remote"};
  ofParameter<void> vLoadFromRemote{"Load from Remote"};
  ofParameter<void> vRefreshListRemote{"Refresh List"};
  ofParameter<void> vDeleteSelectedRemote{"Delete Selected"};
  ofParameter<void> vClearDatabase{"Clear Database"};
  ofParameter<void> vSelectNextRemote{">"};
  ofParameter<void> vSelectPreviousRemote{"<"};
  ofParameter<int> selectedPresetIndexRemote{"Selected",0,0,10};
  // ofParameter<bool> bShowPresetManager{"Show Preset Manager", true};
  // ofParameter<void> vLoadAndApply{"Load"};
  // ofParameter<void> vSaveSceneDirect{"Save Direct"};
  
  // Status
  std::string getConnectionStatus() const;
  bool isConnected() const { return bConnected; }
  
private:
  int selectedPresetIndexRemotePrev = -1;

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
  
  std::vector<std::string> presetsNamesRemote;
  // int selectedPresetIndexRemote;
  
  // UI
  ofxPanel gui_;
  ofParameterGroup params_;
  ofParameterGroup paramsManager_;
  
  // Event listeners
  ofEventListener e_vReconnect;
  ofEventListener e_vSaveToRemote;
  ofEventListener e_vLoadFromRemote;
  ofEventListener e_vRefreshListRemnote;
  ofEventListener e_vDeleteSelected;
  ofEventListener e_vClearDatabase;
  ofEventListener e_vSelectNextRemote;
  ofEventListener e_vSelectPreviousRemote;
  ofEventListener e_selectedPresetIndexRemote;
  // ofEventListener e_vSaveSceneDirect;
  // ofEventListener e_vLoadAndApply;
  
  // Constants
  static const std::string CREDENTIALS_PATH;
  static const std::string TABLE_NAME;
};
