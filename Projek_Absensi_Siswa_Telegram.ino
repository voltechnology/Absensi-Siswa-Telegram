#include <WiFi.h>
#include <UniversalTelegramBot.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Preferences.h>
#include <WiFiClientSecure.h>
#include <LiquidCrystal_I2C.h>

#define sda 5
#define reset 4
#define led 27
 
const char* ssid = "KOS KEJAWAN 24";
const char* password = "cleanhouse";

const char* token = "8378932633:AAFBloeO1FKWaVduc0t4tOlLtT-3TzYgvXs";
String id = "1473084272";

LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 mfrc522(sda, reset);
Preferences prefs;

String lastUID = "";

WiFiClientSecure client;
UniversalTelegramBot bot(token, client);

//konversi array ke HEX
String uidToString(MFRC522::Uid uid){
  String kartu = "";
    for(byte i = 0; i < uid.size; i++){
      if(uid.uidByte[i] < 0x10) kartu += "0";
      kartu += String(uid.uidByte[i], HEX);
    }
  kartu.toUpperCase();
  return kartu;
}

//menyimpan nama kartu
void simpanNama(String uid, String name){
  prefs.begin("Nama", false);
  prefs.putString(uid.c_str(), name);
  prefs.end();
}

String dapatNama(String uid){
  prefs.begin("Nama", true);
  String nama = prefs.getString(uid.c_str(), "");
  prefs.end();
  return nama;
}

//input nama
void handleNewMessages(int num) {
  for (int i = 0; i < num; i++) {
    String text = bot.messages[i].text;
    String chat_id = bot.messages[i].chat_id;

    if (text.startsWith("/nama ")) {
      String namaBaru = text.substring(6);

      if (lastUID == "") {
        bot.sendMessage(chat_id, "Tidak ada kartu");
      }else{
        simpanNama(lastUID, namaBaru);
        bot.sendMessage(chat_id, "Nama : " + namaBaru + " \nTersimpan untuk UID -> " + lastUID);
        bot.sendMessage(chat_id, "Daftar kartu berhasil‼️");
      }
    }
  }
}

void setup(){
  SPI.begin();
  lcd.init();
  lcd.backlight();
  pinMode(led, OUTPUT);

  mfrc522.PCD_Init();
  prefs.begin("rfid", false);
  prefs.clear(); //kalau gamau reset dari awal, hapus aja

  //WIFI
  lcd.setCursor(0, 0);
  lcd.print("CONNECTING WIFI"); 
  WiFi.begin(ssid, password);
  int percobaan = 0;
  while (WiFi.status() !=  WL_CONNECTED && percobaan < 30) {
    percobaan++;
  }
  lcd.setCursor(0, 1); 
  lcd.print("WIFI CONNECTED");

  client.setInsecure();

  bot.sendMessage(id, "HALLO ROPIUL 🫡");
  bot.sendMessage(id, "MENUNGGU KARTU PENDAFTAR😉");
  lcd.setCursor(0, 0);
  lcd.print("SELAMAT DATANG ");
  lcd.setCursor(0, 1); 
  lcd.print("TEMPELKAN KARTU");

  digitalWrite(led, HIGH);
  delay(500);
  digitalWrite(led, LOW);
  delay(200);
  digitalWrite(led, HIGH);
  delay(200);
  digitalWrite(led, LOW);
  }

void loop(){
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  while(numNewMessages) {
    handleNewMessages(numNewMessages);
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }

  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  String uidStr = uidToString(mfrc522.uid);
  lastUID = uidStr;

  String nama = dapatNama(uidStr);

  if (nama == "") {
    bot.sendMessage(id,"Kartu Terdeteksi 🪪");
    bot.sendMessage(id,"Pendaftar Baru 😊 \nNomor Kartu -> " + uidStr + "\nSilahkan Mengisi -> /nama (pendaftar) ");
    
    digitalWrite(led, HIGH);
    delay(800);
    digitalWrite(led, LOW);
    delay(200);
    digitalWrite(led, HIGH);
    delay(80);
    digitalWrite(led, LOW);

    lcd.clear();
    lcd.setCursor(0, 0); 
    lcd.print("KARTU TERDAFTAR");
    lcd.setCursor(0, 1); 
    lcd.print("TERIMA KASIH");
    delay(4000);

  } else {
    bot.sendMessage(id, "ABSENSI TERBARU ⬇️⬇️ \nNama  : " + nama + " \nStatus : Hadir");
    lcd.clear();
    lcd.setCursor(0, 0); 
    lcd.print("Nama   -> " + nama);
    lcd.setCursor(0, 1); 
    lcd.print("Status -> HADIR");

    digitalWrite(led, HIGH);
    delay(500);
    for (int i = 0; i < 3; i++){
    digitalWrite(led, LOW);
    delay(200);
    digitalWrite(led, HIGH);
    delay(200);
    digitalWrite(led, LOW);
  }

  while(mfrc522.PICC_IsNewCardPresent() || mfrc522.PICC_ReadCardSerial()) {
    delay(100);
    }
  }
  delay(4000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SELAMAT DATANG   ");
  lcd.setCursor(0, 1);
  lcd.print("TEMPELKAN KARTU");
}
