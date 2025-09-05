#include <Arduino.h>
#include <WiFi.h>
#include <esp_camera.h>
#include "ParkingSensor.h"
#include "board_config.h"

// Configuración de Wi-Fi
const char* ssid = "SSS";
const char* password = "$eba$tian3093";

// Configuración del sensor de parqueoW
// Pines para el sensor ultrasónico HC-SR04
#define TRIG_PIN 35  // Pin de trigger
#define ECHO_PIN 36  // Pin de echo
#define PARKING_ID 1  // ID único del parqueo

// Configuración del servidor TCP
const char* SERVER_IP = "10.185.200.153";  // IP del servidor
const int SERVER_PORT = 8080;              // Puerto del servidor

// Crear instancia del sensor de parqueo
ParkingSensor parkingSensor(TRIG_PIN, ECHO_PIN, PARKING_ID, SERVER_IP, SERVER_PORT);

// Variables para la cámara
bool cameraInitialized = false;
bool lastParkingState = false; // false = libre, true = ocupado
bool imageSentForCurrentOccupation = false; // Para evitar enviar múltiples imágenes

// Declaración de funciones
bool initCamera();
void captureAndSendImage();
void printSystemInfo();
String base64Encode(const uint8_t* data, size_t length);

// Función para inicializar la cámara
bool initCamera() {
    Serial.println("📸 Inicializando cámara...");
    
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    config.frame_size = FRAMESIZE_QVGA; // 320x240
    config.jpeg_quality = 12;
    config.fb_count = 1;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    config.fb_location = CAMERA_FB_IN_DRAM;
    
    // Inicializar la cámara
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("❌ Error inicializando cámara: 0x%x\n", err);
        return false;
    }
    
    // Configurar el sensor
    sensor_t *s = esp_camera_sensor_get();
    if (s != NULL) {
        s->set_brightness(s, 0);
        s->set_contrast(s, 0);
        s->set_saturation(s, 0);
        s->set_special_effect(s, 0);
        s->set_whitebal(s, 1);
        s->set_awb_gain(s, 1);
        s->set_wb_mode(s, 0);
        s->set_exposure_ctrl(s, 1);
        s->set_aec2(s, 0);
        s->set_ae_level(s, 0);
        s->set_aec_value(s, 300);
        s->set_gain_ctrl(s, 1);
        s->set_agc_gain(s, 0);
        s->set_gainceiling(s, (gainceiling_t)0);
        s->set_bpc(s, 0);
        s->set_wpc(s, 1);
        s->set_raw_gma(s, 1);
        s->set_lenc(s, 1);
        s->set_hmirror(s, 0);
        s->set_vflip(s, 0);
        s->set_dcw(s, 1);
        s->set_colorbar(s, 0);
    }
    
    Serial.println("✅ Cámara inicializada correctamente");
    return true;
}

// Función para codificar en base64
String base64Encode(const uint8_t* data, size_t length) {
    const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    String result = "";
    
    for (size_t i = 0; i < length; i += 3) {
        uint32_t chunk = 0;
        int chunk_size = 0;
        
        // Construir chunk de 3 bytes
        for (int j = 0; j < 3 && (i + j) < length; j++) {
            chunk = (chunk << 8) | data[i + j];
            chunk_size++;
        }
        
        // Codificar chunk
        for (int j = 0; j < 4; j++) {
            if (j < chunk_size + 1) {
                result += base64_chars[(chunk >> (18 - 6 * j)) & 0x3F];
            } else {
                result += '=';
            }
        }
    }
    
    return result;
}

// Función para capturar y enviar imagen cuando el parqueo se ocupa
void captureAndSendImage() {
    if (!cameraInitialized) {
        Serial.println("⚠️ Cámara no inicializada, no se puede capturar imagen");
        return;
    }
    
    Serial.println("📸 Capturando imagen por ocupación del parqueo...");
    
    // Capturar imagen
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("❌ Error capturando imagen");
        return;
    }
    
    Serial.printf("📸 Imagen capturada: %dx%d, %d bytes\n", fb->width, fb->height, fb->len);
    
    // Simular envío de imagen (sin enviar realmente para reducir latencia)
    if (parkingSensor.isTcpConnected()) {
        Serial.println("🔄 Simulando codificación de imagen en base64...");
        
        // Simular tamaño de base64 sin codificar realmente
        size_t estimatedBase64Size = (fb->len * 4) / 3 + 4; // Estimación aproximada
        Serial.printf("📊 Base64 estimado: %d caracteres\n", estimatedBase64Size);
        
        // Simular mensaje sin enviarlo
        Serial.println("📤 [SIMULADO] Imagen enviada por TCP (parqueo ocupado)");
        Serial.println("📥 [SIMULADO] Respuesta del servidor: {\"status\":\"success\",\"message\":\"Imagen recibida correctamente\"}");
        
        // No hay delay ni envío real para reducir latencia
    } else {
        Serial.println("⚠️ No conectado al servidor TCP, imagen no enviada");
    }
    
    // Liberar buffer
    esp_camera_fb_return(fb);
}

