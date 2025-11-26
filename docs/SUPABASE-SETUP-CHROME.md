# Configuración de Supabase - Guía Paso a Paso (Chrome)

## 📋 Requisitos previos
- Cuenta de Supabase (gratuita)
- Browser Chrome
- ofxSurfingSupabase compilado

---

## 🚀 Paso 1: Crear Proyecto en Supabase

1. **Ir a** https://supabase.com
2. **Click** en "Start your project" o "Sign In"
3. **Login** con GitHub, Google, etc.
4. **Dashboard** → Click "New project"
5. **Rellenar:**
   - Name: `ofxSurfingTest` (o el nombre que quieras)
   - Database Password: (genera una fuerte o usa la sugerida)
   - Region: Elige la más cercana
   - Pricing Plan: **Free**
6. **Click** "Create new project"
7. **Esperar** ~2 minutos mientras se crea

---

## 🗄️ Paso 2: Crear Tabla `presets`

### 2.1 Abrir SQL Editor
1. **Sidebar izquierda** → Click "SQL Editor"
2. **Click** "New query"

### 2.2 Pegar este SQL:

```sql
-- Crear tabla presets
CREATE TABLE presets (
  id uuid DEFAULT gen_random_uuid() PRIMARY KEY,
  user_id text NOT NULL,
  preset_name text NOT NULL,
  preset_data jsonb NOT NULL,
  created_at timestamptz DEFAULT now(),
  updated_at timestamptz DEFAULT now(),
  CONSTRAINT unique_user_preset UNIQUE(user_id, preset_name)
);

-- Indices para performance
CREATE INDEX idx_presets_user ON presets(user_id);
CREATE INDEX idx_presets_name ON presets(preset_name);

-- Activar Row Level Security
ALTER TABLE presets ENABLE ROW LEVEL SECURITY;

-- Politicas de seguridad (para email/password auth)
CREATE POLICY "Users see own presets"
  ON presets FOR SELECT
  USING (user_id = auth.uid()::text);

CREATE POLICY "Users insert own presets"
  ON presets FOR INSERT
  WITH CHECK (user_id = auth.uid()::text);

CREATE POLICY "Users update own presets"
  ON presets FOR UPDATE
  USING (user_id = auth.uid()::text);

CREATE POLICY "Users delete own presets"
  ON presets FOR DELETE
  USING (user_id = auth.uid()::text);
```

### 2.3 Ejecutar
1. **Click** botón "Run" (esquina inferior derecha)
2. **Verificar** mensaje "Success" verde

---

## 👤 Paso 3: Configurar Autenticación

### Opción A: Email/Password (Recomendado)

#### 3.1 Habilitar Email Auth
1. **Sidebar** → "Authentication"
2. **Click** tab "Providers"
3. **Buscar** "Email"
4. **Toggle** ON si no está activado
5. **Settings:**
   - ✅ Enable Email provider
   - ✅ **IMPORTANTE:** Desactivar "Confirm email" (para testing)
     - Click en "Email" → Settings
     - **Desmarcar** "Enable email confirmations"
     - Save
6. **Click** "Save"

#### 3.2 Crear Usuario de Prueba
1. **Sidebar** → "Authentication"
2. **Click** tab "Users"
3. **Click** "Add user" → "Create new user"
4. **Rellenar:**
   - Email: `test@ofxsurfing.com` (o tu email)
   - Password: `testpass123` (o tu password)
   - ✅ **Auto Confirm User** (importante!)
5. **Click** "Create user"
6. **Copiar** el UUID que aparece (lo usarás después)

### Opción B: API Key (Simple, menos seguro)

1. **Sidebar** → "Authentication" → "Users"
2. **Click** "Add user" → "Create new user"
3. **Copiar** el UUID del usuario creado

---

## 🔑 Paso 4: Obtener Credenciales

### 4.1 Project URL
1. **Sidebar** → "Settings" (icono engranaje abajo)
2. **Click** "API"
3. **Copiar** "Project URL"
   - Ejemplo: `https://abcdefgh.supabase.co`

### 4.2 API Key
1. **Misma pantalla** "Settings" → "API"
2. **Buscar** "Project API keys"
3. **Copiar** el `anon public` key
   - Es un JWT muy largo que empieza con `eyJhbGc...`
   - **NO** uses el `service_role` key (muy peligroso)

