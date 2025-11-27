# 🚀 ofxSurfingSupabase - Guía de Uso

## 📋 Nueva UI - ofxGui Based

### ✅ Lo Implementado

#### Panel Principal "Supabase"
```
┌─ Supabase ────────────────┐
│ [x] Auto Sync (OFF)       │
│ [x] Show Debug            │
│ [x] Show Preset Manager   │
│ [Send to Remote]          │
│ [Load from Remote]        │
└───────────────────────────┘
```

#### Panel "Remote Presets"
```
┌─ Remote Presets ─────────┐
│ Selected: preset_001     │
│ [◀ Prev] [Next ▶]        │
│ [Refresh List]           │
│ [Load Selected]          │
│ [Delete Selected]        │
│ [Clear Database (Debug)] │
│                          │
│ Remote Presets (3):      │
│ ▶ preset_001            │
│   preset_002            │
│   preset_003            │
└──────────────────────────┘
```

---

## 🎯 Flujo de Trabajo

### Modo Manual (Recomendado)

1. **Trabaja con presetsLite normalmente:**
   - Ajusta parámetros de la escena
   - Guarda preset local (presetsManager UI)
   - El preset se guarda en `bin/data/Kit-00/XX.json`

2. **Envía a Supabase cuando quieras:**
   - Asegúrate de tener el preset guardado localmente
   - Click **"Send to Remote"** en panel Supabase
   - El JSON local se sube a Supabase

3. **Navega presets remotos:**
   - Usa **[◀ Prev]** / **[Next ▶]** para browsear
   - Click **"Load from Remote"** descarga y guarda en Kit
   - Luego cárgalo desde presetsManager UI

### Auto-Sync Mode (Opcional)

1. Activa **"Auto Sync"** toggle
2. Cada vez que guardes preset local, se sube automático
3. ⚠️ Puede saturar BD si guardas mucho

---

## 🔧 Botones Explicados

### Panel Supabase

**Auto Sync**
- OFF: Solo manual con "Send to Remote"
- ON: Cada save local → auto-push

**Send to Remote**
- Lee archivo `Kit-00/XX.json` (preset actual)
- Lo sube a Supabase con nombre `preset_XXX`
- ⚠️ Debes guardar localmente primero!

**Load from Remote**
- Descarga preset seleccionado
- Lo guarda como `Kit-00/remote_preset_XXX.json`
- Cárgalo después desde presetsLite

### Panel Remote Presets

**◀ Prev / Next ▶**
- Navega lista de presets remotos
- Actualiza "Selected:"

**Refresh List**
- Recarga lista desde Supabase
- Útil si otros usuarios suben presets

**Load Selected**
- Mismo que "Load from Remote" arriba

**Delete Selected**
- Borra preset remoto (irreversible)
- Solo borra TU preset (por user_id)

**Clear Database (Debug)**
- ⚠️ BORRA TODOS tus presets remotos
- Solo para testing/desarrollo

---

## 📂 Estructura de Archivos

```
bin/data/
├── credentials.txt        ← Config Supabase
└── Kit-00/
    ├── 00.json           ← Preset local 0
    ├── 01.json           ← Preset local 1
    └── remote_preset_XXX.json  ← Presets descargados
```

---

## 🆚 Local vs Remote

### Local (presetsLite)
- Archivos JSON en `Kit-00/`
- Rápido, offline
- Solo en tu máquina

### Remote (Supabase)
- Base de datos PostgreSQL
- Accesible desde cualquier lugar
- Compartible (mismo user_id)
- Requiere conexión

---

## 💡 Tips

### ✅ DO:
- Guarda localmente PRIMERO, luego "Send to Remote"
- Usa "Refresh List" para ver presets de otros
- Usa "Clear Database" para empezar limpio en tests

### ❌ DON'T:
- No actives "Auto Sync" si guardas mucho seguido
- No uses "Send to Remote" sin guardar local antes
- No borres `credentials.txt` (lo necesitas siempre)

---

## 🐛 Problemas Comunes

### "Preset file not found"
```
[error] Preset file not found: Kit-00/01.json
[error] Save preset locally first!
```
**Solución:** Guarda el preset con presetsLite antes de enviar a remoto

### "Selected: none"
```
[warning] No remote preset selected
```
**Solución:** Usa **[Next ▶]** para seleccionar un preset de la lista

### Lista vacía
```
Remote Presets (0):
(empty)
```
**Solución:** No hay presets remotos aún. Usa "Send to Remote" primero.

---

## 🎨 Ejemplo Completo

```cpp
// ofApp.h
SurfingPresetsLiteOfxGui presetsManager;
ofxSurfingSupabase db;

// ofApp.cpp
void ofApp::setup() {
    presetsManager.setup(scene.params);
    db.setup();
    db.syncWithPresetsManager(presetsManager);
}

void ofApp::draw() {
    presetsManager.drawGui();
    db.draw();
}
```

**Workflow:**
1. Run app
2. Ajusta sliders de escena
3. Presiona 'S' (save en presetsLite) → Guarda en `00.json`
4. Click "Send to Remote" → Sube a Supabase como `preset_000`
5. Click "Refresh List" en otro usuario → Ve tu preset
6. Click "Load Selected" → Descarga a local
7. Usa presetsLite para aplicarlo

---

✅ **¡Listo para usar!**

