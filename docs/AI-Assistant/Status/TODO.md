#### file: TODO.md

# OVERVIEW

- addon para oF llamado ofxSurfingSupabase y su ofApp /example. 
- quiero usar Supabase como backend para guardar y recuperar datos en json.
- concretamente quiero poder guardar y recuperar configuraciones de ofParameter dentro de ofParameterGroups de oF.
- guardo configuraciones/presets de un ofParameterGroup en archivos json en OF, que de-serializo para aplicar en un ofParameterGroup.
- los genero serializando un ofParameterGroup a json.
- quiero hacer lo mismo pero usando Supabase como almacenamiento remoto. quizas manteniendo un mirror de todos los presets del usuario loggeado en supabase en local (archivos json) y remoto (Supabase). podria aver un toggle para activar cada modo: asi a veces usa local y a veces remoto. o los dos. boton void con trigger para sincronizar local -> remoto o remoto -> local.
- quiero usar la base de datos de Supabase para guardar los jsons.
- quiero una interfaz de usuario simple para gestionar los presets (guardar, cargar, borrar, listar)
- por ahora el sistema de login lo quiero manejar hardcoded con un archivo de texto secreto en local bin/data/credentials.txt que contenga el email y password del usuario de Supabase o un api token key. (la mejor manera, piensa que soy nuevo en en supabase.) debes revisar la doc de supabase para ver como autenticar usuarios y manejar sesiones y ayudarme a configurar supabase por el navegador chrome. quizas estas configuraciones se pueden hacer desde mi app OF usando la api de supabase?
    mas adelante cuando integrase un ui como dear imgui, me gustaria implementar un sistema log con usurio de google, por ejemplo.
- quiero que sea bien modular para poder usarlo en otros proyectos de oF facilmente y tambien para mejorar funcionalidades en el futuro. por ejemplo otros usos que no sea guardar arcivos json, sino ddbb de usuarios con sus propios datos. browsear filtrando por usuarios, fechas u otros criterios.
- quizas podrias estudiar de usar una lib cpp para realiazar las conexiones http a supabase, como cpp-httplib o similar si lo consideras mas isolado y portable que usar classes de OF core. cprefiero no usar ofxAddons de http que puedan estar desactualizados o no mantenidos.
- mas adelante integraremos algun sistema multithread para no bloquear el hilo principal de oF al hacer las conexiones http a supabase. podemos ir enfocandolo para que sea facil de adaptar a multithread en el futuro.
- en la ui deberia haber botones ofParameter void (ofGui) con sus callbackks lambda para borrar el kit, borrar la databadse remota, sincronizar local -> remoto, remoto -> local, etc.
- implementa buenos logs de debug en consola para saber que esta pasando en cada momento.
- implemente el /example funcional que sera el que probara el uso bassico del addon ofxSurfingSupabase que estamos haciendo.

## IMPORTANTE:

- el addon que usamos como gestor de presets con archivos json varios en un kit es ofxSurfingPresetsLite. Debes revisar su codigo y entender como serializa y deserializa los ofParameterGroups a json y viceversa. Tenemos funciones para de/serializar ofPArameterGroups a json strings y viceversa en src/ofxSurfingSerializers.h que es usado por ofxSurfingPresetsLite.
- hay ya varias clases pre planteadas en ofxSurfingSupabase para gestionar la conexion a supabase, autenticacion, gestion de la base de datos, etc. Debes revisar su codigo y entender su estructura y funcionalidad. despues las modificaras para las nuevas ideas.

# DO NOW LIST

- [ ] Compilar y testear el example
- [ ] Agregar mouse interaction al SupabasePresetManager UI (click to select/load)
- [ ] Implementar dialogs modales para save/rename/delete con ofxGui
- [ ] Agregar text input para nombres de presets
- [ ] Implementar toggle local/remoto mode
- [ ] Botones void para sync operations (local->remoto, remoto->local)
- [ ] Testing con datos reales de Supabase

# DONE (CHANGELOG)

## Session 2025-11-26 01:35

### ✅ Dual Authentication System
- Implementado soporte para API Key mode (simple)
- Implementado soporte para Email/Password mode (recommended)
- Auto-detección del modo en SupabaseConfig
- Authentication flow con Supabase Auth (/auth/v1/token)
- Headers dinámicos según auth mode
- SupabaseClient::authenticateAsync() para email/password

### ✅ SupabasePresetManager - UI completa
- Nueva clase SupabasePresetManager.h/cpp para gestión manual de presets
- GUI con lista de presets renderizada
- Botones: Load, Save, Delete, Refresh
- Loading indicator visual
- Event system completo:
  - onPresetLoaded
  - onPresetSaved
  - onPresetDeleted
  - onError
- Métodos públicos:
  - refreshPresetList() - Actualiza lista desde Supabase
  - loadPreset(name) - Carga preset por nombre
  - savePreset(name, data) - Guarda/actualiza preset (upsert)
  - deletePreset(name) - Borra preset
  - renamePreset(old, new) - Renombra preset
- Estructura PresetInfo con metadata (id, name, data, timestamps)

### ✅ Mejoras en SupabaseClient
- Cambiado setup() a recibir SupabaseConfig& (non-const) para poder modificar
- Método authenticateAsync() implementado
- isAuthenticated() status check
- getUserId() getter
- Mejor manejo de headers según auth mode en getHeaders()
- Access token support en headers

### ✅ Mejoras en ofxSurfingSupabase
- Integración de SupabasePresetManager
- Nuevo parámetro bShowPresetManager para toggle UI
- getPresetManager() para acceso directo
- Mejor info de status en draw() (auth mode, connection, sync)
- Setup de presetManager en setup()
- isConnected() ahora verifica client.isAuthenticated()

### ✅ Mejoras en SupabaseConfig
- AuthMode enum (API_KEY, EMAIL_PASSWORD)
- Campos email, password, accessToken
- authMode detection automático
- needsAuthentication() método
- getAuthUrl() para auth endpoints
- Mejor validación según modo

### ✅ Documentación
- Actualizado IMPLEMENTATION.md con nuevas features
- Sección completa de Dual Authentication
- Documentación de SupabasePresetManager
- Ejemplos de uso actualizados
- Actualizado credentials.txt template con dual auth options

### ✅ Code Quality
- Fixed listener syntax en SupabasePresetSync (addListener en vez de newListener)
- Mejor separación de responsabilidades:
  - SupabaseClient → HTTP + Auth
  - SupabasePresetSync → Auto-sync con presetsManager
  - SupabasePresetManager → UI + manual operations
- Event-driven architecture
- Modern C++ patterns (lambdas, auto, structured bindings)

## Session anterior (fecha desconocida)

### ✅ Core Implementation
- SupabaseConfig.h - credentials loader
- SupabaseClient.h/cpp - REST API client con ofURLFileLoader
- SupabasePresetSync.h/cpp - auto-sync system
- ofxSurfingSupabase.h/cpp - public API
- Example app integration basic
- IMPLEMENTATION.md documentation inicial
- Workaround PATCH → POST + X-HTTP-Method-Override

# IGNORE (MY NOTES)

- OAuth Google → Pendiente para futuro con Dear ImGui
- Conflict resolution → No prioritario ahora
- Offline queue → Nice to have
- Preset versioning → Futuro
- cpp-httplib → Por ahora usando ofURLFileLoader (funciona bien)
- Multithread → Enfocado para futuro (async callbacks ya implementados)
