/*******************************************************************/
/*                            INCLUDES                             */
/*******************************************************************/

#include <WiFi.h>
#include <base64.h>
#include <esp_camera.h>

/* Installed Using Arduino Package Manager */
/* URL: https://github.com/khoih-prog/WebSockets_Generic */
#include "WebSocketsClient_Generic.h"
/* URL: https://github.com/khoih-prog/MQTTPubSubClient_Generic */
#include "MQTTPubSubClient_Generic.h"

/*******************************************************************/
/*                            MACROS                               */
/*******************************************************************/

/* Application */
#define WIFI_TIMEOUT    15000
#define HANDLE_ERROR    ESP.restart

/* Camera */
#define CAM_PIN_PWDN    32
#define CAM_PIN_RESET   -1
#define CAM_PIN_XCLK     0
#define CAM_PIN_SIOD    26
#define CAM_PIN_SIOC    27

#define CAM_PIN_D7      35
#define CAM_PIN_D6      34
#define CAM_PIN_D5      39
#define CAM_PIN_D4      36
#define CAM_PIN_D3      21
#define CAM_PIN_D2      19
#define CAM_PIN_D1      18
#define CAM_PIN_D0       5
#define CAM_PIN_VSYNC   25
#define CAM_PIN_HREF    23
#define CAM_PIN_PCLK    22

/*******************************************************************/
/*                         GLOBAL VARIABLES                        */
/*******************************************************************/

/* Temporary Image Buffer (As Defined In Assignment Steps)*/
camera_fb_t tmpimage;

/* Variable To Hold Base64 Encoded Value */
String imageBase64 = "";

/* Variables To Hold WiFi Network Name & Password */

const char *networkName     = "MNet_Hank 2.4GHz";
const char *networkPassword = "69325095487719894718";

/* Variable To Hold The ESP32 CAM Configuration */
static camera_config_t ESP32CAMConfig = 
{
    .pin_pwdn      = CAM_PIN_PWDN,
    .pin_reset     = CAM_PIN_RESET,
    .pin_xclk      = CAM_PIN_XCLK,
    .pin_sscb_sda  = CAM_PIN_SIOD,
    .pin_sscb_scl  = CAM_PIN_SIOC,

    .pin_d7        = CAM_PIN_D7,
    .pin_d6        = CAM_PIN_D6,
    .pin_d5        = CAM_PIN_D5,
    .pin_d4        = CAM_PIN_D4,
    .pin_d3        = CAM_PIN_D3,
    .pin_d2        = CAM_PIN_D2,
    .pin_d1        = CAM_PIN_D1,
    .pin_d0        = CAM_PIN_D0,
    .pin_vsync     = CAM_PIN_VSYNC,
    .pin_href      = CAM_PIN_HREF,
    .pin_pclk      = CAM_PIN_PCLK,

    //XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
    .xclk_freq_hz  = 20000000,
    .ledc_timer    = LEDC_TIMER_0,
    .ledc_channel  = LEDC_CHANNEL_0,

    .pixel_format  = PIXFORMAT_JPEG, //YUV422,GRAYSCALE,RGB565,JPEG
    .frame_size    = FRAMESIZE_QVGA, //QQVGA-UXGA

    .jpeg_quality  = 12,       //0-63 Low Number = Higher Quality
    .fb_count      = 2,        //I2S Runs In Continuous Mode For > 1
    .grab_mode     = CAMERA_GRAB_WHEN_EMPTY,
};

WebSocketsClient webSocketsClientHandle;
MQTTPubSub::PubSubClient<16384> MQTTClientHandle;

char topicName[] = "test/hello";

/* All messages should be wrapped in the following JSON format: {“id”:
Matriculation#, “data”: “message”}. Example: {“id”: 01234567, “data”:
“AdvTopic Max Mustermann/01234567 Start Sending”} */
String greetingMessage = "{\"id\": 03751049, \"data\": \"AdvTopic prm oq/03751059 Start Sending\"}";
String endingMessage   = "{\"id\": 03751059, \"data\": \"AdvTopic prm oq/03751059 End of Sending\"}";

