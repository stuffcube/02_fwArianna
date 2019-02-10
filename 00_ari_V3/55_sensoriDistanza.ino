/** @brief riporta al controller la distanza letta dal lidar, la risposta ha formato "mis;" + distanza in cm".
  aggiorna anche la il valore "lidardist" ogni 250 ms.

*/
void testOstacoli(){
static int i;
  if (monitorDati) return;


  // 
  if ((millis()-timeLidar) > 1000){
    timeLidar  = millis();
    if (sensore_ost==1){         //ho il laser anteriore attivo

      VL53L0X_RangingMeasurementData_t measure;
      lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
      
      if (measure.RangeStatus != 4) {  // phase failures have incorrect data
      
        vl53dist = measure.RangeMilliMeter/10;
        //risposta = "ost;" + String(measure.RangeMilliMeter/10);
        //sendAnswer2(port);
      }
      else{
        vl53dist=999;
      }
    }
    //risposta  = "mis;" + String(lidarDistance);
    //sendAnswer2(port);
  }
}

/** @brief libreria lettura lidar
  viene usata solo dalla funzione di scan..
  la funzione tfmini.getDistance() aspetta un pacchetto da seriale
  chiamare tutta lka funzione lidar è inutile
  da rivedere

*/
String lidar()
{
  if (!LIDAR_PRESENT){
    return String("9999");
  }

    // Take one TF Mini distance measurement
     uint16_t dist = tfmini.getDistance()/divLidar;
//     dist = tfmini.getDistance();
//     dist = tfmini.getDistance();
//     dist = tfmini.getDistance();
     uint16_t strength = tfmini.getRecentSignalStrength();
     //Serial.print("forza");
    //Serial.println(strength);
    return String(dist);

}

/** @brief legge il lidar e prepara la stringa di risposta.

  inizializza la stringa di risposta come "echo-"+String(ang)+"-";

  effettua dapprima 10 letture a vuoto,

  appende "numeroMisure" misure alla risposta ognuna separata da "-"


  @param numeroMisure su cui viene
  @param ang
*/
String lidarMisuraAmb(int numeroMisure,int ang ){
float  distance = 0;
String risp   = "echo-"+String(ang)+"-";

  for (int i = 0; i < numeroMisure+10; i++){
    // Read in times pulse
    if(i<10){// svuoto il lidar
      delay(25);  // tempo assestamento pan
      lidar();
    }
    else{
      delay(25);  // tempo assestamento pan
      risp += lidar();
      risp += "-";
    }
  }
  risp += "+";
  return(risp);
}
