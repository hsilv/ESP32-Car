#include "ESP32Monitor.h"

// Constructor
ESP32Monitor::ESP32Monitor(unsigned long interval, bool enableSerial) {
    this->updateInterval = interval;
    this->serialEnabled = enableSerial;
    this->lastUpdate = 0;
}

// Inicialización
void ESP32Monitor::begin() {
    if (serialEnabled) {
        Serial.begin(115200);
        delay(1000);
        
        Serial.println("\n=== INFORMACIÓN DEL ESP32-S3-CAM ===");
        
        // Información básica del chip
        Serial.println("📱 CHIP:");
        Serial.println("  Modelo: " + getChipModel());
        Serial.println("  Revisión: " + getChipRevision());
        Serial.println("  Frecuencia CPU: " + String(getCpuFreq()) + " MHz");
        
        // Información de memoria
        Serial.println("\n💾 MEMORIA:");
        Serial.println("  Flash Size: " + String(getFlashSize()) + " MB");
        Serial.println("  Flash Speed: " + String(getFlashSpeed()) + " MHz");
        Serial.println("  Free Heap: " + String(getFreeHeap()) + " bytes");
        
        // Diagnóstico detallado de PSRAM
        Serial.println("\n🔍 DIAGNÓSTICO PSRAM:");
        Serial.println("  psramFound(): " + String(isPSRAMFound() ? "SÍ" : "NO"));
        Serial.println("  Free PSRAM: " + String(getFreePSRAM()) + " bytes");
        Serial.println("  Total PSRAM: " + String(getTotalPSRAM()) + " bytes");
        
        // Información adicional de memoria
        Serial.println("\n📊 MEMORIA DETALLADA:");
        Serial.println("  Total Heap: " + String(heap_caps_get_total_size(MALLOC_CAP_DEFAULT)) + " bytes");
        Serial.println("  Free Heap: " + String(heap_caps_get_free_size(MALLOC_CAP_DEFAULT)) + " bytes");
        Serial.println("  Largest Free Block: " + String(heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT)) + " bytes");
        
        // Información de tiempo
        Serial.println("\n⏰ TIEMPO:");
        Serial.println("  Uptime: " + String(getUptime()) + " segundos");
        
        Serial.println("\n✅ Board funcionando correctamente!");
        Serial.println("=====================================");
        
        // Inicializar cámara
        Serial.println("\n📷 INICIALIZANDO CÁMARA...");
        if (initializeCamera()) {
            Serial.println("✅ Cámara inicializada correctamente");
            testCamera();
        } else {
            Serial.println("❌ Error al inicializar la cámara");
        }
    }
}

// Actualización periódica
void ESP32Monitor::update() {
    if (serialEnabled && (millis() - lastUpdate > updateInterval)) {
        lastUpdate = millis();
        printStatus();
    }
}

// Imprimir información del sistema
void ESP32Monitor::printSystemInfo() {
    if (!serialEnabled) return;
    
    Serial.println("\n=== INFORMACIÓN COMPLETA DEL SISTEMA ===");
    Serial.println("📱 CHIP:");
    Serial.println("  Modelo: " + getChipModel());
    Serial.println("  Revisión: " + getChipRevision());
    Serial.println("  Frecuencia CPU: " + String(getCpuFreq()) + " MHz");
    
    Serial.println("\n💾 MEMORIA:");
    Serial.println("  Flash Size: " + String(getFlashSize()) + " MB");
    Serial.println("  Flash Speed: " + String(getFlashSpeed()) + " MHz");
    Serial.println("  Free Heap: " + String(getFreeHeap()) + " bytes");
    Serial.println("  Free PSRAM: " + String(getFreePSRAM()) + " bytes");
    Serial.println("  Total PSRAM: " + String(getTotalPSRAM()) + " bytes");
    Serial.println("  PSRAM Found: " + String(isPSRAMFound() ? "SÍ" : "NO"));
    
    Serial.println("\n⏰ TIEMPO:");
    Serial.println("  Uptime: " + String(getUptime()) + " segundos");
    Serial.println("=========================================");
}

// Imprimir status periódico
void ESP32Monitor::printStatus() {
    if (!serialEnabled) return;
    
    Serial.println("\n--- STATUS ---");
    Serial.println("Uptime: " + String(getUptime()) + "s");
    Serial.println("Free Heap: " + String(getFreeHeap()) + " bytes");
    Serial.println("Free PSRAM: " + String(getFreePSRAM()) + " bytes");
    Serial.println("PSRAM Found: " + String(isPSRAMFound() ? "SÍ" : "NO"));
    Serial.println("Total PSRAM: " + String(getTotalPSRAM()) + " bytes");
    Serial.println("-------------");
}

// Getters para información del sistema
String ESP32Monitor::getChipModel() {
    return String(ESP.getChipModel());
}

String ESP32Monitor::getChipRevision() {
    return String(ESP.getChipRevision());
}

uint32_t ESP32Monitor::getCpuFreq() {
    return ESP.getCpuFreqMHz();
}

uint32_t ESP32Monitor::getFlashSize() {
    return ESP.getFlashChipSize() / 1024 / 1024;
}

uint32_t ESP32Monitor::getFlashSpeed() {
    return ESP.getFlashChipSpeed() / 1000000;
}

uint32_t ESP32Monitor::getFreeHeap() {
    return ESP.getFreeHeap();
}

uint32_t ESP32Monitor::getFreePSRAM() {
    return heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
}

uint32_t ESP32Monitor::getTotalPSRAM() {
    return heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
}

bool ESP32Monitor::isPSRAMFound() {
    return psramFound();
}

unsigned long ESP32Monitor::getUptime() {
    return millis() / 1000;
}

// Configuración
void ESP32Monitor::setUpdateInterval(unsigned long interval) {
    this->updateInterval = interval;
}

void ESP32Monitor::enableSerial(bool enable) {
    this->serialEnabled = enable;
}

void ESP32Monitor::disableSerial() {
    this->serialEnabled = false;
}

// Métodos de cámara
bool ESP32Monitor::initializeCamera() {
    return camera.begin();
}

void ESP32Monitor::testCamera() {
    if (serialEnabled) {
        Serial.println("\n📷 TESTING CÁMARA:");
        Serial.println("  Estado: " + getCameraStatus());
        Serial.println("  Información:");
        Serial.println(getCameraInfo());
        
        if (camera.testCamera()) {
            Serial.println("  ✅ Test de captura exitoso");
        } else {
            Serial.println("  ❌ Test de captura falló");
        }
    }
}

String ESP32Monitor::getCameraStatus() {
    return camera.getCameraStatus();
}

String ESP32Monitor::getCameraInfo() {
    return camera.getCameraInfo();
}
