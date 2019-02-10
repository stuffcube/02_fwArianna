
/*!
    @brief  gestisce dei valori sulle E2prom. I vengono letti e scritti nello stesso ordine. La procedura gestisce anche il valore di default.
        letti i valori si procede ad aggiornare i parametri dipendenti dai valori assegnati.
      Questa procedura viene chiamata al termine dell' startUp con comando LEGGI. Vengono quindi attivati i parametri salvati in EEprom.


  @param comando

    i comandi sono:

    SCRIVI  0
    LEGGI   1
    DEFAULT 2
    ATTIVA  3 (attiva e ricalcola i valori GIRO_RUOTA DX, SX etc

    temporaneamente i valori 4 e 5 caricano dei valori di default calibrati su due macchine. 4 ARI_02, 5 ARI_03

*/

void DataEEprom(char comando){
/*
assegnamento dati in e2prom
0   ED_BASE   f
1   ED      f
2   BASELINE  f
3   giroRuota f
4   kpTeta    f
5   ox      f
6   oy      f
7   ky      f
8   kiTeta    f
9   kp_guida  f
10  kd_guida  f
11  divLidar  i
12  offsetPan i

ID_010
*/
int eeAddress = 0;   //Location we want the data to be put/get.

char endList = 0;
int  i = 0;

  while (endList == 0){

    switch (i) {

      case 0: //
          if (comando == SCRIVI)        EEPROM.put(eeAddress, ED);
          if (comando == LEGGI)       EEPROM.get(eeAddress, ED);
          if (comando == DEFAULT)       ED = 1.0;

            eeAddress += sizeof(float); //Move address to the next byte after float 'f'.
        break;

      case 1: //
          if (comando == SCRIVI)        EEPROM.put(eeAddress, ED_BASE);
          if (comando == LEGGI)       EEPROM.get(eeAddress, ED_BASE);
          if (comando == DEFAULT)       ED_BASE = 1.0;

            eeAddress += sizeof(float);
        break;

      case 2: //
          if (comando == SCRIVI)      EEPROM.put(eeAddress, BASELINE);
          if (comando == LEGGI)     EEPROM.get(eeAddress, BASELINE);
          if (comando == DEFAULT)     BASELINE = 220.0;

            eeAddress += sizeof(float);
        break;

      case 3: //
          if (comando == SCRIVI)        EEPROM.put(eeAddress, GIRO_RUOTA);
          if (comando == LEGGI)       EEPROM.get(eeAddress, GIRO_RUOTA);
          if (comando == DEFAULT)       GIRO_RUOTA = 1.995;

            eeAddress += sizeof(float);
        break;

      case 4: //
          if (comando == SCRIVI)        EEPROM.put(eeAddress, kpTeta);
          if (comando == LEGGI)         EEPROM.get(eeAddress, kpTeta);
          if (comando == DEFAULT)       kpTeta = 8.0;

            eeAddress += sizeof(float);
        break;

      case 5: //
          if (comando == SCRIVI)        EEPROM.put(eeAddress, ox);
          if (comando == LEGGI)     EEPROM.get(eeAddress, ox);
          if (comando == DEFAULT)       ox = 0.0;

            eeAddress += sizeof(float);
        break;

      case 6: //
          if (comando == SCRIVI)        EEPROM.put(eeAddress, oy);
          if (comando == LEGGI)       EEPROM.get(eeAddress, oy);
          if (comando == DEFAULT)       oy = 0.0;

            eeAddress += sizeof(float);
        break;

      case 7: //
          if (comando == SCRIVI)        EEPROM.put(eeAddress, ky);
          if (comando == LEGGI)       EEPROM.get(eeAddress, ky);
          if (comando == DEFAULT)       ky = 1.0;

            eeAddress += sizeof(float);
        break;

      case 8: //
          if (comando == SCRIVI)        EEPROM.put(eeAddress, kiTeta);
          if (comando == LEGGI)       EEPROM.get(eeAddress, kiTeta);
          if (comando == DEFAULT)       kiTeta = 0.2;

            eeAddress += sizeof(float);
        break;

      case 9: //
          if (comando == SCRIVI)        EEPROM.put(eeAddress, kp_guida);
          if (comando == LEGGI)       EEPROM.get(eeAddress, kp_guida);
          if (comando == DEFAULT)       kp_guida = 0.5;

            eeAddress += sizeof(float);
        break;

      case 10:  //
          if (comando == SCRIVI)        EEPROM.put(eeAddress, kd_guida);
          if (comando == LEGGI)       EEPROM.get(eeAddress, kd_guida);
          if (comando == DEFAULT)       kd_guida = 40.0;

            eeAddress += sizeof(float);
        break;

      case 11:  //
          if (comando == SCRIVI)        EEPROM.put(eeAddress, divLidar);
          if (comando == LEGGI)     EEPROM.get(eeAddress, divLidar);
          if (comando == DEFAULT)       divLidar = 1;

            eeAddress += sizeof(int);
        break;

      case 12:  //
          if (comando == SCRIVI)        EEPROM.put(eeAddress, offsetPan);
          if (comando == LEGGI)       EEPROM.get(eeAddress, offsetPan);
          if (comando == DEFAULT)       offsetPan = 0;

            eeAddress += sizeof(int);
        break;

      case 13:  //
          endList = 1;
        break;


    }

    i++;
  }

  // aggiornamento parametri dipendenti

  GIRO_RUOTA_SX   = GIRO_RUOTA*2.0/(1.0 +      ED*ED_BASE );
  GIRO_RUOTA_DX   = GIRO_RUOTA*2.0/(1.0 + 1.0/(ED*ED_BASE));

  LAGHEZZA_A_MEZZI= BASELINE/2000.0;
  //printDatiCalibrazione();

}


void printDatiCalibrazione(void){

  risposta  = "ED:      " + String(ED         , 6) + '\n';
  risposta += "ED_BASE: " + String(ED_BASE      , 6)  + '\n';
  risposta += "BASELINE " + String(BASELINE     , 6)  + '\n';

  risposta += "GIRO_RUOTA:" + String(GIRO_RUOTA     , 6)  + '\n';
//  risposta += "GIRO_RUOTA_SX:   " + String(GIRO_RUOTA_SX    , 6)  + '\n';
//  risposta += "GIRO_RUOTA_DX:   " + String(GIRO_RUOTA_DX    , 6)  + '\n';
//  risposta += "LAGHEZZA_A_MEZZI:" + String(LAGHEZZA_A_MEZZI , 6)  + '\n';
  risposta += "kpTeta:  " + String(kpTeta) + '\n';
  risposta += "kiTeta:  " + String(kiTeta) + '\n';
  risposta += "kp_guida: " + String(kp_guida) + '\n';
  risposta += "kd_guida: " + String(kd_guida) + '\n';
  risposta += "ox: " + String(ox) + '\n';
  risposta += "oy: " + String(oy) + '\n';
  risposta += "ky: " + String(ky) + '\n';
}
