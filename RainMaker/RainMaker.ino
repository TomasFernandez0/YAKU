#include "RMaker.h"
#include "WiFi.h"
#include <SimpleTimer.h>
#include "WiFiProv.h"
#include <wifi_provisioning/manager.h>

//------------------------------------------- Definicion de algunos valores por defecto -------------------------------------------//

#define DEFAULT_RELAY_MODE true
#define DEFAULT_Humidity 0

const int tiempo_12hs = (60*12)*60000;

float humidity_value;
float ldr_value;

// Valores del sensor de humedad

const float max_value_humidity = 550;
const float min_value_humidity = 250; 

// Valores del sensor ldr

const float max_value_ldr = 1023;
const float min_value_ldr = 0;

// BLE Credentils

const char *service_name = "PROV_12345";
const char *pop = "1234567";

//------------------------------------------- Declaracion de pines -------------------------------------------//

static uint8_t gpio_reset = 0; //boton de resetear
static uint8_t gpio_humidity = 35; //ADC35 para sensor de humedad
static uint8_t gpio_ldr = 34; //ADC34 para sensor de luminosidad
static uint8_t relay = 2; // pin que maneja el relay
bool relay_state = true;

bool wifi_connected = 0;

SimpleTimer Timer;
SimpleTimer Water_bomb_Timer;
SimpleTimer delay_5s;

//------------------------------------------- Declaracion de dispositivos -------------------------------------------//

static TemperatureSensor humidity("Humedad"); //Sensor de humedad 
static TemperatureSensor ldr("LDR"); //Sensor de luminosidad
static Switch my_switch("Relay", &relay); //relay

void sysProvEvent(arduino_event_t *sys_event)
{
  switch (sys_event->event_id) {
    case ARDUINO_EVENT_PROV_START:
#if CONFIG_IDF_TARGET_ESP32
      Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on BLE\n", service_name, pop);
      printQR(service_name, pop, "ble");
#else
      Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on SoftAP\n", service_name, pop);
      printQR(service_name, pop, "softap");
#endif
      break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      Serial.printf("\nConnected to Wi-Fi!\n");
      wifi_connected = 1;
      delay(500);
      break;
    case ARDUINO_EVENT_PROV_CRED_RECV: {
        Serial.println("\nReceived Wi-Fi credentials");
        Serial.print("\tSSID : ");
        Serial.println((const char *) sys_event->event_info.prov_cred_recv.ssid);
        Serial.print("\tPassword : ");
        Serial.println((char const *) sys_event->event_info.prov_cred_recv.password);
        break;
      }
    case ARDUINO_EVENT_PROV_INIT:
      wifi_prov_mgr_disable_auto_stop(10000);
      break;
    case ARDUINO_EVENT_PROV_CRED_SUCCESS:
      Serial.println("Stopping Provisioning!!!");
      wifi_prov_mgr_stop_provisioning();
      break;
  }
}

  //------------------------------------------- Callback del relay -------------------------------------------//

void write_callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx)
{
  const char *device_name = device->getDeviceName();
  Serial.println(device_name);
  const char *param_name = param->getParamName();
  
  if (strcmp(device_name, "Relay") == 0) //detectar relay
  {
    if (strcmp(param_name, "Power") == 0) //detectar si el relay esta prendido
    {
      Serial.printf("Received value = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
      relay_state = val.val.b;
      /*if (relay_state == false)
        digitalWrite(relay, LOW)
      else
        digitalWrite(relay, HIGH);*/
      (relay_state == false) ? digitalWrite(relay, LOW) : digitalWrite(relay, HIGH);
      param->updateAndReport(val);
    }
  }
}



void setup()
{
  
  digitalWrite(gpio_reset, HIGH);
  Serial.begin(115200);

  // Setear gpios
  pinMode(gpio_reset, INPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, DEFAULT_RELAY_MODE);

  //------------------------------------------- Declaracion del nodo y relay -------------------------------------------//
  
  Node my_node;
  my_node = RMaker.initNode("ET.36");
  
  my_switch.addCb(write_callback); // Callback del relay
  
  //------------------------------------------- Añadiendo dispositivos al nodo -------------------------------------------//
  
  my_node.addDevice(humidity);
  my_node.addDevice(ldr);
  my_node.addDevice(my_switch);


  RMaker.enableOTA(OTA_USING_PARAMS);
  RMaker.enableTZService();
  RMaker.enableSchedule();

  Serial.printf("\nStarting ESP-RainMaker\n");
  RMaker.start();

  //Intervalo de tiempo para enviar los datos del sensor
  Timer.setInterval(3000);
  Water_bomb_Timer.setInterval(tiempo_12hs);
  
  WiFi.onEvent(sysProvEvent);

#if CONFIG_IDF_TARGET_ESP32
  WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, pop, service_name);
#else
  WiFiProv.beginProvision(WIFI_PROV_SCHEME_SOFTAP, WIFI_PROV_SCHEME_HANDLER_NONE, WIFI_PROV_SECURITY_1, pop, service_name);
#endif

}


