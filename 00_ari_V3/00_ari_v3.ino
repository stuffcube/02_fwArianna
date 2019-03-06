/*
 * questo è il file che determina il nome del progetto.
 * La struttura è Arduino Style.
 * Per la compilazione i file vengono accorpati a partire da quello con il nome 
 * del folder uno dietro l'altro in ordine alfabetico.
 * Le variabili globali 
 * gli include
 * le define
 * vengono quindi messe in questo primo file.
 * 
 * i nomi file iniziano con un numero per forzare l'ordine di accodamento.
 * 
 * nel file readMe ci sono dei commenti sul metodo.
 * 
 * il codice inizia nel mainCode
 * 
 */
#include <SPI.h>
#include <SD.h>
#include <Servo.h>
#include <TFMini.h>
#include <EEPROM.h>     // ID_006

// ID_009
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <math.h>
#include "Adafruit_VL53L0X.h"
// \ID_009




#define SCRIVI  0
#define LEGGI   1
#define DEFAULT 2

#define DUE_PI  6.28318

/* Assign a unique ID to this sensor at the same time */
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345); //ID_009

Servo servoPan;         // create servo object to control a servo
Servo servoTilt;        // create servo object to control a servo


//********** ID_002
#define ESPserial   Serial2
#define TFserial    Serial1   // LaserRadar

//creo tfmini
TFMini  tfmini;

String  inputString       	= "";
String  inputStringTmp    	= "";
int     okcomm            	= 0;
static  String risposta;
char    port            	= 0;

//**********fine ID_002

// dc motor
#include <DRV8833.h>

// Create an instance of the DRV8833:
DRV8833 driver = DRV8833();

//creo istanza vl530l0x
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// Pin numbers. Replace with your own!
// Attach the Arduino's pin numbers below to the
// Ain1 and Ain2 DRV8833 pins.
// N.B. la libreria sembra non gestire i pin 44-46 (timer diverso)
const int MTR_A_DX_P1   = 5;  /*!< output pin 1 for right motor */
const int MTR_A_DX_P2   = 6;  /*!< output pin 2 for right motor */
const int MTR_B_SX_P1   = 9;  /*!< output pin 1 for left motor */
const int MTR_B_SX_P2   = 10; /*!< output pin 2 for left motor */


#define nOfSamples  100
float ch1[nOfSamples]; 
float ch2[nOfSamples]; 
float ch3[nOfSamples]; 
float ch4[nOfSamples]; 

int sampleRate = 0;

int motorSpeedRef   = 0;
int direzione       = 1;
int dirVA         = 1;    // ID_005
int dirVB         = 1;

int VB_zero       = 0;
int VA_zero       = 0;


#define SERVO_PAN_PIN       45  ///< Digital IO pin connected to the servo pin.
#define SERVO_TILT_PIN      44  ///< Digital IO pin connected to the servo pin.

#define GIRO_DX_PIN       3   ///< encoder  rotazione albero motore ID_009
#define GIRO_SX_PIN       2   ///< encoder  rotazione albero motore ID_009

#define R_SIDE_FRONT      29  ///< sensore frontale Dx      Low = ostacolo
#define L_SIDE_FRONT      31  ///< sensore frontale Sx
#define ENB_R_SIDE_FRONT    28  ///< abilita sensore frontale Dx. High = Enable
#define ENB_L_SIDE_FRONT    30  ///< abilita frontale Sx
#define LED1            34  ///< LED1
#define LED2            33  ///< LED2
#define LED3            32  ///< LED3




#define ledPin            13  ///< led segnalazioni varie su Arduino
#define laserPin          8   ///< porta puntatore laser

#define  TEST_MOTORE        0   ///< modalità di test per fasi di debug
#define  TEST_STERZO        1   ///< modalità di test per fasi di debug
#define  TEST_SENSORS       2   ///< modalità di test per fasi di debug
#define  TEST_CONTROLLO     3   ///< modalità di test per fasi di debug
#define  TEST_GIRO_SENSOR   4   ///< modalità di test per fasi di debug

long odometroCnt, odometroDxCnt, odometroSxCnt;

char firstRun;


#define TEMPO_CONTROLLO   3   ///< tempo del controllo sterzo, posizione etc in 10ms


float ED          	= 1.0;      ///< definiscono la meccanica del robot. descrizione nella parte cinematica 
float ED_BASE       = 1.0;      ///< definiscono la meccanica del robot. descrizione nella parte cinematica  
float BASELINE      = 130.0;    ///< larghezza carreggiata in [mm]. descrizione nella parte cinematica  
float LAGHEZZA_A_MEZZI = 0.065; ///< larghezza meta' carreggiata in [m]. descrizione nella parte cinematica  

float GIRO_RUOTA_SX = 1.0;      ///< [mm per impulso*0.5]  sviluppo ruota[mm]/(2*ppr) (pulses per revolution)]. descrizione nella parte cinematica  
float GIRO_RUOTA_DX = 1.0;      ///< [mm per impulso*0.5]  sviluppo ruota[mm]/(2*ppr) (pulses per revolution)]. descrizione nella parte cinematica  
float GIRO_RUOTA  	= 1.0;      ///< (GIRO_RUOTA_SX + GIRO_RUOTA_DX)/2. descrizione nella parte cinematica  

