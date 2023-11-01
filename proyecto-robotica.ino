#include <HTTPClient.h>
#include <WiFi.h>
#include <DHT11.h>
#include <IRLibSendBase.h>    //We need the base code
#include <IRLib_HashRaw.h>    //Only use raw sender
#include <NTPClient.h> //libreria para la hora
#include <WiFiUdp.h> // libreria para la hora
//codigo para el sensor de movimiento
int inputPin = 27;             // Pin donde está conectado el sensor de radar
int motion = 0;               // Estado de movimiento
unsigned long tiempoSinMovimiento = 0;  // Variable para rastrear el tiempo sin movimiento
bool flag_Mov = false;        // Bandera de movimiento

DHT11 dht11(4);
// Definir credenciales de red
const char* ssid = "TP-LINK_6150";
const char* password = "69928956";
//const char* ssid = "GITCE";      // SSID de tu red Wi-Fi

//const char* ssid = "Infoplaza 464 UTP";

//const char* ssid = "POCO X3 Pro";
//const char* password = "HoliHolita";

int temperatura;
int valor_bd = 0; // Variable para almacenar el valor de la base de datos
int valor_bd2 = 0; // Variable para almacenar el valor de la base de datos
int newEst=0;
// Pin del LED
const int ledPin = 25; // Cambia esto al número del pin donde está conectado tu LED
const int volS =12 ; // led indicador subir temperatura
const int volB = 13; // led indicador bajar temperatura
//definir funcion de envio ir
IRsendRaw mySender;
//valor booleano para evitar envio de senales dobles
bool est=0;
//controlador para enviar solo una senal de temperatura
bool estTemp=0;
bool temperaturaModificada = false; // Variable para verificar si se ha modificado la temperatura
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
int timeZone = -5 * 3600;


void setup() {
  Serial.begin(115200);
  Serial.println(F("DHT 11 prueba de conexión con el servidor"));

  WiFi.begin(ssid, password);
  Serial.print("Conectando...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conexión OK!");
  Serial.print("IP Local: ");
  Serial.println(WiFi.localIP());
  pinMode(ledPin,OUTPUT);
  pinMode(volS,OUTPUT);
  pinMode(volB,OUTPUT);
   timeClient.begin();
  timeClient.setTimeOffset(timeZone); // Ajusta el desplazamiento horario
  //codigo para el sensor de movimiento
  pinMode(inputPin, INPUT);   // Declarar el pin del sensor como entrada

}

