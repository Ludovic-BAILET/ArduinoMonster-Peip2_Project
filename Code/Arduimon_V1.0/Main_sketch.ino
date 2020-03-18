#include "Pokemon.h"
#include <ILI9341_t3.h> // Hardware specific library, a modified version adapted
#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>

#define TFT_DC  9
#define TFT_CS 10
#define TFT_RST 8
#define SD_CS 4
ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC, 8);

#define blueSerial Serial1

                                                  /*To be ble to play music, we need to setup some commands
                                                  in bytes, to use them in a serial object which is the
                                                  mp3 player itself. This is necessary to play any sound.
                                                  We also need to declare this serial object, using one of
                                                  the several serial ports the teensy 4.0 offer to us.*/
////////////////////////////////////////////////////////////////////MP3 SOUND PLAYER////////////////////////////////////////////////////////////////////

#define MP3Serial Serial4

static int8_t Send_buf[8] = {0} ;//The MP3 player undestands orders in a 8 int string
                                 //0X7E FF 06 command 00 00 00 EF;(if command =01 next song order) 
#define NEXT_SONG 0X01 
#define PREV_SONG 0X02 

#define CMD_PLAY_W_INDEX 0X03 //DATA IS REQUIRED (number of song)

#define VOLUME_UP_ONE 0X04
#define VOLUME_DOWN_ONE 0X05
#define CMD_SET_VOLUME 0X06//DATA IS REQUIRED (number of volume from 0 up to 30(0x1E))
#define SET_DAC 0X17
#define CMD_PLAY_WITHVOLUME 0X22 //data is needed  0x7E 06 22 00 xx yy EF;(xx volume)(yy number of song)

#define CMD_SEL_DEV 0X09 //SELECT STORAGE DEVICE, DATA IS REQUIRED
#define DEV_TF 0X02 //HELLO,IM THE DATA REQUIRED
                
#define SLEEP_MODE_START 0X0A
#define SLEEP_MODE_WAKEUP 0X0B

#define CMD_RESET 0X0C//CHIP RESET
#define CMD_PLAY 0X0D //RESUME PLAYBACK
#define CMD_PAUSE 0X0E //PLAYBACK IS PAUSED

#define CMD_PLAY_WITHFOLDER 0X0F//DATA IS NEEDED, 0x7E 06 0F 00 01 02 EF;(play the song with the directory \01\002xxxxxx.mp3

#define STOP_PLAY 0X16

#define PLAY_FOLDER 0X17// data is needed 0x7E 06 17 00 01 XX EF;(play the 01 folder)(value xx we dont care)

#define SET_CYCLEPLAY 0X19//data is needed 00 start; 01 close

#define SET_DAC 0X17//data is needed 00 start DAC OUTPUT;01 DAC no output
                                                           
////////////////////////////////////////////////////////////////////MP3 SOUND PLAYER////////////////////////////////////////////////////////////////////


                                                  /*To hold values related to maps   and sprites in general,
                                                  a viable way to draw fast images on the TFT display is to
                                                  use FLASH bitmap, what's an array of 16-bit colored value.
                                                  Then we'll just have to read each color and write it (or not),
                                                  maybe depending on the location of Peter or anytyhing.
                                                  We could also use SD image, and of course we'll use SD image,
                                                  but only to draw slower : and the MAP and caracter's sprites
                                                  need to be drawn as fast as possible. Here are the FLASH bitmap.*/                                                           
////////////////////////////////////////////////////////////////////BITMAP FLASH SPRITES////////////////////////////////////////////////////////////////////

extern uint16_t MAP [] ;

extern uint16_t PETER0 [];extern uint16_t PETER_GRASS0 [];
extern uint16_t PETER1 [];extern uint16_t PETER_GRASS1 [];
extern uint16_t PETER2 [];extern uint16_t PETER_GRASS2 [];
extern uint16_t PETER3 [];extern uint16_t PETER_GRASS3 [];
extern uint16_t PETER4 [];extern uint16_t PETER_GRASS4 [];
extern uint16_t PETER5 [];extern uint16_t PETER_GRASS5 [];
extern uint16_t PETER6 [];extern uint16_t PETER_GRASS6 [];
extern uint16_t PETER7 [];extern uint16_t PETER_GRASS7 [];
extern uint16_t PETER8 [];extern uint16_t PETER_GRASS8 [];
extern uint16_t PETER9 [];extern uint16_t PETER_GRASS9 [];

extern uint16_t BATTLE_BACKGROUND1 [];
extern uint16_t BATTLE_ENNEMYSPRITEBACKGROUND [];
extern uint16_t BATTLE_MYSPRITEBACKGROUND [];
extern uint16_t BATTLE_ENNEMYSLIFEBARBACKGROUND [];
extern uint16_t BATTLE_MYSLIFEBARBACKGROUND [];
extern uint16_t BATTLE_ENNEMYLIFEBAR [];
extern uint16_t BATTLE_MYLIFEBAR [];
extern uint16_t BATTLE_THROWBACKGROUND [];

////////////////////////////////////////////////////////////////////BITMAP FLASH SPRITES////////////////////////////////////////////////////////////////////
                                                  /*We have to know exactyly is our character can walk or not
                                                  on a tile, what we didn't master so far. To do this, we hold
                                                  the information of every tiles in a double dimesion array, a
                                                  matrice, and we'll use it for collisions and stuff like PNJ.*/
//////////////////////////////////////////////////////////////////////COLLISIONS ARRAY//////////////////////////////////////////////////////////////////////

extern uint8_t COLLISION [25][25] ;

