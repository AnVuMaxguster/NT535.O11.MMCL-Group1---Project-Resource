#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <AESLib.h>

AESLib aesLib;

#define INPUT_BUFFER_LIMIT 3072

char cleartext[INPUT_BUFFER_LIMIT] = {0}; // THIS IS INPUT BUFFER (FOR TEXT)
char ciphertext[2*INPUT_BUFFER_LIMIT] = {0}; // THIS IS OUTPUT BUFFER (FOR BASE64-ENCODED ENCRYPTED DATA)

char cleartextR[INPUT_BUFFER_LIMIT] = {0}; // THIS IS INPUT BUFFER (FOR TEXT)
char ciphertextR[2*INPUT_BUFFER_LIMIT] = {0}; // THIS IS OUTPUT BUFFER (FOR BASE64-ENCODED ENCRYPTED DATA)

byte aes_key[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };

byte aes_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

const char* ssid = "Bao";
const char* pswd = "0938003938";
const char* usr = "user3";
const char* upswd = "33333";
const char* mqtt_server =                                                                                 
const char* topic = "/TopicA";    // this is the [root topic]

long timeBetweenMessages = 1000 * 15 * 1;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;

int status = WL_IDLE_STATUS;     // the starting Wifi radio's status

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pswd);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String msg;
  for (int i = 0; i < length; i++) {
    msg+=(char)payload[i];
    // Serial.print((char)payload[i]);
  }
  // ReceivedNormal(msg);
  ReceiveEncrypted(msg);
}

void ReceiveEncrypted(String msg)
{
  Serial.println("Received Encrypted: "+msg);
  char tempcipher[msg.length()+1];
  msg.toCharArray(tempcipher, msg.length()+1);
  dencrypt_message(tempcipher);
  // Serial.println("lenstr: \""+(String)tempcipher)+"\"";
  String cutPadding="";
  for(int i=0;i<strlen(cleartextR);i++)
  {
    if((int)cleartextR[i]>=32)cutPadding+=cleartextR[i];
  }
  Serial.println("Received Decrypted: "+cutPadding);
  // Serial.println("len:"+(String)msg.length());
}
void ReceivedNormal(String msg)
{
  Serial.println(": "+msg);
}


String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}

String composeClientID() {
  uint8_t mac[6];
  WiFi.macAddress(mac);
  String clientId;
  clientId += "esp-";
  clientId += macToStr(mac);
  return clientId;
}

void reconnect() {
  // Loop until we're reconnected 
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    String clientId = composeClientID() ;
    clientId += "-";
    clientId += String(micros() & 0xff, 16); // to randomise. sort of

    // Attempt to connect
    encrypt_message(usr);
    char EncryptUSRName[2*INPUT_BUFFER_LIMIT];
    strcpy(EncryptUSRName,ciphertext);
    encrypt_message(upswd);
    char EncryptPWD[2*INPUT_BUFFER_LIMIT];
    strcpy(EncryptPWD,ciphertext);
    if (client.connect(clientId.c_str(),EncryptUSRName,EncryptPWD)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(topic, ("connected " + composeClientID()).c_str() , true );
      // ... and resubscribe
      // topic + clientID + in
      String subscription;
      subscription += topic;
      // subscription += "/";
      // subscription += composeClientID() ;
      // subscription += "/in";
      client.subscribe(subscription.c_str() );
      Serial.print("subscribed to : ");
      Serial.println(subscription);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.print(" wifi=");
      Serial.print(WiFi.status());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


uint16_t encrypt_to_ciphertext(char * msg, uint16_t msgLen, byte iv[]) {
  // Serial.println("Calling encrypt (string)...");
  int cipherlength = aesLib.get_cipher64_length(msgLen);
  aesLib.encrypt64((byte*)msg, msgLen, ciphertext, aes_key, sizeof(aes_key), iv);
  return cipherlength;
}

uint16_t decrypt_to_cleartext(char * msg, uint16_t msgLen, byte iv[]) {
  // Serial.print("Calling decrypt...; ");
  // Serial.print("[decrypt_to_cleartext] free heap: "); Serial.println(ESP.getFreeHeap());
  uint16_t dec_len = aesLib.decrypt64(msg, msgLen, (byte*)cleartextR, aes_key, sizeof(aes_key), iv);
  // Serial.print("Decrypted bytes: "); Serial.println(dec_len);
  return dec_len;
}

void aes_init() {
  aesLib.set_paddingmode((paddingMode)0);
}

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  aes_init();
}

void encrypt_message(String msg)
{
  char dataCharArr[msg.length()+1];
  msg.toCharArray(dataCharArr, msg.length()+1);
  sprintf(cleartext, "%s", dataCharArr);
  byte enc_iv[N_BLOCK];
  memcpy(enc_iv,aes_iv, sizeof(aes_iv));
  uint16_t len = encrypt_to_ciphertext(cleartext, strlen(dataCharArr),enc_iv);
  // Serial.print("Encrypted length = "); Serial.println(len);
}

void dencrypt_message(char data[])
{
  byte enc_iv[N_BLOCK];
  memcpy(enc_iv,aes_iv, sizeof(aes_iv));
  uint16_t dlen = decrypt_to_cleartext(data,strlen(data), enc_iv);

  // Serial.print("Decrypted cleartext in length of:\n"); Serial.println(dlen);
  // Serial.print("Decrypted cleartext:\n"); Serial.println(cleartextR);
}


int counter=0;
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();


  if (now - lastMsg > timeBetweenMessages ) {
    Serial.println("--------------------------------------------");
    String str = "test "+((String)counter);
    String payload="{\"test\":\""+str+"\"}";
    Serial.println("Before Encrypt: "+payload);
    encrypt_message(payload);
    String encrypted=(String)ciphertext;
    Serial.println("After Encrypt: "+encrypted);
    dencrypt_message(ciphertext);

    lastMsg = now;
    String pubTopic;
    pubTopic += topic ;
    // pubTopic += "/";
    // pubTopic += composeClientID();
    // pubTopic += "/out";
    Serial.print("Publish topic: ");
    Serial.println(pubTopic);
    Serial.print("Publish message: ");
    Serial.println((String)ciphertext);
    client.publish( (char*) pubTopic.c_str() , ciphertext, true );
    counter++;
    Serial.println("--------------------------------------------\n\n");
  }
}