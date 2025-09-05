#include <Arduino.h>
#include <ESP32Monitor.h>
#include "board_config.h"

// Crear instancia del monitor
ESP32Monitor monitor(5000); // Actualizar cada 5 segundos

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  // Inicializar el monitor (incluye la cámara)
  monitor.begin();

  // Probar la cámara
  monitor.testCamera();
  
  // Verificar el estado de la cámara
  if (monitor.getCameraStatus() == "Funcionando correctamente") {
    Serial.println("Cámara funcionando correctamente");
  } else {
    Serial.println("Error en la cámara: " + monitor.getCameraStatus());
  }
}

void loop() {
  // Actualizar el monitor (maneja automáticamente la actualización periódica)
  monitor.update();
  
  delay(100);
}