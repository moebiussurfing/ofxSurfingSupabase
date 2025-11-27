# example2 - Pure Remote Presets

**Standalone remote preset system WITHOUT local JSON files**

## Features

- ✅ Direct save/load from Supabase database
- ✅ No local JSON files required
- ✅ Apply presets directly to scene parameters
- ✅ Browse remote presets with arrow buttons
- ✅ Backup original locally (optional)

## How It Works

### 1. Save Scene Direct
```cpp
// Serialize scene.params → Upload to Supabase
// Generates name: scene_20251127_010245
```

### 2. Browse Remotes
- Use **◀ Previous / Next ▶** buttons
- See all presets from database

### 3. Load & Apply
```cpp
// Download JSON → Deserialize to scene.params
// NO local file created
```

### 4. Remote Mode Toggle
- **OFF:** Load saves to local file first
- **ON:** Load applies directly (pure remote)

## Usage

```cpp
// In ofApp.h
SurfingSceneSimple scene;
ofxSurfingSupabase db;

// In ofApp.cpp setup()
scene.setup();
db.setup();
db.setupSceneParams(scene.params); // Link scene
db.bRemoteMode = true; // Enable pure remote

// Draw
db.draw(); // Shows UI panel
```

## UI Panel

```
┌─ Supabase ─────────────────┐
│ [x] Auto Sync              │
│ [x] Remote Mode       ✅   │  
│ [x] Show Debug             │
│ [x] Show Preset Manager    │
│ [Save Scene Direct]        │ ← Upload current state
│ [Load & Apply]             │ ← Download & apply  
│                            │
│ ◀ Previous | Next ▶        │ ← Browse
│ preset_005                 │ ← Selected
│ [Delete Selected]          │
└────────────────────────────┘
```

## Config

`bin/data/credentials.txt`:
```
AUTH_MODE=EMAIL_PASSWORD
SUPABASE_URL=https://your-project.supabase.co
SUPABASE_ANON_KEY=your-anon-key
EMAIL=user@example.com
PASSWORD=yourpassword
```

## Workflow

1. Adjust scene sliders
2. Click **"Save Scene Direct"**
3. Preset uploaded to Supabase ✅
4. Browse with **◀ ▶**
5. Click **"Load & Apply"**
6. Scene restored from remote ✅

## vs example (Hybrid Mode)

| Feature | example (Hybrid) | example2 (Pure Remote) |
|---------|------------------|------------------------|
| Local JSON files | ✅ Yes | ❌ No |
| presetsLite integration | ✅ Yes | ❌ Not used |
| Direct scene save | ❌ No | ✅ Yes |
| Load & Apply direct | ❌ No | ✅ Yes |
| Use case | Local + Cloud sync | Cloud-first |

## Notes

- Addon `ofxSurfingPresetsLite` is included but **NOT used**
- It's required for compilation only (dependency in `SupabasePresetSync.h`)
- All preset operations go directly to/from Supabase
- No `Kit-00/` folder or local JSON files created

## Build

```bash
cd example2
make
make RunRelease
```

