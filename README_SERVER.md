# Servidor Python para Sensor de Parqueo ESP32

Servidor TCP que recibe datos del sensor de parqueo ESP32 y opcionalmente imágenes.

## Características

- **Recepción TCP**: Recibe datos JSON del sensor de parqueo
- **Guardado de imágenes**: Guarda imágenes enviadas por el ESP32
- **Logging**: Guarda datos del sensor en archivo de log
- **Multi-cliente**: Maneja múltiples sensores simultáneamente
- **Comandos**: Responde a comandos del ESP32

## Instalación

### 1. Instalar Python
Asegúrate de tener Python 3.7 o superior instalado.

### 2. Instalar dependencias (opcional)
```bash
pip install -r requirements.txt
```

Las dependencias son opcionales:
- `Pillow`: Para crear imágenes de prueba
- `pytest`: Para testing

## Uso

### 1. Iniciar el servidor
```bash
python parking_server.py
```

El servidor se iniciará en `0.0.0.0:8080` por defecto.

### 2. Configurar el ESP32
Asegúrate de que el ESP32 esté configurado con:
- **IP del servidor**: La IP de tu computadora
- **Puerto**: 8080 (por defecto)

### 3. Monitorear datos
El servidor mostrará en consola:
```
🚗 Servidor de Parqueo ESP32 iniciado
📍 Escuchando en 0.0.0.0:8080
📁 Imágenes se guardarán en: /ruta/completa/parking_images
==================================================

🔌 Cliente conectado: ('192.168.1.100', 12345)

📊 DATOS DEL SENSOR - 2024-01-15 14:30:25
   Cliente: ('192.168.1.100', 12345)
   ID Parqueo: 1
   Estado: 🔴 OCUPADO
   Distancia: 25.5 cm
   Timestamp: 1705327825000
----------------------------------------
```

## Estructura de Archivos

```
proyecto/
├── parking_server.py      # Servidor principal
├── test_client.py         # Cliente de prueba
├── image_sender.py        # Enviador de imágenes
├── requirements.txt       # Dependencias
├── README_SERVER.md       # Este archivo
├── parking_images/        # Directorio de imágenes (creado automáticamente)
└── parking_sensor.log     # Log de datos (creado automáticamente)
```

## Formato de Datos

### Datos del Sensor (JSON)
```json
{
  "parkingId": 1,
  "occupied": true,
  "distance": 25.5,
  "timestamp": 1705327825000
}
```

### Comandos Soportados
- `COMMAND:STATUS` - Obtener estado del servidor
- `COMMAND:PING` - Ping al servidor

### Imágenes
- Formato: `IMAGE:base64_data`
- Se guardan como JPG en el directorio `parking_images/`
- Nombre: `parking_YYYYMMDD_HHMMSS_IP.jpg`

## Testing

### 1. Cliente de Prueba
```bash
python test_client.py
```

Opciones:
- **Simular escenarios**: Simula diferentes estados de parqueo
- **Simulación continua**: Envía datos aleatorios continuamente

### 2. Enviar Imágenes
```bash
python image_sender.py
```

Opciones:
- **Crear imagen de prueba**: Genera y envía una imagen de prueba
- **Enviar imagen existente**: Envía una imagen desde archivo

## Configuración

### Cambiar Puerto
Edita en `parking_server.py`:
```python
PORT = 8080  # Cambiar por el puerto deseado
```

### Cambiar Directorio de Imágenes
Edita en `parking_server.py`:
```python
self.images_dir = "mi_directorio_imagenes"
```

### Cambiar Host
Edita en `parking_server.py`:
```python
HOST = '127.0.0.1'  # Solo local
# o
HOST = '0.0.0.0'    # Todas las interfaces (por defecto)
```

## Logs

### Consola
- Conexiones de clientes
- Datos recibidos del sensor
- Errores y advertencias

### Archivo de Log
- Archivo: `parking_sensor.log`
- Formato: `timestamp | client_address | json_data`
- Ejemplo:
```
2024-01-15 14:30:25 | ('192.168.1.100', 12345) | {"parkingId":1,"occupied":true,"distance":25.5,"timestamp":1705327825000}
```

## Solución de Problemas

### Puerto en uso
```
❌ Error iniciando servidor: [Errno 98] Address already in use
```
**Solución**: Cambiar el puerto o cerrar la aplicación que lo usa.

### Cliente no conecta
```
❌ Error conectando: [Errno 111] Connection refused
```
**Solución**: 
- Verificar que el servidor esté ejecutándose
- Verificar IP y puerto
- Verificar firewall

### Imagen no se guarda
```
❌ Error procesando imagen: Invalid base64
```
**Solución**: Verificar que los datos de imagen estén en formato base64 válido.

## Integración con ESP32

### Configuración en ESP32
En `src/main.cpp`:
```cpp
const char* SERVER_IP = "192.168.1.100";  // IP de tu computadora
const int SERVER_PORT = 8080;              // Puerto del servidor
```

### Enviar Imágenes desde ESP32
Para enviar imágenes, modifica el ESP32 para enviar:
```cpp
// Convertir imagen a base64 y enviar
String imageBase64 = base64Encode(imageData);
String message = "IMAGE:" + imageBase64;
tcpClient.println(message);
```

## Desarrollo

### Agregar Nuevos Comandos
En `parking_server.py`, método `handle_command()`:
```python
elif command == "MI_COMANDO":
    response = json.dumps({"status": "ok", "data": "respuesta"})
    client_socket.send(response.encode('utf-8'))
```

### Agregar Nuevos Tipos de Datos
En `parking_server.py`, método `process_non_json_data()`:
```python
elif data.startswith("MI_TIPO:"):
    self.handle_mi_tipo_data(data, client_socket, client_address)
```

## Licencia

Este proyecto es de código abierto y está disponible bajo la licencia MIT.
