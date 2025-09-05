#ifndef PARKINGSENSOR_H
#define PARKINGSENSOR_H

#include <Arduino.h>
#include <WiFi.h>

class ParkingSensor {
private:
    // Pines del sensor ultrasónico HC-SR04
    int trigPin;
    int echoPin;
    
    // Configuración de parqueo
    int parkingId;
    float thresholdDistance; // Distancia umbral en cm (50cm por defecto)
    
    // Estado del parqueo
    bool isOccupied;
    bool previousOccupied; // Estado anterior para detectar cambios
    float lastDistance;
    unsigned long lastMeasurement;
    unsigned long measurementInterval; // Intervalo entre mediciones en ms
    
    // Configuración TCP
    const char* serverIP;
    int serverPort;
    WiFiClient tcpClient;
    bool tcpConnected;
    unsigned long lastTcpAttempt;
    unsigned long tcpReconnectInterval;
    
    // Métodos privados
    float measureDistance();
    bool connectToServer();
    void sendParkingData();
    bool isDistanceValid(float distance);
    
public:
    // Constructor
    ParkingSensor(int trigPin, int echoPin, int parkingId, 
                  const char* serverIP = "192.168.1.100", 
                  int serverPort = 8080,
                  float thresholdDistance = 50.0);
    
    // Métodos principales
    void begin();
    void update();
    
    // Getters
    bool getIsOccupied() const;
    float getLastDistance() const;
    int getParkingId() const;
    bool isTcpConnected() const;
    WiFiClient& getTcpClient();
    bool hasStateChanged() const;
    
    // Setters
    void setThresholdDistance(float distance);
    void setServerConfig(const char* ip, int port);
    void setParkingId(int id);
    
    // Métodos de utilidad
    String getStatusString() const;
    void forceMeasurement();
};

#endif // PARKINGSENSOR_H
