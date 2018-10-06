#include<stdlib.h>
#include "DHT.h"

#define SSID "network name" //Network adı
#define PASS "password" //şifre
#define IP "184.106.153.149" // thingspeak.com'un ip adresi
#define DHTPIN 7     // DHT'nin bağlandığı pin
#define DHTTYPE DHT11   // Kullanılan DHT tipi
#define Baud_Rate 115200 //Baud hızı
#define GREEN_LED 3 //Hata kontrolü için(Veri gönderildiğinde yeşil led yanar)
#define RED_LED 4 //Hata kontrolü için(Veri gönderilemediğinde kırmızı led yanar)
#define DELAY_TIME 60000 //Verilerin thingspeak'e gönderilme aralığı

//Get yerine post da kullanılabilir.
String GET = "GET /update?key=DG1LM2BFZ2YGZ1PT&field1=";
String FIELD2 = "&field2=";

//Diğer grafikler buraya eklenebilir.
//String FIELD3 = "&field3=";

bool updated;

DHT dht(DHTPIN, DHTTYPE);


void setup()
{
  Serial.begin(Baud_Rate);
  Serial.println("AT");
  
  delay(5000);
  
  if(Serial.find("OK")){
    //Wifi'ye bağlanma
    bool connected = connectWiFi();
    if(!connected){
      //bağlanamazsa tekrar kontrol yapar ve dener
      Error();
    }
  }else{
    Error();
  }
  
  //DHT sensörünü başlatma
  dht.begin();
}

//Değerleri alma
void loop(){
  float h = dht.readHumidity();
  // parametre true ise fahrenheit olarak alır, boşsa celcius
  float f = dht.readTemperature();
  

  // Okuma kontrolü
  if (isnan(h) || isnan(f)) {
    LightRed();
    return;
  }
  
  //Thingspeak kanalını yeni değerlerle güncelle
  updated = updateTemp(String(f), String(h));
  
  //Değer gönderilirse yeşil led,gönderilemediyse kırmızı led yanar
  if(updated){
    LightGreen();
  }else{
    LightRed();
  }
  
  //Bir sonraki post a kadar bekle
  delay(DELAY_TIME);
}

bool updateTemp(String tenmpF, String humid){
  //initialize your AT command string
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  
  //IP Adresi ve port ekle
  cmd += IP;
  cmd += "\",80";
  
  //bağlanma
  Serial.println(cmd);
  delay(2000);
  if(Serial.find("Error")){
    return false;
  }
  
  //GET komutu oluşturulması,Thingspeak POST ve GET kullanır.Burada GET kullanıldı.
  cmd = GET;
  cmd += tenmpF;
  cmd += FIELD2;
  cmd += humid;
  
  
  
  cmd += "\r\n";
  
  //AT komutlarıyla dataları gönderme
  Serial.print("AT+CIPSEND=");
  Serial.println(cmd.length());
  if(Serial.find(">")){
    //komutlarla dataların güncellenmesi
    Serial.print(cmd);
  }else{
    Serial.println("AT+CIPCLOSE");
  }
  
  if(Serial.find("OK")){
    //başarılı olduğunda son değerler thingspeake gider.
    return true;
  }else{
    return false;
  }
}
 
boolean connectWiFi(){
  //AT komutlarıyla ESP8266 mod belirleme
  Serial.println("AT+CWMODE=1");
  delay(2000);

  //bağlantı komutları
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  
  //Wifi'ye bağlan ve 5 saniye bekle
  Serial.println(cmd);
  delay(5000);
  
  //bağlanırsa true bağlanmazsa false
  if(Serial.find("OK")){
    return true;
  }else{
    return false;
  }
}

void LightGreen(){
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);  
}

void LightRed(){
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, HIGH);
}

//Ledlerle hata kontrolü
void Error(){      
  while(true){      
    LightRed();      
    delay(2000);      
    LightGreen();
    delay(2000);
  }
}
