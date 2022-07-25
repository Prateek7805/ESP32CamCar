//                                                                                                                                          
#include<Arduino.h>
#include "esp_camera.h"
#include "WiFi.h"
#include "esp_http_server.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "index.h"
#include "hstyles.h"
#include "hscript.h"
#include "LITTLEFS.h"
#include "soc/soc.h" //disable brownout problems
#include "soc/rtc_cntl_reg.h"  //disable brownout problems

#define CAMERA_MODEL_AI_THINKER
#define T_LITTLEFS_UPDATE 1000


const char* ssid = "Your SSID";   //Enter SSID WIFI Name
const char* password = "Your Pass";   //Enter WIFI Password

#if defined(CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

#else
#error "Camera model not selected"
#endif

#define MTR_FORWARD 0x06
#define MTR_REVERSE 0x09
#define MTR_RIGHT 0x03
#define MTR_LEFT 0x0C
#define MTR_STOP 0x0

uint8_t hLight = 12;
uint8_t tLight = 4;
uint8_t hChannel = 9;
uint8_t tChannel = 10;

uint8_t mPins[4] = {13, 14, 15, 2};
uint8_t mChannel[4] = {5, 6, 7, 8};

uint8_t mState[4] = {0,};
uint8_t out = 0;

struct stateFields {
  uint8_t hLightOn = 0;
  uint8_t tLightOn = 0;
  uint8_t hBright = 0;
  uint8_t tBright = 0;
  uint8_t speed;
} state;

struct credentials {
  uint8_t mode = 0;
  String STA_SSID;
  String STA_PASS;
  String ESP_SSID;
  String ESP_PASS;
} creds;

struct timers {
  unsigned long t_littlefs_update = 0;
} timer;

struct notification{
  const uint8_t T_NOTIFY_ASYNC = 200;
  unsigned long t_notify_async = 0;
  uint8_t Notification = 4;
  bool start = false;
} notify;

void startCameraServer();

void initMotors() {
  for (uint8_t  i = 0; i < 4; i++) {
    ledcAttachPin(mPins[i], mChannel[i]);
    ledcSetup(mChannel[i], 2000, 8);
    ledcWrite(mChannel[i], 0);
  }
}
void initLights() {
  ledcAttachPin(hLight , hChannel);
  ledcSetup(hChannel, 2000, 8);
  ledcWrite(hChannel, 0);
  ledcAttachPin(tLight , tChannel);
  ledcSetup(tChannel, 2000, 8);
  ledcWrite(tChannel, 0);
}

void headLightUpdate() {
  ledcWrite(hChannel, (state.hLightOn) ? state.hBright : 0);
  ledcWrite(tChannel, (state.tLightOn) ? state.tBright : 0);
}


void resetState() {
  File file = LITTLEFS.open("/state.txt", "w");
  file.print("0\n0\n0\n0\n100\n");
  file.close();
}
void resetCreds() {
  File file = LITTLEFS.open("/creds.txt", "w");
  file.print("1\n\n\nAppendCar\nappendcar\n");
  file.close();
}


void updateState() {
  File file = LITTLEFS.open("/state.txt", "w");
  file.printf("%d\n%d\n%d\n%d\n%d\n", state.hLightOn,state.tLightOn,state.hBright,state.tBright,state.speed);
  file.close();
}

void updateCreds() {
  File file = LITTLEFS.open("/creds.txt", "w");
  file.printf("%d\n%s\n%s\n%s\n%s\n", creds.mode, creds.STA_SSID.c_str(), creds.STA_PASS.c_str(), creds.ESP_SSID.c_str(), creds.ESP_PASS.c_str());
  file.close();
}

void loadState() {
  if (!LITTLEFS.exists("/state.txt")) {
    resetState();
  }
  File file = LITTLEFS.open("/state.txt", "r");
  state.hLightOn = (uint8_t)file.readStringUntil('\n').toInt();
  state.tLightOn = (uint8_t)file.readStringUntil('\n').toInt();
  state.hBright = (uint8_t)file.readStringUntil('\n').toInt();
  state.tBright = (uint8_t)file.readStringUntil('\n').toInt();
  state.speed = (uint8_t)file.readStringUntil('\n').toInt();
  file.close();
}

