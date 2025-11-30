# Compile and Test - ofxSurfingSupabase v0.9.0-beta

**¡Todo listo para compilar y probar!**

---

## ✅ Pre-requisitos Completados

- [x] cpp-httplib descargado (417 KB)
- [x] addon_config.mk configurado
- [x] Código fuente completo (892 líneas)
- [x] credentials.txt preparado por ti
- [x] Example app listo

---

## 🚀 Pasos para Compilar y Probar

### 1. Verificar credentials.txt

```bash
cd example/bin/data
cat credentials.txt
```

Debe contener:
```
AUTH_MODE=EMAIL_PASSWORD
SUPABASE_URL=https://xxxxx.supabase.co
SUPABASE_ANON_KEY=eyJhbGc...
EMAIL=tu@email.com
PASSWORD=tupassword
```

---

### 2. Compilar (macOS)

```bash
cd example
make clean
make
```

**Salida esperada:**
```
checking pkg-config libraries:   gtk+-3.0  
...
compiling ofxSurfingSupabase
compiling example
linking
```

**Si hay error de OpenSSL:**
```bash
# macOS ya tiene OpenSSL, pero si falla:
brew install openssl
```

---

### 3. Ejecutar

```bash
./bin/example
```

O desde Xcode:
```bash
open example.xcodeproj
# Build & Run (⌘R)
```

---

### 4. Verificar Salida en Consola

**Debe mostrar:**
```
[notice] ofApp: setup()
[notice] ofxSurfingSupabase: setup()
[notice] ofxSurfingSupabase: setupParameters()
[notice] ofxSurfingSupabase: setupCallbacks()
[notice] ofxSurfingSupabase: setupGui()
[notice] ofxSurfingSupabase: startup()
[notice] ofxSurfingSupabase: loadCredentials()
[notice] ofxSurfingSupabase: Auth mode: EMAIL_PASSWORD
[notice] ofxSurfingSupabase: URL: https://xxxxx.supabase.co
[notice] ofxSurfingSupabase: authenticate()
[notice] ofxSurfingSupabase: Authenticated successfully
[notice] ofxSurfingSupabase: User ID: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
[notice] ofxSurfingSupabase: refreshPresetList()
[notice] ofxSurfingSupabase: ✓ Found 0 presets
[notice] ofApp: Setup complete
```

**GUI debe mostrar:**
```
Status: 🟢 CONNECTED
```

---

### 5. Probar Funcionalidad Básica

#### Test 1: Guardar Preset
```
1. Ajusta parámetros de la escena (colores, amount, scale)
2. Presiona ESPACIO
3. Consola debe mostrar:
   [notice] ofxSurfingSupabase: sendSceneDirect()
   [notice] ofxSurfingSupabase: savePreset(): scene_20251130_HHMMSS
   [notice] ofxSurfingSupabase: ✓ Preset saved successfully
   [notice] ofxSurfingSupabase: ✓ Found 1 presets
```

#### Test 2: Verificar en Supabase Dashboard
```
1. Abre https://supabase.com/dashboard
2. Ve a tu proyecto
3. Table Editor → presets
4. Debes ver tu preset guardado con:
   - user_id: tu ID
   - preset_name: scene_YYYYMMDD_HHMMSS
   - preset_data: JSON con tus parámetros
```

#### Test 3: Cargar Preset
```
1. Cambia los parámetros de la escena
2. Presiona L
3. La escena debe volver al estado guardado
4. Consola debe mostrar:
   [notice] ofxSurfingSupabase: loadAndApplyRemote()
   [notice] ofxSurfingSupabase: loadPreset(): scene_20251130_HHMMSS
   [notice] ofxSurfingSupabase: ✓ Preset loaded and applied
```

#### Test 4: Navegar Presets
```
1. Guarda varios presets (ESPACIO múltiples veces)
2. Usa FLECHAS IZQUIERDA/DERECHA para navegar
3. GUI debe mostrar:
   Selected: scene_20251130_120000 (1/3)
   Selected: scene_20251130_121500 (2/3)
   etc.
```

#### Test 5: Borrar Preset
```
1. Selecciona un preset (flechas)
2. Click en "Delete Selected"
3. Consola debe mostrar:
   [notice] ofxSurfingSupabase: ✓ Preset deleted successfully
```

---

## 🎯 Tests Completos

Para tests exhaustivos, sigue: **docs/TESTING-GUIDE.md**

---

## 🐛 Troubleshooting

### Error: "Authentication failed"
```
Verifica credentials.txt:
- URL correcta (con https://)
- Anon key correcta
- Email/password correctos
- Usuario creado y confirmado en Supabase
```

### Error: "No such file httplib.h"
```
Verifica:
ls -lh libs/cpp-httplib/include/httplib.h

Debe existir (417 KB)
Si no existe:
cd libs/cpp-httplib/include
curl -L -o httplib.h https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h
```

### Error: SSL/OpenSSL linking
```bash
# macOS
brew install openssl

# Verify:
which openssl
openssl version
```

### Error: "Preset not found"
```
- Base de datos vacía (guarda primero con ESPACIO)
- Verifica en Supabase Dashboard que hay datos
- Presiona R para refrescar lista
```

### GUI muestra "🔴 DISCONNECTED"
```
- Verifica credentials.txt
- Check consola para error específico
- Verifica conexión a internet
- Verifica que Supabase proyecto está activo
```

---

## 📊 Esperado vs Real

### ✅ Si Todo Funciona:

**Consola:**
- Authenticated successfully ✓
- Preset saved successfully ✓
- Preset loaded and applied ✓
- Found N presets ✓

**GUI:**
- Status: 🟢 CONNECTED
- Selected: scene_YYYYMMDD_HHMMSS (1/N)

**Supabase Dashboard:**
- Tabla `presets` con tus datos
- JSON válido en `preset_data`
- Timestamps correctos

**Visual:**
- Escena se dibuja correctamente
- Parámetros se aplican al cargar
- UI responde a clicks

### ❌ Si Algo Falla:

1. Copia toda la salida de consola
2. Copia mensajes de error
3. Screenshot del GUI
4. Describe qué esperabas vs qué pasó

---

## 🎉 ¡Éxito!

Si todos los tests pasan:
```
✅ Autenticación funciona
✅ Guardar presets funciona
✅ Cargar presets funciona
✅ Navegación funciona
✅ Borrar funciona

🎊 ¡El addon está completamente funcional!
```

**Next:**
- Integrar en tus propios proyectos
- Añadir tus propios parámetros
- Personalizar UI
- (Opcional) Implementar multithreading

---

## 📚 Recursos

- **docs/TESTING-GUIDE.md** - Tests exhaustivos
- **docs/CHANGELOG.md** - Historial de versiones
- **README.md** - Documentación completa
- **example/README.md** - Guía del ejemplo

---

**¡Mucha suerte con las pruebas!**

Si encuentras problemas, revisa:
1. Consola (mensajes de error)
2. credentials.txt (configuración)
3. Supabase Dashboard (datos)
4. docs/TESTING-GUIDE.md (casos específicos)
