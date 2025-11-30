# Quick Start Guide

**Get ofxSurfingSupabase running in 5 minutes**

---

## 1. Prerequisites

✅ openFrameworks 0.12.0+ installed  
✅ Supabase account (free tier OK)  
✅ Internet connection  

---

## 2. Setup Supabase (3 minutes)

### Create Project
1. Go to https://supabase.com → Sign in
2. **New project** → Name: `ofxSurfingTest`
3. Choose region, wait 2 minutes

### Create Table
1. **SQL Editor** → New query
2. Copy/paste from `SUPABASE-SETUP.md` (Step 2)
3. Click **Run**

### Create User
1. **Authentication** → **Providers** → Email
2. ❌ **Disable** "Confirm email"
3. **Users** → **Add user**:
   - Email: `test@ofxsurfing.com`
   - Password: `testpass123`
   - ✅ Auto Confirm User

### Get Credentials
1. **Settings** → **API**
2. Copy:
   - Project URL
   - anon public key

---

## 3. Configure App (1 minute)

```bash
cd example/bin/data
cp credentials.txt.example credentials.txt
nano credentials.txt  # or use any editor
```

Fill in your credentials:
```txt
AUTH_MODE=EMAIL_PASSWORD
SUPABASE_URL=https://xxxxx.supabase.co
SUPABASE_ANON_KEY=eyJhbGc...your_key
EMAIL=test@ofxsurfing.com
PASSWORD=testpass123
```

---

## 4. Compile & Run (1 minute)

### Using Makefile (Linux/macOS)
```bash
cd example
make
make run
```

### Using Xcode (macOS)
1. Open `example/example.xcodeproj`
2. Build & Run (⌘R)

### Using VS Code
1. Install OF extension
2. `F5` to build and run

---

## 5. Test It!

### Expected Console Output:
```
[notice] ofApp: setup()
[notice] ofxSurfingSupabase: setup()
[notice] ofxSurfingSupabase: Auth mode: EMAIL_PASSWORD
[notice] ofxSurfingSupabase: Authenticated successfully
[notice] ofxSurfingSupabase: User ID: abc123...
```

### Expected GUI:
```
┌─ Supabase ────────────┐
│ Status: 🟢 CONNECTED  │
│ [ ] Remote Mode       │
│ [ ] Auto Sync         │
│ [x] Show Debug        │
│ [Save Scene Direct]   │
│ [Load & Apply]        │
└───────────────────────┘
```

### Try It:
1. **Adjust scene parameters** (colors, amount, scale)
2. **Press Space** → Saves to cloud
3. **Press Left/Right** → Browse presets
4. **Press L** → Load preset

---

## 6. Troubleshooting

### ❌ Red status
**Check:**
- `credentials.txt` exists in `bin/data/`
- URL/key are correct
- Network connection

### ❌ Compile errors
**Common fixes:**
- Add `ofxGui` to `addons.make`
- Clean build: `make clean && make`
- Update openFrameworks to latest

### ❌ No presets shown
**Normal!** Database is empty on first run.
- Press Space to save first preset
- Press R to refresh list

---

## Next Steps

✨ **You're ready!**

**Customize:**
- Add your own parameters to `SurfingSceneTesters`
- Implement custom preset naming
- Add your own UI elements

**Learn more:**
- [README.md](../README.md) - Full documentation
- [SUPABASE-SETUP.md](../SUPABASE-SETUP.md) - Detailed setup
- [IMPLEMENTATION-STATUS.md](IMPLEMENTATION-STATUS.md) - Technical details

**Need help?**
- Check console logs (set `bShowDebug = true`)
- Review Supabase Dashboard → Table Editor
- See `example/README.md` for detailed usage

---

**Total Time:** ~5 minutes  
**Difficulty:** ⭐⭐☆☆☆ (Easy)
