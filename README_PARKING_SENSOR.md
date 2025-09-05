# ESP32 Parking Sensor + Camera System

Sistema completo de detección de parqueo usando ESP32-S3-CAM con sensor ultrasónico HC-SR04 y cámara integrada.

## Características

- **Sensor ultrasónico HC-SR04**: Detecta la distancia hasta objetos
- **Cámara integrada**: Captura imágenes del parqueo
- **Detección de ocupación**: Si la distancia es menor a 50cm, el parqueo está ocupado
- **Comunicación TCP**: Envía datos JSON e imágenes al servidor
- **Reconexión automática**: WiFi y TCP se reconectan automáticamente
- **Monitoreo en tiempo real**: Actualiza el estado cada segundo
- **Captura de imágenes**: Simula envío de imagen solo cuando el parqueo se ocupa (sin envío real para reducir latencia)

## Hardware Requerido

- **ESP32-S3-CAM** (con cámara integrada)
- **Sensor ultrasónico HC-SR04**
- Cables de conexión
- Fuente de alimentación 5V

## Conexiones

### Sensor HC-SR04
- **VCC** → 5V del ESP32
- **GND** → GND del ESP32
- **Trig** → GPIO 35 del ESP32
- **Echo** → GPIO 36 del ESP32

### Cámara
- **Integrada** en el ESP32-S3-CAM
- **No requiere conexiones adicionales**

## Configuración

### 1. Configurar WiFi
Edita las credenciales en `src/main.cpp`:
```cpp
const char* ssid = "TU_WIFI_SSID";
const char* password = "TU_WIFI_PASSWORD";
```

### 2. Configurar Servidor TCP
Edita la configuración del servidor en `src/main.cpp`:
```cpp
const char* SERVER_IP = "192.168.1.100";  // IP del servidor
const int SERVER_PORT = 8080;              // Puerto del servidor
```

### 3. Configurar ID de Parqueo
```cpp
#define PARKING_ID 1  // ID único del parqueo
```

### 4. Configurar Pines (si es necesario)
```cpp
#define TRIG_PIN 35  // Pin de trigger
#define ECHO_PIN 36  // Pin de echo
```

### 5. Configurar Captura de Imágenes
```cpp
// Las imágenes se capturan automáticamente cuando el parqueo se ocupa
// No se requiere configuración adicional
```

## Formato de Datos

El sistema envía dos tipos de datos por TCP:

### 1. Datos del Sensor (JSON)
```json
{
  "parkingId": 1,
  "occupied": true,
  "distance": 25.5,
  "timestamp": 1234567890
}
```

**Campos:**
- **parkingId**: ID único del parqueo
- **occupied**: `true` si está ocupado, `false` si está libre
- **distance**: Distancia medida en centímetros
- **timestamp**: Tiempo en milisegundos desde el inicio

### 2. Imágenes (Base64)
```
IMAGE:base64_encoded_image_data
```

**Formato:**
- **Prefijo**: `IMAGE:`
- **Datos**: Imagen codificada en base64
- **Resolución**: 320x240 (QVGA)
- **Formato**: JPEG

## Lógica de Detección

- **Distancia ≥ 50cm**: Parqueo LIBRE
- **Distancia < 50cm**: Parqueo OCUPADO
- **Rango válido**: 2cm - 400cm (límites del HC-SR04)

## Uso

1. **Compilar y subir** el código al ESP32
2. **Conectar** el sensor HC-SR04 según el diagrama
3. **Configurar** WiFi y servidor TCP
4. **Monitorear** el puerto serie para ver el estado
5. **Recibir datos** en el servidor TCP

## Monitoreo

### Puerto Serie (115200 baudios)
```
🚗 ESP32 Parking Sensor System v1.0
=====================================
=== INFORMACIÓN DEL SISTEMA ===
ESP32 Parking Sensor + Camera v1.0
ID de parqueo: 1
Pines sensor: Trig=35, Echo=36
Servidor TCP: 192.168.1.100:8080
Cámara: Inicializada
Memoria libre: 123456 bytes
Uptime: 123 segundos
===============================

✅ WiFi conectado exitosamente!
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

### Estado del Sistema (cada 30 segundos)
```
=== ESTADO DEL SENSOR DE PARQUEO ===
ID: 1
Distancia: 45.2 cm
Estado: OCUPADO
Umbral: 50.0 cm
TCP: Conectado
Servidor: 192.168.1.100:8080
=====================================
```

## Solución de Problemas

### WiFi no conecta
- Verificar SSID y password
- Verificar que la red esté disponible
- Verificar señal WiFi

### TCP no conecta
- Verificar IP y puerto del servidor
- Verificar que el servidor esté ejecutándose
- Verificar firewall

### Sensor no responde
- Verificar conexiones del HC-SR04
- Verificar alimentación 5V
- Verificar pines Trig y Echo

### Mediciones incorrectas
- Verificar que no haya obstáculos cerca
- Verificar que el sensor esté bien posicionado
- Verificar conexiones

## Personalización

### Cambiar distancia umbral
```cpp
parkingSensor.setThresholdDistance(30.0); // 30cm en lugar de 50cm
```

### Cambiar intervalo de medición
Modifica en `ParkingSensor.cpp`:
```cpp
this->measurementInterval = 2000; // 2 segundos en lugar de 1
```

### Cambiar intervalo de reconexión TCP
```cpp
this->tcpReconnectInterval = 10000; // 10 segundos en lugar de 5
```

## Estructura del Proyecto

```
lib/
├── ParkingSensor/
│   ├── ParkingSensor.h      # Definición de la clase
│   └── ParkingSensor.cpp    # Implementación
└── ESP32Monitor/            # (No usado en este proyecto)
src/
└── main.cpp                 # Código principal
```

## Dependencias

- Arduino Core para ESP32
- WiFi (incluido en Arduino Core)
- No se requieren librerías adicionales

## Licencia

Este proyecto es de código abierto y está disponible bajo la licencia MIT.
