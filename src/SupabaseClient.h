// file: src/SupabaseClient.h

#pragma once
#include "ofMain.h"
#include "SupabaseConfig.h"
#include <functional>

class SupabaseClient {
public:
	void setup(SupabaseConfig& cfg);
	
	// Authentication
	void authenticateAsync(std::function<void(string userId)> onSuccess,
	                       std::function<void(string)> onError);
	
	bool isAuthenticated() const;
	string getUserId() const;
	
	// Events
	ofEvent<string> onAuthSuccess;
	ofEvent<string> onAuthError;
	
	// Async operations
	void selectAsync(const string& table, const string& query,
	                 std::function<void(ofJson)> onSuccess,
	                 std::function<void(string)> onError);
	
	void insertAsync(const string& table, const ofJson& data,
	                 std::function<void(ofJson)> onSuccess,
	                 std::function<void(string)> onError);
	
	void updateAsync(const string& table, const string& filter,
	                 const ofJson& data,
	                 std::function<void()> onSuccess,
	                 std::function<void(string)> onError);
	
	void deleteAsync(const string& table, const string& filter,
	                 std::function<void()> onSuccess,
	                 std::function<void(string)> onError);
	
	bool isConfigured() const { return config.isValid(); }
	
private:
	SupabaseConfig config;
	bool bAuthenticated = false;
	
	map<string, string> getHeaders(bool usePatch = false) const;
	
	void handleResponse(ofHttpResponse& response,
	                    std::function<void(ofJson)> onSuccess,
	                    std::function<void(string)> onError);
};
