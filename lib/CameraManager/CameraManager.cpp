#include "CameraManager.h"
#include "board_config.h"

CameraManager::CameraManager() {
    cameraInitialized = false;
    cameraDetected = false;
    setupCameraConfig();
}

void CameraManager::setupCameraConfig() {
    // Configuración para ESP32-S3-CAM con TY-OV2640
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
    
    // Configuración conservadora para evitar crashes
    config.frame_size = FRAMESIZE_QQVGA; // 160x120 - resolución más pequeña
    config.jpeg_quality = 15; // Calidad más baja
    config.fb_count = 1; // Un buffer
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    config.fb_location = CAMERA_FB_IN_DRAM; // Usar DRAM en lugar de PSRAM
}

bool CameraManager::begin() {
    Serial.println("Iniciando cámara TY-OV2640 en ESP32-S3-CAM...");
    Serial.println("Configuración de pines:");
    Serial.printf("  D0-D7: %d,%d,%d,%d,%d,%d,%d,%d\n", 
                  config.pin_d0, config.pin_d1, config.pin_d2, config.pin_d3,
                  config.pin_d4, config.pin_d5, config.pin_d6, config.pin_d7);
    Serial.printf("  XCLK: %d, PCLK: %d, VSYNC: %d, HREF: %d\n",
                  config.pin_xclk, config.pin_pclk, config.pin_vsync, config.pin_href);
    Serial.printf("  SDA: %d, SCL: %d, PWDN: %d, RESET: %d\n",
                  config.pin_sccb_sda, config.pin_sccb_scl, config.pin_pwdn, config.pin_reset);
    Serial.printf("  Frecuencia XCLK: %d Hz\n", config.xclk_freq_hz);
    Serial.printf("  Formato: %d, Resolución: %d, Calidad: %d\n", 
                  config.pixel_format, config.frame_size, config.jpeg_quality);
    Serial.printf("  Ubicación buffer: %s\n", 
                  (config.fb_location == CAMERA_FB_IN_PSRAM) ? "PSRAM" : "DRAM");
    
    // Verificar memoria disponible
    Serial.printf("  Memoria libre: %d bytes\n", esp_get_free_heap_size());
    
    // Esperar un poco para que la cámara se estabilice
    delay(3000);
    
    // Intentar inicializar la cámara
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Error al inicializar la cámara: 0x%x\n", err);
        Serial.println("Posibles causas:");
        Serial.println("  - Cámara no conectada");
        Serial.println("  - Pines incorrectos");
        Serial.println("  - Problema de alimentación");
        Serial.println("  - Sensor dañado");
        cameraInitialized = false;
        cameraDetected = false;
        return false;
    }
    
    // Obtener información del sensor
    sensor_t *s = esp_camera_sensor_get();
    if (s == NULL) {
        Serial.println("Error: No se pudo obtener el sensor de la cámara");
        cameraInitialized = false;
        cameraDetected = false;
        return false;
    }
    
    // Configurar el sensor como en el ejemplo que funciona
    s->set_brightness(s, 0);     // -2 to 2
    s->set_contrast(s, 0);       // -2 to 2
    s->set_saturation(s, 0);     // -2 to 2
    s->set_special_effect(s, 0); // 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
    s->set_whitebal(s, 1);       // 0 = disable , 1 = enable
    s->set_awb_gain(s, 1);       // 0 = disable , 1 = enable
    s->set_wb_mode(s, 0);        // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
    s->set_exposure_ctrl(s, 1);  // 0 = disable , 1 = enable
    s->set_aec2(s, 0);           // 0 = disable , 1 = enable
    s->set_ae_level(s, 0);       // -2 to 2
    s->set_aec_value(s, 300);    // 0 to 1200
    s->set_gain_ctrl(s, 1);      // 0 = disable , 1 = enable
    s->set_agc_gain(s, 0);       // 0 to 30
    s->set_gainceiling(s, (gainceiling_t)0);  // 0 to 6
    s->set_bpc(s, 0);            // 0 = disable , 1 = enable
    s->set_wpc(s, 1);            // 0 = disable , 1 = enable
    s->set_raw_gma(s, 1);        // 0 = disable , 1 = enable
    s->set_lenc(s, 1);           // 0 = disable , 1 = enable
    s->set_hmirror(s, 0);        // 0 = disable , 1 = enable
    s->set_vflip(s, 0);          // 0 = disable , 1 = enable
    s->set_dcw(s, 1);            // 0 = disable , 1 = enable
    s->set_colorbar(s, 0);       // 0 = disable , 1 = enable
    
    cameraInitialized = true;
    cameraDetected = true;
    
    Serial.println("Cámara inicializada correctamente");
    Serial.printf("Sensor detectado correctamente\n");
    Serial.printf("Resolución: %d\n", s->status.framesize);
    
    // Esperar un poco más para que el sensor se estabilice
    delay(500);
    
    return true;
}