void loop() {
  //codigo para el sensor de movimiento
  motion = digitalRead(inputPin);  // Leer el pin de entrada

  timeClient.update();
  Serial.print("La hora es: ");
  Serial.println(timeClient.getFormattedTime());
  #define RAW_DATA_LEN 200

//apagar y encender segun horarios
  // Obtener la hora actual
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int currentSecond = timeClient.getSeconds();
  int temperatura = dht11.readTemperature();
  LecturaTH();
  Serial.println("temperatura real: ");
  Serial.println(temperatura);
  Serial.println("");
  Serial.println();
  WiFi.status();
  Serial.println(WiFi.status());
  int valorExtraido = ObtenerValorDesdeBD();
  int temperaturaMod = ObtenerTemperaturaMod();
  Serial.print("Temperatura Modificada:");
  Serial.println(temperaturaMod);
  Serial.print("Valor extraído: ");
  Serial.println(valorExtraido);


 
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;  // Crear objeto HTTP

    // Obtener datos a enviar
    String datos_a_enviar = "temperatura=" + String(temperatura) + "&valor=" + String(valor_bd);

    http.begin("http://aires-acondicionados.000webhostapp.com/php/EspPost.php");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int codigo_respuesta = http.POST(datos_a_enviar);

    if (codigo_respuesta > 0) {
      Serial.println("Código HTTP: " + String(codigo_respuesta));
      if (codigo_respuesta == 200) {
        String cuerpo_respuesta = http.getString();
        Serial.println("El servidor respondió: ");
        Serial.println(cuerpo_respuesta);
      }
    } else {
      Serial.print("Error al enviar POST, código: ");
      Serial.println(codigo_respuesta);
    }

    http.end();  // Liberar recursos
  } else {
    Serial.println("Error en la conexión WiFi");
  }
  delay(1000); // Espera 60 segundos


    if (valorExtraido == 1 && est== 0) {
    digitalWrite(ledPin, HIGH); // Enciende el LED
    Serial.print("Encendiendo led");
    //encender aire acondicionado
    
    uint16_t rawData[RAW_DATA_LEN]={
      4434, 4458, 502, 1682, 482, 574, 502, 1666, 
      498, 1686, 482, 574, 478, 598, 478, 1690, 
      502, 574, 498, 578, 502, 1682, 482, 574, 
      502, 578, 498, 1686, 482, 1682, 482, 574, 
      502, 1686, 478, 1686, 482, 574, 502, 1682, 
      482, 1686, 482, 1662, 502, 1682, 482, 1686, 
      482, 1662, 502, 574, 502, 1682, 482, 578, 
      498, 578, 502, 574, 502, 574, 502, 574, 
      502, 574, 502, 574, 502, 578, 502, 1682, 
      482, 574, 502, 574, 502, 578, 498, 578, 
      498, 578, 502, 1682, 482, 1682, 482, 578, 
      498, 1686, 482, 1682, 482, 1686, 478, 1686, 
      482, 1682, 482, 5286, 4434, 4462, 502, 1682, 
      482, 574, 502, 1686, 482, 1682, 482, 602, 
      474, 602, 478, 1662, 502, 598, 478, 598, 
      478, 1662, 498, 606, 450, 626, 454, 1686, 
      478, 1686, 482, 598, 478, 1686, 478, 1686, 
      478, 602, 478, 1686, 498, 1666, 502, 1662, 
      502, 1666, 498, 1666, 502, 1662, 502, 574, 
      502, 1666, 498, 578, 498, 578, 502, 574, 
      502, 574, 502, 578, 498, 574, 502, 578, 
      498, 578, 502, 1662, 502, 574, 502, 578, 
      498, 578, 498, 578, 502, 574, 502, 1666, 
      498, 1686, 478, 578, 498, 1686, 482, 1686, 
      478, 1686, 478, 1686, 482, 1686, 478, 1000};

     mySender.send(rawData,RAW_DATA_LEN,36);//Pass the buffer,length, optionally frequency
    est=1;

  } else if  (valorExtraido == 0 && est== 1){
    digitalWrite(ledPin, LOW);  // Apaga el LED
    Serial.print("Apagando led");
     //apagar aire acondicionado

    uint16_t rawData[RAW_DATA_LEN]={
      4438, 4454, 502, 1682, 482, 574, 502, 1682, 
      482, 1686, 506, 550, 502, 574, 502, 1682, 
      486, 574, 498, 578, 502, 1682, 482, 574, 
      502, 574, 478, 1706, 486, 1682, 458, 598, 
      502, 1682, 486, 574, 502, 1682, 482, 1682, 
      482, 1682, 482, 1686, 482, 574, 502, 1682, 
      482, 1686, 482, 1682, 482, 574, 502, 574, 
      502, 578, 498, 578, 502, 1682, 482, 574, 
      502, 574, 502, 1682, 486, 1682, 486, 1678, 
      486, 598, 478, 598, 478, 598, 478, 598, 
      478, 602, 474, 602, 470, 606, 470, 606, 
      454, 1686, 478, 1686, 482, 1686, 478, 1686, 
      478, 1686, 482, 5306, 4434, 4462, 502, 1662, 
      502, 578, 498, 1666, 502, 1662, 502, 574, 
      502, 574, 502, 1666, 498, 578, 502, 574, 
      502, 1682, 482, 574, 502, 578, 498, 1686, 
      482, 1682, 482, 574, 502, 1686, 478, 578, 
      498, 1686, 482, 1686, 478, 1686, 478, 1686, 
      478, 578, 502, 1686, 478, 1686, 478, 1690, 
      478, 598, 478, 602, 474, 602, 474, 602, 
      474, 1694, 470, 606, 470, 606, 470, 1698, 
      470, 1694, 470, 1694, 470, 606, 470, 606, 
      470, 610, 466, 610, 466, 610, 466, 610, 
      466, 610, 470, 610, 466, 1722, 442, 1722, 
      442, 1726, 442, 1722, 442, 1722, 442, 1000};

      mySender.send(rawData,RAW_DATA_LEN,36);//Pass the buffer,length, optionally frequency
      est=0;
  }

    // Verificar la hora y enviar la señal correspondiente
  if (currentHour == 7 && currentMinute == 50 && est == 0  && valorExtraido == 0) {
    digitalWrite(ledPin, HIGH); // Enciende el LED
    newEst=1;
     //encender aire acondicionado
    
    uint16_t rawData[RAW_DATA_LEN]={
      4434, 4458, 502, 1682, 482, 574, 502, 1666, 
      498, 1686, 482, 574, 478, 598, 478, 1690, 
      502, 574, 498, 578, 502, 1682, 482, 574, 
      502, 578, 498, 1686, 482, 1682, 482, 574, 
      502, 1686, 478, 1686, 482, 574, 502, 1682, 
      482, 1686, 482, 1662, 502, 1682, 482, 1686, 
      482, 1662, 502, 574, 502, 1682, 482, 578, 
      498, 578, 502, 574, 502, 574, 502, 574, 
      502, 574, 502, 574, 502, 578, 502, 1682, 
      482, 574, 502, 574, 502, 578, 498, 578, 
      498, 578, 502, 1682, 482, 1682, 482, 578, 
      498, 1686, 482, 1682, 482, 1686, 478, 1686, 
      482, 1682, 482, 5286, 4434, 4462, 502, 1682, 
      482, 574, 502, 1686, 482, 1682, 482, 602, 
      474, 602, 478, 1662, 502, 598, 478, 598, 
      478, 1662, 498, 606, 450, 626, 454, 1686, 
      478, 1686, 482, 598, 478, 1686, 478, 1686, 
      478, 602, 478, 1686, 498, 1666, 502, 1662, 
      502, 1666, 498, 1666, 502, 1662, 502, 574, 
      502, 1666, 498, 578, 498, 578, 502, 574, 
      502, 574, 502, 578, 498, 574, 502, 578, 
      498, 578, 502, 1662, 502, 574, 502, 578, 
      498, 578, 498, 578, 502, 574, 502, 1666, 
      498, 1686, 478, 578, 498, 1686, 482, 1686, 
      478, 1686, 478, 1686, 482, 1686, 478, 1000};

     mySender.send(rawData,RAW_DATA_LEN,36);//Pass the buffer,length, optionally frequency
    est = 1; // Establecer el estado como encendido
    EnviarEstado(); //enviar nuevo estado a la bd
 } else if (currentHour == 23 && currentMinute == 00 && est == 1 && valorExtraido == 1) {
    digitalWrite(ledPin, LOW); // Apaga el LED
     //apagar aire acondicionado
    newEst=0;
    uint16_t rawData[RAW_DATA_LEN]={
      4438, 4454, 502, 1682, 482, 574, 502, 1682, 
      482, 1686, 506, 550, 502, 574, 502, 1682, 
      486, 574, 498, 578, 502, 1682, 482, 574, 
      502, 574, 478, 1706, 486, 1682, 458, 598, 
      502, 1682, 486, 574, 502, 1682, 482, 1682, 
      482, 1682, 482, 1686, 482, 574, 502, 1682, 
      482, 1686, 482, 1682, 482, 574, 502, 574, 
      502, 578, 498, 578, 502, 1682, 482, 574, 
      502, 574, 502, 1682, 486, 1682, 486, 1678, 
      486, 598, 478, 598, 478, 598, 478, 598, 
      478, 602, 474, 602, 470, 606, 470, 606, 
      454, 1686, 478, 1686, 482, 1686, 478, 1686, 
      478, 1686, 482, 5306, 4434, 4462, 502, 1662, 
      502, 578, 498, 1666, 502, 1662, 502, 574, 
      502, 574, 502, 1666, 498, 578, 502, 574, 
      502, 1682, 482, 574, 502, 578, 498, 1686, 
      482, 1682, 482, 574, 502, 1686, 478, 578, 
      498, 1686, 482, 1686, 478, 1686, 478, 1686, 
      478, 578, 502, 1686, 478, 1686, 478, 1690, 
      478, 598, 478, 602, 474, 602, 474, 602, 
      474, 1694, 470, 606, 470, 606, 470, 1698, 
      470, 1694, 470, 1694, 470, 606, 470, 606, 
      470, 610, 466, 610, 466, 610, 466, 610, 
      466, 610, 470, 610, 466, 1722, 442, 1722, 
      442, 1726, 442, 1722, 442, 1722, 442, 1000};

      mySender.send(rawData,RAW_DATA_LEN,36);//Pass the buffer,length, optionally frequency
    est = 0; // Establecer el estado como apagado
    EnviarEstado(); //enviar nuevo estado a la bd
  }




  //comparacion de la temperaturas real y la extraida de la base de datos
    if (temperatura < temperaturaMod && est==1) {
    if (!temperaturaModificada) {
      subirTemperatura(); // Llama a la función para subir la temperatura
      temperaturaModificada = true; // Marca la temperatura como modificada
      delay(1000); // Ajusta el retardo según sea necesario
    }
  } else if (temperatura > temperaturaMod && est==1) {
    if (!temperaturaModificada) {
      bajarTemperatura(); // Llama a la función para bajar la temperatura
      temperaturaModificada = true; // Marca la temperatura como modificada
      delay(1000); // Ajusta el retardo según sea necesario
    }
  } else if (temperatura == temperaturaMod && est==1) {
    temperaturaModificada = false; // Restablece la variable si no se cumple ninguna condición
  }

  //controlar apagado y encendido por movimiento
    if (motion == HIGH && est==0 ) {
    flag_Mov = true;
    Serial.println("¡Movimiento detectado!");
    tiempoSinMovimiento = millis(); // Restablecer el temporizador si hay movimiento
         //encender aire acondicionado
    
    uint16_t rawData[RAW_DATA_LEN]={
      4434, 4458, 502, 1682, 482, 574, 502, 1666, 
      498, 1686, 482, 574, 478, 598, 478, 1690, 
      502, 574, 498, 578, 502, 1682, 482, 574, 
      502, 578, 498, 1686, 482, 1682, 482, 574, 
      502, 1686, 478, 1686, 482, 574, 502, 1682, 
      482, 1686, 482, 1662, 502, 1682, 482, 1686, 
      482, 1662, 502, 574, 502, 1682, 482, 578, 
      498, 578, 502, 574, 502, 574, 502, 574, 
      502, 574, 502, 574, 502, 578, 502, 1682, 
      482, 574, 502, 574, 502, 578, 498, 578, 
      498, 578, 502, 1682, 482, 1682, 482, 578, 
      498, 1686, 482, 1682, 482, 1686, 478, 1686, 
      482, 1682, 482, 5286, 4434, 4462, 502, 1682, 
      482, 574, 502, 1686, 482, 1682, 482, 602, 
      474, 602, 478, 1662, 502, 598, 478, 598, 
      478, 1662, 498, 606, 450, 626, 454, 1686, 
      478, 1686, 482, 598, 478, 1686, 478, 1686, 
      478, 602, 478, 1686, 498, 1666, 502, 1662, 
      502, 1666, 498, 1666, 502, 1662, 502, 574, 
      502, 1666, 498, 578, 498, 578, 502, 574, 
      502, 574, 502, 578, 498, 574, 502, 578, 
      498, 578, 502, 1662, 502, 574, 502, 578, 
      498, 578, 498, 578, 502, 574, 502, 1666, 
      498, 1686, 478, 578, 498, 1686, 482, 1686, 
      478, 1686, 478, 1686, 482, 1686, 478, 1000};

     mySender.send(rawData,RAW_DATA_LEN,36);//Pass the buffer,length, optionally frequency
    newEst = 1; // Establecer el estado como encendido
    EnviarEstado(); //enviar nuevo estado a la bd
    delay(4000);
  } else if (flag_Mov && millis() - tiempoSinMovimiento > 3000  && est==1) { // 600000 ms = 10 minutos
    digitalWrite(ledPin, LOW);     // Apagar el LED después de 10 minutos sin movimiento
    Serial.println("No se detectó movimiento durante 10 minutos.");
    Serial.println(tiempoSinMovimiento);
    //apagar aire acondicionado
        uint16_t rawData[RAW_DATA_LEN]={
      4438, 4454, 502, 1682, 482, 574, 502, 1682, 
      482, 1686, 506, 550, 502, 574, 502, 1682, 
      486, 574, 498, 578, 502, 1682, 482, 574, 
      502, 574, 478, 1706, 486, 1682, 458, 598, 
      502, 1682, 486, 574, 502, 1682, 482, 1682, 
      482, 1682, 482, 1686, 482, 574, 502, 1682, 
      482, 1686, 482, 1682, 482, 574, 502, 574, 
      502, 578, 498, 578, 502, 1682, 482, 574, 
      502, 574, 502, 1682, 486, 1682, 486, 1678, 
      486, 598, 478, 598, 478, 598, 478, 598, 
      478, 602, 474, 602, 470, 606, 470, 606, 
      454, 1686, 478, 1686, 482, 1686, 478, 1686, 
      478, 1686, 482, 5306, 4434, 4462, 502, 1662, 
      502, 578, 498, 1666, 502, 1662, 502, 574, 
      502, 574, 502, 1666, 498, 578, 502, 574, 
      502, 1682, 482, 574, 502, 578, 498, 1686, 
      482, 1682, 482, 574, 502, 1686, 478, 578, 
      498, 1686, 482, 1686, 478, 1686, 478, 1686, 
      478, 578, 502, 1686, 478, 1686, 478, 1690, 
      478, 598, 478, 602, 474, 602, 474, 602, 
      474, 1694, 470, 606, 470, 606, 470, 1698, 
      470, 1694, 470, 1694, 470, 606, 470, 606, 
      470, 610, 466, 610, 466, 610, 466, 610, 
      466, 610, 470, 610, 466, 1722, 442, 1722, 
      442, 1726, 442, 1722, 442, 1722, 442, 1000};

      mySender.send(rawData,RAW_DATA_LEN,36);//Pass the buffer,length, optionally frequency
    newEst = 0; // Establecer el estado como apagado
    EnviarEstado(); //enviar nuevo estado a la bd
    flag_Mov = false;
    tiempoSinMovimiento = 0;  // Reiniciar el tiempo de inactividad
    delay(4000);
  }




  delay(2000);
}

