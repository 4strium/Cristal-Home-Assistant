#include <Arduino.h>
#include <U8g2lib.h>
#include <stdio.h>
#include <Wire.h>
#include <pwm.h>
#include <open-meteo.h>
#include <WiFi.h>
#include <biticons.cpp>
#include <wavserv.h>
#include <rec-sound.h>
#include <date-heure.h> 
#include <gasdk.h>
#include <open-news-api.h>

#define I2C_SDA 27
#define I2C_SCL 22
#define BLUE_LED 12
#define BTN_PIN 4
#define TRIG_PIN 15
#define ECHO_PIN 2
#define SOUND_SPEED 0.034

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R2, /* reset=*/ U8X8_PIN_NONE);

// Variables :
char input_refl;
char input;

long duration;
float distanceCm;

int count_inactivity = 0;
bool actived = false;
int passive_counter = 0;

String API_key;
String Device_Id;
String Model_Id;

class Eyes {
private :
  int x1, x2, y, width, height;
public :
  int step;
  void initialize(){
    x1 = 8;
    x2 = 70;
    y = 24;
    height = 14;
    width = 50;
    step = 0;
  }

  void det_eyes_in(){
    for (int brightness = 0; brightness < 256; brightness+=15){
      u8g2.clearBuffer();
      u8g2.setContrast(brightness);
      u8g2.drawRBox(x1, y, width, height, 7);
      u8g2.drawRBox(x2, y, width, height, 7);
      u8g2.sendBuffer();
    }
    step = 1;
  }

  void det_eyes_out(){
    for (int brightness = 255; brightness >= 0; brightness-=15){
      u8g2.clearBuffer();
      u8g2.setContrast(brightness);
      u8g2.drawRBox(x1, y, width, height, 7);
      u8g2.drawRBox(x2, y, width, height, 7);
      u8g2.sendBuffer();
    }
    step = 0;
  }

  void clign_in(){
    if (step == 2){ 
      for (int i = 0; i < 12; i++){
        u8g2.clearBuffer();
        u8g2.drawRBox(x1, y, width, height, 7);
        u8g2.drawRBox(x2, y, width, height, 7);
        u8g2.sendBuffer();
        y += 1;
        height -= 2;
      };
      step = 1;
    }
  } 

