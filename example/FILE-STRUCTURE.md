# Project File Structure

```
example/
├── example.xcodeproj/
│   ├── xcshareddata/
│   │   └── xcschemes/
│   │       ├── example Debug.xcscheme
│   │       └── example Release.xcscheme
│   └── project.pbxproj
├── src/
│   ├── main.cpp
│   ├── ofApp.cpp
│   ├── ofApp.h
│   └── surfingSceneTesters.h
├── FILE-STRUCTURE.md
├── Makefile
├── Project.xcconfig
├── addons.make
├── config.make
├── example.code-workspace
├── generate-FILE-STRUCTURE.py
├── of.entitlements
└── openFrameworks-Info.plist
```

---

## Used Addons (from addons.make)

- `ofxGui`
- `ofxSurfingHelpersLite`
- `ofxSurfingSupabase`

### OpenFrameworks Paths

- OF root path (relative to project root): `../../../`

- OF addons path (relative to project root): `../../addons/`
