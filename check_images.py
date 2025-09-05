#!/usr/bin/env python3
"""
Script para verificar las imágenes recibidas del ESP32
"""

import os
import glob
from datetime import datetime

def check_received_images():
    """Verificar imágenes recibidas"""
    
    print("🔍 Verificando imágenes recibidas del ESP32")
    print("=" * 50)
    
    # Directorios a verificar
    directories = ["received_images", "parking_images", "test_images"]
    
    total_images = 0
    
    for directory in directories:
        if os.path.exists(directory):
            print(f"\n📁 Directorio: {directory}")
            
            # Buscar archivos de imagen
            image_files = glob.glob(os.path.join(directory, "*.jpg")) + \
                         glob.glob(os.path.join(directory, "*.jpeg")) + \
                         glob.glob(os.path.join(directory, "*.png"))
            
            if image_files:
                for image_file in sorted(image_files):
                    file_size = os.path.getsize(image_file)
                    file_time = datetime.fromtimestamp(os.path.getmtime(image_file))
                    
                    print(f"  📸 {os.path.basename(image_file)}")
                    print(f"     Tamaño: {file_size:,} bytes")
                    print(f"     Fecha: {file_time.strftime('%Y-%m-%d %H:%M:%S')}")
                    total_images += 1
            else:
                print("  ❌ No se encontraron imágenes")
        else:
            print(f"\n📁 Directorio: {directory} (no existe)")
    
    print(f"\n📊 Total de imágenes encontradas: {total_images}")
    
    if total_images == 0:
        print("\n💡 Sugerencias:")
        print("  1. Asegúrate de que el servidor esté ejecutándose")
        print("  2. Verifica que el ESP32 esté conectado")
        print("  3. Revisa que la cámara esté funcionando")
        print("  4. Comprueba la conexión TCP")

def list_directories():
    """Listar directorios del proyecto"""
    
    print("\n📂 Directorios del proyecto:")
    print("-" * 30)
    
    for item in os.listdir("."):
        if os.path.isdir(item):
            print(f"📁 {item}/")

if __name__ == "__main__":
    list_directories()
    check_received_images()