  void clign_out(){
    if (step == 1){
      for (int i = 0; i < 12; i++){
        u8g2.clearBuffer();
        u8g2.drawRBox(x1, y, width, height, 7);
        u8g2.drawRBox(x2, y, width, height, 7);
        u8g2.sendBuffer();
        y -= 1;
        height += 2;
      };
      step = 2;
    }
  }

/*
  void round_in(){
    if (step == 0){
      for (int i = 0; i < 16; i++){
        u8g2.clearBuffer();
        u8g2.drawRBox(x1, y, width, height, 7);
        u8g2.drawRBox(x2, y, width, height, 7);
        u8g2.sendBuffer();
        y += 1;
        height -= 1;
        x1 += 1;
        x2 += 1;
        width -= 2;
      };
      step = 2;
    }
  }

  void round_out(){
    if (step == 2){
      for (int i = 0; i < 16; i++){
        u8g2.clearBuffer();
        u8g2.drawRBox(x1, y, width, height, 7);
        u8g2.drawRBox(x2, y, width, height, 7);
        u8g2.sendBuffer();
        y -= 1;
        height += 1;
        x1 -= 1;
        x2 -= 1;
        width += 2;
      };
      step = 0;
      digitalWrite(BLUE_LED, LOW);
    }
  }

  void look_right(){
    if (step == 2){
      for (int i = 0; i < 14; i++){
        u8g2.clearBuffer();
        u8g2.drawRBox(x1, y, width, height, 7);
        u8g2.drawRBox(x2, y, width, height, 7);
        u8g2.sendBuffer();
        x1 += 4;
        x2 += 1;
      }
      delay(1000);
      for (int i = 0; i < 14; i++){
        u8g2.clearBuffer();
        u8g2.drawRBox(x1, y, width, height, 7);
        u8g2.drawRBox(x2, y, width, height, 7);
        u8g2.sendBuffer();
        x1 -= 4;
        x2 -= 1;
      }
    }
  }

  void look_left(){
    if (step == 2){
      for (int i = 0; i < 14; i++){
        u8g2.clearBuffer();
        u8g2.drawRBox(x1, y, width, height, 7);
        u8g2.drawRBox(x2, y, width, height, 7);
        u8g2.sendBuffer();
        x1 -= 1;
        x2 -= 4;
      }
      delay(1000);
      for (int i = 0; i < 14; i++){
        u8g2.clearBuffer();
        u8g2.drawRBox(x1, y, width, height, 7);
        u8g2.drawRBox(x2, y, width, height, 7);
        u8g2.sendBuffer();
        x1 += 1;
        x2 += 4;
      }
    }
  }

  void look_up(){
    if (step == 2){ 
      for (int i = 0; i < 20; i++){
        u8g2.clearBuffer();
        u8g2.drawRBox(x1, y, width, height, 7);
        u8g2.drawRBox(x2, y, width, height, 7);
        u8g2.sendBuffer();
        x1 += 1;
        x2 -= 1;
        y -= 1;
      }
      delay(1000);
      for (int i = 0; i < 20; i++){
        u8g2.clearBuffer();
        u8g2.drawRBox(x1, y, width, height, 7);
        u8g2.drawRBox(x2, y, width, height, 7);
        u8g2.sendBuffer();
        x1 -= 1;
        x2 += 1;
        y += 1;
      }
    }
  }

  void look_down(){
    if (step == 2){
      for (int i = 0; i < 20; i++){
        u8g2.clearBuffer();
        u8g2.drawRBox(x1, y, width, height, 7);
        u8g2.drawRBox(x2, y, width, height, 7);
        u8g2.sendBuffer();
        x1 += 1;
        x2 -= 1;
        y += 1;
      }
      delay(1000);
      for (int i = 0; i < 20; i++){
        u8g2.clearBuffer();
        u8g2.drawRBox(x1, y, width, height, 7);
        u8g2.drawRBox(x2, y, width, height, 7);
        u8g2.sendBuffer();
        x1 -= 1;
        x2 += 1;
        y -= 1;
      }
    }
  }
*/
};

class Meteo_aff {
  public :
  int wmo_code, dn_cond;
  float temp;
  mutable const char* location;

  void print_4user(){
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_helvB18_te);
    u8g2.setFontDirection(2);
    u8g2.enableUTF8Print();
    char buffer_te[8];
    snprintf(buffer_te, sizeof(buffer_te), "%.1f°C", temp);
    u8g2.drawUTF8(70,5,buffer_te);

    u8g2.setFont(u8g2_font_helvB14_te);
    u8g2.drawUTF8(125,40,location);

    u8g2.setBitmapMode(1);

    if (wmo_code == 0 && dn_cond == 1){
      u8g2.drawXBM(85, 2, 32, 32, epd_bitmap_soleil);
    }
    else if ((wmo_code == 0 || wmo_code == 1 || wmo_code == 2) && dn_cond == 0){
      u8g2.drawXBM(85, 2, 32, 32, epd_bitmap_lune);
    }
    else if ((wmo_code == 1 || wmo_code == 2) && dn_cond == 1){
      u8g2.drawXBM(85, 2, 32, 32, epd_bitmap_ciel_clair);
    }
    else if (wmo_code == 3){
      u8g2.drawXBM(85, 2, 32, 32, epd_bitmap_nuage);
    }
    else if (wmo_code == 45 || wmo_code == 48 || wmo_code == 51 || wmo_code == 53 || wmo_code == 55 || wmo_code == 56 || wmo_code == 57){
      u8g2.drawXBM(85, 2, 32, 32, epd_bitmap_brumeux);
    }
    else if (wmo_code == 61 || wmo_code == 63 || wmo_code == 65 || wmo_code == 66 || wmo_code == 67 || wmo_code == 80 || wmo_code == 81 || wmo_code == 82){
      u8g2.drawXBM(85, 2, 32, 32, epd_bitmap_pluie_abondante);
    }
    else if (wmo_code == 71 || wmo_code == 73 || wmo_code == 75 || wmo_code == 77 || wmo_code == 85 || wmo_code == 86){
      u8g2.drawXBM(85, 2, 32, 32, epd_bitmap_flocon_de_neige);
    }
    else if (wmo_code == 95 || wmo_code == 96 || wmo_code == 99){
      u8g2.drawXBM(85, 2, 32, 32, epd_bitmap_orage);
    }

