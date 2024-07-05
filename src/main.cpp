#include <Arduino.h>
#include <U8g2lib.h>
#include <stdio.h>
#include <Wire.h>
#include <pwm.h>
#include <open-meteo.h>
#include <WiFi.h>
#include <biticons.cpp>

#define I2C_SDA 27
#define I2C_SCL 26
#define RED_LED 14
#define BLUE_LED 12
#define BTN_PIN 33
#define TRIG_PIN 5
#define ECHO_PIN 18
#define SOUND_SPEED 0.034

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R2, /* reset=*/ U8X8_PIN_NONE);

// Variables :
char input_refl;
char input;

float current_temp = 300.0;
int current_wmo, current_dn;

const char* ssid;
const char* password;

long duration;
float distanceCm;

int count_inactivity = 0;
bool actived = false;

class Eyes {
private :
  int x1, x2, y, width, height;
public :
  int step = 10;
  void initialize(){
    x1 = 8;
    x2 = 70;
    y = 12;
    height = 36;
    width = 50;
    step = 0;
  }

  void det_eyes_in(){
    for (int brightness = 0; brightness < 256; brightness+=15){
      u8g2.clearBuffer();
      u8g2.setContrast(brightness);
      u8g2.drawRBox(x1, 24, width, 14, 7);
      u8g2.drawRBox(x2, 24, width, 14, 7);
      u8g2.sendBuffer();
    }
  }

  void det_eyes_out(){
    for (int brightness = 255; brightness >= 0; brightness-=15){
      u8g2.clearBuffer();
      u8g2.setContrast(brightness);
      u8g2.drawRBox(x1, 24, width, 14, 7);
      u8g2.drawRBox(x2, 24, width, 14, 7);
      u8g2.sendBuffer();
    }
  }

  void clign_in(){
    if (step == 0){ 
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
      y -= 1;
      height += 2;
      for (int i = 0; i < 11; i++){
        u8g2.clearBuffer();
        u8g2.drawRBox(x1, y, width, height, 7);
        u8g2.drawRBox(x2, y, width, height, 7);
        u8g2.sendBuffer();
        y -= 1;
        height += 2;
      };
      step = 0;
    }
  }

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
  Serial.println(distanceCm);
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
  pinMode(RED_LED, OUTPUT);

  pinMode(BTN_PIN, INPUT_PULLUP);

  Serial.println("Entrez le SSID de votre réseau Wifi :");
  while (!Serial.available()) {
    // Wait for user input
  }
  ssid = Serial.readString().c_str();

  Serial.println("Entrez le mot-de-passe de votre réseau Wifi :");
  while (!Serial.available()) {
    // Wait for user input
  }
  password = Serial.readString().c_str();

  Serial.println("Connexion au réseau WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connexion en cours...");
  }

  Serial.println("Connecté au réseau WiFi !");

}

void loop(void) {

  count_inactivity++;
  float dist = get_distance_ultrasonic();
  if ((dist < 15.0) || (dist>600.0))
  {
    count_inactivity--;

    e1.det_eyes_in();
    actived = true;

    while (analogRead(BTN_PIN) > 100) {
      float dist = get_distance_ultrasonic();
      if (!((dist < 15.0) || (dist>600.0))){break;}
      delay(300);
    }

    if (analogRead(BTN_PIN) < 100)
    {
      if (e1.step == 1)
      {
        xTaskCreate([](void *parameter)
                    {
        e1.clign_out();
        vTaskDelete(NULL); }, "task1", 2048, NULL, 5, NULL);
        xTaskCreate([](void *parameter)
                    {
        vari_blue(parameter);
        vTaskDelete(NULL); }, "task2", 2048, NULL, 5, NULL);
      }
      delay(2000);
    }
    while (analogRead(BTN_PIN) < 100)
    {
      if (Serial.available())
      {
        input_refl = Serial.read();
        if (input_refl == '9')
        {
          current_temp = get_temp();
          current_wmo = get_WMO();
          current_dn = get_day_night();
        }
        if (input_refl == '8')
        {
          Timer(600);
        }
      }
    }

    if (current_temp != 300.0)
    {
      Meteo_aff m1;
      m1.temp = current_temp;
      m1.dn_cond = current_dn;
      m1.wmo_code = current_wmo;
      m1.setLocation("Guilers");
      m1.print_4user();
      delay(10000);
      current_temp = 300.0;
    }

    if (e1.step == 0)
    {
      xTaskCreate([](void *parameter)
                  {
      e1.clign_in();
      vTaskDelete(NULL); }, "task1", 2048, NULL, 5, NULL);
      xTaskCreate([](void *parameter)
                  {
      fade_out_blue(parameter);
      vTaskDelete(NULL); }, "task2", 2048, NULL, 5, NULL);
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
