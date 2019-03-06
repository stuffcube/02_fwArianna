/** @brief odometri

  la misura della posizione viene fatta leggendo gli impulsi prodotti dagli encoder rotativi montati sull'albero delle ruote.
  Gli encoder sono dischi con delle fessure regolari. Queste interrompono a tartti il fascio luminoso sul sensore optoelettronico.
  Ogni transizione genera un interrupt su Arduino.
  Contando il numero degli impulsi letti si misura lo spazio percorso.
  La direzione dipende dalla tensione applicata al motore. Gli impulsi vengono sommati in un caso e sottratti nell'altro.

  Il flag Vx_zero indica queando il motore è fermo. In questo caso si evitano i conteggi per evitarne di falsi.
  Se gli impulsi giugono troppo velocemente vengono considerati errori.

  La routine incrementa contatori di interi per essere veloce.
*/
void odometroDxMisuraHW(void){
unsigned long pulseTime;

  if ((millis() - pulseTime) < MIN_TIME_TRA_PULSE) return;
  pulseTime = millis();

  //Serial.println("dx");
  digitalWrite(ledPin, !digitalRead(ledPin));

  if (VA_zero) return;    // se tensione zero non conta

  if  (statoRun == 0)          return;

  if (dirVA == 1)  odometroDxCnt ++;    // ID_005
  else             odometroDxCnt --;
}

/** @brief vedi Sx
*/
void odometroSxMisuraHW(void){
unsigned long pulseTime;

  if ((millis() - pulseTime) < MIN_TIME_TRA_PULSE) return;
  pulseTime = millis();

  //Serial.println("Sx");
  digitalWrite(ledPin, !digitalRead(ledPin));

  if (VB_zero) return;    // se tensione zero non conta

  if  (statoRun == 0) return;

  if (dirVB == 1)  odometroSxCnt ++;    // ID_005
  else             odometroSxCnt --;
}


void updatePosition(void){

static long SxCnt_k_1 = 0;  // valore cnt a k-1
static long DxCnt_k_1 = 0;  // valore cnt a k-1
static long dDxCnt, dSxCnt;   // delta cnt
static long letturaDx;      // congelo cnt
static long letturaSx;      // congelo cnt
static float deltaC;      // delta cnt
static int  counter = 0;


  // valore complessivo: usato temporaneamente
  odometro = (odometroDxCnt + odometroSxCnt)*GIRO_RUOTA;

  // calcolo evoluzione nel periodo

  // congelo le letture per lavorare su valori coerenti

  noInterrupts();
  letturaDx= odometroDxCnt;
  letturaSx= odometroSxCnt;
  interrupts();

  dDxCnt   = letturaDx - DxCnt_k_1;       // delta sx e dx in count
  dSxCnt   = letturaSx - SxCnt_k_1;

  // esporto velocità encoder
  spdDxCnt = dDxCnt;
  spdSxCnt = dSxCnt;

  deltaC   = (dDxCnt + dSxCnt)*GIRO_RUOTA;// avanzamento del centro nel periodo in mm

  DxCnt_k_1= letturaDx;             // memoria per prossimo ciclo
  SxCnt_k_1= letturaSx;

  // integro teta
  // delta_teta è la velocità angolare
  delta_teta =((float)dDxCnt*GIRO_RUOTA_DX - (float)dSxCnt*GIRO_RUOTA_SX)*2.0/BASELINE;
  teta  += delta_teta;

  // integro posizioni
  if (modoGuida == 3) {
    xpos    +=  deltaC*cos(tetaCompass);
    ypos    +=  deltaC*sin(tetaCompass);
  }
  else{
    xpos    +=  deltaC*cos(teta);
    ypos    +=  deltaC*sin(teta);
  }
  //  getSensorReading(); // ID_009

  /* Get a new sensor event */
  counter ++;

  if (bussola) mag.getEvent(&event);

  // monitor dati
  if (monitorDati && (counter >=4) ){
    counter = 0;
    //risposta  = "dDxCnt; dSxCnt; deltaC; teta; xpos; ypos; i_part;X;Y;Z;"
    risposta  = "mon;";
    risposta += (dDxCnt);
    risposta += (";");
    risposta += (dSxCnt);
    risposta += (";");
    risposta += (deltaC);
    //    risposta += (actualTetaRef);
    risposta += (";");
    risposta += (teta);
    risposta += (";");
    risposta += (xpos);
    risposta += (";");
    risposta += (ypos);
    risposta += (";");
    risposta += (i_part);// raggiorSterzo
    risposta += (";");
    /* Display the results (magnetic vector values are in micro-Tesla (uT)) */

    risposta += event.magnetic.x;
    risposta += ";";
    risposta += event.magnetic.y;
    risposta += ";";
    risposta += event.magnetic.z;
    risposta += ";";
    //    dato += "uT";
    sendAnswer2(port);
  }

}