    u8g2.sendBuffer();
  }

  void setLocation(const char* newLocation) const {
    location = newLocation;
  }
};

class Timer {
  private :
    int requ_time, minutes, secondes;
    bool already_act;

    void set_normal_time(void){
      if (requ_time >= 60){
        minutes = requ_time / 60;
        secondes = requ_time - (minutes*60);
      }
      else{
        minutes = 0;
        secondes = requ_time;
      }
    }

    void start_timer(void){
      if (!already_act){
        already_act = true;
        char buffer_time_curr[5];
        while (requ_time > 0){
          u8g2.clearBuffer();
          u8g2.setFont(u8g2_font_helvB18_te);
          u8g2.setFontDirection(2);
          u8g2.enableUTF8Print();
          u8g2.drawUTF8(65,40,"Timer");

          u8g2.setFont(u8g2_font_logisoso24_tr);

          snprintf(buffer_time_curr, sizeof(buffer_time_curr), "%i:%02i", minutes, secondes);
          u8g2.drawUTF8(65,5,buffer_time_curr);

          u8g2.setBitmapMode(1);
          if (requ_time % 4 == 0){
            u8g2.drawXBM(66, 2, 64, 64, epd_bitmap_sablier_top);
          }
          else if (requ_time % 4 == 1){
            u8g2.drawXBM(66, 2, 64, 64, epd_bitmap_sablier_left);
          }
          else if (requ_time % 4 == 2){
            u8g2.drawXBM(66, 2, 64, 64, epd_bitmap_sablier_bottom);
          }
          else {
            u8g2.drawXBM(70, 2, 64, 64, epd_bitmap_sablier_right);
          }

          requ_time--;
          u8g2.sendBuffer();
          set_normal_time();
          delay(1000);
        }       
      }
    }
  public :
    Timer(int inp_t){
      requ_time = inp_t;
      already_act = false;
      set_normal_time();
      start_timer();
    }
};

void bonjour_func(){
  for (int i=0;i<25;i++){
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_helvB18_te);
    u8g2.setFontDirection(2);
    u8g2.enableUTF8Print();

    u8g2.setFont(u8g2_font_lubB14_tr);
    u8g2.drawUTF8(110,45,"Bonjour !");

    u8g2.setBitmapMode(1);
    
    if (i % 2 == 0){
      u8g2.drawXBM(50, 2, 32, 32, epd_bitmap_smiley);
    }
    else {
      u8g2.drawXBM(50, 2, 32, 32, epd_bitmap_sourire);
    }
    u8g2.sendBuffer();
    delay(300);
  }
}

void print_datetime(){
  u8g2.clearBuffer();
  u8g2.setFontDirection(2);
  u8g2.enableUTF8Print();

  u8g2.setFont(u8g2_font_logisoso20_tr);
  String timec = get_time();
  u8g2.drawUTF8(96,30,timec.c_str());

  u8g2.setFont(u8g2_font_logisoso18_tr);
  String date = get_date();
  u8g2.drawUTF8(120,2,date.c_str());

  u8g2.sendBuffer();
  delay(8000);
}

float get_distance_ultrasonic(){
  // Clears the trigPin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(ECHO_PIN, HIGH);
  
  // Calculate the distance
  float distanceCm = duration * SOUND_SPEED/2;
  return distanceCm;
}

Eyes e1;

