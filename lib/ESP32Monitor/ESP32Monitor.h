#ifndef ESP32MONITOR_H
#define ESP32MONITOR_H

#include <Arduino.h>
#include <esp_system.h>
#include <esp_heap_caps.h>
#include "CameraManager.h"

class ESP32Monitor {
private:
    unsigned long lastUpdate;
    unsigned long updateInterval;
    bool serialEnabled;
    CameraManager camera;
    
public:
    // Constructor
    ESP32Monitor(unsigned long interval = 5000, bool enableSerial = true);
    
    // Métodos principales
    void begin();
    void update();
    void printSystemInfo();
    void printStatus();
    
    // Métodos de cámara
    bool initializeCamera();
    void testCamera();
    String getCameraStatus();
    String getCameraInfo();
    
    // Métodos para cambiar resolución de cámara
    void setCameraQQVGA();
    void setCameraQVGA();
    void setCameraVGA();
    void setCameraSVGA();
    
    // Getters para información del sistema
    String getChipModel();
    String getChipRevision();
    uint32_t getCpuFreq();
    uint32_t getFlashSize();
    uint32_t getFlashSpeed();
    uint32_t getFreeHeap();
    uint32_t getFreePSRAM();
    uint32_t getTotalPSRAM();
    bool isPSRAMFound();
    unsigned long getUptime();
    
    // Configuración
    void setUpdateInterval(unsigned long interval);
    void enableSerial(bool enable);
    void disableSerial();
};

#endif // ESP32MONITOR_H
