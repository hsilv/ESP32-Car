#!/usr/bin/env python3
"""
Servidor de prueba para recibir imágenes del ESP32
"""

import socket
import json
import base64
import time
import os
from datetime import datetime

def test_image_server():
    """Servidor de prueba para imágenes"""
    
    # Configuración
    HOST = '0.0.0.0'
    PORT = 8080
    
    print("🧪 Servidor de Prueba para Imágenes ESP32")
    print("=" * 50)
    
    # Crear directorio para imágenes
    images_dir = "received_images"
    if not os.path.exists(images_dir):
        os.makedirs(images_dir)
        print(f"📁 Directorio creado: {images_dir}")
    
    # Crear socket del servidor
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_socket.bind((HOST, PORT))
    server_socket.listen(1)
    
    print(f"📡 Servidor escuchando en {HOST}:{PORT}")
    print("Esperando conexión del ESP32...")
    print("Presiona Ctrl+C para detener")
    
    try:
        while True:
            # Aceptar conexión
            client_socket, client_address = server_socket.accept()
            print(f"\n🔌 Cliente conectado: {client_address}")
            
            try:
                while True:
                    # Recibir datos
                    data = client_socket.recv(4096)
                    if not data:
                        break
                    
                    message = data.decode('utf-8').strip()
                    print(f"📥 Mensaje recibido: {len(message)} caracteres")
                    
                    # Procesar mensaje
                    if message.startswith("IMAGE:"):
                        print("📸 Procesando imagen...")
                        
                        # Extraer datos base64
                        image_data = message[6:]  # Remover "IMAGE:"
                        
                        # Decodificar imagen
                        try:
                            image_bytes = base64.b64decode(image_data)
                            
                            # Generar nombre de archivo
                            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
                            filename = f"esp32_image_{timestamp}.jpg"
                            filepath = os.path.join(images_dir, filename)
                            
                            # Guardar imagen
                            with open(filepath, 'wb') as f:
                                f.write(image_bytes)
                            
                            print(f"✅ Imagen guardada: {filename}")
                            print(f"   Tamaño: {len(image_bytes)} bytes")
                            print(f"   Ruta: {filepath}")
                            print(f"   Base64: {len(image_data)} caracteres")
                            
                            # Enviar confirmación
                            response = json.dumps({
                                "status": "success",
                                "message": "Imagen recibida correctamente",
                                "filename": filename,
                                "size": len(image_bytes)
                            })
                            client_socket.send(response.encode('utf-8'))
                            
                        except Exception as e:
                            print(f"❌ Error procesando imagen: {e}")
                            error_response = json.dumps({
                                "status": "error",
                                "message": str(e)
                            })
                            client_socket.send(error_response.encode('utf-8'))
                    
                    elif message.startswith("COMMAND:"):
                        # Es un comando
                        command = message[8:]
                        print(f"⚡ Comando recibido: {command}")
                        
                        if command == "STATUS":
                            response = json.dumps({
                                "status": "running",
                                "message": "Servidor de prueba funcionando"
                            })
                            client_socket.send(response.encode('utf-8'))
                    
                    else:
                        # Intentar parsear como JSON (datos del sensor)
                        try:
                            sensor_data = json.loads(message)
                            print(f"📊 Datos del sensor: {sensor_data}")
                            
                            # Enviar confirmación
                            response = json.dumps({"status": "received"})
                            client_socket.send(response.encode('utf-8'))
                            
                        except json.JSONDecodeError:
                            print(f"📝 Mensaje de texto: {message}")
            
            except Exception as e:
                print(f"❌ Error manejando cliente: {e}")
            finally:
                client_socket.close()
                print(f"🔌 Cliente desconectado: {client_address}")
    
    except KeyboardInterrupt:
        print("\n🛑 Deteniendo servidor de prueba...")
    finally:
        server_socket.close()

if __name__ == "__main__":
    test_image_server()