void LecturaTH() {
  // Aquí puedes agregar el código para leer la temperatura y humedad si es necesario
}

// Rutina de envío de datos por POST
void EnvioDatos() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;  // Crear objeto HTTP

    // Obtener datos a enviar
    String datos_a_enviar = "temperatura=" + String(temperatura) + "&valor=" + String(valor_bd);

    http.begin("http://aires-acondicionados.000webhostapp.com/php/EspPost.php");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int codigo_respuesta = http.POST(datos_a_enviar);

    if (codigo_respuesta > 0) {
      Serial.println("Código HTTP: " + String(codigo_respuesta));
      if (codigo_respuesta == 200) {
        String cuerpo_respuesta = http.getString();
        Serial.println("El servidor respondió: ");
        Serial.println(cuerpo_respuesta);
      }
    } else {
      Serial.print("Error al enviar POST, código: ");
      Serial.println(codigo_respuesta);
    }

    http.end();  // Liberar recursos
  } else {
    Serial.println("Error en la conexión WiFi");
  }
  delay(6000); // Espera 60 segundos
}

//funcion para enviar valo a la bd del estado cambiado segun horario
void EnviarEstado() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;  // Crear objeto HTTP

    // Obtener datos a enviar
    String datos_a_enviar = "newEst=" + String(newEst) + "&valor=" + String(valor_bd2);

    http.begin("http://aires-acondicionados.000webhostapp.com/php/enviarestadoHora.php");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int codigo_respuesta = http.POST(datos_a_enviar);

    if (codigo_respuesta > 0) {
      Serial.println("Código HTTP: " + String(codigo_respuesta));
      if (codigo_respuesta == 200) {
        String cuerpo_respuesta = http.getString();
        Serial.println("El servidor respondió: ");
        Serial.println(cuerpo_respuesta);
      }
    } else {
      Serial.print("Error al enviar POST, código: ");
      Serial.println(codigo_respuesta);
    }

    http.end();  // Liberar recursos
  } else {
    Serial.println("Error en la conexión WiFi");
  }
  delay(100); // Espera 60 segundos
}


