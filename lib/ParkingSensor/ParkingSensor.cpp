#include "ParkingSensor.h"

ParkingSensor::ParkingSensor(int trigPin, int echoPin, int parkingId, 
                             const char* serverIP, int serverPort,
                             float thresholdDistance) {
    this->trigPin = trigPin;
    this->echoPin = echoPin;
    this->parkingId = parkingId;
    this->serverIP = serverIP;
    this->serverPort = serverPort;
    this->thresholdDistance = thresholdDistance;
    
    // Estado inicial
    this->isOccupied = false;
    this->previousOccupied = false;
    this->lastDistance = 0.0;
    this->lastMeasurement = 0;
    this->measurementInterval = 1000; // Medir cada 1 segundo
    
    // TCP
    this->tcpConnected = false;
    this->lastTcpAttempt = 0;
    this->tcpReconnectInterval = 5000; // Intentar reconectar cada 5 segundos
}

void ParkingSensor::begin() {
    Serial.println("=== INICIALIZANDO SENSOR DE PARQUEO ===");
    Serial.printf("ID de parqueo: %d\n", parkingId);
    Serial.printf("Pines - Trig: %d, Echo: %d\n", trigPin, echoPin);
    Serial.printf("Distancia umbral: %.1f cm\n", thresholdDistance);
    Serial.printf("Servidor TCP: %s:%d\n", serverIP, serverPort);
    
    // Configurar pines del sensor ultrasónico
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    
    // Estado inicial del trigger
    digitalWrite(trigPin, LOW);
    
    Serial.println("Sensor ultrasónico configurado correctamente");
    Serial.println("=============================================");
}

void ParkingSensor::update() {
    unsigned long currentTime = millis();
    
    // Medir distancia si ha pasado el intervalo
    if (currentTime - lastMeasurement >= measurementInterval) {
        float distance = measureDistance();
        
        if (isDistanceValid(distance)) {
            lastDistance = distance;
            
            // Determinar si está ocupado
            bool newOccupied = (distance < thresholdDistance);
            
            // Actualizar estado anterior antes de cambiar el actual
            previousOccupied = isOccupied;
            isOccupied = newOccupied;
            
            // Solo enviar datos si cambió el estado o es la primera medición
            if (newOccupied != previousOccupied || lastMeasurement == 0) {
                sendParkingData();
                
                Serial.printf("Parqueo %d - Distancia: %.1f cm, Estado: %s\n", 
                             parkingId, distance, isOccupied ? "OCUPADO" : "LIBRE");
                
                if (newOccupied != previousOccupied) {
                    Serial.printf("🔄 Cambio de estado: %s → %s\n", 
                                 previousOccupied ? "OCUPADO" : "LIBRE",
                                 isOccupied ? "OCUPADO" : "LIBRE");
                }
            }
        } else {
            // Si la medición no es válida, no actualizar el timestamp
            // para intentar de nuevo más rápido
            Serial.println("🔄 Reintentando medición en 500ms...");
            lastMeasurement = currentTime - measurementInterval + 500;
        }
        
        lastMeasurement = currentTime;
    }
    
    // Intentar conectar TCP si no está conectado
    if (!tcpConnected && currentTime - lastTcpAttempt >= tcpReconnectInterval) {
        connectToServer();
        lastTcpAttempt = currentTime;
    }
}

float ParkingSensor::measureDistance() {
    // Limpiar el pin trigger
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    
    // Enviar pulso de 10 microsegundos
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    // Leer el tiempo de respuesta del echo con timeout más largo
    unsigned long duration = pulseIn(echoPin, HIGH, 50000); // Timeout de 50ms
    
    if (duration == 0) {
        Serial.println("⚠️ Timeout en medición ultrasónica - reintentando...");
        
        // Segundo intento con delay
        delay(100);
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);
        
        duration = pulseIn(echoPin, HIGH, 50000);
        
        if (duration == 0) {
            Serial.println("❌ Error: Sensor ultrasónico no responde");
            return -1.0; // Valor de error
        }
    }
    
    // Calcular distancia en cm
    // Velocidad del sonido = 343 m/s = 0.0343 cm/μs
    // Distancia = (tiempo * velocidad) / 2 (ida y vuelta)
    float distance = (duration * 0.0343) / 2.0;
    
    return distance;
}

