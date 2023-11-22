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
int grafica=0;
int newEst=0;
int contadorRepeticiones=0;
// Pin del LED
const int ledPin = 25; //  pin LED
const int volS =12 ; // led indicador subir temperatura
const int volB = 13; // led indicador bajar temperatura
unsigned long previousMillis = 0;   // Variable para almacenar el tiempo previo
const long interval = 3600000;     // Intervalo de tiempo (una hora en milisegundos)
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
  String hora = timeClient.getFormattedTime();
  //graficar
    unsigned long currentMillis = millis();  // Obtén el tiempo actual

  // Verifica si ha pasado una hora
  if (currentMillis - previousMillis >= interval) {
    // Actualiza el tiempo previo
    previousMillis = currentMillis;

    // Llama a la función enviarDatosAPHPMyAdmin con los parámetros adecuados
    enviarDatosAPHPMyAdmin(temperatura, hora);
  }
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

    http.begin("http://acontroles.000webhostapp.com/php/EspPost.php");
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


//cambiar temperatura
        if (temperatura != temperaturaMod && est == 1) {
          if (!temperaturaModificada) {
            cambiarTemperatura(temperaturaMod);
            temperaturaModificada = true; // Marca la temperatura como modificada
            digitalWrite(volB,HIGH);
            delay(1000);
            digitalWrite(volB,LOW);

          }
        } else if (temperatura == temperaturaMod && est == 1) {
          temperaturaModificada = false; // Restablece la variable si no se cumple ninguna condición
        }
  // contadorRepeticiones=0;
  // //comparacion de la temperaturas real y la extraida de la base de datos
  //   if (temperatura < temperaturaMod && est==1) {
  //   if (!temperaturaModificada) {
  //     subirTemperatura(); // Llama a la función para subir la temperatura
  //     temperaturaModificada = true; // Marca la temperatura como modificada
  //     delay(1000); // Ajusta el retardo según sea necesario
  //   }
  // } else if (temperatura > temperaturaMod && est==1) {
  //   if (!temperaturaModificada) {
  //     bajarTemperatura(); // Llama a la función para bajar la temperatura
  //     temperaturaModificada = true; // Marca la temperatura como modificada
  //     delay(1000); // Ajusta el retardo según sea necesario
  //   }
  // } else if (temperatura == temperaturaMod && est==1) {
  //   temperaturaModificada = false; // Restablece la variable si no se cumple ninguna condición
  // }

  // //controlar apagado y encendido por movimiento
  //   if (motion == HIGH && est==0 ) {
  //   flag_Mov = true;
  //   Serial.println("¡Movimiento detectado!");
  //   tiempoSinMovimiento = millis(); // Restablecer el temporizador si hay movimiento
  //   newEst = 1; // Establecer el estado como encendido
  //   EnviarEstado(); //enviar nuevo estado a la bd
  //   delay(4000);
  // } else if (flag_Mov && millis() - tiempoSinMovimiento > 3000  && est==1) { // 600000 ms = 10 minutos
  //   digitalWrite(ledPin, LOW);     // Apagar el LED después de 10 minutos sin movimiento
  //   Serial.println("No se detectó movimiento durante 10 minutos.");
  //   Serial.println(tiempoSinMovimiento);

  //   newEst = 0; // Establecer el estado como apagado
  //   EnviarEstado(); //enviar nuevo estado a la bd
  //   flag_Mov = false;
  //   tiempoSinMovimiento = 0;  // Reiniciar el tiempo de inactividad
  //   delay(4000);
  // }



 

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

    http.begin("http://acontroles.000webhostapp.com/php/EspPost.php");
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

//funcion para crear graficas
void enviarDatosAPHPMyAdmin(int temperatura, String hora) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;  // Crear objeto HTTP

    // Construir los datos a enviar
    String data = "temperatura=" + String(temperatura) + "&hora=" + hora;

    // Especificar la URL del script PHP en tu servidor
    http.begin("http://acontroles.000webhostapp.com/php/dataGrafica.php"); // Reemplaza esto con la URL de tu script PHP

    // Agregar encabezados si es necesario
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Enviar la solicitud POST
    int httpCode = http.POST(data);

    // Verificar la respuesta del servidor
    if (httpCode > 0) {
      Serial.println("Código de estado HTTP: " + String(httpCode));
      if (httpCode == HTTP_CODE_OK) {
        String respuesta = http.getString();
        Serial.println("El servidor respondió con: ");
        Serial.println(respuesta);
      }
    } else {
      Serial.println("Error al realizar la solicitud");
    }

    // Liberar los recursos
    http.end();
  } else {
    Serial.println("Error en la conexión WiFi");
  }
  delay(1000);
}


