#!/usr/bin/env python3
"""
Script para enviar imágenes al servidor de parqueo
"""

import socket
import base64
import json
import os
import time

def send_image_to_server(image_path, server_host='localhost', server_port=8080):
    """Enviar una imagen al servidor"""
    
    # Verificar que el archivo existe
    if not os.path.exists(image_path):
        print(f"❌ Error: El archivo {image_path} no existe")
        return False
    
    try:
        # Leer imagen y convertir a base64
        with open(image_path, 'rb') as f:
            image_data = f.read()
        
        image_base64 = base64.b64encode(image_data).decode('utf-8')
        
        # Crear mensaje
        message = f"IMAGE:{image_base64}"
        
        # Conectar y enviar
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_socket.connect((server_host, server_port))
        
        print(f"📤 Enviando imagen: {image_path}")
        print(f"   Tamaño: {len(image_data)} bytes")
        
        client_socket.send(message.encode('utf-8'))
        
        # Recibir respuesta
        response = client_socket.recv(1024).decode('utf-8')
        response_data = json.loads(response)
        
        if response_data.get('status') == 'success':
            print(f"✅ Imagen enviada correctamente")
            print(f"   Archivo guardado como: {response_data.get('filename')}")
        else:
            print(f"❌ Error del servidor: {response_data.get('message')}")
        
        client_socket.close()
        return True
        
    except Exception as e:
        print(f"❌ Error enviando imagen: {e}")
        return False

def create_test_image():
    """Crear una imagen de prueba simple"""
    try:
        from PIL import Image, ImageDraw, ImageFont
        
        # Crear imagen de prueba
        img = Image.new('RGB', (320, 240), color='lightblue')
        draw = ImageDraw.Draw(img)
        
        # Dibujar texto
        try:
            font = ImageFont.truetype("arial.ttf", 20)
        except:
            font = ImageFont.load_default()
        
        draw.text((50, 100), "Test Parking Image", fill='black', font=font)
        draw.text((50, 130), f"Time: {time.strftime('%H:%M:%S')}", fill='black', font=font)
        
        # Guardar imagen
        filename = "test_parking_image.jpg"
        img.save(filename)
        print(f"📸 Imagen de prueba creada: {filename}")
        return filename
        
    except ImportError:
        print("⚠️ PIL no está instalado. Creando archivo de texto como imagen de prueba...")
        
        # Crear archivo de texto como "imagen" de prueba
        filename = "test_parking_image.txt"
        with open(filename, 'w') as f:
            f.write("Test Parking Image\n")
            f.write(f"Time: {time.strftime('%H:%M:%S')}\n")
            f.write("This is a test file to simulate an image\n")
        
        print(f"📄 Archivo de prueba creado: {filename}")
        return filename

def main():
    """Función principal"""
    print("📸 Enviador de Imágenes - Servidor de Parqueo")
    print("=" * 45)
    
    while True:
        print("\nSelecciona una opción:")
        print("1. Crear y enviar imagen de prueba")
        print("2. Enviar imagen existente")
        print("3. Salir")
        
        choice = input("\nOpción: ").strip()
        
        if choice == "1":
            # Crear imagen de prueba
            image_path = create_test_image()
            if image_path:
                send_image_to_server(image_path)
                
        elif choice == "2":
            # Enviar imagen existente
            image_path = input("Ruta de la imagen: ").strip()
            if image_path:
                send_image_to_server(image_path)
                
        elif choice == "3":
            print("👋 ¡Hasta luego!")
            break
        else:
            print("❌ Opción inválida")

if __name__ == "__main__":
    main()
