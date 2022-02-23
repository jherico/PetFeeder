#include <WebServer.h>
#include <esp_camera.h>
#include <memory>
#include "camera_index.h"
#include "secrets.h"
#include "utils.hpp"

WebServer server(0);

int getListenPort();

void handleError(int code, const String& messageLine) {
    String message = messageLine;
    message += F("\n\nURI: ");
    message += server.uri();
    message += F("\nMethod: ");
    message += (server.method() == HTTP_GET) ? F("GET") : F("POST");
    message += F("\nArguments: ");
    message += server.args();
    message += F("\n");
    for (uint8_t i = 0; i < server.args(); i++) {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, F("text/plain"), message);
}

void handleNotFound() {
    handleError(404, F("File Not Found\n\n"));
}

void handleCmd() {
    if (!server.hasArg("var") && !server.hasArg("val")) {
        handleNotFound();
        return;
    }
    auto variable = server.arg("var");
    auto val = atoi(server.arg("val").c_str());
    auto s = esp_camera_sensor_get();
    int res = 0;

    if (variable.equalsIgnoreCase("framesize") && s->pixformat == PIXFORMAT_JPEG)
        res = s->set_framesize(s, (framesize_t)val);
    else if (variable.equalsIgnoreCase("quality"))
        res = s->set_quality(s, val);
    else if (variable.equalsIgnoreCase("contrast"))
        res = s->set_contrast(s, val);
    else if (variable.equalsIgnoreCase("brightness"))
        res = s->set_brightness(s, val);
    else if (variable.equalsIgnoreCase("saturation"))
        res = s->set_saturation(s, val);
    else if (variable.equalsIgnoreCase("gainceiling"))
        res = s->set_gainceiling(s, (gainceiling_t)val);
    else if (variable.equalsIgnoreCase("colorbar"))
        res = s->set_colorbar(s, val);
    else if (variable.equalsIgnoreCase("awb"))
        res = s->set_whitebal(s, val);
    else if (variable.equalsIgnoreCase("agc"))
        res = s->set_gain_ctrl(s, val);
    else if (variable.equalsIgnoreCase("aec"))
        res = s->set_exposure_ctrl(s, val);
    else if (variable.equalsIgnoreCase("hmirror"))
        res = s->set_hmirror(s, val);
    else if (variable.equalsIgnoreCase("vflip"))
        res = s->set_vflip(s, val);
    else if (variable.equalsIgnoreCase("awb_gain"))
        res = s->set_awb_gain(s, val);
    else if (variable.equalsIgnoreCase("agc_gain"))
        res = s->set_agc_gain(s, val);
    else if (variable.equalsIgnoreCase("aec_value"))
        res = s->set_aec_value(s, val);
    else if (variable.equalsIgnoreCase("aec2"))
        res = s->set_aec2(s, val);
    else if (variable.equalsIgnoreCase("dcw"))
        res = s->set_dcw(s, val);
    else if (variable.equalsIgnoreCase("bpc"))
        res = s->set_bpc(s, val);
    else if (variable.equalsIgnoreCase("wpc"))
        res = s->set_wpc(s, val);
    else if (variable.equalsIgnoreCase("raw_gma"))
        res = s->set_raw_gma(s, val);
    else if (variable.equalsIgnoreCase("lenc"))
        res = s->set_lenc(s, val);
    else if (variable.equalsIgnoreCase("special_effect"))
        res = s->set_special_effect(s, val);
    else if (variable.equalsIgnoreCase("wb_mode"))
        res = s->set_wb_mode(s, val);
    else if (variable.equalsIgnoreCase("ae_level"))
        res = s->set_ae_level(s, val);
    else if (variable.equalsIgnoreCase("face_detect")) {
        res = 0;
    } else if (variable.equalsIgnoreCase("face_enroll")) {
        res = 0;
    } else if (variable.equalsIgnoreCase("face_recognize")) {
        res = 0;
    } else {
        res = -1;
    }

    if (res) {
        handleError(500, F("Invalid command"));
        handleNotFound();
        server.send(500, F("text/plain"), String(" ") + variable);
        return;
    }
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200);
}

bool readFeederSwitch();