void setup(void) {
  pinMode(35,INPUT);
  Serial.begin(9600);
  Wire.begin(I2C_SDA, I2C_SCL);
  u8g2.begin();
  e1.initialize();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(BLUE_LED, OUTPUT);

  pinMode(BTN_PIN, INPUT);

  Serial.println("Entrez le SSID de votre réseau Wifi :");
  while (!Serial.available()) {
    // Wait for user input
  }
  String ssid = Serial.readString();

  Serial.println("Entrez le mot-de-passe de votre réseau Wifi :");
  while (!Serial.available()) {
    // Wait for user input
  }
  String password = Serial.readString();

  Serial.println("Entrez votre clé API pour accéder à l'Assistant Google :");
  while (!Serial.available()) {
    // Wait for user input
  }
  API_key = Serial.readString();

  Serial.println("Entrez le modèle de votre appareil :");
  while (!Serial.available()) {
    // Wait for user input
  }
  Model_Id = Serial.readString();


  Serial.println("Entrez l'identifiant unique de votre appareil :");
  while (!Serial.available()) {
    // Wait for user input
  }
  Device_Id = Serial.readString();

  // Utilisez les objets String directement, ou copiez leur contenu si nécessaire
  // Exemple : utiliser les méthodes c_str() lorsqu'il est sûr que les objets String restent en scope
  const char* ssid_cstr = ssid.c_str();
  const char* password_cstr = password.c_str();

  // Utilisation des valeurs
  Serial.println("SSID: " + ssid);
  Serial.println("Password: " + password);

  Serial.println("Connexion au réseau WiFi...");
  WiFi.begin(ssid_cstr, password_cstr);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connexion en cours...");
  }

  Serial.println("Connecté au réseau WiFi !");

}

void loop(void) {

  count_inactivity++;
  float dist = get_distance_ultrasonic();
  Serial.println(dist);
  
  if (dist < 15.0)
  {
    count_inactivity--;

    e1.det_eyes_in();
    actived = true;

    while (digitalRead(BTN_PIN) == LOW) {
      float dist = get_distance_ultrasonic();
      Serial.println(dist);

      if ((dist > 15.0)&&(passive_counter >= 20)){
        passive_counter = 0;
        break;
      }
      else if (dist > 15.0){passive_counter++;}
      delay(300);
    }

    if (digitalRead(BTN_PIN) == HIGH)
    {
      if (e1.step == 1)
      {
        xTaskCreate([](void *parameter)
        {
          e1.clign_out();
          vTaskDelete(NULL); 
        }, "task1", 2048, NULL, 5, NULL);
        xTaskCreate([](void *parameter)
        {
          vari_blue(parameter);
          vTaskDelete(NULL); 
        }, "task2", 2048, NULL, 5, NULL);
      }
      delay(500);
      record_mic();
      delay(500);
      String word = recowav();
      Serial.println(word);
      if (word == String("météo"))
        {
          Meteo_aff m1;
          m1.temp =  get_temp();
          m1.dn_cond = get_day_night();
          m1.wmo_code = get_WMO();
          m1.setLocation("Guilers");
          m1.print_4user();
          delay(10000);
        }
      else if (word == String("minuteur"))
        {
          Timer(600);
        }
      else if (word == String("bonjour"))
        {
          bonjour_func();
        }
      else if ((word == String("date"))||(word == String("heure")||(word == String("jour"))))
        {
          print_datetime();
        }
      else if (word == String("allume bureau"))
        {
          String input_phrase = String("Turn on Bureau Romain");
          exec_com_assistant(API_key,Device_Id,Model_Id,input_phrase);
        }
      else if (word == String("éteins bureau"))
        {
          String input_phrase = String("Turn off Bureau Romain");
          exec_com_assistant(API_key,Device_Id,Model_Id,input_phrase);
        }
      else if (word == String("ouvre le volet"))
        {
          String input_phrase = String("set velux romain at 100%");
          exec_com_assistant(API_key,Device_Id,Model_Id,input_phrase);
        }
      else if (word == String("ferme le volet"))
        {
          String input_phrase = String("set velux romain at 50%");
          exec_com_assistant(API_key,Device_Id,Model_Id,input_phrase);
        }
      else if (word == String("info")||word == String("infos"))
        {
          display_titles();
        }
    }

    if (e1.step == 2)
    {
      delay(1000);
      xTaskCreate([](void *parameter)
      {
        e1.clign_in();
        vTaskDelete(NULL); 
      }, "task1", 2048, NULL, 5, NULL);
      xTaskCreate([](void *parameter)
      {
        fade_out_blue(parameter);
        vTaskDelete(NULL); 
      }, "task2", 2048, NULL, 5, NULL);
    }

    delay(2000);

  }

  if (count_inactivity >= 20){
    if (actived){
      e1.det_eyes_out();
      actived = false;
    }
    count_inactivity = 0;
  }

  delay(300);
}