/*******************************************************************/
/*                            FUNCTIONS                            */
/*******************************************************************/

/*******************************************************************/
/*                          WiFi & CAMERA                          */
/*******************************************************************/

/* void init_camera() that initializes the camera on ESP32-CAM and
set the resolution to QVGA (320 x 240 px) */
static void init_camera()
{
    esp_err_t error = esp_camera_init(&ESP32CAMConfig);
    if (error == ESP_OK)
    {
        Serial.println("ESP32 CAM Initialization Successful!");
    }
    else
    {
        delay(100);
        Serial.println("ESP32 CAM Initialization Unsuccessful!");
        HANDLE_ERROR();
    }
}

/* void init_wifi() that initializes the Wi-Fi connection on ESP32-CAM */
static void init_wifi()
{
    /* Set The WiFi Adapter To Station Mode */
    WiFi.mode(WIFI_STA);
    Serial.println("Connecting To WiFi Network: " + String(networkName));
  
    /* Configure The Network ID & Password */
    WiFi.begin(networkName, networkPassword);
  
    /* Connect To The WiFi Network */
    unsigned long WiFiTimer = millis();
    while ((millis() - WiFiTimer <= WIFI_TIMEOUT) && (WiFi.status() != WL_CONNECTED))
    {
        delay(500);
        Serial.print('.');
    }
  
    Serial.println();
         
    /* Inform The Status Of The Connection To The User */
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("WiFi Network Connection Successful!");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
    }
    else 
    {
        delay(100);
        Serial.println("WiFi Network Connection Unsuccessful!");
        WiFi.disconnect();
        HANDLE_ERROR();
    }
}

/* bool capture_image() that allows ESP32-CAM to take a picture and
store it in a global variable named tmpimage (you should decide the type
of the variable). The function returns true if the capture was successful and
false if not. */
static bool capture_image()
{
    camera_fb_t *frameBuffer = NULL;
  
    /* Capture A Picture Using ESP32 CAM */
    frameBuffer = esp_camera_fb_get();
  
    if(frameBuffer)
    {
        Serial.println("ESP32 CAM Image Capture Successful!");

        /* Store THe Picture In A Global Variable - tmpimage */
        tmpimage.len    = frameBuffer->len;
        tmpimage.width  = frameBuffer->width;
        tmpimage.height = frameBuffer->height;
        tmpimage.format = frameBuffer->format;
        tmpimage.timestamp.tv_sec  = (frameBuffer->timestamp).tv_sec;
        tmpimage.timestamp.tv_usec = (frameBuffer->timestamp).tv_usec;
        tmpimage.buf    = (uint8_t *)malloc(sizeof(uint8_t) * frameBuffer->len);

        /* Copy The FrameBuffer */
        memcpy(tmpimage.buf, frameBuffer->buf, frameBuffer->len);
      
        /* Return The Frame Buffer (Reuse) */
        esp_camera_fb_return(frameBuffer);

        return true; 
    }
    else
    {
        delay(100);
        Serial.println("ESP32 CAM Image Capture Unsuccessful!");
        HANDLE_ERROR();
        return false; 
    }
}

/*******************************************************************/
/*                               MQTT                              */
/*******************************************************************/

static void init_mqtt_wss()
{  Serial.println("line209 inside init_mqtt_wss");
//noderedlnxse20220516.o1jkfiv0l98.eu-de.codeengine.appdomain.cloud
    webSocketsClientHandle.beginSSL("noderedlnxse20220516.o1jkfiv0l98.eu-de.codeengine.appdomain.cloud", 443, "/ws/mqtt");
    Serial.println("line210 inside init_mqtt_wss");
    webSocketsClientHandle.setReconnectInterval(2000);
    //webSocketsClientHandle

    MQTTClientHandle.begin(webSocketsClientHandle);

    while (!MQTTClientHandle.connect("esp32_client"))
    {
        delay(500);
        Serial.print("Still Connecting to MQTT");        
    }

    Serial.println("ESP32 CAM Connection To WebSocketServer Successful!");  
}