void handleStatus() {
    static char json_response[2048];
    sensor_t* s = esp_camera_sensor_get();
    char* p = json_response;
    *p++ = '{';
    p += sprintf(p, "\t\"feed_switch\":%d,\n", readFeederSwitch() ? 1 : 0);
    p += sprintf(p, "\t\"colorbar\":%u,\n", s->status.colorbar);
    p += sprintf(p, "\t\"framesize\":%u,\n", s->status.framesize);
    p += sprintf(p, "\t\"quality\":%u,\n", s->status.quality);
    p += sprintf(p, "\t\"brightness\":%d,\n", s->status.brightness);
    p += sprintf(p, "\t\"contrast\":%d,\n", s->status.contrast);
    p += sprintf(p, "\t\"saturation\":%d,\n", s->status.saturation);
    p += sprintf(p, "\t\"sharpness\":%d,\n", s->status.sharpness);
    p += sprintf(p, "\t\"special_effect\":%u,\n", s->status.special_effect);
    p += sprintf(p, "\t\"wb_mode\":%u,\n", s->status.wb_mode);
    p += sprintf(p, "\t\"awb\":%u,\n", s->status.awb);
    p += sprintf(p, "\t\"awb_gain\":%u,\n", s->status.awb_gain);
    p += sprintf(p, "\t\"aec\":%u,\n", s->status.aec);
    p += sprintf(p, "\t\"aec2\":%u,\n", s->status.aec2);
    p += sprintf(p, "\t\"ae_level\":%d,\n", s->status.ae_level);
    p += sprintf(p, "\t\"aec_value\":%u,\n", s->status.aec_value);
    p += sprintf(p, "\t\"agc\":%u,\n", s->status.agc);
    p += sprintf(p, "\t\"agc_gain\":%u,\n", s->status.agc_gain);
    p += sprintf(p, "\t\"gainceiling\":%u,\n", s->status.gainceiling);
    p += sprintf(p, "\t\"bpc\":%u,\n", s->status.bpc);
    p += sprintf(p, "\t\"wpc\":%u,\n", s->status.wpc);
    p += sprintf(p, "\t\"raw_gma\":%u,\n", s->status.raw_gma);
    p += sprintf(p, "\t\"lenc\":%u,\n", s->status.lenc);
    p += sprintf(p, "\t\"vflip\":%u,\n", s->status.vflip);
    p += sprintf(p, "\t\"hmirror\":%u,\n", s->status.hmirror);
    p += sprintf(p, "\t\"dcw\":%u,\n", s->status.dcw);
    p += sprintf(p, "\t\"colorbar\":%u,\n", s->status.colorbar);

    *p++ = '}';
    *p++ = 0;
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, F("application/json"), json_response);
}

using CamPtr = std::shared_ptr<camera_fb_t>;

static CamPtr getFrame() {
    return {esp_camera_fb_get(), esp_camera_fb_return};
}

void handleCapture() {
    int64_t fr_start = esp_timer_get_time();
    auto fb = getFrame();

    if (!fb) {
        handleError(500, F("Camera capture failed"));
        return;
    }
    server.sendHeader("Content-Disposition", "inline; filename=capture.jpg");
    server.send_P(200, "image/jpeg", (const char*)fb->buf, fb->len);
    int64_t fr_end = esp_timer_get_time();
    Serial.printf("JPG: %uB %ums\n", (uint32_t)(fb->len), (uint32_t)((fr_end - fr_start) / 1000));
}

