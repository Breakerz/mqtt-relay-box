
#include <WiFi.h>
#include <PubSubClient.h>


/* change it with your ssid-password */
const char* ssid = "SSID_WIFI_NETWORK_NAME";
const char* password = "PASSWORD_WIFI";
const char* mqtt_server = "192.168.0.4";

const int RelayPins[5] = {0, 1, 2, 3, 4};
const int ButtonsPins[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
const int LedPins[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
int LedStates[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/* create an instance of PubSubClient client */
WiFiClient espClient;
PubSubClient client(espClient);

/*LED GPIO pin*/
const char led = 33;

/* topics */
#define SET_TOPIC    "esp32/set/" /* 1=on, 0=off */
#define GET_TOPIC   "esp32/get"
#define SUB_TOPICS     "esp32/#"

char STATE_TOPIC[14] = "esp32/state/?";

char pubState[2] = "0";

long lastMsg = 0;
//char msg[20];

void setRelay(int relay, int state)
{
  digitalWrite(RelayPins[relay], ( state == 0 ? LOW : HIGH ));
}

void setLed(int led, int state)
{
  LedStates[led] = state;
  digitalWrite(LedPins[led], ( state == 0 ? LOW : HIGH ));
  if(led<5){
    setRelay(led, state);
  }

}
void getLed(int led)
{
     snprintf (pubState, 1, "%i", LedStates[led]);
     //pubState[0] = char(LedStates[led] + '0');
     STATE_TOPIC[12] = char(led + '0');
     client.publish(STATE_TOPIC, pubState);
}

void receivedCallback(char* topic, byte* payload, unsigned int length) {
  String strTopic((char*)topic);

  if (strTopic.startsWith(SET_TOPIC))
  {
    int led = strTopic.substring(strTopic.length()-2).toInt();
    //todo : check if between 0 and 9
    setLed(led,payload[0]-'0');
  }
  if (strTopic.startsWith(GET_TOPIC))
  {
    int led = strTopic.substring(strTopic.length()-2).toInt();
    //todo : check if between 0 and 9
    getLed(led);
  }

  Serial.print("Message received: ");
  Serial.println(topic);

  Serial.print("payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
}


//
// // searches for the string sfind in the string str
// // returns 1 if string found
// // returns 0 if string not found
// char StrContains(char *str, char *sfind)
// {
//     char found = 0;
//     char index = 0;
//     char len;
//
//     len = strlen(str);
//
//     if (strlen(sfind) > len) {
//         return 0;
//     }
//     while (index < len) {
//         if (str[index] == sfind[found]) {
//             found++;
//             if (strlen(sfind) == found) {
//                 return 1;
//             }
//         }
//         else {
//             found = 0;
//         }
//         index++;
//     }
//     return 0;
// }


void mqttconnect() {
  /* Loop until reconnected */
  while (!client.connected()) {
    Serial.print("MQTT connecting ...");
    /* client ID */
    String clientId = "ESP32Client";
    /* connect now */
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      /* subscribe topic with default QoS 0*/
      client.subscribe(SUB_TOPICS);
    } else {
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      /* Wait 5 seconds before retrying */
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  /* set led as output to control led on-off */
  pinMode(led, OUTPUT);

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  /* configure the MQTT server with IPaddress and port */
  client.setServer(mqtt_server, 1883);
  /* this receivedCallback function will be invoked
  when client received subscribed topic */
  client.setCallback(receivedCallback);
  /*start DHT sensor */
//  dht.begin();
}
void loop() {
  /* if client was disconnected then try to reconnect again */
  if (!client.connected()) {
    mqttconnect();
  }
  /* this function will listen for incomming
  subscribed topic-process-invoke receivedCallback */
  client.loop();
  /* we measure temperature every 3 secs
  we count until 3 secs reached to avoid blocking program if using delay()*/
  long now = millis();
  if (now - lastMsg > 3000) {
    lastMsg = now;

    /* read DHT11/DHT22 sensor and convert to string */
//    temperature = dht.readTemperature();
//    if (!isnan(temperature)) {
//      snprintf (msg, 20, "%lf", temperature);
      /* publish the message */
//      client.publish(TEMP_TOPIC, msg);

    }
  }