String stateJSON() {
  String JSON = "{\"hl\" : "+(String)state.hLightOn + ",";
  JSON += "\"tl\" : "+(String)state.tLightOn + ",";
  JSON += "\"hb\" : "+(String)state.hBright + ",";
  JSON += "\"tb\" : "+(String)state.tBright + ",";
  JSON += "\"speed\" : " + (String)state.speed + ",";
  JSON += "\"WIFIMODE\" : " + (String)creds.mode + "}";
  return JSON;
}

void loadCreds() {
  if (!LITTLEFS.exists("/creds.txt")) {
    resetCreds();
  }
  File file = LITTLEFS.open("/creds.txt", "r");
  creds.mode = (uint8_t)file.readStringUntil('\n').toInt();
  creds.STA_SSID = file.readStringUntil('\n');
  creds.STA_PASS = file.readStringUntil('\n');
  creds.ESP_SSID = file.readStringUntil('\n');
  creds.ESP_PASS = file.readStringUntil('\n');
  file.close();
}

bool validateCreds(String _ssid, String _pass) {
  if (_ssid == "" || _ssid.length() < 1 || _ssid.length() > 32)
    return false;
  if (_pass == "" || _pass.length() < 8 || _pass.length() > 63)
    return false;
  return true;
}

void moveCar() {
  Serial.println();
  out = 0;
  out |= (mState[0]) ? MTR_FORWARD : 0;
  out |= (mState[1]) ? MTR_RIGHT : 0;
  out |= (mState[2]) ? MTR_LEFT : 0;
  out |= (mState[3]) ? MTR_REVERSE : 0;
  Serial.println(out, BIN);
  for (int j = 0; j < 4; j++) {
    ledcWrite(mChannel[j], (out >> j) & 0x01 ? state.speed : 0);
  }
}

void connect(){
  if (creds.mode) {
    if(creds.ESP_SSID != "" && creds.ESP_PASS != ""){
      WiFi.mode(WIFI_AP);
      WiFi.softAP(creds.ESP_SSID.c_str(), creds.ESP_PASS.c_str());
    }else{
      resetCreds();
      loadCreds();
      connect();
      return;
    }
  } else {
    if(creds.STA_SSID!="" && creds.STA_PASS!=""){
      
      WiFi.mode(WIFI_STA);
      WiFi.begin(creds.STA_SSID.c_str(), creds.STA_PASS.c_str());

      Serial.print("Connecting ...");
      uint8_t count = 0;
      while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        if(count == 50){
          creds.mode = 1;
          connect();
          return;
        }
        count++;
      }
    }else{
      creds.mode = 1;
      connect();
    }
  }
}

void notification(){
  if(millis() - notify.t_notify_async> notify.T_NOTIFY_ASYNC){
    notify.t_notify_async = millis();
    if(notify.start){
      ledcWrite(hChannel, (notify.Notification%2 == 0) ? 255 : 0);
      ledcWrite(tChannel, 0);
      notify.Notification--;
      if(notify.Notification == 0){
        notify.Notification = 4;
        notify.start = false;
        headLightUpdate();
      }
    }
    
  }
}

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  initMotors();
  initLights();
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
  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  if (LITTLEFS.begin(true)) {
    loadState();
    loadCreds();
    headLightUpdate();
  } else {
    //reset everything
    ESP.restart();
  }
  // camera init
  int count = 0;

  esp_err_t err = esp_camera_init(&config);
  while (err != ESP_OK) {
    if (++count == 10) {
      break;
    }
    err = esp_camera_init(&config);
  }

  //drop down frame size for higher initial frame rate
  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_CIF);
  s->set_brightness(s, 1); // up the brightness just a bit
  s->set_saturation(s, -2); // lower the saturation
  connect();
  startCameraServer();

  Serial.print("Camera Ready! Go To 'http://");
  Serial.print(WiFi.localIP());

  Serial.println("' to connect");
  notify.start = true;
}


void loop() {
  // put your main code here, to run repeatedly:
  if(millis() - timer.t_littlefs_update> T_LITTLEFS_UPDATE){
    updateState();
    timer.t_littlefs_update = millis();
  }
  notification();
}

typedef struct {
  httpd_req_t *req;
  size_t len;
} jpg_chunking_t;

#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

httpd_handle_t stream_httpd = NULL;
httpd_handle_t camera_httpd = NULL;

static size_t jpg_encode_stream(void * arg, size_t index, const void* data, size_t len) {
  jpg_chunking_t *j = (jpg_chunking_t *)arg;
  if (!index) {
    j->len = 0;
  }
  if (httpd_resp_send_chunk(j->req, (const char *)data, len) != ESP_OK) {
    return 0;
  }
  j->len += len;
  return len;
}

