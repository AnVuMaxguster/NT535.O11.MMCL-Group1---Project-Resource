#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* classcode = "NT535.O11.MMCL";
const char* groupname = "OneOneOne";

const char* ssid = "Nexus";
const char* password = "weareone11";
const char* mqtt_server = "192.168.1.4";
const char* mqtt_user = "user3";
const char* mqtt_pwd = "33333";
const int mqtt_port = 8883;

const char* Topic = "/TopicA";

const char caCert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDUzCCAjugAwIBAgIEBm9/ZjANBgkqhkiG9w0BAQsFADBaMQswCQYDVQQGEwJW
TjEMMAoGA1UECBMDSENNMQwwCgYDVQQHEwNIQ00xDDAKBgNVBAoTA1VJVDELMAkG
A1UECxMCTkMxFDASBgNVBAMTCzE5Mi4xNjguMS40MB4XDTIzMTIxMDIwMTY0NVoX
DTI0MTIwNDIwMTY0NVowWjELMAkGA1UEBhMCVk4xDDAKBgNVBAgTA0hDTTEMMAoG
A1UEBxMDSENNMQwwCgYDVQQKEwNVSVQxCzAJBgNVBAsTAk5DMRQwEgYDVQQDEwsx
OTIuMTY4LjEuNDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKdz9b0K
HV+ZfbEXPGQMYoj3JGSMoc2u2S/kG+/7QczhH/3iLSsGtV4NDc/6KQO9YSNFy+Lg
hDb19bNxqoQd4cOmU/qOwXCwx43gFgOJzj0z7yqjCxdfmUlG6juTrphnSmSn6tCH
8eMqI0pWD8wZDLw/i4PG0B9zkkrEba8/Gn6hxcORGQnrxnG4sRMmnuuuMvorWFe4
GS7LqTmR7iMe8MOQ5W7MMPaEH0OeyK8715yZE2A2iZb3CwCY7KKJvocTnGhFm/Nu
kaK20OQnBHS1t+7Cxk1kQwdokduoyHbAdJIqiImDpKfFZIZXScXYeSwxQisP7XtC
wPw1jNBbd+1PK18CAwEAAaMhMB8wHQYDVR0OBBYEFLw/FraAM9BssleipLgywQjl
fOL4MA0GCSqGSIb3DQEBCwUAA4IBAQCIX3M/Z+32saOxK5omLz9u9YYRhSpIZNro
DDpqwlOCR1P8yCYTJj96LNkG8zhFMQNmhPqvzPO8/5/Tm/sKQAPJ1xACDcxpbMIR
PPe1hlPfAl8JHyjDEiB30V3aT/wgFwJ8Kub8KNpQc0rRtKFJ172DkKB2EQqbn6dX
RSfbSvees6melJMoGoSO5v/ssTguByuaCaYgseBxvJKlh039pTKW6LOMga3sbE6w
0XgS0byQZE72KEcLi9/KqNsp7Ecn/JVGPZmvmACAd9RefoZego5bmoEd8uV13Po8
ru3MbrrXi9EevscmWksF6LU2yim6Q1j0Madnbiz6hoXsjtRXgV+q
-----END CERTIFICATE-----
)EOF";

X509List caCertX509(caCert); 
WiFiClientSecure espClient;
PubSubClient client(espClient);

const unsigned long interval = 3000;
unsigned long previousMillis = 0;

void setup_wifi() {
  delay(500);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// MQTT: CALLBACK FROM SUBCRIBED TOPICS
void callback(char* topic, byte* payload, unsigned int length) {
  StaticJsonDocument<256> doc;
  deserializeJson(doc, payload, length);

  if (strcmp(topic, Topic) == 0) 
  {
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(),mqtt_user,mqtt_pwd)) {
      Serial.println("connected");
      // MQTT: TOPIC SUBSCRIPTION
      client.subscribe(Topic);
      //
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
// 

// SETUP
void setup() {
  Serial.begin(9600);
  setup_wifi();

  espClient.setTrustAnchors(&caCertX509);         
  espClient.allowSelfSignedCerts();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void publish_data_mqtt()
{
  // Create a JSON document
  const size_t capacity = JSON_OBJECT_SIZE(3) + 3*JSON_ARRAY_SIZE(1) + 40;
  DynamicJsonDocument doc(capacity);
  char data[100];

  // Add data to the JSON document
  doc["class"] = classcode;
  doc["group"] = groupname;

  // Create a JSON array for "members"
  JsonArray members = doc.createNestedArray("members");
  members.add(20521056);
  members.add(20521104);

  // Serialize the JSON document to char array
  serializeJson(doc, data);

  // Publish to Broker
  client.publish(Topic, data);
  doc.clear();
}

// LOOP
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long currentMillis = millis();  
  if (currentMillis - previousMillis >= interval) 
  {
    // Save the current time
    previousMillis = currentMillis;
    publish_data_mqtt();
  }
}