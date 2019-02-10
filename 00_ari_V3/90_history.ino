/*
  01dic18 3.04.00
    sensore ostacolo anteriore off alla partenza. va abilitato per usarlo
    aggiunta V_FW_ATMEGA
    formato M.mm.sb
    M: il cambio richiede modifiche hw. Aggiungere un sensore (es sensore frontale) non è una modifica hw, è una opzione
    mm: aggiunta di una funzione, le versioni precedenti non la supportano. es la gestione del sensore frontale.
    sb: subversion. si migliorano funzioni o corregono bachi. nulla viene aggiunto.

    richiesta "h" versione V_FW_ATMEGA
    printDatiCalibrazione dava errore per risposta troppo lunga

  25nov18 definiti LED1..3
    attivato led1 insieme a LedPuntatore

  11Nov18
    gestione sensore ostacolo frontale
    comando B per abilitarlo B0 off, B1 On
    nota che il Lidar si sente sull'IR. Se tolgo lidar dall'asse
    non interferisce.


  01Nov18 
    rivisto lidar. Introdotta guida a distanza dalla parete. Modo R1 e R2. Parametro 'O' distRef.
    nellla stringa "pos:..." aggiunto in coda "statoRun". Questo permette di coordinare il sw su pc.
    introdotti parametri per guadagni regolatori vari

  08ott18
    lidardist da float a int, Piccola modifica in testOstacoli()

  04set18
  //    delay(25); rimosso in lidar() insieme a 3 assegnamenti

  07ago18
  revisione 3.0
  rimosso sonar
  rimosso BT
  rimossa gestione comandi BT
  sensori di distanza IR: tolto interrupt per misura lineare, inserito stato ON/Off
  FILE_PATTERNS = [...] *.ino
  EXTENSION_MAPPING = ino=c doxygen config filebuf

  da ora i commenti più recenti sono in alto
  intestazioni routine per doxygen
/ * *

     @brief  Gets the most recent sensor values and transform in angle.
            This function reads only x and y magnetometer values
            The coefficients for correction are applied.

      the output is tetaCompass which is a global variable.

    @param  a .....
    @return b ....


 * /

/// Brief description.
https://learn.adafruit.com/the-well-automated-arduino-library/doxygen-tips
http://www.stack.nl/~dimitri/doxygen/manual/markdown.html






----------------------------------------------------------------------------

note : 30lug16
fare un doppio movimento per la posizione
il primo fa in grosso e nel caso eccede.
il secondo si posiziona fine.
la macchina a stati è già pronta

giocare con baseline per la giusta larghezza

R5 si ferma troppo avanti, l'angolo che trova è sbagliato
devo dare due R5 per farlo andare


testare con modifica VA_zero attiva, nei test precedenti non lo era
testare modo R5 che gira su una ruota.


sistemata cablatura:
motore DX morsetti A - per il piu e meno fare foto
motore SX morsetti B
IR DX a ingDX
IR SX a ingSX
Encoder SX a ing SX
Encoder DX a ing DX

parametri taratura salvati e recuperati da E2prom
talvolta perde dei dati (giroRuota). Confusione con aggancio BT ??
messo comando f per leggere distanza degli IR
R5 e R6 girano ma non danno risultati ripetibili
m sonar rivedere





aggiustare le dirVx = 0 gestite in odometro


*/
/*
 * arianna
 * giorgio323@gmail.com
 * infostuffcube@gmail.com
 *
 * 16nov16  inizio

  test su sensore ir
  l'idea è trovare il tempo a 1 rispetto al tempo totale
  questo è un indicatore della distanza

  10dic16 sensore rotazione motore (odometro)
      spostato su Raspberry la compilazione
      ID_000  digitalPinToInterrupt(SIDE_IR_PIN) non dichiarato

  27dic16 comandi da seriale
      gestione arrivo in posizione
      aggiunto sonar montato su servo

      per trovare la direzione da puntare cerco il punto a distanza minima
      con un algoritmo di bisezione
      faccio con uno script esterno

  02gen17 penso a gestire sensore dx e sx
  numbers 0 (on digital pin 2) and 1 (on digital pin 3)

  19gen17 differenziale elettronico
      tolgo ovfTimer perchè usato dai PWM
      gestire misura sensore senza interrupt timer

  28gen17 add blueTooth on serial port1
      vanno ignorati i vari messaggi da BT
      quando non è connesso
      code 0000
      http://wiki.seeedstudio.com/wiki/Bluetooth_Bee

  23mar17 ID_001 rimosso limite angolo teta

  14mag17 si aggiunge comunicazione con esp ID_002
      si usa la serial2 verso esp
      modifica protocollo Alessandro Airaghi

  21mag17 ripristinato BT in parallelo ID_003
      Ok

  07giu17 cambiati encoder
      # come indicatore inizio msg monitor
        aggiunto comado di home 'H', prende la posizione attuale come 0, 0, 0
      cambiato terminatore seriale, da println a print + \n ID_004
      nel controllo di teta porto -2 <= S <= 2  ID_005

  29lug17 macchina a stati e rampa su tetaRef Rr


  11ago17 comando H modificato (distanza, odometro, lastPosition = 0.0)
      introdotto statoRun 99. Questo indica una rampa in corso.
      precedentemente lo statoRun 0 era presente anche durante la rampa di fermata
      il 99 serve per eviatre di inserire un ritardo coi comandi seriali per attendere la fine rampa

      nella lettura encoder se tensione al motore zero non conto impulsi.
      serve per evitare che quando curva su una ruota ferma possano esserci movimenti

  22ago17 ariPi_2DC_esp_02.ino
      messo ordine su nomi e posizioni encoder motori etc
      sistemati segni delle reazioni
      questo per capire su cosa si agisce
      Lettura velocità angolare e lineare

  10set17 diventa _03
      salvo dati di taratura in eeprom  ID_006
      rivisti sensori IR left e dx
  20set17
      inserita richiesta p per avere posx posy e teta in una sola volta
  060tt17
    ale
    aggiunta richiesta q per scansione area circostante con sonar per mappa

  12dic2017
    rimossa parte di bt libero porta SerialBT per rilevatore laser
    pulizia generale codice esp su serial 3


  08nov17
      inizializzato 'i' in eeprom
      rimosso #include <TimerOne.h>


  03dic17
      ID_007
      porta4 per comunicazione con la scheda sensori IR per la ricerca della
      docking station
      un micro esegue il sw nel foleder IRrecvDemo_x.ino
      viene fatto un bridge tra comandi da seriale verso la porta 4
      al momento è bloccante

  sistemare i parametri in funzione di ed anche per ari02



  10feb18
    merge

  21feb18
    // ID_008 dimenticato un pezzo

  05mag18
    merge per ari06.
    Lidar,
    Esp con IR
    tolgo BlueTooth
    tolgo ponte verso scheda IR

  12giu18
    // ID_009
    aggiunto getione sensore compass, gyro etc
    liberate porte 20 e 21, I2C (SCL, SDAT)
    eliminati sensori dx e sx per avere liberi interrupt per encoder (erano sul pin 3)
    modificati pin interrupt encoder
    pin 21 ora su pin 2
    pin 20 ora su pin 3
  aggiunta richiesta bussola 'f'
  agginto comando selezione guida 'G' con odometro, bussola, fusione ..

    ID_010
  riportati comandi seriale mancanti su "comandi"
  aggiungta possibilita di indice ai parametri
  la prima cifra dopo la lettera seleziona sino a 10
  indici (Es dati taratura F0, F1, F2 ..
  Rimango compatibile con comandi precedenti
  Salvati coefficienti taratura bussola in EEprom


  liberia tfmini.h   https://github.com/opensensinglab/tfmini/tree/master/src


 * Simple test for the DRV8833 library.
 * The DRV8833 is a dual motor driver carrier made by Pololu.
 * You can find it here: https://www.pololu.com/product/2130


 */
