# Changelog

**Latest Version:** 1.0.0  
**Date:** 2025-11-27

## Fixes

### DELETE Operations
- **Fixed HTTP 400 errors**
- Query by preset name → Delete by ID
- Methods: `deletePreset()`, `clearDatabase()`

### UPSERT Logic
- **Fixed 409 (duplicate) and 403 (forbidden) errors**
- SELECT first → UPDATE if exists → INSERT if not
- Method: `upsertPreset(name, json)`

### RLS Policy
- **Fixed UPDATE permission denied**
- Added `WITH CHECK` clause to policy
- See SUPABASE-SETUP.md for SQL

---

## New Features

### Remote Mode Toggle
```cpp
ofParameter<bool> bRemoteMode{"Remote Mode", false};
```
- **OFF:** Hybrid mode (local files + cloud sync)
- **ON:** Pure remote (cloud-only)

### Save Scene Direct
```cpp
void sendSceneDirect();
```
- Serializes scene parameters directly
- No presetsLite required
- Auto-generated names: `scene_YYYYMMDD_HHMMSS`

### Load & Apply
```cpp
void loadAndApplyRemote();
```
- Downloads remote preset
- Deserializes directly to scene parameters
- NO local file created

### example2/
**Pure remote preset system without presetsLite:**
- No local JSON files
- Direct scene parameter save/load
- Browse remote presets with UI
- Standalone workflow

---

## UI Improvements

### Status Monitoring
- 🟢 **CONNECTED** - Authenticated successfully
- 🟡 **AUTHENTICATING...** - Waiting for response
- 🔴 **ERROR** - Check credentials.txt

### Panel Layout
```
┌─ Supabase ────────────────────┐
│ [x] Auto Sync                 │
│ [x] Remote Mode          NEW  │
│ [x] Show Debug                │
│ [x] Show Preset Manager       │
│                               │
│ [Save Scene Direct]      NEW  │
│ [Load & Apply]           NEW  │
│ [Send to Remote]              │
│ [Load from Remote]            │
└───────────────────────────────┘
```

---

## Workflow Comparison

### Hybrid Mode (example/)
1. Use presetsLite normally (local JSON files)
2. "Send to Remote" → Uploads current preset
3. "Load from Remote" → Downloads to Kit-00/
4. Load from presetsLite UI

### Pure Remote Mode (example2/) **RECOMMENDED**
1. Adjust scene parameters
2. "Save Scene Direct" → Upload to Supabase
3. Browse with ◀ ▶ buttons
4. "Load & Apply" → Restore from cloud
5. NO local files created

---

## Modified Files

**Core:**
- `src/SupabaseClient.cpp` - DELETE fix
- `src/SupabasePresetManager.h/cpp` - UPSERT logic
- `src/ofxSurfingSupabase.h/cpp` - Remote mode toggle + new methods

**Examples:**
- `example2/` - Pure remote implementation (standalone)

**Documentation:**
- All documentation translated to English
- Consolidated Supabase setup guide

