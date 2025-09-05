#ifndef CAMERA_PINS_H
#define CAMERA_PINS_H

// Pines para ESP32-S3-CAM con cámara TY-OV2640
// Configuración específica para ESP32-S3-CAM

// Pines de datos de la cámara
#define Y2_GPIO_NUM 11
#define Y3_GPIO_NUM 9
#define Y4_GPIO_NUM 8
#define Y5_GPIO_NUM 10
#define Y6_GPIO_NUM 12
#define Y7_GPIO_NUM 18
#define Y8_GPIO_NUM 17
#define Y9_GPIO_NUM 16

// Pines de control de la cámara
#define XCLK_GPIO_NUM 15
#define PCLK_GPIO_NUM 13
#define VSYNC_GPIO_NUM 6
#define HREF_GPIO_NUM 7

// Pines de comunicación I2C (SCCB)
#define SIOD_GPIO_NUM 4  // SDA
#define SIOC_GPIO_NUM 5  // SCL

// Pines de control de potencia
#define PWDN_GPIO_NUM -1
#define RESET_GPIO_NUM -1

#endif // CAMERA_PINS_H