//////////////////////////////////////////////////////////////////////COLLISIONS ARRAY//////////////////////////////////////////////////////////////////////
                                                  /*We need alot of variables here to run some functions, because 
                                                  of the looping stuff another time Theses variables are also used
                                                  in the library, that's why we need to use alot of extern variables.*/
//////////////////////////////////////////////////////////////////////OBJECT DATAVALUES/////////////////////////////////////////////////////////////////////

const int objectNumber = 4;
extern String ObjectName[objectNumber];

ObjetOverworld objetOverworld [objectNumber] {
  ObjetOverworld(0, 3, 0, 12, 5),
  ObjetOverworld(0, 3, 0, 12, 4),
  ObjetOverworld(0, 3, 0, 13, 4),
  ObjetOverworld(0, 3, 0, 13, 3),
};

//////////////////////////////////////////////////////////////////////OBJECT DATAVALUES/////////////////////////////////////////////////////////////////////
                                                  /*We need alot of variables here to run some functions, because 
                                                  of the looping stuff another time Theses variables are also used
                                                  in the library, that's why we need to use alot of extern variables.*/
////////////////////////////////////////////////////////////////////////PNJ DATAVALUES//////////////////////////////////////////////////////////////////////

const int PNJNumber = 1;
extern String trainersLoosingText[2];

PNJOverworld PnjOverworld [PNJNumber] {
  PNJOverworld(0, 3, 0, 21, 5),
};

////////////////////////////////////////////////////////////////////////PNJ DATAVALUES//////////////////////////////////////////////////////////////////////
                                                  /*We need to create all the object to draw them correctly in the game.
                                                  We hold them in several arrays separated by the value of the area*/
//////////////////////////////////////////////////////////////////////EXTERN DATAVALUES/////////////////////////////////////////////////////////////////////

extern uint16_t musicID;
extern int previousActionPressed;
extern int previousBackPressed;
extern bool actionPressed;
extern bool actionLocked;
extern bool backPressed;
extern bool leftPressed;
extern bool previousLeftPressed;
extern bool rightPressed;
extern bool previousRightPressed;
extern bool upPressed;
extern bool previousUpPressed;
extern bool downPressed;
extern bool previousDownPressed;
extern bool startPressed;
extern int menuPhase;
extern int gamePhase;

extern int Peter_x;
extern int Peter_y;
extern int Peter_dir;
extern bool Peter_move;
extern int Peter_area;
extern int Peter_items[];

extern String chatBoxText;
extern int chatBoxTimer;
extern int chatBoxStartUse;
extern bool chatBox;
extern bool refresh;
extern bool ennemyArduimon;
extern bool ennemySprite;
extern bool myArduimon;
extern bool mySprite;

extern Equipe ennemyTeam;
extern int ennemyTeamID;
extern Equipe myTeam;

extern int ecrire1;  //Les 4
extern bool ecrire2; //variables
extern bool ecrire3; //limitant
extern bool ecrire4; //l'écriture
extern bool combat;  //un combat est-il en cours ?

extern int battleTransitionTimer;
extern bool battleTransition;
extern bool combat;

extern int battlePhase;
extern int pointer;
extern int previousPointer;
extern int ArduimonOrTrainerBackground;
extern int battleLifeBar;
extern float ennemyArduimonLifeBar;
extern float myArduimonLifeBar;
extern int SDDraw;
extern bool evolution;

//////////////////////////////////////////////////////////////////////EXTERN DATAVALUES/////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////SETUP AND LOOP FUNCTIONS//////////////////////////////////////////////////////////////////


void setup(void) {
  pinMode(SD_CS, INPUT_PULLUP);
  delay(200);

  tft.begin();
  tft.setTextColor(ILI9341_BLACK);
  Serial.begin(9600);
   
  Serial.println(F("Initializing SD card..."));
  tft.println(F("Init SD card..."));
  while (!SD.begin(SD_CS)) {
    Serial.println(F("failed to access SD card!"));
    tft.println(F("failed to access SD card!"));
    delay(2000);
  };

  Serial.println("Serial OK !");

  MP3Serial.begin(9600);//Start our Serial coms for THE MP3
  sendCommand(CMD_SEL_DEV, DEV_TF);//select the TF card  
  delay(200);//wait for 200ms
  musicID = 0X0F01;

  Serial.println("MP3 OK !");

  blueSerial.begin(38400);
  
  Serial.println("Bluetooth OK !");


  pinMode(15,INPUT);
  pinMode(18,INPUT);
  pinMode(19,INPUT);
  pinMode(20,INPUT);
  pinMode(21,INPUT);
  pinMode(22,INPUT);
  pinMode(23,INPUT);
  
  addItemsOnMap();
  addPNJOnMap();
      myTeam.pokemons[0].setActualStats(0,1);
      float actualLife = myTeam.pokemons[myTeam.pokemonFrontID].getActualStats(0);
      float maxLife = myTeam.pokemons[myTeam.pokemonFrontID].getMaxStats(0);
      myArduimonLifeBar = (actualLife/maxLife)*100*100;
}

void loop() {
  buttonUpdate();
  
  refreshScreen();

  drawPeterMove();
  
  action();

  changeMapUpdate();
  
  drawChatBox();
  battleUpdate();
  drawBattleUpdate();

  actionPressed = false;
  backPressed = false;
  leftPressed = false;
  rightPressed = false;
  upPressed = false;
  downPressed = false;

  playMusic();

}

//////////////////////////////////////////////////////////////////SETUP AND LOOP FUNCTIONS//////////////////////////////////////////////////////////////////