bool ParkingSensor::isDistanceValid(float distance) {
    // Rango válido para HC-SR04: 2cm a 400cm
    // También verificar que no sea valor de error (-1.0)
    if (distance < 0) {
        Serial.println("⚠️ Distancia inválida: valor de error");
        return false;
    }
    
    if (distance < 2.0) {
        Serial.println("⚠️ Distancia muy cercana: posible error de medición");
        return false;
    }
    
    if (distance > 400.0) {
        Serial.println("⚠️ Distancia muy lejana: posible error de medición");
        return false;
    }
    
    return true;
}

bool ParkingSensor::connectToServer() {
    Serial.printf("Intentando conectar a servidor TCP %s:%d...\n", serverIP, serverPort);
    
    if (tcpClient.connect(serverIP, serverPort)) {
        tcpConnected = true;
        Serial.println("✅ Conectado al servidor TCP exitosamente");
        return true;
    } else {
        tcpConnected = false;
        Serial.println("❌ Error al conectar al servidor TCP");
        return false;
    }
}

void ParkingSensor::sendParkingData() {
    if (!tcpConnected) {
        Serial.println("⚠️ No conectado al servidor TCP, no se pueden enviar datos");
        return;
    }
    
    // Crear JSON con los datos del parqueo
    String jsonData = "{";
    jsonData += "\"parkingId\":" + String(parkingId) + ",";
    jsonData += "\"occupied\":" + String(isOccupied ? "true" : "false") + ",";
    jsonData += "\"distance\":" + String(lastDistance, 1) + ",";
    jsonData += "\"timestamp\":" + String(millis());
    jsonData += "}";
    
    // Enviar datos
    tcpClient.println(jsonData);
    
    // Verificar si la conexión sigue activa
    if (!tcpClient.connected()) {
        tcpConnected = false;
        Serial.println("⚠️ Conexión TCP perdida");
    } else {
        Serial.printf("📤 Datos enviados: %s\n", jsonData.c_str());
    }
}

// Getters
bool ParkingSensor::getIsOccupied() const {
    return isOccupied;
}

float ParkingSensor::getLastDistance() const {
    return lastDistance;
}

int ParkingSensor::getParkingId() const {
    return parkingId;
}

bool ParkingSensor::isTcpConnected() const {
    return tcpConnected;
}

WiFiClient& ParkingSensor::getTcpClient() {
    return tcpClient;
}

bool ParkingSensor::hasStateChanged() const {
    return (isOccupied != previousOccupied);
}

// Setters
void ParkingSensor::setThresholdDistance(float distance) {
    thresholdDistance = distance;
    Serial.printf("Distancia umbral cambiada a: %.1f cm\n", distance);
}

void ParkingSensor::setServerConfig(const char* ip, int port) {
    serverIP = ip;
    serverPort = port;
    tcpConnected = false; // Forzar reconexión
    Serial.printf("Configuración de servidor cambiada a: %s:%d\n", ip, port);
}

void ParkingSensor::setParkingId(int id) {
    parkingId = id;
    Serial.printf("ID de parqueo cambiado a: %d\n", id);
}

String ParkingSensor::getStatusString() const {
    String status = "=== ESTADO DEL SENSOR DE PARQUEO ===\n";
    status += "ID: " + String(parkingId) + "\n";
    status += "Distancia: " + String(lastDistance, 1) + " cm\n";
    status += "Estado: " + String(isOccupied ? "OCUPADO" : "LIBRE") + "\n";
    status += "Umbral: " + String(thresholdDistance, 1) + " cm\n";
    status += "TCP: " + String(tcpConnected ? "Conectado" : "Desconectado") + "\n";
    status += "Servidor: " + String(serverIP) + ":" + String(serverPort) + "\n";
    status += "=====================================";
    return status;
}

void ParkingSensor::forceMeasurement() {
    float distance = measureDistance();
    if (isDistanceValid(distance)) {
        lastDistance = distance;
        bool newOccupied = (distance < thresholdDistance);
        
        if (newOccupied != isOccupied) {
            isOccupied = newOccupied;
            sendParkingData();
        }
        
        Serial.printf("Medición forzada - Distancia: %.1f cm, Estado: %s\n", 
                     distance, isOccupied ? "OCUPADO" : "LIBRE");
    }
}