//funcion para enviar valo a la bd del estado cambiado segun horario
void EnviarEstado() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;  // Crear objeto HTTP

    // Obtener datos a enviar
    String datos_a_enviar = "newEst=" + String(newEst) + "&valor=" + String(valor_bd2);

    http.begin("http://acontroles.000webhostapp.com/php/enviarestadoHora.php");
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
    String url = "http://acontroles.000webhostapp.com/php/EspGet.php";

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
    String url = "http://acontroles.000webhostapp.com/php/ExtraerTempMod.php";

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


void cambiarTemperatura(int temperaturaMod) {
   #define RAW_DATA_LEN 200
  
  switch(temperaturaMod) {
    case 17:{
      Serial.println("Se recibió el número 17");
        uint16_t rawData[RAW_DATA_LEN]={
        4422, 4410, 554, 1618, 522, 574, 530, 1634, 
        530, 1638, 526, 550, 502, 574, 526, 1614, 
        554, 550, 526, 550, 526, 1614, 550, 550, 
        526, 550, 530, 1610, 526, 1642, 526, 550, 
        526, 1662, 502, 1662, 526, 554, 526, 1638, 
        502, 1662, 526, 1618, 550, 1610, 554, 1638, 
        526, 1638, 530, 550, 526, 1638, 526, 550, 
        526, 550, 526, 526, 554, 550, 526, 550, 
        502, 574, 526, 550, 526, 550, 502, 574, 
        526, 550, 526, 550, 530, 550, 526, 526, 
        550, 550, 526, 1614, 550, 1642, 522, 1618, 
        550, 1618, 546, 1614, 526, 1638, 578, 1614, 
        526, 1614, 554, 5234, 4482, 4410, 554, 1614, 
        550, 526, 550, 1614, 554, 1638, 526, 550, 
        526, 550, 530, 1614, 546, 530, 550, 550, 
        526, 1638, 502, 574, 526, 550, 530, 1634, 
        502, 1642, 550, 526, 550, 1638, 526, 1618, 
        550, 550, 526, 1638, 526, 1638, 502, 1638, 
        554, 1638, 502, 1638, 550, 1614, 554, 546, 
        502, 1638, 578, 526, 526, 550, 526, 550, 
        530, 550, 522, 554, 522, 554, 578, 498, 
        526, 550, 502, 574, 554, 522, 530, 550, 
        526, 550, 526, 550, 526, 550, 526, 1614, 
        550, 1638, 526, 1618, 550, 1610, 554, 1614, 
        578, 1614, 526, 1614, 550, 1638, 526, 1000};
        mySender.send(rawData,RAW_DATA_LEN,36);//Pass the buffer,length, optionally frequency
    };
       
      break;
    case 18:{
       Serial.println("Se recibió el número 18");
        uint16_t rawData[RAW_DATA_LEN]={
        4450, 4410, 554, 1614, 550, 550, 526, 1638, 
        526, 1614, 554, 550, 526, 550, 526, 1614, 
        554, 550, 498, 550, 530, 1634, 554, 526, 
        550, 550, 502, 1638, 526, 1666, 498, 554, 
        526, 1662, 522, 1642, 526, 554, 522, 1642, 
        502, 1638, 550, 1638, 502, 1638, 554, 1610, 
        530, 1666, 526, 550, 526, 1638, 526, 526, 
        550, 550, 526, 550, 530, 522, 550, 554, 
        526, 550, 526, 550, 526, 550, 526, 550, 
        526, 1614, 554, 546, 530, 550, 526, 550, 
        526, 550, 526, 1638, 530, 1634, 502, 1666, 
        526, 526, 550, 1614, 550, 1642, 550, 1590, 
        526, 1638, 550, 5234, 4482, 4418, 550, 1610, 
        530, 550, 550, 1614, 554, 1638, 526, 550, 
        526, 526, 526, 1662, 526, 554, 526, 550, 
        526, 1610, 554, 526, 550, 550, 526, 1614, 
        550, 1618, 550, 550, 502, 1634, 554, 1614, 
        550, 554, 526, 1638, 526, 1614, 554, 1634, 
        526, 1618, 526, 1638, 550, 1614, 550, 550, 
        526, 1614, 554, 550, 502, 574, 526, 550, 
        554, 526, 522, 554, 498, 578, 554, 498, 
        550, 550, 530, 546, 502, 1638, 554, 550, 
        498, 578, 526, 522, 530, 550, 550, 1638, 
        554, 1614, 526, 1614, 554, 546, 526, 1614, 
        554, 1638, 526, 1614, 526, 1662, 526, 1000};
         mySender.send(rawData,RAW_DATA_LEN,36);//Pass the buffer,length, optionally frequency
    };

      break;
    case 19:
    {
     Serial.println("Se recibió el número 19");
         uint16_t rawData[RAW_DATA_LEN]={
        4502, 4382, 554, 1614, 550, 550, 526, 1618, 
        546, 1614, 526, 578, 522, 554, 526, 1614, 
        550, 550, 526, 526, 550, 1614, 554, 550, 
        526, 550, 502, 1662, 502, 1638, 554, 550, 
        526, 1614, 550, 1614, 554, 546, 530, 1610, 
        554, 1614, 550, 1614, 554, 1610, 526, 1642, 
        550, 1638, 530, 546, 530, 1610, 554, 550, 
        526, 550, 550, 502, 574, 526, 554, 522, 
        530, 522, 554, 550, 526, 550, 526, 1614, 
        526, 1662, 530, 550, 526, 550, 498, 578, 
        526, 550, 526, 1614, 550, 1614, 554, 550, 
        550, 526, 526, 1614, 550, 1638, 526, 1614, 
        554, 1610, 554, 5234, 4482, 4414, 578, 1590, 
        550, 550, 526, 1614, 550, 1638, 530, 546, 
        554, 526, 526, 1614, 550, 550, 526, 550, 
        530, 1610, 554, 550, 526, 550, 526, 1614, 
        550, 1638, 502, 550, 554, 1610, 554, 1610, 
        554, 550, 554, 1586, 554, 1614, 550, 1614, 
        550, 1618, 550, 1614, 522, 1666, 526, 550, 
        554, 1582, 558, 550, 526, 550, 526, 550, 
        526, 550, 526, 550, 502, 574, 530, 526, 
        550, 526, 522, 1666, 526, 1614, 554, 546, 
        530, 546, 530, 550, 526, 526, 550, 1610, 
        554, 1638, 530, 526, 550, 550, 526, 1638, 
        526, 1638, 554, 1610, 530, 1614, 526, 1000};
        mySender.send(rawData,RAW_DATA_LEN,36);//Pass the buffer,length, optionally frequency
    };
 
      break;
    case 20:{
           Serial.println("Se recibió el número 20");

        uint16_t rawData[RAW_DATA_LEN]={
        4450, 4462, 526, 1638, 526, 550, 526, 1614, 
        554, 1638, 526, 550, 502, 574, 526, 1638, 
        530, 550, 522, 554, 526, 1614, 526, 574, 
        530, 546, 526, 1614, 526, 1638, 554, 550, 
        502, 1662, 558, 1606, 526, 554, 550, 1614, 
        526, 1638, 526, 1642, 526, 1610, 550, 1614, 
        558, 1614, 522, 578, 526, 1638, 526, 526, 
        550, 550, 526, 550, 526, 554, 526, 550, 
        526, 550, 526, 550, 502, 574, 526, 1642, 
        526, 550, 526, 550, 538, 538, 522, 554, 
        506, 570, 526, 1638, 526, 1642, 526, 550, 
        526, 1614, 550, 1614, 554, 1610, 550, 1618, 
        554, 1634, 530, 5258, 4454, 4414, 554, 1610, 
        554, 550, 526, 1642, 498, 1666, 526, 550, 
        526, 550, 526, 1614, 550, 554, 522, 554, 
        526, 1638, 526, 550, 530, 546, 526, 1614, 
        554, 1610, 530, 574, 526, 1614, 550, 1638, 
        530, 522, 526, 1666, 526, 1638, 526, 1638, 
        582, 1558, 554, 1614, 554, 1634, 530, 526, 
        550, 1638, 526, 550, 526, 550, 530, 546, 
        530, 550, 526, 526, 550, 550, 502, 574, 
        502, 574, 502, 1638, 550, 554, 526, 550, 
        526, 550, 526, 550, 526, 550, 526, 1638, 
        530, 1610, 554, 550, 502, 1634, 554, 1638, 
        526, 1642, 526, 1614, 526, 1662, 526, 1000};
        mySender.send(rawData,RAW_DATA_LEN,36);//Pass the buffer,length, optionally frequency
    };
 
      break;
    case 21:{
         Serial.println("Se recibió el número 21");

      uint16_t rawData[RAW_DATA_LEN]={
        4454, 4434, 554, 1610, 554, 550, 526, 1614, 
        574, 1590, 554, 546, 554, 522, 530, 1614, 
        550, 550, 526, 550, 530, 1634, 530, 546, 
        554, 526, 526, 1610, 558, 1610, 554, 522, 
        554, 1610, 554, 1638, 530, 546, 530, 1634, 
        530, 1614, 550, 1614, 554, 1610, 554, 1610, 
        554, 1610, 558, 546, 530, 1610, 582, 522, 
        526, 550, 526, 526, 550, 550, 530, 522, 
        554, 546, 530, 550, 526, 1610, 558, 1634, 
        530, 546, 530, 522, 554, 550, 526, 550, 
        526, 550, 530, 1610, 578, 526, 550, 526, 
        526, 1614, 554, 1610, 554, 1610, 582, 1610, 
        526, 1638, 530, 5234, 4454, 4442, 550, 1614, 
        550, 550, 526, 1614, 554, 1638, 526, 550, 
        530, 546, 530, 1634, 530, 546, 530, 550, 
        526, 1638, 526, 550, 530, 546, 530, 1610, 
        554, 1610, 558, 522, 550, 1614, 554, 1638, 
        526, 550, 530, 1634, 530, 1610, 554, 1610, 
        554, 1638, 530, 1634, 530, 1610, 554, 550, 
        526, 1614, 554, 546, 530, 546, 530, 522, 
        554, 526, 550, 550, 526, 550, 526, 550, 
        530, 1638, 526, 1610, 554, 550, 530, 522, 
        554, 522, 554, 546, 530, 546, 530, 1610, 
        578, 526, 526, 550, 554, 1586, 554, 1610, 
        554, 1614, 554, 1610, 554, 1614, 550, 1000};

        mySender.send(rawData,RAW_DATA_LEN,36);//Pass the buffer,length, optionally frequency
    };
   
      break;
    case 22:{
          Serial.println("Se recibió el número 22");

        uint16_t rawData[RAW_DATA_LEN]={
        4454, 4434, 554, 1638, 526, 550, 526, 1614, 
        550, 1638, 502, 578, 498, 550, 526, 1642, 
        550, 550, 530, 546, 530, 1634, 530, 550, 
        502, 574, 550, 1614, 526, 1614, 526, 578, 
        526, 1614, 522, 1666, 526, 550, 526, 1614, 
        554, 1614, 526, 1638, 550, 1614, 550, 1642, 
        526, 1610, 558, 546, 530, 1610, 550, 526, 
        554, 550, 526, 550, 526, 550, 526, 550, 
        522, 558, 522, 550, 554, 1614, 526, 1638, 
        526, 1614, 550, 554, 526, 550, 526, 550, 
        526, 550, 526, 1614, 554, 546, 526, 554, 
        526, 550, 526, 1614, 526, 1638, 550, 1638, 
        530, 1614, 526, 5258, 4458, 4438, 550, 1614, 
        554, 550, 502, 1638, 550, 1638, 526, 550, 
        526, 526, 554, 1638, 526, 550, 526, 550, 
        502, 1662, 502, 550, 526, 578, 526, 1638, 
        502, 1638, 554, 550, 526, 1638, 498, 1666, 
        526, 550, 502, 1662, 530, 1614, 550, 1614, 
        574, 1590, 526, 1642, 550, 1614, 550, 550, 
        554, 1586, 554, 550, 522, 554, 526, 526, 
        550, 550, 526, 550, 526, 550, 526, 554, 
        526, 1614, 550, 1614, 546, 1618, 550, 530, 
        550, 550, 526, 550, 526, 626, 478, 1610, 
        554, 522, 502, 574, 554, 522, 530, 1638, 
        502, 1638, 554, 1634, 526, 1642, 502, 1000};
        mySender.send(rawData,RAW_DATA_LEN,36);//Pass the buffer,length, optionally frequency
    };
  
      break;
    case 23:{
          Serial.println("Se recibió el número 23");

        uint16_t rawData[RAW_DATA_LEN]={
        4426, 4434, 554, 1614, 550, 530, 550, 1610, 
        554, 1614, 550, 550, 530, 546, 530, 1610, 
        530, 574, 502, 574, 526, 1638, 530, 546, 
        554, 498, 554, 1614, 550, 1638, 502, 574, 
        530, 1634, 554, 1614, 502, 574, 526, 1638, 
        526, 1638, 554, 1586, 554, 1610, 554, 1614, 
        554, 1638, 554, 522, 526, 1638, 526, 550, 
        530, 522, 554, 546, 526, 554, 526, 526, 
        570, 530, 526, 550, 526, 1618, 550, 550, 
        526, 1610, 526, 578, 526, 550, 526, 550, 
        526, 550, 502, 1642, 550, 550, 526, 1614, 
        550, 550, 526, 1614, 550, 1614, 554, 1614, 
        526, 1638, 554, 5234, 4482, 4410, 558, 1610, 
        550, 550, 530, 1638, 502, 1638, 526, 574, 
        526, 550, 526, 1614, 550, 554, 526, 550, 
        526, 1614, 550, 526, 550, 554, 526, 1610, 
        530, 1662, 526, 550, 526, 1614, 550, 1618, 
        550, 550, 526, 1638, 526, 1614, 554, 1634, 
        530, 1614, 550, 1614, 550, 1614, 530, 574, 
        502, 1662, 526, 550, 526, 550, 530, 550, 
        522, 554, 502, 550, 526, 574, 530, 546, 
        526, 1638, 526, 526, 554, 1614, 550, 550, 
        526, 550, 526, 550, 526, 550, 502, 1638, 
        554, 550, 526, 1638, 526, 550, 526, 1618, 
        550, 1610, 558, 1610, 526, 1638, 554, 1000};
        mySender.send(rawData,RAW_DATA_LEN,36);//Pass the buffer,length, optionally frequency
    };
  
      break;
    case 24:{
          Serial.println("Se recibió el número 24");

        uint16_t rawData[RAW_DATA_LEN]={
        4358, 4410, 550, 1638, 526, 550, 526, 1642, 
        526, 1614, 550, 550, 526, 554, 522, 1614, 
        550, 526, 554, 550, 502, 1638, 550, 550, 
        530, 526, 550, 1610, 530, 1634, 530, 574, 
        526, 1638, 502, 1642, 550, 550, 522, 1618, 
        526, 1638, 550, 1618, 574, 1590, 602, 1558, 
        554, 1614, 526, 574, 502, 1666, 502, 550, 
        550, 554, 522, 554, 522, 554, 526, 550, 
        526, 550, 526, 550, 526, 1638, 530, 546, 
        526, 554, 526, 550, 526, 550, 526, 550, 
        502, 574, 502, 1638, 526, 578, 526, 1638, 
        526, 1638, 526, 1638, 530, 1610, 530, 1638, 
        550, 1614, 550, 5234, 4458, 4438, 554, 1614, 
        550, 550, 530, 1610, 554, 1638, 526, 526, 
        550, 550, 554, 1610, 526, 554, 526, 550, 
        502, 1638, 526, 574, 526, 550, 526, 1618, 
        522, 1642, 550, 550, 554, 1586, 550, 1618, 
        522, 578, 526, 1638, 526, 1638, 530, 1610, 
        554, 1614, 526, 1638, 526, 1638, 526, 578, 
        502, 1634, 554, 550, 526, 550, 526, 554, 
        522, 554, 514, 562, 526, 550, 526, 550, 
        526, 1614, 550, 530, 546, 530, 550, 526, 
        550, 550, 502, 574, 526, 550, 578, 1562, 
        554, 546, 506, 1662, 526, 1638, 526, 1614, 
        550, 1642, 526, 1610, 554, 1638, 530, 1000};
        mySender.send(rawData,RAW_DATA_LEN,36);//Pass the buffer,length, optionally frequency
    };
  
      break;
    case 25:{
           Serial.println("Se recibió el número 25");

      uint16_t rawData[RAW_DATA_LEN]={
      4478, 4410, 550, 1614, 554, 550, 526, 1610, 
      558, 1610, 554, 550, 526, 550, 526, 1614, 
      550, 550, 526, 554, 522, 1614, 554, 550, 
      526, 526, 554, 1634, 526, 1614, 554, 550, 
      522, 1642, 526, 1614, 526, 574, 530, 1638, 
      526, 1614, 550, 1638, 526, 1614, 554, 1614, 
      550, 1638, 502, 574, 502, 1638, 526, 578, 
      526, 550, 502, 574, 526, 550, 526, 550, 
      526, 550, 502, 1666, 526, 1638, 502, 574, 
      526, 526, 554, 546, 526, 554, 498, 578, 
      526, 550, 526, 550, 526, 550, 526, 1638, 
      502, 1638, 554, 1610, 554, 1638, 526, 1642, 
      498, 1642, 550, 5234, 4482, 4414, 550, 1642, 
      526, 550, 526, 1614, 526, 1638, 530, 574, 
      498, 578, 498, 1642, 550, 526, 550, 550, 
      502, 1638, 554, 550, 526, 550, 502, 1662, 
      526, 1638, 526, 554, 526, 1610, 578, 1590, 
      554, 546, 502, 1666, 526, 1638, 526, 1614, 
      554, 1634, 526, 1618, 550, 1638, 550, 526, 
      526, 1638, 526, 554, 550, 526, 522, 554, 
      526, 550, 502, 550, 550, 550, 530, 1610, 
      554, 1638, 526, 550, 554, 522, 526, 526, 
      550, 526, 554, 550, 526, 550, 526, 550, 
      526, 526, 550, 1614, 550, 1618, 546, 1614, 
      554, 1614, 526, 1638, 554, 1614, 550, 1000};
      mySender.send(rawData,RAW_DATA_LEN,36);//Pass the buffer,length, optionally frequency
    };
 
      break;
    case 26:{
         Serial.println("Se recibió el número 26");

      uint16_t rawData[RAW_DATA_LEN]={
      4478, 4406, 550, 1618, 526, 574, 530, 1610, 
      554, 1614, 550, 550, 526, 526, 578, 1590, 
      574, 526, 498, 554, 550, 1614, 550, 550, 
      526, 526, 550, 1618, 522, 1642, 550, 550, 
      526, 1638, 530, 1638, 522, 554, 526, 1638, 
      526, 1614, 550, 1614, 530, 1638, 578, 1586, 
      578, 1586, 526, 578, 526, 1610, 554, 550, 
      554, 522, 530, 546, 530, 550, 550, 526, 
      526, 550, 526, 1638, 530, 1634, 502, 574, 
      526, 1614, 554, 550, 526, 550, 526, 554, 
      498, 578, 550, 526, 526, 550, 526, 1614, 
      550, 550, 526, 1638, 530, 1610, 530, 1662, 
      526, 1638, 530, 5234, 4482, 4414, 550, 1614, 
      550, 550, 530, 1614, 550, 1614, 550, 526, 
      550, 550, 526, 1614, 526, 578, 534, 542, 
      526, 1614, 550, 526, 554, 546, 526, 1642, 
      526, 1634, 530, 550, 526, 1614, 550, 1614, 
      554, 550, 502, 1638, 550, 1638, 554, 1614, 
      498, 1666, 526, 1614, 550, 1614, 554, 550, 
      526, 1614, 550, 550, 526, 550, 526, 550, 
      506, 550, 546, 554, 526, 550, 526, 1638, 
      502, 1662, 530, 546, 530, 1614, 550, 550, 
      526, 550, 526, 550, 530, 550, 498, 578, 
      526, 522, 550, 1618, 526, 574, 530, 1634, 
      530, 1638, 502, 1638, 550, 1614, 550, 1000};
      mySender.send(rawData,RAW_DATA_LEN,36);//Pass the buffer,length, optionally frequency
    };
   
      break;
    case 27:{
          Serial.println("Se recibió el número 27");

      uint16_t rawData[RAW_DATA_LEN]={
      4462, 4406, 554, 1638, 526, 554, 522, 1614, 
      550, 1618, 550, 550, 526, 550, 522, 1618, 
      550, 554, 526, 550, 526, 1610, 582, 522, 
      530, 546, 526, 1642, 526, 1614, 526, 574, 
      502, 1638, 526, 1662, 530, 550, 502, 1662, 
      526, 1614, 554, 1614, 550, 1614, 574, 1614, 
      502, 1638, 578, 526, 526, 1638, 526, 550, 
      526, 526, 550, 554, 526, 546, 530, 550, 
      526, 550, 498, 1642, 526, 574, 502, 574, 
      530, 1610, 554, 550, 526, 550, 526, 550, 
      526, 550, 526, 550, 558, 1586, 578, 1610, 
      526, 550, 526, 1614, 550, 1618, 526, 1638, 
      554, 1610, 554, 5230, 4486, 4414, 550, 1614, 
      550, 550, 502, 1642, 550, 1638, 526, 550, 
      526, 550, 526, 1614, 554, 522, 554, 550, 
      526, 1614, 550, 550, 502, 574, 502, 1642, 
      550, 1638, 526, 526, 554, 1610, 550, 1614, 
      554, 550, 526, 1610, 530, 1638, 526, 1666, 
      526, 1610, 530, 1662, 502, 1662, 502, 550, 
      554, 1610, 554, 550, 526, 550, 530, 546, 
      506, 574, 498, 578, 526, 550, 526, 1638, 
      498, 578, 526, 550, 526, 1614, 554, 550, 
      526, 550, 502, 574, 530, 546, 526, 526, 
      550, 1642, 526, 1638, 526, 550, 526, 1638, 
      526, 1614, 530, 1662, 526, 1614, 554, 1000};
      mySender.send(rawData,RAW_DATA_LEN,36);//Pass the buffer,length, optionally frequency
    };
  
      break;
     case 28:{
         Serial.println("Se recibió el número 28");

      uint16_t rawData[RAW_DATA_LEN]={
      4474, 4434, 502, 1638, 530, 574, 526, 1638, 
      502, 1666, 522, 554, 526, 550, 526, 1614, 
      550, 550, 526, 550, 526, 1614, 554, 550, 
      502, 550, 574, 1586, 530, 1638, 554, 550, 
      522, 1642, 554, 1586, 550, 526, 554, 1638, 
      498, 1638, 554, 1610, 554, 1614, 554, 1638, 
      526, 1614, 550, 550, 526, 1614, 554, 526, 
      526, 574, 526, 526, 550, 550, 526, 550, 
      502, 574, 554, 1614, 502, 574, 526, 550, 
      502, 574, 554, 522, 522, 554, 502, 578, 
      522, 526, 554, 550, 526, 1610, 558, 1634, 
      530, 1614, 550, 1614, 550, 1614, 550, 1642, 
      526, 1638, 526, 5262, 4454, 4414, 578, 1614, 
      526, 522, 554, 1638, 530, 1610, 526, 578, 
      526, 550, 526, 1638, 526, 526, 554, 546, 
      530, 1638, 498, 578, 502, 574, 526, 1614, 
      526, 1638, 554, 550, 498, 1638, 554, 1614, 
      526, 578, 526, 1634, 530, 1614, 550, 1638, 
      502, 1666, 498, 1642, 550, 1610, 554, 526, 
      550, 1618, 550, 550, 526, 526, 550, 550, 
      502, 574, 526, 554, 522, 554, 498, 1642, 
      546, 530, 550, 550, 526, 550, 554, 522, 
      526, 554, 498, 578, 526, 550, 526, 550, 
      526, 1638, 526, 1642, 526, 1610, 526, 1666, 
      502, 1662, 526, 1642, 522, 1642, 502, 1000};
      mySender.send(rawData,RAW_DATA_LEN,36);//Pass the buffer,length, optionally frequency
     };
   
      break;
    case 29:{
          Serial.println("Se recibió el número 29");

      uint16_t rawData[RAW_DATA_LEN]={
      4398, 4434, 574, 1614, 502, 578, 550, 1614, 
      526, 1638, 526, 526, 526, 574, 530, 1614, 
      522, 554, 526, 574, 530, 1614, 546, 554, 
      498, 578, 526, 1638, 530, 1634, 526, 554, 
      502, 1634, 526, 1666, 502, 574, 526, 1614, 
      550, 1618, 550, 1638, 502, 1662, 530, 1614, 
      550, 1610, 554, 550, 526, 1618, 550, 550, 
      526, 526, 550, 550, 526, 550, 526, 550, 
      526, 550, 530, 1634, 502, 574, 530, 1614, 
      554, 546, 502, 574, 530, 526, 522, 578, 
      522, 554, 526, 550, 526, 1614, 578, 522, 
      526, 1642, 526, 1610, 530, 1638, 578, 1586, 
      554, 1638, 526, 5234, 4454, 4442, 550, 1642, 
      526, 550, 526, 1638, 526, 1638, 502, 574, 
      502, 578, 498, 1666, 502, 574, 526, 526, 
      578, 1586, 550, 550, 554, 526, 526, 1638, 
      526, 1614, 550, 550, 530, 1638, 526, 1614, 
      550, 550, 526, 1638, 530, 1638, 550, 1614, 
      526, 1614, 550, 1614, 526, 1638, 554, 550, 
      554, 1586, 554, 550, 522, 554, 498, 554, 
      550, 550, 502, 574, 506, 570, 526, 1642, 
      498, 578, 498, 1666, 502, 574, 526, 550, 
      526, 554, 526, 550, 498, 554, 550, 550, 
      498, 1642, 526, 574, 554, 1614, 522, 1642, 
      526, 1610, 554, 1638, 530, 1614, 550, 1000};
      mySender.send(rawData,RAW_DATA_LEN,36);//Pass the buffer,length, optionally frequency
    };
  
      break; 
    case 30:{
          Serial.println("Se recibió el número 30");

      uint16_t rawData[RAW_DATA_LEN]={
      4454, 4430, 554, 1614, 554, 522, 554, 1614, 
      574, 1586, 554, 526, 554, 546, 530, 1634, 
      530, 550, 526, 526, 550, 1614, 554, 546, 
      530, 550, 526, 1610, 554, 1638, 526, 550, 
      530, 1634, 530, 1610, 554, 526, 550, 1614, 
      554, 1634, 554, 1590, 550, 1610, 558, 1634, 
      530, 1610, 554, 550, 526, 1614, 554, 546, 
      530, 546, 530, 550, 526, 550, 526, 550, 
      530, 522, 554, 1634, 530, 550, 526, 1638, 
      526, 1610, 558, 522, 578, 502, 550, 550, 
      526, 550, 530, 546, 530, 1610, 554, 522, 
      554, 550, 530, 1610, 554, 1610, 554, 1610, 
      554, 1638, 530, 5258, 4430, 4438, 554, 1610, 
      554, 526, 550, 1614, 554, 1610, 554, 550, 
      526, 550, 530, 1610, 554, 546, 530, 550, 
      526, 1638, 530, 546, 526, 550, 526, 1614, 
      578, 1590, 550, 522, 554, 1614, 554, 1634, 
      530, 550, 526, 1614, 574, 1590, 554, 1610, 
      554, 1610, 578, 1590, 554, 1634, 530, 550, 
      526, 1638, 526, 526, 554, 546, 530, 522, 
      554, 550, 526, 550, 526, 550, 530, 1610, 
      554, 546, 554, 1610, 530, 1614, 550, 550, 
      530, 546, 530, 550, 526, 522, 554, 526, 
      550, 1614, 554, 546, 530, 546, 530, 1610, 
      554, 1638, 526, 1614, 578, 1586, 554, 1000};
      mySender.send(rawData,RAW_DATA_LEN,36);//Pass the buffer,length, optionally frequency
    };
  
      break;
    default:
      Serial.println("Numero fuera de rango prro!");
  }
}