static esp_err_t capture_handler(httpd_req_t *req) {
  camera_fb_t * fb = NULL;
  esp_err_t res = ESP_OK;

  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.printf("Camera capture failed");
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  httpd_resp_set_type(req, "image/jpeg");
  httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");

  if (fb->format == PIXFORMAT_JPEG) {
    
    res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
  } else {
    jpg_chunking_t jchunk = {req, 0};
    res = frame2jpg_cb(fb, 80, jpg_encode_stream, &jchunk) ? ESP_OK : ESP_FAIL;
    httpd_resp_send_chunk(req, NULL, 0);
    
  }
  esp_camera_fb_return(fb);
  return res;
}

static esp_err_t stream_handler(httpd_req_t *req) {
  camera_fb_t * fb = NULL;
  esp_err_t res = ESP_OK;
  size_t _jpg_buf_len = 0;
  uint8_t * _jpg_buf = NULL;
  char * part_buf[64];

  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
  if (res != ESP_OK) {
    return res;
  }
  while (true) {
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.printf("Camera capture failed");
      res = ESP_FAIL;
    } else {
      if (fb->format != PIXFORMAT_JPEG) {
        bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
        esp_camera_fb_return(fb);
        fb = NULL;
        if (!jpeg_converted) {
          Serial.printf("JPEG compression failed");
          res = ESP_FAIL;
        }
      } else {
        _jpg_buf_len = fb->len;
        _jpg_buf = fb->buf;
      }
    }
    if (res == ESP_OK) {
      size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
      res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
    }
    if (res == ESP_OK) {
      res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
    }
    if (res == ESP_OK) {
      res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
    }
    if (fb) {
      esp_camera_fb_return(fb);
      fb = NULL;
      _jpg_buf = NULL;
    } else if (_jpg_buf) {
      free(_jpg_buf);
      _jpg_buf = NULL;
    }
    if (res != ESP_OK) {
      break;
    }

  }

  return res;
}


void stringToInt(uint8_t arr[4], String st) {
  uint8_t len = st.length();
  uint8_t sum = 0;
  uint8_t ind = 0;
  for (uint8_t i = 0; i < len; i++) {
    if (st[i] != ',') {
      sum = sum * 10 + (st[i] - '0');
    } else {
      arr[ind++] = sum;
      sum = 0;
    }
  }
}