unsigned long MIN_TIME_TRA_PULSE = 1;     ///< tempo minimo tra impulsi encoder per evitare errate letture [ms]
     ///< contatori encoder

#define E_POSIZIONAMENTO    10    ///< [mm], distanza dall'obbiettivo dove il robot si arresta
#define E_APPROCCIO       300   ///< [mm], distanza dall'obbiettivo dove il robot inizia a rallentare

/*
  MODERATA    è il pwm di movimento normale
  APPROCCIO     durante l'avvicinamento alla poszione
  ACCELERAZIONE   nella fase del partenza
*/
#define FERMO         0     ///< valore pwm di uscita durante stato di FERMO
#define MODERATA      190     ///< valore pwm di uscita durante marcia MODERATA
#define APPROCCIO     110     ///< valore pwm di uscita durante marcia APPROCCIO
#define ACCELERAZIONE   160     ///< valore pwm di uscita durante fase accelerazione (non usato)
#define AVANTI      0
#define INDIETRO    1

#define MAX_STERZO  1.0         ///< massimo valore di sterzo. con 1 si ha una ruota ferma e l'altra al doppio della velocita'

// parametri lettura e/o scrittura
int   statoRun      = 0;        ///< macchina a stati del movimento. Definisce anche il tipo controllo guida.
float   odometro    = 0;        ///< distanza percorsa in mm
float   distanza    = 0;        ///< distanza da percorrere col prossio Run
float   raggiorSterzo = 0.0;    ///< raggiorSterzo indica lo scorrimento che applichiamo alle ruote. vedi function differenziale
int     lidardist     = 0;      ///< [cm] distanza misurata dal Lidar
float   errore;

int   motorSpeed    = 0;
int   motorSpeedValue = MODERATA;
int   panAngle      = 90;       ///< angolo destinazione del servo Pan
int   tiltAngle     = 90;     ///< angolo destinazione del servo Tilt
char  laser         = 0;        ///< stato uscita puntatore laser
float kp_guida;         ///< guadagno proporzionale per controllo su distanza muro
float kd_guida;           ///< guadagno derivativo    per controllo su distanza muro

float kpTeta;             ///< guadagno proporzionale per controllo su teta
float kiTeta;             ///< guadagno integrale per controllo su teta

float posTarget;
float posRef ;
float step;
int counterArresto;

float   MAX_S       = 0.2;      ///< max_s = LARGHEZZA_A_MEZZI/Raggio massimo
float   teta        = 0.0;      ///< teta attuale misurato da odometria
float   xpos, ypos  = 0.0;      ///< x, y attuale misurato da odometria
float   tetaRef     = 0.0;      ///<teta di setPoint, Reference

int   distRef       = 60;       ///< distanza riferimento per modo R1/R2           [cm]
int   distOstacolo  = 51;       ///< distanza dell'Ostacolo per arresto automatico [cm]. N.B. sotto i 50 cm a volte non legge.
int   divLidar      = 1;        ///< alcune versioni del lidar ritornano in mm altre in cm, uniformiamo a cm
int   vl53dist      = 999;      ///distanza letta da vl53
float tetaMisura    = 0.0;      ///< teta da bussola
float xc, yc, tetaCompass;      ///< vettori magnetici dalla bussola
float deltaCompass,tetaCompass_prev, tetaCompassRead;   ///< variabili bussola
char  modoGuida       = 0;      ///< modalita' di guida: odometro, bussola, misto, fusione ...
char  bussola;                ///< presenza bussola
char  sensore_ost;            ///< presenza laser ostacoli VL53L0X, 1 attivo

int offsetPan;          ///< offset su algolo Pan. Corregge errore allineamento

/** coefficienti calibrazione bussola 
sono salvati in E2prom */
float ox = 0.0;           ///< offset lungo l'asse x. viene ricavato a seguito della calibrazione
float oy = 0.0;           ///< offset lungo l'asse y. viene ricavato a seguito della calibrazione
float ky = 0.0;         ///< guadagno lungo asse y. viene ricavato a seguito della calibrazione

// variabili appoggio

float VA, VB;
int   cntStop = 0;
int   mode = TEST_STERZO;
char  monitorDati = 0;
float i_part = 0;           ///< parte integrale regolatore guida
float delta_teta;
float deltaS;
long  spdDxCnt, spdSxCnt;     ///< velocità encoder, impulsi per tempo di campionamento


int   lidarDistance;        ///< distance measured by lidar
float deltaErrore;          ///< variabili parte derivativa
float erroreK_1;            ///< variabili parte derivativa
float DerActive;            ///< variabili parte derivativa

char  occlusoDavanti;       ///< sensore IR anteriore, LOW in presenza di ostacolo


unsigned long   lastTimeFast,timeLidar;
char      cnt10ms;
float         teta_;
float         actualTetaRef;
char          SM_R5 = 0;
unsigned long   inizioStabilita; //Timer


sensors_event_t event;    // ID_009

// prototipi funzione
//void displaySensorDetails (void);
//void getSensorReading     (void);
