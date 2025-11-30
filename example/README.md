# ofxSurfingSupabase - Example

Basic example demonstrating **pure remote mode** with Supabase integration.

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

Follow **[../SUPABASE-SETUP.md](../SUPABASE-SETUP.md)** to:
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

### 3. Compile & Run

Using Makefile:
```bash
make
make run
```

Or use your IDE (Xcode, VS Code, etc.)

---

## Usage

### Keyboard Controls

- **Space**: Save current scene directly to Supabase
- **L**: Load selected preset and apply to scene
- **Left/Right Arrow**: Browse presets
- **R**: Refresh preset list from database
- **Shift+D**: Clear entire database (WARNING!)

### GUI Controls

The Supabase panel shows:
- **Remote Mode**: Toggle pure cloud mode
- **Save Scene Direct**: Upload current parameters with timestamp name
- **Load & Apply**: Download and apply selected preset
- **Preset Manager**: Browse/delete presets

---

## Workflow

1. **Adjust scene parameters** (colors, amount, scale, etc.)
2. **Press Space** or click "Save Scene Direct" → Uploads to cloud
3. **Browse presets** with arrow keys or ◀ ▶ buttons
4. **Press L** or click "Load & Apply" → Restores from cloud
5. **NO local files** are created (pure remote)

---

## Expected Output

Console:
```
[notice] ofApp: setup()
[notice] ofxSurfingSupabase: setup()
[notice] ofxSurfingSupabase: Auth mode: EMAIL_PASSWORD
[notice] ofxSurfingSupabase: Authenticated successfully
[notice] ofxSurfingSupabase: User ID: abc123...
[notice] ofApp: Setup complete
```

GUI Status:
```
Status: 🟢 CONNECTED
Selected: scene_20251130_120000 (1/5)
```

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
scene.params.setName("Scene");
db.setup();
db.setupSceneParams(scene.params); // Link to scene
db.bRemoteMode = true; // Pure remote mode

// Save
db.sendSceneDirect(); // Auto-generates timestamp name

// Load
db.loadAndApplyRemote(); // Applies directly to scene.params

// Browse
db.selectNext();
db.selectPrevious();
```

---

## Next Steps

- Integrate with your own parameter groups
- Add custom preset naming
- Implement multithreading for async operations
- Add user authentication UI
- Implement preset categories/tags

---

**See also:**
- [SUPABASE-SETUP.md](../SUPABASE-SETUP.md) - Database configuration
- [README.md](../README.md) - Addon documentation
