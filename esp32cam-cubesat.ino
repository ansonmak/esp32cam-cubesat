/*
  ESP32-CAM CubeSat (ESP32 library V.1.0.6 required)
  cubesat-esp32.ino (requires app_httpd.cpp)
  Based upon Espressif ESP32CAM Examples
  Uses TBA6612FNG H-Bridge Controller
  
*/

#include "esp_wifi.h"
#include "esp_camera.h"
#include <WiFi.h>
#include "esp32cam_pins.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "Wire.h"

// Setup WiFi Access Point Credentials
const char* ssid = "chao";
const char* password = "9C9fIfrw";

unsigned long prev_t = 0;
unsigned long slave_request_time = 100; // time interval get data from slave
int battery_voltage = 0;
extern bool is_deploy;

void cubesat_setup();
void startCameraServer();

void setup() 
{
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // disable brownout detector
  
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  //init with high specs to pre-allocate larger buffers
  if(psramFound()){
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  //drop down frame size for higher initial frame rate
  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, (framesize_t)3);
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
  s->set_quality(s, 35);

  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");

  cubesat_setup();

  // setup I2C
  Wire.begin(I2C_SDA, I2C_SCL);
  prev_t = millis();
}

void loop() {
  unsigned long cur_t = millis();
  if (cur_t - prev_t >= slave_request_time) {
    prev_t = cur_t;
    Wire.requestFrom(SLAVE_ADDR, 1); // request 1 byte

    while (Wire.available()) { 
      char c = Wire.read(); //read byte
      battery_voltage = int(c);
    }

    if (!is_deploy){

    } else{

    }
  }
}