### 4.3 User ID (solo para API Key mode)
1. **Sidebar** → "Authentication" → "Users"
2. **Copiar** el UUID del usuario (ej: `a1b2c3d4-...`)

---

## 📝 Paso 5: Configurar `credentials.txt`

### Opción A: Email/Password (Recomendado)

Edita `example/bin/data/credentials.txt`:

```txt
project_url=https://abcdefgh.supabase.co
api_key=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.tu_key_anon_aqui
email=test@ofxsurfing.com
password=testpass123
```

### Opción B: API Key

```txt
project_url=https://abcdefgh.supabase.co
api_key=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.tu_key_anon_aqui
user_id=a1b2c3d4-5678-90ab-cdef-1234567890ab
```

---

## ✅ Paso 6: Verificar Todo

### 6.1 En Supabase Dashboard
1. **Sidebar** → "Table Editor"
2. **Verificar** que existe tabla `presets` (vacía)
3. **Sidebar** → "Authentication" → "Users"
4. **Verificar** que existe tu usuario test

### 6.2 Compilar y Ejecutar ofApp
1. Compila el example
2. Run
3. **Console debe mostrar:**
   ```
   [notice ] ofxSurfingSupabase: Setup
   [notice ] SupabaseConfig: Auth mode: EMAIL_PASSWORD
   [notice ] SupabaseConfig: Loaded successfully
   [notice ] SupabaseClient: Email/password mode - authenticating...
   [notice ] SupabaseClient: Authenticated successfully, user: a1b2c3...
   [notice ] ofxSurfingSupabase: Setting up sync for user: a1b2c3...
   ```

### 6.3 En la App
Debes ver:
- **Status GUI:** Verde "CONNECTED"
- **Auth:** Email/Password
- **Preset Manager UI** con lista vacía

---

## 🎨 Monitoring de Estado (Color Coding)

La app muestra estados visuales:

### 🟢 Verde - "CONNECTED"
- Todo OK, autenticado correctamente
- Puedes usar preset manager

### 🟡 Amarillo - "AUTHENTICATING..."
- Esperando respuesta de Supabase Auth
- Espera unos segundos

### 🔴 Rojo - "ERROR - Check credentials.txt"
- Credenciales incorrectas o archivo no existe
- Revisa console logs para detalles

---

## 🧪 Paso 7: Probar Funcionalidad

### Test 1: Guardar Preset
1. En la app, cambia parámetros de la escena
2. Click botón **SAVE** en Preset Manager (o usa presetsManager)
3. **Verificar en Supabase:**
   - Dashboard → Table Editor → `presets`
   - Deberías ver una fila nueva con tu preset

### Test 2: Cargar Preset
1. En Supabase, edita el JSON del preset manualmente
2. En la app, click **REFRESH** 
3. Click en el preset de la lista
4. Click **LOAD**
5. Los parámetros deben cambiar

### Test 3: Borrar Preset
1. Selecciona preset de la lista
2. Click **DELETE**
3. Verificar que desaparece de Table Editor

---

## 🐛 Troubleshooting

### "Status: ERROR"
- ✅ Verifica `credentials.txt` existe en `bin/data/`
- ✅ Verifica project_url es correcto
- ✅ Verifica api_key es el `anon public` (no service_role)

### "Authentication failed: HTTP 400"
- ✅ Email/password incorrectos
- ✅ Usuario no existe en Authentication → Users
- ✅ Email confirmations debe estar desactivado

### "SELECT failed: 403"
- ✅ RLS policies no están creadas (ejecuta SQL del Paso 2)
- ✅ user_id no coincide con auth.uid()

### Tabla vacía pero salvé presets
- ✅ Verifica user_id del usuario autenticado
- ✅ Console logs debe mostrar "Preset saved: ..."
- ✅ Revisa errores HTTP en console

---

## 📚 Recursos

- **Supabase Docs:** https://supabase.com/docs
- **Dashboard:** https://app.supabase.com
- **API Reference:** https://supabase.com/docs/reference/javascript

---

✅ **¡Listo! Tu Supabase está configurado y funcionando con ofxSurfingSupabase.**