void loop()
{
  if (Timer.isReady() && wifi_connected) { // Chequea si el contador termino
    Serial.println("Sending Sensor's Data");
    //float humidity, ldr = Update_Sensor();
    Send_Sensor();
    Regado();
    Timer.reset(); // Resetar el contador
  };

  // Leer GPIO0 (boton externo para resetear)
  if (digitalRead(gpio_reset) == LOW) { //mantener boton presionado
    Serial.printf("Reset Button Pressed!\n");
    // manejo de rebote clave
    delay(100);
    int startTime = millis();
    while (digitalRead(gpio_reset) == LOW) delay(50);
    int endTime = millis();

    if ((endTime - startTime) > 10000) {
      // Si se presiona por mas de 10 segundos se resetea todo
      Serial.printf("Reset to factory.\n");
      wifi_connected = 0;
      RMakerFactoryReset(2);
    } else if ((endTime - startTime) > 3000) {
      Serial.printf("Reset Wi-Fi.\n");
      wifi_connected = 0;
      // Si se presiona entre 3 y 10 segundos se resetea el wifi
      RMakerWiFiReset(2);
    }
  }
  delay(100);
}

/*float Update_Sensor()
{
  //------------------------------------------- Actualiza el valor de los dispositivos ------------------------------------------- //
  float humidity_value = analogRead(gpio_humidity);
  float ldr_value = analogRead(gpio_ldr);

  return humidity_value, ldr_value;
}*/

void Send_Sensor()
{
  //------------------------------------------- Pasa el valor de los dispositivos a porcentaje-------------------------------------------//
  
  float humidity_mapped = map(analogRead(gpio_humidity), min_value_humidity, max_value_humidity, 100, 0); 
  float ldr_mapped =  map(analogRead(gpio_ldr), min_value_ldr, max_value_ldr, 100, 0);

  
  //------------------------------------------- Muestra por pantalla el valor del dispositivo tanto en su valor original como en su valor en porcentaje -------------------------------------------//
  
  Serial.print("Humidity - "); Serial.print(analogRead(gpio_humidity)); Serial.print(" Humedad en %"); Serial.println(humidity_mapped);
  Serial.print("Luminosidad - "); Serial.print(analogRead(gpio_ldr)); Serial.print(" Luminosidad en %"); Serial.println(ldr_mapped);

  //------------------------------------------- Envia los valores a la Rainmaker -------------------------------------------//
  
  humidity.updateAndReportParam("Temperature", humidity_mapped);
  ldr.updateAndReportParam("Temperature", ldr_mapped);
}

void Regado()
{
  //------------------------------------------- Este metodo abre la bomba de agua durante 5s cada 12hs -------------------------------------------//
  if(Water_bomb_Timer.isReady()){
    if(analogRead(gpio_ldr) > 1000 && analogRead(gpio_humidity) < 250){
      delay_5s.setInterval(5000);
      digitalWrite(relay, HIGH); relay_state = true;
      Serial.println("REGO");
      if(delay_5s.isReady()){
        Serial.println("Dejo de regar");
        digitalWrite(relay, LOW); relay_state = false;
        delay_5s.reset();
      }
    
      
    }
    Water_bomb_Timer.reset();
  }
  
}