static esp_err_t cmd_handler(httpd_req_t *req) {
  char*  buf;
  size_t buf_len;
  char variable[32] = {0,};
  char value[32] = {0,};
  buf_len = httpd_req_get_url_query_len(req) + 1;
  if (buf_len > 1) {
    buf = (char*)malloc(buf_len);
    if (!buf) {
      httpd_resp_send_500(req);
      return ESP_FAIL;
    }
    if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
      if (httpd_query_key_value(buf, "com", variable, sizeof(variable)) == ESP_OK &&
          httpd_query_key_value(buf, "val", value, sizeof(value)) == ESP_OK) {
      } else {
        free(buf);
        httpd_resp_send_404(req);
        return ESP_FAIL;
      }
    } else {
      free(buf);
      httpd_resp_send_404(req);
      return ESP_FAIL;
    }
    free(buf);
  } else {
    httpd_resp_send_404(req);
    return ESP_FAIL;
  }

  int val = atoi(value);


  sensor_t * s = esp_camera_sensor_get();
  int res = 0;
  if (variable[0] == '_') {
    if (variable[1] == 'f') {
      mState[0] = val;
      moveCar();
    }
    if (variable[1] == 'r') {
      mState[1] = val;
      moveCar();
    }
    if (variable[1] == 'l') {
      mState[2] = val;
      moveCar();
    }
    if (variable[1] == 'b') {
      mState[3] = val;
      moveCar();
    }
    if (variable[1] == 's') {
      for (uint8_t i = 0; i < 4; i++)
        mState[i] = 0;
      moveCar();
    }
  }else if(variable[0] == 'W'){
    if(variable[1] == 'S'){
      String word = (String)variable;
      String _ssid = word.substring(2, val+2);
      String _pass = word.substring(val+2);
      if(validateCreds(_ssid, _pass)){
        creds.STA_SSID = _ssid;
        creds.STA_PASS = _pass;
      }
    }else if(variable[1] == 'A'){
      String word = (String)variable;
      String _ssid = word.substring(2, val+2);
      String _pass = word.substring(val+2);
      if(validateCreds(_ssid, _pass)){
        creds.ESP_SSID = _ssid;
        creds.ESP_PASS = _pass;
      }
    }else if(variable[1] == 'M'){
      if(val == 1 || val == 0){
        creds.mode = val;
      }
    }
    updateCreds();
  }else if (!strcmp(variable, "speed")) {
    state.speed = val;
  } else if (!strcmp(variable, "hl")) {
    state.hLightOn = val;
    headLightUpdate();
  } else if (!strcmp(variable, "tl")) {
    state.tLightOn = val;
    headLightUpdate();
  } else if (!strcmp(variable, "hb")) {
    state.hBright = val;
    headLightUpdate();
  } else if (!strcmp(variable, "tb")) {
    state.tBright = val;
    headLightUpdate();
  }
  
  else if (!strcmp(variable, "quality")) res = s->set_quality(s, val);
  else if (!strcmp(variable, "rm")) {
    ESP.restart();
  } else if (!strcmp(variable, "rs")) {
    resetState();
    loadState();
  } else if (!strcmp(variable, "rc")) {
    resetCreds();
    loadCreds();
  } else if (!strcmp(variable, "ra")) {
    resetCreds();
    resetState();
    loadState();
    loadCreds();
  } else if (!strcmp(variable, "status")) {
    String data = stateJSON();
    httpd_resp_set_hdr(req, "Content-Type", "application/json");
    return httpd_resp_send(req, data.c_str(), data.length());
  } else if (!strcmp(variable, "rssi")) {
    long rssi = WiFi.RSSI();
    String data = "{\"rssi\":"+(String)rssi+"}";
    httpd_resp_set_hdr(req, "Content-Type", "application/json");
    return httpd_resp_send(req, data.c_str(), data.length());
  } 

  else if (!strcmp(variable, "framesize")) {
    if (s->pixformat == PIXFORMAT_JPEG) res = s->set_framesize(s, (framesize_t)val);
  }
  else if (!strcmp(variable, "bright")) res = s->set_brightness(s, val);

  else {
    res = -1;
    for (uint8_t i = 0; i < 4; i++)
      mState[i] = 0;
    moveCar();
  }
  if (res) {
    return httpd_resp_send_500(req);
  }

  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, NULL, 0);
}


static esp_err_t index_handler(httpd_req_t *req) {
  String dataType = "text/html";
  const char * DATA;
  DATA = _index;
  httpd_resp_set_type(req, dataType.c_str());
  return httpd_resp_send(req, DATA, strlen(DATA));
}

static esp_err_t render(httpd_req_t *req) {
  String dataType = "text/css";
  const char * DATA;
  DATA = _hstyles;
  String URI = (String)req->uri;
  if (URI.endsWith(".js")) {
    dataType = "application/js";
    DATA = _hscript;
  }
  httpd_resp_set_type(req, dataType.c_str());
  return httpd_resp_send(req, DATA, strlen(DATA));
}
void startCameraServer() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.uri_match_fn = httpd_uri_match_wildcard;

  httpd_uri_t index_uri = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = index_handler,
    .user_ctx  = NULL
  };
  httpd_uri_t render_uri = {
    .uri       = "/h*",
    .method    = HTTP_GET,
    .handler   = render,
    .user_ctx  = NULL
  };

  httpd_uri_t cmd_uri = {
    .uri       = "/control",
    .method    = HTTP_GET,
    .handler   = cmd_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t capture_uri = {
    .uri       = "/capture",
    .method    = HTTP_GET,
    .handler   = capture_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t stream_uri = {
    .uri       = "/stream",
    .method    = HTTP_GET,
    .handler   = stream_handler,
    .user_ctx  = NULL
  };


  Serial.printf("Starting web server on port: '%d'", config.server_port);
  if (httpd_start(&camera_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(camera_httpd, &index_uri);
    httpd_register_uri_handler(camera_httpd, &render_uri);
    httpd_register_uri_handler(camera_httpd, &cmd_uri);
    httpd_register_uri_handler(camera_httpd, &capture_uri);
  }

  config.server_port += 1;
  config.ctrl_port += 1;
  Serial.printf("Starting stream server on port: '%d'", config.server_port);
  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &stream_uri);
  }
}