/* void publish_mqtt(char *message) that allows ESP32-CAM to
send a message through MQTT over WebSocket (SSL) with the topic
ADVTOPIC to a MQTT broker */
static void publish_mqtt(const char *topic, const char *message, size_t len)
{
    MQTTClientHandle.publish(topic, message, len);
}

static void subscribe_mqtt(const char *topic, const std::function<void(const char*, const size_t)> callback)
{
    MQTTClientHandle.subscribe(topic, callback);
}

/* void send_picture() that invokes capture_image and encodes
the captured picture in Base64. Then it invokes publish_mqtt to send
the encoded picture to the MQTT broker */
static void send_picture()
{
    /* Initialize Status & tmpimage Global Variable */
    bool status = false;
    memset(&tmpimage, 0, sizeof(camera_fb_t));
    
    status = capture_image();
    if(true == status)
    {
        String messagePrefix = String("{\"id\": 03751049, \"data\":");
        imageBase64.concat(messagePrefix);
        imageBase64.concat('\"');
        imageBase64.concat(base64::encode(tmpimage.buf, tmpimage.len));
        imageBase64.concat('\"');
        imageBase64.concat('}');

        Serial.println("ESP32 CAM Image Encoding Completed!");
        Serial.println("Publishing Message Over MQTT...");

        /* Publish The Encoded Image Over MQTT */
        publish_mqtt(topicName, imageBase64.c_str(), imageBase64.length());
        
        imageBase64.remove(0);
        free(tmpimage.buf);      
    }
    else
    {
        delay(100);
        Serial.println("ESP32 CAM Image Capture Unsuccessful!");
        HANDLE_ERROR();  
    }
}

/*******************************************************************/
/*                       CALLBACK FUNCTIONS                        */
/*******************************************************************/

void camera_capture_callback(const String &payload, const size_t size)
{
    Serial.println("ESP32 CAM Received A Camera Capture Command");
    send_picture();
}

void ADVTOPIC_ack_callback(const String &payload, const size_t size)
{
    Serial.println("ESP32 CAM Received An Acknowledgement From ADVTOPIC");
    Serial.println(payload);
}

/*******************************************************************/
/*                          SETUP & LOOP                           */
/*******************************************************************/
/* Publishing Message Over MQTT...
13:49:20.146 -> ESP32 CAM Image Capture Successful!
13:49:20.146 -> ESP32 CAM Image Encoding Completed!
*/
void setup() 
{
    /* DEBUG: Setup The Serial Port (For Debug Logs) */
    Serial.begin(115200);
    Serial.println("its stuck here");
    /* Step 1: You have to set up WebSocket for MQTT communication */
    

    /* Step 2: ESP32-CAM should connect to the internet via Wi-Fi */
    init_camera();
    init_wifi();
    init_mqtt_wss();
    /* Subscribe To MQTT Topics */
    subscribe_mqtt("camera/capture", camera_capture_callback);
    subscribe_mqtt("ADVTOPIC", ADVTOPIC_ack_callback);
}

void loop() 
{
    /* Refresh Parameters */
    MQTTClientHandle.update();
    delay(500);

    /* Step 3: After ESP32-CAM has started and connected to the internet, it should send
     a greeting message using your name and matriculation number with the
     following format “AdvTopic Name/Matriculation# Start Sending” */
    publish_mqtt(topicName, greetingMessage.c_str(), greetingMessage.length());

    /* Step 4: Then, ESP32-CAM should take a picture and send it to the MQTT broker
     every 3 seconds for ten times. Take different pictures of you moving to
     mimic an intrusion detected by a surveillance camera */
    for (int index = 0; index < 10; index++)
    {
        send_picture();
        delay(3000);
    }
   
    /* Step 5: Lastly, ESP32-CAM should send an ending message “End of Sending” */
    publish_mqtt(topicName, endingMessage.c_str(), endingMessage.length());
}
