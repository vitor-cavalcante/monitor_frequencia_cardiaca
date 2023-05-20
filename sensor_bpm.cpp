
#include <Adafruit_GFX.h>        //Bibliotecas do display OLED
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "MAX30102.h"           //Bibliotecas do sensor MAX30102
#include "heartRate.h"          //Algoritmo de cálculo da frequência cardíaca

MAX30102 particleSensor;

const byte RATE_SIZE = 4;
byte rates[RATE_SIZE]; //Array de frequências
byte rateSpot = 0;
long lastBeat = 0; //Momento em que o último batimento ocorreu
float beatsPerMinute;
int beatAvg;

//Dimensões e reset do display OLED
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 32 
#define OLED_RESET    -1 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //Declaração do nome do display

void setup() {  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //Inicia o display OLED
  display.display();
  delay(3000);
  // Inicializa o sensor
  particleSensor.begin(Wire, I2C_SPEED_FAST); //Utiliza a porta padrão I2C, 400kHz de velocidade
  particleSensor.setup(); //Configura o sensor com as configurações padrões
  particleSensor.setPulseAmplitudeRed(0x0A); //Liga o LED vermelho para indicar o funcionamento do sensor

}

void loop() {
 long irValue = particleSensor.getIR();    //A leitura do valor IR permite saber se o dedo está, ou não, em contato com o sensor
                                           //Também detecta os batimentos
if(irValue > 7000){                                           //Se um dedo for detectado
    display.clearDisplay();                                   //Limpa o display
    display.drawBitmap(5, 5, logo2_bmp, 24, 21, WHITE);       //Desenha a imagem do coração
    display.setTextSize(2);                                   //Próximo ao coração, é mostrada a frequência mensurada em BPM
    display.setTextColor(WHITE); 
    display.setCursor(50,0);                
    display.println("BPM");             
    display.setCursor(50,18);                
    display.println(beatAvg); 
    display.display();
    
  if (checkForBeat(irValue) == true)                        //Se um batimento for detectado
  {
    display.clearDisplay();                                //Limpa o dislay
    display.drawBitmap(0, 0, logo3_bmp, 32, 32, WHITE);    //Desenha a imagem do coração
    display.setTextSize(2);                                //Ainda monstra a frequência mensurada em BPM
    display.setTextColor(WHITE);             
    display.setCursor(50,0);                
    display.println("BPM");             
    display.setCursor(50,18);                
    display.println(beatAvg); 
    display.display();
    delay(100);
    
    long delta = millis() - lastBeat;                   //Mede a duração entre dois batimentos
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);           //Cálculo dos batimentos (BPM)

    if (beatsPerMinute < 255 && beatsPerMinute > 20)               //Cálculo da média de batimentos
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Guarda a informação dentro do Array
      rateSpot %= RATE_SIZE;

      //Pega a média das leituras
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

}
  if (irValue < 7000){       //Se nenhum dedo for detectado, informa ao usuário através do display com a mensagem "Please place your finger"
     beatAvg=0;
     display.clearDisplay();
     display.setTextSize(1);                    
     display.setTextColor(WHITE);             
     display.setCursor(30,5);                
     display.println("Please Place "); 
     display.setCursor(30,15);
     display.println("your finger ");  
     display.display();
     noTone(3);
     }

}