// file: src/SupabaseConfig.h

#pragma once
#include "ofMain.h"

enum class AuthMode {
	API_KEY,      // Direct API key auth (anon key)
	EMAIL_PASSWORD // Email/password authentication
};

struct SupabaseConfig {
	string projectUrl;
	string apiKey;
	string userId;
	
	// Email/password auth fields
	string email;
	string password;
	string accessToken;
	AuthMode authMode = AuthMode::API_KEY;
	
	bool loadFromFile(const string& path = "credentials.txt") {
		string fullPath = ofToDataPath(path);
		ofBuffer buffer = ofBufferFromFile(fullPath);
		
		if (!buffer.size()) {
			ofLogError("SupabaseConfig") << "Could not load: " << fullPath;
			return false;
		}
		
		for (auto& line : buffer.getLines()) {
			if (line.empty() || line[0] == '#') continue;
			
			auto parts = ofSplitString(line, "=");
			if (parts.size() != 2) continue;
			
			string key = parts[0];
			string val = parts[1];
			
			// Trim whitespace manually
			key.erase(0, key.find_first_not_of(" \t\r\n"));
			key.erase(key.find_last_not_of(" \t\r\n") + 1);
			val.erase(0, val.find_first_not_of(" \t\r\n"));
			val.erase(val.find_last_not_of(" \t\r\n") + 1);
			
			if (key == "project_url") projectUrl = val;
			else if (key == "api_key") apiKey = val;
			else if (key == "user_id") userId = val;
			else if (key == "email") email = val;
			else if (key == "password") password = val;
		}
		
		// Determine auth mode
		if (!email.empty() && !password.empty()) {
			authMode = AuthMode::EMAIL_PASSWORD;
			ofLogNotice("SupabaseConfig") << "Auth mode: EMAIL_PASSWORD";
		} else if (!apiKey.empty() && !userId.empty()) {
			authMode = AuthMode::API_KEY;
			ofLogNotice("SupabaseConfig") << "Auth mode: API_KEY";
		}
		
		bool valid = !projectUrl.empty() && 
		            ((authMode == AuthMode::API_KEY && !apiKey.empty() && !userId.empty()) ||
		             (authMode == AuthMode::EMAIL_PASSWORD && !email.empty() && !password.empty()));
		
		if (valid) {
			ofLogNotice("SupabaseConfig") << "Loaded successfully";
			ofLogNotice("SupabaseConfig") << "Project: " << projectUrl;
			if (authMode == AuthMode::API_KEY) {
				ofLogNotice("SupabaseConfig") << "User ID: " << userId;
			} else {
				ofLogNotice("SupabaseConfig") << "Email: " << email;
			}
		} else {
			ofLogError("SupabaseConfig") << "Missing required fields";
			ofLogError("SupabaseConfig") << "Provide either: (project_url + api_key + user_id) OR (project_url + email + password)";
		}
		
		return valid;
	}
	
	string getRestUrl() const {
		return projectUrl + "/rest/v1";
	}
	
	string getAuthUrl() const {
		return projectUrl + "/auth/v1";
	}
	
	bool isValid() const {
		return !projectUrl.empty() && 
		       ((authMode == AuthMode::API_KEY && !apiKey.empty() && !userId.empty()) ||
		        (authMode == AuthMode::EMAIL_PASSWORD && !email.empty() && !password.empty()) ||
		        (authMode == AuthMode::EMAIL_PASSWORD && !accessToken.empty()));
	}
	
	bool needsAuthentication() const {
		return authMode == AuthMode::EMAIL_PASSWORD && accessToken.empty();
	}
};
