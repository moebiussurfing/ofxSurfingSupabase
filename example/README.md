# ofxSurfingSupabase

## WIP
Presets (ofParametersGroup) management with Supabase backend database for openFrameworks.

# Example

Basic example demonstrating **pure remote mode** with Supabase integration.  
Scene parameters are saved and loaded **directly** to/from the cloud database without using any local JSON files.

---

## Features Demonstrated

✅ Email/Password authentication  
✅ Direct scene parameter save/load  
✅ Browse remote presets  
✅ No local JSON files (cloud-first)  
✅ ofxGui integration  

---

## Setup

### 1. Configure Supabase

Follow **[docs/SUPABASE-SETUP.md](docs/SUPABASE-SETUP.md)** to:
- Create Supabase project
- Create `presets` table
- Create test user
- Get credentials

### 2. Configure Credentials

Copy the template:
```bash
cd bin/data
cp credentials.txt.example credentials.txt
```

Edit `bin/data/credentials.txt`:
```txt
AUTH_MODE=EMAIL_PASSWORD
SUPABASE_URL=https://your-project.supabase.co
SUPABASE_ANON_KEY=eyJhbGc...your_anon_key
EMAIL=test@ofxsurfing.com
PASSWORD=testpass123
```

---

## Usage

### Keyboard Controls

- **Space**: Save current scene directly to Supabase
- **L**: Load selected preset and apply to scene
- **Left/Right Arrow**: Browse presets
- **R**: Refresh preset list from database
- **Shift+D**: Clear entire database (WARNING!)

---

## Workflow

1. **Adjust scene parameters** (colors, amount, scale, etc.)
2. **Press Space** or click "Save Scene Direct" → Uploads to cloud
3. **Browse presets** with arrow keys or ◀ ▶ buttons
4. **Press L** or click "Load & Apply" → Restores from cloud
5. **NO local files** are created (pure remote)
6. Use `bAutoSync` to auto-load on preset change

---

## Troubleshooting

**Red status / Not connected:**
- Check `credentials.txt` exists in `bin/data/`
- Verify URL and API key are correct
- Check network connection

**Authentication failed:**
- Email/password incorrect
- User not created in Supabase dashboard
- Email confirmation not disabled

**No presets shown:**
- Database empty (upload some first)
- RLS policies not configured correctly

---

## Code Structure

```cpp
// Setup
db.setupSceneParams(scene.params); // Link to scene
db.bRemoteMode = true; // Pure remote mode
//WIP: db.bRemoteBackup = false; // Remote + local backup

// Save
db.sendSceneDirect(); // Auto-generates timestamp name

// Load
db.loadAndApplyRemote(); // Applies directly to scene.params

// Browse
db.selectNext();
db.selectPrevious();
db.selectedPresetIndex_ = 5; // Current index parameter to control
```

---

## Next Steps

- Threading loader to avoid blocking UI
- Integrate with your own presets manager parameter groups
- Add custom preset naming
- Add user login authentication UI
- Implement preset categories/tags to handle multiple kits