void CameraManager::end() {
    if (cameraInitialized) {
        esp_camera_deinit();
        cameraInitialized = false;
        cameraDetected = false;
        Serial.println("Cámara desactivada");
    }
}

bool CameraManager::isInitialized() {
    return cameraInitialized;
}

bool CameraManager::isDetected() {
    return cameraDetected;
}

bool CameraManager::testCamera() {
    if (!cameraInitialized) {
        Serial.println("Error: Cámara no inicializada");
        return false;
    }
    
    Serial.println("Probando captura de imagen...");
    
    // Intentar capturar una imagen
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Error: No se pudo capturar imagen");
        Serial.println("Posibles causas:");
        Serial.println("  - Sensor no responde");
        Serial.println("  - Problema de memoria");
        Serial.println("  - Configuración incorrecta");
        return false;
    }
    
    Serial.printf("Imagen capturada: %dx%d, %d bytes\n", 
                  fb->width, fb->height, fb->len);
    Serial.printf("Formato: %d\n", fb->format);
    Serial.printf("Timestamp: %llu\n", fb->timestamp.tv_sec);
    
    // Liberar el buffer
    esp_camera_fb_return(fb);
    
    Serial.println("Test de cámara exitoso");
    return true;
}

String CameraManager::getCameraStatus() {
    if (!cameraInitialized) {
        return "No inicializada";
    }
    
    if (!cameraDetected) {
        return "No detectada";
    }
    
    return "Funcionando correctamente";
}

String CameraManager::getCameraInfo() {
    if (!cameraInitialized) {
        return "Cámara no disponible";
    }
    
    sensor_t *s = esp_camera_sensor_get();
    if (s == NULL) {
        return "Error al obtener información del sensor";
    }
    
    String info = "Sensor: Detectado correctamente\n";
    info += "Resolución: " + String(s->status.framesize) + "\n";
    info += "Calidad JPEG: " + String(s->status.quality) + "\n";
    info += "Brillo: " + String(s->status.brightness) + "\n";
    info += "Contraste: " + String(s->status.contrast) + "\n";
    
    return info;
}

bool CameraManager::captureTest() {
    return testCamera();
}

void CameraManager::setResolution(framesize_t resolution) {
    if (!cameraInitialized) return;
    
    sensor_t *s = esp_camera_sensor_get();
    if (s != NULL) {
        s->set_framesize(s, resolution);
        Serial.printf("Resolución cambiada a: %d\n", resolution);
    }
}

void CameraManager::setQuality(int quality) {
    if (!cameraInitialized) return;
    
    sensor_t *s = esp_camera_sensor_get();
    if (s != NULL) {
        s->set_quality(s, quality);
        Serial.printf("Calidad JPEG cambiada a: %d\n", quality);
    }
}

void CameraManager::setBrightness(int brightness) {
    if (!cameraInitialized) return;
    
    sensor_t *s = esp_camera_sensor_get();
    if (s != NULL) {
        s->set_brightness(s, brightness);
        Serial.printf("Brillo cambiado a: %d\n", brightness);
    }
}

void CameraManager::setContrast(int contrast) {
    if (!cameraInitialized) return;
    
    sensor_t *s = esp_camera_sensor_get();
    if (s != NULL) {
        s->set_contrast(s, contrast);
        Serial.printf("Contraste cambiado a: %d\n", contrast);
    }
}

framesize_t CameraManager::getCurrentResolution() {
    if (!cameraInitialized) return FRAMESIZE_INVALID;
    
    sensor_t *s = esp_camera_sensor_get();
    if (s != NULL) {
        return (framesize_t)s->status.framesize;
    }
    return FRAMESIZE_INVALID;
}

int CameraManager::getCurrentQuality() {
    if (!cameraInitialized) return -1;
    
    sensor_t *s = esp_camera_sensor_get();
    if (s != NULL) {
        return s->status.quality;
    }
    return -1;
}

int CameraManager::getCurrentBrightness() {
    if (!cameraInitialized) return -1;
    
    sensor_t *s = esp_camera_sensor_get();
    if (s != NULL) {
        return s->status.brightness;
    }
    return -1;
}

int CameraManager::getCurrentContrast() {
    if (!cameraInitialized) return -1;
    
    sensor_t *s = esp_camera_sensor_get();
    if (s != NULL) {
        return s->status.contrast;
    }
    return -1;
}
