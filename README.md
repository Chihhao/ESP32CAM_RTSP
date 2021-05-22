# ESP32CAM_RTSP (針對NAS使用)

## 本專案是精簡版本，源自 https://github.com/geeksville/Micro-RTSP  

## 成品
![image](https://github.com/Chihhao/ESP32CAM_RTSP/blob/main/image/2.jpg)  
![image](https://github.com/Chihhao/ESP32CAM_RTSP/blob/main/image/4.jpg)  
![image](https://github.com/Chihhao/ESP32CAM_RTSP/blob/main/image/3.jpg)  

## 材料
(1) ESP32-CAM  

## 3D列印外殼
(1) thingiverse: https://www.thingiverse.com/thing:4860203  
(2) tinkercad: https://www.tinkercad.com/things/3lzPcMxvVgm  
(3) tinkercad: https://www.tinkercad.com/things/0DIKF8FIIfn  

## 特點
(1) HTTP 串流  
(2) RTSP 串流  
(3) HTTP/RTSP 只能有一個連入，使用HTTP連上後，RTSP會自動斷掉，當HTTP關閉後，RTSP會重新啟動  

## 設定wifi
```C++
char *ssid = "MY_SSID";          // Put your SSID here
char *password = "MY_PASSWORD";  // Put your PASSWORD here
```

## 選擇開發板
![image](https://github.com/Chihhao/ESP32CAM_RTSP/blob/main/image/1.png)

## Synology NAS 設定
![image](https://github.com/Chihhao/ESP32CAM_RTSP/blob/main/image/6.png)
![image](https://github.com/Chihhao/ESP32CAM_RTSP/blob/main/image/5.png)



