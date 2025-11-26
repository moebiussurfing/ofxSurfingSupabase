// file: src/SupabaseClient.h

#pragma once
#include "ofMain.h"
#include "SupabaseConfig.h"

class SupabaseClient {
public:
	void setup(SupabaseConfig& cfg);
	
	// Authentication
	void authenticateAsync(function<void(string userId)> onSuccess,
	                       function<void(string)> onError);
	
	bool isAuthenticated() const;
	string getUserId() const;
	
	// Events
	ofEvent<string> onAuthSuccess;
	ofEvent<string> onAuthError;
	
	// Async operations
	void selectAsync(const string& table, const string& query,
	                 function<void(ofJson)> onSuccess,
	                 function<void(string)> onError);
	
	void insertAsync(const string& table, const ofJson& data,
	                 function<void(ofJson)> onSuccess,
	                 function<void(string)> onError);
	
	void updateAsync(const string& table, const string& filter,
	                 const ofJson& data,
	                 function<void()> onSuccess,
	                 function<void(string)> onError);
	
	void deleteAsync(const string& table, const string& filter,
	                 function<void()> onSuccess,
	                 function<void(string)> onError);
	
	bool isConfigured() const { return config.isValid(); }
	
private:
	SupabaseConfig config;
	bool bAuthenticated = false;
	
	map<string, string> getHeaders(bool usePatch = false) const;
	
	void handleResponse(ofHttpResponse& response,
	                    function<void(ofJson)> onSuccess,
	                    function<void(string)> onError);
};
