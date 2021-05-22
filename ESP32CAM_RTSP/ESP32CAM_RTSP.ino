//https://github.com/geeksville/Micro-RTSP

#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include "OV2640.h"
#include "SimStreamer.h"
#include "OV2640Streamer.h"
#include "CRtspSession.h"
 
char *ssid = "MY_SSID";          // Put your SSID here
char *password = "MY_PASSWORD";  // Put your PASSWORD here
 
WebServer server(80);
WiFiServer rtspServer(554);
OV2640 cam;
CStreamer *streamer;
 
//mjpeg串流
void handle_jpg_stream(void){
  WiFiClient client = server.client();
  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
  server.sendContent(response);
 
  while (1)  {
    cam.run();
    if (!client.connected())
      break;
    response = "--frame\r\n";
    response += "Content-Type: image/jpeg\r\n\r\n";
    server.sendContent(response);
 
    client.write((char *)cam.getfb(), cam.getSize());
    server.sendContent("\r\n");
    if (!client.connected())
      break;
  }
}
 
//靜態影像
void handle_jpg(void){
  WiFiClient client = server.client();
  cam.run();
  if (!client.connected())
  {
    return;
  }
  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-disposition: inline; filename=capture.jpg\r\n";
  response += "Content-type: image/jpeg\r\n\r\n";
  server.sendContent(response);
  client.write((char *)cam.getfb(), cam.getSize());
}
 
//錯誤處理
void handleNotFound() {
  String message = "Server is running!\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  server.send(200, "text/plain", message);
}
 
//WiFi連線
void WifiConnect() {
  if(WiFi.status() == WL_CONNECTED) return;
  
  int t=0;
  WiFi.begin(ssid, password);
  while(true){
    if(WiFi.status() == WL_CONNECTED) break;
    delay(1000);
    t++;
    if(t>=5){
      t=0;
      WiFi.begin(ssid, password);
    }    
  } 
  Serial.println("WiFi連線成功");
  Serial.print("IP Address:");
  Serial.println(WiFi.localIP());
}
 
void setup(){
  Serial.begin(115200);
  //設定影像大小：UXGA(1600x1200),SXGA(1280x1024),XGA(1024x768),SVGA(800x600),VGA(640x480),CIF(400x296),QVGA(320x240),HQVGA(240x176),QQVGA(160x120)
  esp32cam_aithinker_config.frame_size = FRAMESIZE_SVGA;
  esp32cam_aithinker_config.jpeg_quality = 10;
  cam.init(esp32cam_aithinker_config);
  //開始WiFi連線
  WifiConnect();
  
  server.on("/", HTTP_GET, handle_jpg_stream);
  server.on("/jpg", HTTP_GET, handle_jpg);
  server.onNotFound(handleNotFound);
  server.begin();
  rtspServer.begin();
  streamer = new OV2640Streamer(cam);//啟動服務
  Serial.println("WiFi connected");
  Serial.print("Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("/' to watch mjpeg stream");
  Serial.print("Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("/jpg' to see still jpg");
  Serial.print("Use RTRP:'");
  Serial.print(WiFi.localIP());
  Serial.println("', URL:'/mjpeg/1' and port:554 to start rtsp stream");
}
 
void loop() {
  
  WifiConnect();

  server.handleClient();
  uint32_t msecPerFrame = 100;
  static uint32_t lastimage = millis();
  // If we have an active client connection, just service that until gone
  streamer->handleRequests(0); // we don't use a timeout here,
  // instead we send only if we have new enough frames
  uint32_t now = millis();
  if (streamer->anySessions()) {
    if (now > lastimage + msecPerFrame || now < lastimage) { // handle clock rollover
      streamer->streamImage(now);
      lastimage = now;
 
      // check if we are overrunning our max frame rate
      now = millis();
      if (now > lastimage + msecPerFrame) {
        printf("warning exceeding max frame rate of %d ms\n", now - lastimage);
      }
    }
  }
  WiFiClient rtspClient = rtspServer.accept();
  if (rtspClient) {
    Serial.print("client: ");
    Serial.print(rtspClient.remoteIP());
    Serial.println();
    streamer->addSession(rtspClient);
  }
}
