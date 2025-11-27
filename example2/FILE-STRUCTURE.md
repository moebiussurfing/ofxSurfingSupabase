# Project File Structure

```
example2/
├── example2.xcodeproj/
│   ├── xcshareddata/
│   │   └── xcschemes/
│   │       ├── example2 Debug.xcscheme
│   │       └── example2 Release.xcscheme
│   └── project.pbxproj
├── src/
│   ├── main.cpp
│   ├── ofApp.cpp
│   ├── ofApp.h
│   └── surfingSceneTesters.h
├── FILE-STRUCTURE.md
├── Makefile
├── Project.xcconfig
├── README.md
├── addons.make
├── config.make
├── example2.code-workspace
├── generate-FILE-STRUCTURE.py
├── of.entitlements
└── openFrameworks-Info.plist
```

---

## Used Addons (from addons.make)

- `ofxGui`
- `ofxSurfingHelpersLite`
- `ofxSurfingPresetsLite`
- `ofxSurfingSupabase`

### OpenFrameworks Paths

- OF root path (relative to project root): `../../../`

- OF addons path (relative to project root): `../../addons/`