void handleStream() {
    // struct ra_filter_t{
    //   const size_t size; //number of values used for filtering
    //   size_t index{0}; //current value index
    //   int sum{0};
    //   std::vector<int> values; //array to be filled with values
    //   ra_filter_t(size_t _size = 20) : size{_size} {
    //     values.reserve(size);
    //   }
    //   int run(int value){
    //     if (values.size() != size) {
    //       values.push_back(value);
    //       sum += value;
    //     } else {
    //       sum -= values[index];
    //       values[index] = value;
    //       sum += values[index];
    //     }
    //     index = (index + 1) % size;
    //     return sum / values.size();
    //   }
    // };

    // typedef struct {
    //         httpd_req_t *req;
    //         size_t len;
    // } jpg_chunking_t;

    // #define PART_BOUNDARY "123456789000000000000987654321"
    // static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
    // static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
    // static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

    // static ra_filter_t ra_filter;
    // httpd_handle_t stream_httpd = NULL;
    // httpd_handle_t camera_httpd = NULL;

    // static size_t jpg_encode_stream(void * arg, size_t index, const void* data, size_t len){
    //     jpg_chunking_t *j = (jpg_chunking_t *)arg;
    //     if(!index){
    //         j->len = 0;
    //     }
    //     if(httpd_resp_send_chunk(j->req, (const char *)data, len) != ESP_OK){
    //         return 0;
    //     }
    //     j->len += len;
    //     return len;
    // }

    // static esp_err_t stream_handler(httpd_req_t *req){
    //     static char * part_buf[64];
    //     static int64_t last_frame = 0;
    //     if(!last_frame) {
    //         last_frame = esp_timer_get_time();
    //     }

    //     esp_err_t res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    //     if(res != ESP_OK){
    //         return res;
    //     }

    //     httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    //     while(true){
    //         auto fb = getFrame();
    //         if (!fb) {
    //             Serial.println("Camera capture failed");
    //             res = ESP_FAIL;
    //             break;
    //         }

    //         int64_t fr_start = esp_timer_get_time();
    //         if(res == ESP_OK){
    //             res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
    //         }
    //         if(res == ESP_OK){
    //             size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, fb->len);
    //             res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
    //         }

    //         if(res == ESP_OK){
    //             res = httpd_resp_send_chunk(req, (const char *)fb->buf, fb->len);
    //         }
    //         fb.reset();
    //         if(res != ESP_OK){
    //             break;
    //         }
    //         int64_t fr_end = esp_timer_get_time();
    //         int64_t frame_time = fr_end - last_frame;
    //         last_frame = fr_end;
    //         frame_time /= 1000;
    //         uint32_t avg_frame_time = ra_filter.run(frame_time);
    //         Serial.printf("MJPG: %uB %ums (%.1ffps), AVG: %ums (%.1ffps)\n",
    //             (uint32_t)(fb->len),
    //             (uint32_t)frame_time, 1000.0 / (uint32_t)frame_time,
    //             avg_frame_time, 1000.0 / avg_frame_time
    //         );
    //     }
    //     last_frame = 0;
    //     return res;
    // }
    handleNotFound();
}

void feedScoops(uint8_t count);

void handleFeed() {
    if (!server.hasArg("scoops")) {
        handleNotFound();
        return;
    }
    auto val = atoi(server.arg("scoops").c_str());
    feedScoops(val);
    server.send(200);
}

void handleRoot() {
    server.sendHeader(F("Content-Encoding"), F("gzip"));
    auto s = esp_camera_sensor_get();
    if (s->id.PID == OV3660_PID) {
        server.send_P(200, "text/html", (const char*)index_ov3660_html_gz, index_ov3660_html_gz_len);
    } else {
        server.send_P(200, "text/html", (const char*)index_ov2640_html_gz, index_ov2640_html_gz_len);
    }
}

void setupWeb() {
    server.enableCORS();
    server.onNotFound(handleNotFound);
    server.on(F("/"), handleRoot);
    server.on(F("/control"), handleCmd);
    server.on(F("/status"), handleStatus);
    server.on(F("/capture"), handleCapture);
    server.on(F("/stream"), handleStream);
    server.on(F("/feed"), handleFeed);
    auto listenPort = getListenPort();
    server.begin(listenPort);
    Serial.print(F("HTTP WiFiWebServer is listening on port "));
    Serial.println(listenPort);
}

void loopWeb() {
    server.handleClient();
}

// // Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
// //
// // Licensed under the Apache License, Version 2.0 (the "License");
// // you may not use this file except in compliance with the License.
// // You may obtain a copy of the License at
// //
// //     http://www.apache.org/licenses/LICENSE-2.0
// //
// // Unless required by applicable law or agreed to in writing, software
// // distributed under the License is distributed on an "AS IS" BASIS,
// // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// // See the License for the specific language governing permissions and
// // limitations under the License.

// void loop(void)
// {
//   uPnP->updatePortMappings(600000);  // 10 minutes

// }
// #include <WebServer.h>
// #define FRIENDLY_NAME       ARDUINO_BOARD "-JABBER-WIFI"   // this name will
// appear in your router port forwarding section
