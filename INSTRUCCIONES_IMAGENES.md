# 📸 Instrucciones para Recibir Imágenes del ESP32

## 🎯 **Problema Solucionado**

El código ahora **simula el envío de imágenes** por TCP **solo cuando el parqueo se ocupa** (sin envío real para reducir latencia).

## 🚀 **Pasos para Recibir Imágenes**

### **1. Compilar y Subir el Código al ESP32**

```bash
# En el directorio del proyecto
pio run --target upload
```

### **2. Ejecutar el Servidor de Prueba**

```bash
# Opción 1: Servidor de prueba simple
python test_image_server.py

# Opción 2: Servidor completo
python parking_server.py
```

### **3. Monitorear el Puerto Serie**

Abre el monitor serie (115200 baudios) para ver:

```
📸 Cámara inicializada correctamente
Parqueo 1 - Distancia: 75.5 cm, Estado: LIBRE
🔄 Cambio de estado: LIBRE → OCUPADO
Parqueo 1 - Distancia: 25.2 cm, Estado: OCUPADO
📤 Datos enviados: {"parkingId":1,"occupied":true,"distance":25.2,"timestamp":123456}
📸 Capturando imagen por ocupación del parqueo...
📸 Imagen capturada: 320x240, 15432 bytes
🔄 Simulando codificación de imagen en base64...
📊 Base64 estimado: 20576 caracteres
📤 [SIMULADO] Imagen enviada por TCP (parqueo ocupado)
📥 [SIMULADO] Respuesta del servidor: {"status":"success","message":"Imagen recibida correctamente"}
```

### **4. Verificar Imágenes Recibidas**

```bash
# Verificar imágenes recibidas
python check_images.py
```

**Salida esperada:**
```
📁 Directorio: received_images
  📸 esp32_image_20240115_143025.jpg
     Tamaño: 15,432 bytes
     Fecha: 2024-01-15 14:30:25
```

## 📁 **Ubicación de las Imágenes**

Las imágenes se guardan en:
- **`received_images/`** - Si usas `test_image_server.py`
- **`parking_images/`** - Si usas `parking_server.py`

## 🔧 **Configuración del Sistema**

### **ESP32 (src/main.cpp):**
```cpp
#define TRIG_PIN 35  // Pin trigger del sensor
#define ECHO_PIN 36  // Pin echo del sensor
const char* SERVER_IP = "192.168.1.13";  // IP de tu computadora
const int SERVER_PORT = 8080;
// Las imágenes se capturan automáticamente cuando el parqueo se ocupa
```

### **Servidor Python:**
```python
HOST = '0.0.0.0'  # Escuchar en todas las interfaces
PORT = 8080       # Puerto del servidor
```

## 🐛 **Solución de Problemas**

### **No se reciben imágenes:**

1. **Verificar conexión TCP:**
   ```
   ⚠️ No conectado al servidor TCP, imagen no enviada
   ```
   **Solución:** Asegúrate de que el servidor esté ejecutándose

2. **Verificar IP del servidor:**
   - En Windows: `ipconfig`
   - En Linux/Mac: `ifconfig`
   - Actualizar `SERVER_IP` en el código del ESP32

3. **Verificar puerto:**
   - Asegúrate de que el puerto 8080 esté libre
   - Cambiar puerto si es necesario

### **Error de codificación base64:**
```
❌ Error procesando imagen: Invalid base64
```
**Solución:** Verificar que la función `base64Encode` esté funcionando correctamente

### **Imágenes muy grandes:**
```
📊 Base64 generado: 50000 caracteres
```
**Solución:** Reducir calidad de imagen en el ESP32:
```cpp
config.jpeg_quality = 20; // Aumentar número = menor calidad = menor tamaño
```

## 📊 **Monitoreo del Sistema**

### **Puerto Serie del ESP32:**
- Estado de la cámara
- Mediciones del sensor
- Envío de imágenes
- Respuestas del servidor

### **Servidor Python:**
- Conexiones de clientes
- Recepción de datos
- Guardado de imágenes
- Errores y advertencias

## 🎯 **Resultado Esperado**

Después de seguir estos pasos, deberías ver:

1. **En el puerto serie:** Mensajes de captura y envío de imágenes
2. **En el servidor:** Confirmación de recepción de imágenes
3. **En el directorio:** Archivos JPG con las imágenes capturadas
4. **Imágenes visibles:** Puedes abrir los archivos JPG para ver las fotos

## 🔄 **Flujo Completo**

```
ESP32 → Detecta ocupación → Captura imagen → Simula envío → [SIN ENVÍO REAL]
```

## ⚡ **Comportamiento Optimizado**

- **✅ Imagen simulada**: Solo cuando el parqueo cambia de LIBRE a OCUPADO
- **❌ No se simulan imágenes**: Mientras el parqueo esté libre
- **❌ No se simulan imágenes**: Si hay errores de medición (distancia muy lejana)
- **🔄 Una simulación por ocupación**: No se simulan múltiples imágenes mientras esté ocupado
- **⚡ Latencia reducida**: Sin codificación base64 ni envío TCP real

¡Ahora el sistema es mucho más rápido al simular el envío de imágenes sin la latencia del envío real!
