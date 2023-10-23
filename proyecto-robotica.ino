#include <HTTPClient.h>
#include <WiFi.h>
#include <DHT11.h>
#include <IRLibSendBase.h>    //We need the base code
#include <IRLib_HashRaw.h>    //Only use raw sender

DHT11 dht11(4);
// Definir credenciales de red
const char* ssid = "TP-LINK_6150";
const char* password = "69928956";
//const char* ssid = "GITCE";      // SSID de tu red Wi-Fi
//const char* password = "Tabaquismo123";  // Contraseña de tu red Wi-Fi
//const char* ssid = "Infoplaza 464 UTP";
//const char* password = "1nf0Pla464";
//const char* ssid = "POCO X3 Pro";
//const char* password = "HoliHolita";

int temperatura;
int valor_bd = 0; // Variable para almacenar el valor de la base de datos
// Pin del LED
const int ledPin = 25; // Cambia esto al número del pin donde está conectado tu LED
const int volS =12 ; // led indicador subir temperatura
const int volB = 13; // led indicador bajar temperatura
//definir funcion de envio ir
IRsendRaw mySender;
//valor booleano para evitar envio de senales dobles
bool est=0;
//controlador para enviar solo una senal de temperatura
int estTem=0;



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

}

void loop() {
  #define RAW_DATA_LEN 200
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



//comparacion de la temperaturas real y la extraida de la base de datos
   if (temperatura < temperaturaMod && est==1){
    Serial.println("Subiendo temperatura");
        digitalWrite(volS, HIGH);  
    digitalWrite(volB, LOW); 
    estTem=1;
   }else if (temperatura > temperaturaMod && est==1){
    Serial.println("Bajando temperatura");
    digitalWrite(volS, LOW);  
    digitalWrite(volB, HIGH); 
    estTem=2;
   }else if (temperatura = temperaturaMod && est==1){
    Serial.println("Temperatura sincronizada");
    digitalWrite(volS, HIGH);  
    digitalWrite(volB, HIGH);
    estTem=0;

   }else{
    digitalWrite(volS, LOW);  
    digitalWrite(volB, LOW);    
   }
  




 
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;  // Crear objeto HTTP

    // Obtener datos a enviar
    String datos_a_enviar = "temperatura=" + String(temperatura) + "&valor=" + String(valor_bd);

    http.begin("http://aires-acondicionados.000webhostapp.com/EspPost.php");
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

    http.begin("http://aires-acondicionados.000webhostapp.com/EspPost.php");
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

// Función para extraer valor entero desde la base de datos utilizando GET
int ObtenerValorDesdeBD() {
  int valor = 0;

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;  // Crear objeto HTTP

    // URL del archivo PHP en el servidor que procesa la solicitud GET
    String url = "http://aires-acondicionados.000webhostapp.com/EspGet.php";

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
    String url = "http://aires-acondicionados.000webhostapp.com/ExtraerTempMod.php";

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