// Función para extraer valor entero desde la base de datos utilizando GET
int ObtenerValorDesdeBD() {
  int valor = 0;

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;  // Crear objeto HTTP

    // URL del archivo PHP en el servidor que procesa la solicitud GET
    String url = "http://aires-acondicionados.000webhostapp.com/php/EspGet.php";

    http.begin(url);

    int codigo_respuesta = http.GET();

    if (codigo_respuesta > 0) {
      if (codigo_respuesta == HTTP_CODE_OK) {
        String respuesta = http.getString();
        Serial.print("Respuesta del servidor: ");
        Serial.println(respuesta);
        valor = respuesta.toInt();
      } else {
        Serial.print("Error en la solicitud GET. Código de respuesta HTTP: ");
        Serial.println(codigo_respuesta);
      }
    } else {
      Serial.println("Error en la conexión con el servidor");
    }

    http.end();  // Liberar recursos
  } else {
    Serial.println("Error en la conexión WiFi");
  }

  return valor;
}


int ObtenerTemperaturaMod() {
  int valor = 0;

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;  // Crear objeto HTTP

    // URL del archivo PHP en el servidor que procesa la solicitud GET
    String url = "http://aires-acondicionados.000webhostapp.com/php/ExtraerTempMod.php";

    http.begin(url);

    int codigo_respuesta = http.GET();

    if (codigo_respuesta > 0) {
      if (codigo_respuesta == HTTP_CODE_OK) {
        String respuesta = http.getString();
        Serial.print("Respuesta del servidor: ");
        Serial.println(respuesta);
        valor = respuesta.toInt();
      } else {
        Serial.print("Error en la solicitud GET. Código de respuesta HTTP: ");
        Serial.println(codigo_respuesta);
      }
    } else {
      Serial.println("Error en la conexión con el servidor");
    }

    http.end();  // Liberar recursos
  } else {
    Serial.println("Error en la conexión WiFi");
  }

  return valor;
}


void subirTemperatura() {
  digitalWrite(volS, HIGH); // Encender LED indicador de subir temperatura
  // Aquí debes enviar la señal IR correspondiente para subir la temperatura
  // Usa la función mySender.send con los datos de la señal IR para subir la temperatura.
  Serial.println("Subiendo temperatura");
    Serial.println("Enviando senal ++");
  delay(1000); // Ajusta el retraso según sea necesario
  digitalWrite(volS, LOW); // Apagar LED indicador de subir temperatura
  //falta controlar el delay para el envio de se;al
}

void bajarTemperatura() {
  digitalWrite(volB, HIGH); // Encender LED indicador de bajar temperatura
  // Aquí debes enviar la señal IR correspondiente para bajar la temperatura
  // Usa la función mySender.send con los datos de la señal IR para bajar la temperatura.
      Serial.println("Bajando temperatura");
    Serial.println("Enviando senal --");
  delay(1000); // Ajusta el retraso según sea necesario
  digitalWrite(volB, LOW); // Apagar LED indicador de bajar temperatura
  //falta controlar el delay para el envio de se;al
}