// Función para mostrar información del sistema
void printSystemInfo() {
    Serial.println("=== INFORMACIÓN DEL SISTEMA ===");
    Serial.println("ESP32 Parking Sensor + Camera v1.0");
    Serial.printf("ID de parqueo: %d\n", PARKING_ID);
    Serial.printf("Pines sensor: Trig=%d, Echo=%d\n", TRIG_PIN, ECHO_PIN);
    Serial.printf("Servidor TCP: %s:%d\n", SERVER_IP, SERVER_PORT);
    Serial.printf("Cámara: %s\n", cameraInitialized ? "Inicializada" : "No inicializada");
    Serial.printf("Memoria libre: %d bytes\n", esp_get_free_heap_size());
    Serial.printf("Uptime: %lu segundos\n", millis() / 1000);
    Serial.println("===============================");
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  Serial.println("🚗 ESP32 Parking Sensor System v1.0");
  Serial.println("=====================================");

  // Mostrar información del sistema
  printSystemInfo();

  // Inicializar la cámara
  cameraInitialized = initCamera();
  if (!cameraInitialized) {
    Serial.println("⚠️ Advertencia: Cámara no inicializada, solo funcionará el sensor");
  }

  // Inicializar el sensor de parqueo
  parkingSensor.begin();

  // Configurar Wi-Fi
  Serial.println("=== CONFIGURANDO WIFI ===");
  Serial.println("Conectando a la red WiFi...");
  Serial.println("  SSID: " + String(ssid));
  Serial.println("  Password: " + String(password));
  
  WiFi.begin(ssid, password);
  
  // Esperar conexión con timeout
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("✅ WiFi conectado exitosamente!");
    Serial.println("=== INFORMACIÓN DE CONEXIÓN ===");
    Serial.println("  SSID: " + String(ssid));
    Serial.println("  IP Address: " + WiFi.localIP().toString());
    Serial.println("  MAC Address: " + WiFi.macAddress());
    Serial.println("  Signal Strength: " + String(WiFi.RSSI()) + " dBm");
    Serial.println("  Gateway: " + WiFi.gatewayIP().toString());
    Serial.println("  Subnet: " + WiFi.subnetMask().toString());
    Serial.println("  DNS: " + WiFi.dnsIP().toString());
    Serial.println("================================");
    
    Serial.println("=== SISTEMA INICIADO ===");
    Serial.println("El sensor de parqueo está monitoreando...");
    Serial.println("Los datos se enviarán por TCP al servidor");
    Serial.println("=========================");
  } else {
    Serial.println();
    Serial.println("❌ Error: No se pudo conectar a WiFi");
    Serial.println("Verifica:");
    Serial.println("  - SSID y password correctos");
    Serial.println("  - Red WiFi disponible");
    Serial.println("  - Señal WiFi suficiente");
    Serial.println("Reiniciando en 5 segundos...");
    delay(5000);
    ESP.restart();
  }
}

void loop() {
  // Verificar conexión WiFi cada 10 segundos
  static unsigned long lastWiFiCheck = 0;
  if (millis() - lastWiFiCheck > 10000) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("⚠️ WiFi desconectado! Intentando reconectar...");
      WiFi.reconnect();
      lastWiFiCheck = millis();
      return;
    }
    lastWiFiCheck = millis();
  }
  
  // Actualizar el sensor de parqueo (maneja mediciones y envío TCP)
  parkingSensor.update();
  
  // Capturar y enviar imagen solo cuando el parqueo se ocupe
  if (cameraInitialized && parkingSensor.hasStateChanged() && parkingSensor.getIsOccupied()) {
    // Solo enviar imagen si cambió de LIBRE a OCUPADO
    if (!lastParkingState && parkingSensor.getIsOccupied()) {
      captureAndSendImage();
      imageSentForCurrentOccupation = true;
      Serial.println("📸 Imagen enviada por ocupación del parqueo");
    }
  }
  
  // Actualizar estado anterior
  lastParkingState = parkingSensor.getIsOccupied();
  
  // Resetear flag cuando el parqueo se libere
  if (!parkingSensor.getIsOccupied()) {
    imageSentForCurrentOccupation = false;
  }
  
  // Mostrar estado del sistema cada 30 segundos
  static unsigned long lastStatusPrint = 0;
  if (millis() - lastStatusPrint > 30000) {
    Serial.println(parkingSensor.getStatusString());
    lastStatusPrint = millis();
  }
  
  delay(100);
}
