#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include <Arduino.h>
#include "esp_camera.h"

class CameraManager {
private:
    bool cameraInitialized;
    bool cameraDetected;
    camera_config_t config;
    
    // Configuración específica para ESP32-S3-CAM
    void setupCameraConfig();
    
public:
    // Constructor
    CameraManager();
    
    // Métodos principales
    bool begin();
    void end();
    bool isInitialized();
    bool isDetected();
    
    // Verificación de la cámara
    bool testCamera();
    String getCameraStatus();
    String getCameraInfo();
    
    // Captura de imagen (básica para testing)
    bool captureTest();
    
    // Configuración
    void setResolution(framesize_t resolution);
    void setQuality(int quality);
    void setBrightness(int brightness);
    void setContrast(int contrast);
    
    // Getters para información de la cámara
    framesize_t getCurrentResolution();
    int getCurrentQuality();
    int getCurrentBrightness();
    int getCurrentContrast();
};

#endif // CAMERAMANAGER_H
