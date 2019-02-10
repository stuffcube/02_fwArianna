

/** @brief campiona variabili e le mette in array. Possono essere lettera
  successivamente.

  @param cmd: indica cosa fare
        -1:  attiva solo parte di acquisizione
        '0': init,  inizializza variabili
        '1': run    attiva registrazione, esauriti i campioni si arresta. 
              lo sstato va a -1
        '2': trg    attende l'evento di trigger cha avvia l'acquisizione
        
  @param argomento: eventuale argomento del comando
    
  @param ch1n, .. : indice canale da campionare
  
  @return   stato acquisizione:
        -1: finita
      
  sequenza d'uso
  
  1i5x  x è un intero che fissa il tempo tra due campioni. la formula è tSample = 30ms*(x+1)
      iniviando 0 acquisisco ogni 30 ms. I campioni sono 100
  1i0   inizialliza lo scope
  1i1   avvia l'acquisizione
  1i2   ritorna lo stato acquisizione. 
      4 indica acquisizione finita. 
      1 in corso
  1i3   ritorna il numero di campioni acquisiti
  
  i9nnn ritorna una stringa con i valori registrati al campione nnn.
      nnn va da 0 a 99
      scp:0.00;0.00;0.00;1107021.00
    
  1i0
  scp:init
  1i510
  scp:decimation= 10
  1i1
  scp:statAcd
  1i2
  scp:statoScope= 1
  1i2
  scp:statoScope= 4
  for indice=0 to 99
    1i9indice
    ritorna scp:0.00;0.00;0.00;1069919.00
    splitta e metti su file

*/

int scope(char cmd, int argomento, int ch1n, int ch2n, int ch3n, int ch4n){

static int testa, statoScope, startIndex;
static int sampleAcquired, decimation, decimationCounter;
#define INIT      0
#define WAIT_FOR_RUN  1
#define RECORDING   2
#define RECORDING_ENDED 4

#define SAMPLE_FOR_READING  5

  if (cmd != -1){
    // init
    if (cmd=='0'){
      testa = 0;
      statoScope = INIT;
      risposta =  "scp:init";
    }

    // start acquisizione
    if (cmd=='1'){
      statoScope = WAIT_FOR_RUN;
      startIndex = testa;
      sampleAcquired = 0;
      decimationCounter = decimation;
      risposta =  "scp:statAcd";
    }

    // chiedo stato
    if (cmd=='2' ){
      risposta =  "scp:statoScope= " + String(statoScope);
    }

    if (cmd=='3' ){
      risposta =  "scp:testa= " + String(testa);
    }
    
    // imposta ogni quanti campioni acquisire il segnale
    if (cmd=='5'){
      decimation = argomento;
      risposta =  "scp:decimation= " + String(decimation);
    }
  }

  //read data
  if(cmd=='9'){
    //argomento *= SAMPLE_FOR_READING; // torno cinque campioni alla volte
             // argomento indica la cinquina
    if (argomento < nOfSamples/SAMPLE_FOR_READING){
      risposta = "scp:dati="    +\
      String(argomento)     +";";
      for (int index = 0; index <=(SAMPLE_FOR_READING - 1); index++){
        risposta += String(ch1[argomento*SAMPLE_FOR_READING +  index])    +";"+\
              String(ch2[argomento*SAMPLE_FOR_READING +  index])    +";"+\
              String(ch3[argomento*SAMPLE_FOR_READING +  index])    +";"+\
              String(ch4[argomento*SAMPLE_FOR_READING +  index])    +";"+\
              String("x;");
      }
    }
    else risposta =  "scp: indice troppo grosso";
  }

  // run
  switch (statoScope){
    case WAIT_FOR_RUN: // wait for run
    
      if (statoRun != 0) statoScope = RECORDING;
      break;
    
    case RECORDING: // recording
      if (decimationCounter == 0) decimationCounter = decimation;
      else{
        decimationCounter--;
        return(statoScope);
      }
      if (testa >= nOfSamples) testa = 0;

      ch1[testa] = xpos;//teta;
      ch2[testa] = xpos;
      ch3[testa] = ypos;
      ch4[testa] = millis();
      testa++;
      sampleAcquired++;

      // teminate l'acquisizione va in stop
      if (sampleAcquired >= nOfSamples) statoScope = RECORDING_ENDED;
      break;

    // stop
    case RECORDING_ENDED:
      ;
      break;
  }
  return(statoScope);

}
