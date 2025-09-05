#!/usr/bin/env python3
"""
Servidor TCP para recibir datos del sensor de parqueo ESP32
Recibe datos JSON del sensor y opcionalmente imágenes
"""

import socket
import json
import threading
import time
import os
from datetime import datetime
import base64

class ParkingServer:
    def __init__(self, host='0.0.0.0', port=8080):
        self.host = host
        self.port = port
        self.server_socket = None
        self.running = False
        self.clients = []
        
        # Crear directorio para imágenes si no existe
        self.images_dir = "parking_images"
        if not os.path.exists(self.images_dir):
            os.makedirs(self.images_dir)
            print(f"📁 Directorio creado: {self.images_dir}")
    
    def start_server(self):
        """Iniciar el servidor TCP"""
        try:
            self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.server_socket.bind((self.host, self.port))
            self.server_socket.listen(5)
            
            self.running = True
            print("🚗 Servidor de Parqueo ESP32 iniciado")
            print(f"📍 Escuchando en {self.host}:{self.port}")
            print(f"📁 Imágenes se guardarán en: {os.path.abspath(self.images_dir)}")
            print("=" * 50)
            
            while self.running:
                try:
                    client_socket, client_address = self.server_socket.accept()
                    print(f"🔌 Cliente conectado: {client_address}")
                    
                    # Crear hilo para manejar cada cliente
                    client_thread = threading.Thread(
                        target=self.handle_client,
                        args=(client_socket, client_address)
                    )
                    client_thread.daemon = True
                    client_thread.start()
                    
                except socket.error as e:
                    if self.running:
                        print(f"❌ Error aceptando conexión: {e}")
                        
        except Exception as e:
            print(f"❌ Error iniciando servidor: {e}")
        finally:
            self.stop_server()
    
    def handle_client(self, client_socket, client_address):
        """Manejar comunicación con un cliente"""
        try:
            while self.running:
                # Recibir datos del cliente
                data = client_socket.recv(4096)
                if not data:
                    break
                
                # Decodificar datos
                message = data.decode('utf-8').strip()
                
                # Intentar parsear como JSON (datos del sensor)
                try:
                    sensor_data = json.loads(message)
                    self.process_sensor_data(sensor_data, client_address)
                except json.JSONDecodeError:
                    # Si no es JSON, podría ser una imagen o comando
                    self.process_non_json_data(message, client_socket, client_address)
                    
        except Exception as e:
            print(f"❌ Error manejando cliente {client_address}: {e}")
        finally:
            client_socket.close()
            print(f"🔌 Cliente desconectado: {client_address}")
    
    def process_sensor_data(self, data, client_address):
        """Procesar datos del sensor de parqueo"""
        try:
            # Extraer información
            parking_id = data.get('parkingId', 'N/A')
            occupied = data.get('occupied', False)
            distance = data.get('distance', 0.0)
            timestamp = data.get('timestamp', 0)
            
            # Convertir timestamp a fecha legible
            if timestamp > 0:
                dt = datetime.fromtimestamp(timestamp / 1000)
                time_str = dt.strftime("%Y-%m-%d %H:%M:%S")
            else:
                time_str = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            
            # Determinar estado
            status = "🟢 LIBRE" if not occupied else "🔴 OCUPADO"
            
            # Mostrar información en consola
            print(f"\n📊 DATOS DEL SENSOR - {time_str}")
            print(f"   Cliente: {client_address}")
            print(f"   ID Parqueo: {parking_id}")
            print(f"   Estado: {status}")
            print(f"   Distancia: {distance:.1f} cm")
            print(f"   Timestamp: {timestamp}")
            print("-" * 40)
            
            # Guardar en archivo de log (opcional)
            self.log_sensor_data(data, client_address)
            
        except Exception as e:
            print(f"❌ Error procesando datos del sensor: {e}")
    
    def process_non_json_data(self, data, client_socket, client_address):
        """Procesar datos que no son JSON (imágenes, comandos, etc.)"""
        # Verificar si es un comando especial
        if data.startswith("IMAGE:"):
            self.handle_image_data(data, client_socket, client_address)
        elif data.startswith("COMMAND:"):
            self.handle_command(data, client_socket, client_address)
        else:
            print(f"📝 Mensaje de texto de {client_address}: {data}")
    
    def handle_image_data(self, data, client_socket, client_address):
        """Manejar recepción de imagen"""
        try:
            # Extraer datos de la imagen (base64)
            image_data = data[6:]  # Remover "IMAGE:" del inicio
            
            # Decodificar imagen
            image_bytes = base64.b64decode(image_data)
            
            # Generar nombre de archivo
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            filename = f"parking_{timestamp}_{client_address[0]}.jpg"
            filepath = os.path.join(self.images_dir, filename)
            
            # Guardar imagen
            with open(filepath, 'wb') as f:
                f.write(image_bytes)
            
            print(f"📸 Imagen guardada: {filename}")
            print(f"   Tamaño: {len(image_bytes)} bytes")
            print(f"   Ruta: {filepath}")
            
            # Enviar confirmación al cliente
            response = json.dumps({
                "status": "success",
                "message": "Imagen recibida correctamente",
                "filename": filename
            })
            client_socket.send(response.encode('utf-8'))
            
        except Exception as e:
            print(f"❌ Error procesando imagen: {e}")
            error_response = json.dumps({
                "status": "error",
                "message": str(e)
            })
            client_socket.send(error_response.encode('utf-8'))
    
    def handle_command(self, data, client_socket, client_address):
        """Manejar comandos del cliente"""
        command = data[8:]  # Remover "COMMAND:" del inicio
        
        print(f"⚡ Comando de {client_address}: {command}")
        
        # Procesar comandos específicos
        if command == "STATUS":
            response = json.dumps({
                "status": "running",
                "clients_connected": len(self.clients),
                "uptime": time.time()
            })
            client_socket.send(response.encode('utf-8'))
        elif command == "PING":
            response = json.dumps({"status": "pong"})
            client_socket.send(response.encode('utf-8'))
        else:
            response = json.dumps({"status": "unknown_command"})
            client_socket.send(response.encode('utf-8'))
    
    def log_sensor_data(self, data, client_address):
        """Guardar datos del sensor en archivo de log"""
        try:
            log_file = "parking_sensor.log"
            timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            
            with open(log_file, 'a', encoding='utf-8') as f:
                f.write(f"{timestamp} | {client_address} | {json.dumps(data)}\n")
                
        except Exception as e:
            print(f"⚠️ Error guardando log: {e}")
    
    def stop_server(self):
        """Detener el servidor"""
        self.running = False
        if self.server_socket:
            self.server_socket.close()
        print("🛑 Servidor detenido")
    
    def get_server_info(self):
        """Obtener información del servidor"""
        return {
            "host": self.host,
            "port": self.port,
            "running": self.running,
            "clients": len(self.clients),
            "images_dir": os.path.abspath(self.images_dir)
        }

def main():
    """Función principal"""
    print("🚗 Servidor de Parqueo ESP32")
    print("=" * 30)
    
    # Configuración del servidor
    HOST = '0.0.0.0'  # Escuchar en todas las interfaces
    PORT = 8080       # Puerto del servidor
    
    # Crear e iniciar servidor
    server = ParkingServer(HOST, PORT)
    
    try:
        server.start_server()
    except KeyboardInterrupt:
        print("\n🛑 Deteniendo servidor...")
        server.stop_server()
    except Exception as e:
        print(f"❌ Error fatal: {e}")
        server.stop_server()

if __name__ == "__main__":
    main()
