#!/usr/bin/env python3
"""
Cliente de prueba para simular el ESP32 y probar el servidor
"""

import socket
import json
import time
import random
from datetime import datetime

class TestParkingClient:
    def __init__(self, server_host='localhost', server_port=8080):
        self.server_host = server_host
        self.server_port = server_port
        self.socket = None
        self.parking_id = 1
        self.connected = False
    
    def connect(self):
        """Conectar al servidor"""
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.connect((self.server_host, self.server_port))
            self.connected = True
            print(f"✅ Conectado al servidor {self.server_host}:{self.server_port}")
            return True
        except Exception as e:
            print(f"❌ Error conectando: {e}")
            return False
    
    def send_sensor_data(self, distance, occupied=None):
        """Enviar datos del sensor"""
        if not self.connected:
            print("❌ No conectado al servidor")
            return False
        
        # Determinar ocupación si no se especifica
        if occupied is None:
            occupied = distance < 50.0
        
        # Crear datos del sensor
        data = {
            "parkingId": self.parking_id,
            "occupied": occupied,
            "distance": round(distance, 1),
            "timestamp": int(time.time() * 1000)
        }
        
        try:
            # Enviar datos como JSON
            message = json.dumps(data)
            self.socket.send(message.encode('utf-8'))
            
            print(f"📤 Datos enviados: Distancia={distance:.1f}cm, Ocupado={occupied}")
            return True
            
        except Exception as e:
            print(f"❌ Error enviando datos: {e}")
            return False
    
    def send_command(self, command):
        """Enviar comando al servidor"""
        if not self.connected:
            print("❌ No conectado al servidor")
            return False
        
        try:
            message = f"COMMAND:{command}"
            self.socket.send(message.encode('utf-8'))
            
            # Recibir respuesta
            response = self.socket.recv(1024).decode('utf-8')
            print(f"📤 Comando enviado: {command}")
            print(f"📥 Respuesta: {response}")
            return True
            
        except Exception as e:
            print(f"❌ Error enviando comando: {e}")
            return False
    
    def disconnect(self):
        """Desconectar del servidor"""
        if self.socket:
            self.socket.close()
            self.connected = False
            print("🔌 Desconectado del servidor")

def simulate_parking_scenarios():
    """Simular diferentes escenarios de parqueo"""
    client = TestParkingClient()
    
    if not client.connect():
        return
    
    print("\n🚗 Simulando escenarios de parqueo...")
    print("=" * 40)
    
    # Escenario 1: Parqueo libre
    print("\n📊 Escenario 1: Parqueo libre")
    client.send_sensor_data(75.5, False)
    time.sleep(2)
    
    # Escenario 2: Carro llegando
    print("\n📊 Escenario 2: Carro llegando")
    for distance in [60, 45, 30, 20, 15]:
        client.send_sensor_data(distance, True)
        time.sleep(1)
    
    # Escenario 3: Carro estacionado
    print("\n📊 Escenario 3: Carro estacionado")
    for _ in range(3):
        client.send_sensor_data(12.5, True)
        time.sleep(2)
    
    # Escenario 4: Carro saliendo
    print("\n📊 Escenario 4: Carro saliendo")
    for distance in [20, 35, 50, 70, 85]:
        occupied = distance < 50.0
        client.send_sensor_data(distance, occupied)
        time.sleep(1)
    
    # Escenario 5: Parqueo libre nuevamente
    print("\n📊 Escenario 5: Parqueo libre")
    client.send_sensor_data(90.0, False)
    time.sleep(2)
    
    # Enviar comando de estado
    print("\n⚡ Enviando comando de estado...")
    client.send_command("STATUS")
    time.sleep(1)
    
    client.disconnect()

def continuous_simulation():
    """Simulación continua con datos aleatorios"""
    client = TestParkingClient()
    
    if not client.connect():
        return
    
    print("\n🔄 Simulación continua iniciada...")
    print("Presiona Ctrl+C para detener")
    
    try:
        while True:
            # Generar distancia aleatoria
            distance = random.uniform(10, 100)
            occupied = distance < 50.0
            
            client.send_sensor_data(distance, occupied)
            time.sleep(2)  # Enviar cada 2 segundos
            
    except KeyboardInterrupt:
        print("\n🛑 Simulación detenida")
    finally:
        client.disconnect()

def main():
    """Función principal"""
    print("🧪 Cliente de Prueba - Sensor de Parqueo ESP32")
    print("=" * 50)
    
    while True:
        print("\nSelecciona una opción:")
        print("1. Simular escenarios de parqueo")
        print("2. Simulación continua")
        print("3. Salir")
        
        choice = input("\nOpción: ").strip()
        
        if choice == "1":
            simulate_parking_scenarios()
        elif choice == "2":
            continuous_simulation()
        elif choice == "3":
            print("👋 ¡Hasta luego!")
            break
        else:
            print("❌ Opción inválida")

if __name__ == "__main__":
    main()
