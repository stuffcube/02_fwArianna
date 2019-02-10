
/**
  @brief
*/




/** @brief comandi: i comandi assegnano valori o eseguono funzioni. iniziano con un 3 e hanno la lettera maiuscola, sono seguiti da uno o più numeri.
      non ci sono spazi di separazione.
      i numeri, se non diversamente indicato, sono floating point, la virgola è data col '.', es 3.14

  @param comando

    Axxx: Alfa. assegna Alfa, la direzione del robot. xxx è l'angolo in radianti, es 3.14. La direzione zero è definita all'accensione del robot. E' la direzione in avanti. Coincide con l'asse x.
        Alfa è positivo in senso antiorario.

    Cxxx: assegna "raggiorSterzo" in [m]. La variabile è usata nella movimento R2, stabilisce il raggio della circonferenza su cui ruota il robot.

    Dxxx: Distance. La distanza che viene percorsa nel prossimo Run espressa in [mm]. La distanza è sempre incrementale.

    Ex: EEprom.  Esegue operazioni su dei parametri di taratura. Vedi procedura DataEEprom.

    E0 SCRIVI i parametri in E2prom,
    E1 LEGGI i parametri in E2prom,
    E2 rispristina in valori di DEFAULT,
    E3 mostra i parametri CORRENTI,


    Fnxxx: imposta dei parametri del robot. "n" indica quale parametro, "xxx" è il valore.

    F0xx ED
    F1xx ED_BASE
    F2xx BASELINE   mm
    F3xx GIRO_RUOTA   mm  = sviluppo ruota[mm]/(4*ppr)
    F4xx Divisore lidar
        F5xx offest servo Pan

      N.B. questi valori vanno attivati con un 3E3

    Gxxx: "Guide Mode". definisce il modo di guida nel run "R4". Il teta di feedback, tetaMisura, può provenire dall'odometria, dalla bussola, o altri mix possibili.
        Nel codice sotto teta è l'angolo ricavato dagli encoder.

        Il modo zero si affida alle misure fatte dagli encoder.
        Il modo 1 e 3 ricavano il teta direttamente dalla bussola. Nel modo 3 anche le posizioni x e y
        sono calcolate dal teta della bussola.
        con il modo 2 è possibile imporre l'angolo corrente come angolo zero. Altrimenti la direzione è
        assoluta come determinata dalla bussola.

        if (modoGuida == 0) tetaMisura = teta;      // ID_001
        if (modoGuida == 1) tetaMisura = tetaCompass; // ID_009
        if (modoGuida == 3) tetaMisura = tetaCompass; // ID_009
        if (modoGuida == 2) {
          tetaCompass = 0;  // definisco la direzione corrente come zero.
          modoGuida   = 1;
          }

        in updatePosition integro posizioni in funzione del modoGuida attivo

        if (modoGuida == 3) {
          xpos    +=  deltaC*cos(tetaCompass);
          ypos    +=  deltaC*sin(tetaCompass);
        }
        else{
          xpos    +=  deltaC*cos(teta);
          ypos    +=  deltaC*sin(teta);
        }

    Knxx:   assegna i guadagni
        K0 kpTeta   il guadagno proporzionale kp usato nel modo Run 4
        K1 kiTeta
        K2 kp_guida il guadagno proporzionale kp usato nel modo Run 1 e 3 (sensore di distanza laterale)
        K3 kd_guida il guadagno derivativo    kd usato nel modo Run 1 e 3 (sensore di distanza laterale)

    H0:   Homing, assegna lo posizione corrente (x, y, teta) = (0, 0, 0)

    Lx:   Led. puntatore a Led acceso, x=1, o spento x=0.

    Mx:   Monitor. Attiva, 1, o disattiva, 0, l'invio continuo di misure da parte del robot.
        le msiure sono inviate nella routine "updatePosition" ogni 100 ms.
        La stringa ha l'header "mon:"

        if (monitorDati && (counter >=4) ){
        counter = 0;
        risposta  = "mon;";
        risposta += (dDxCnt);
        risposta += (";");
        risposta += (dSxCnt);
        risposta += (";");
        risposta += (deltaC);
        risposta += (";");
        risposta += (teta);
        risposta += (";");
        risposta += (xpos);
        risposta += (";");
        risposta += (ypos);
        risposta += (";");
        risposta += (i_part);// raggiorSterzo
        risposta += (";");

        risposta += event.magnetic.x;
        risposta += ";";
        risposta += event.magnetic.y;
        risposta += ";";
        risposta += event.magnetic.z;
        risposta += ";";
        sendAnswer2(port);
      }

    Nnxxx:  assegna i coefficienti di correzione della bussola.
        N0: ox
        N1: oy
        N2: ky

    Onxx:
        O0: distRef      [cm]. riferimento per guida a distanza fissa nei modi R1 e R2
        O1: distOstacolo [cm]. distanza minima sotto la quale ARI si arresta

    Pxxx:   orienta il servo PAN della testa. xxx è in gradi, 90° guarda in avanti, 0 a sinistra e 180 a destra.
        in alcuni casi, a seconda del servo usato, è bene limitare l'escursione ad esempio tra 10 e 170°.

    Rx:   Run. Definisce in che modo effettuare la corsa "Run". Il run è definito con un obbiettivo (target).
        Il raggiungimento del target termina il run.

        x=4: R4. Il robot si muove sino a percorrere la distanza impostata con "D" nella direzione alfa impostata con "A".
        x=5: R5. Il robot si muove sino a orientarsi nella direzione alfa impostata con "A". La rotazione fa perno su una ruota che rimane ferma.
        x=6: R6. Il robot si muove sino a orientarsi nella direzione alfa impostata con "A". La rotazione avviene con perno al centro del robot con ruote controrotanti.
        x=2: R2. Il robot effettua un percorso circolare con raggio pari al parametro "C". Il raggiorSterzo può anche essere
             imposto direttamente con il comando "S". In questo caso sarà lo scorrimento tra le ruote dx e sx.

        R1 e 3. i modi 1 e 3 fanno viaggiare il robot a una distanza fissa dalla parete laterale. la misura è fatta orinetando il LIDAR.
            suggerimento, angolo lidar 45 rispetto alla parete
      leggere distanza e metterla come distRef (O0)
      mettere distOstacolo maggiore distRef (O1), per esempio + 30 cm
      gli angoli pan (P) sono P45  seguo parete dx con R3
      gli angoli pan (P) sono P135 seguo parete sx con R1

        x=99: provoca l'arresto del movimento

        Il movimento si arresta quando il lidar vede un ostacolo troppo vicino.

        if (lidardist<50 && (statoRun != 5) &&(statoRun != 6) &&(statoRun != 0))
          {
            statoRun      = 99;   // senza rampa
            motorSpeed    = 0;    // annullo rampa
            motorSpeedRef   = 0;
            risposta    = "ostacolo";
            sendAnswer2(port);
          }

    Sxxx: Scorrimento. Imposta direttamente la varibile "raggiDiSterzo". è un numero tra -1 e 1.
        Zero significa scorrimento nullo, cioè le due ruote hanno la stessa velocità comandata.
        vedi procedura "differenziale"

        ponendo s = raggiorSterzo abbiamo che le velocita' alle ruote valgono:

          VA = motorSpeed*(1-s)
          VB = motorSpeed*(1+s)

    Txxx:   orienta il servo TILT della testa. xxx è in gradi, 90° guarda in avanti, 0 a sinistra e 180 a destra.
        in alcuni casi, a seconda del servo usato, è bene limitare l'escursione ad esempio tra 10 e 170°.

    Vxxx: Velocità. Definisce la velocità del robot. é il valore assegnato al PWM.

    Zxxx:   definisce MAX_S. è il limite dello scorrimento (positivo e negativo) applicabile dal controllo.


*/
void comandi()
{
static float x, y;


//          x==(inputString.substring(2)).toInt();
    //with only one parameter looks for a given substring from the position given to the end of the string.
    x = (inputString.substring(2)).toFloat();
    /*
    Serial.print  ("comandi- ch[1]: ");
    Serial.print  (inputString[1]);
    Serial.print  (",xstr: ");
    Serial.print  (inputString.substring(2));
    Serial.print  (",x: ");
    Serial.println(x);
    */

    switch (char(inputString[1])) {

  case 'B':
    case 'I':
      risposta = "libero";
        break;
    
    case 'A':
      tetaRef  = x*3.14/180.0;
      risposta = "A: " + String( tetaRef, 3);
        break;

      case 'C':
    if (abs(x) < 1000)
      raggiorSterzo = LAGHEZZA_A_MEZZI/x;
    else
      raggiorSterzo = 0.0;
    
    risposta = "S: " + String(raggiorSterzo, 3);
        break;

      case 'D':
          distanza += x;
          risposta = "D: " + String( distanza, 3);
        break;


      case 'E':
          if (x == 0.0){
            DataEEprom(SCRIVI);
            risposta = "E: SCRIVI";
          }
          else if (x == 1.0){
            DataEEprom(LEGGI);
            risposta = "E: LEGGI";
          }
          else if (x == 2.0){
            DataEEprom(DEFAULT);
            risposta = "E: DEFAULT";
          }
          else if (x == 3.0){
            DataEEprom(3.0);
            printDatiCalibrazione();
          }
          else  risposta = "E0: SCRIVI, E1 LEGGI, E2 DEFAULT, E3 CORRENTI";
        break;

      /* ID_010
        la seconda cifra è un numero cosi
        che l'assegnamento di x all'inizio vada sempre bene
        mantiene la compatibilita
        F0xx ED
        F1xx ED_BASE
        F2xx BASELINE
        F3xx GIRO_RUOTA
        F4xx Divisore lidar
        F5xx offest servo Pan

      */
      case 'F':
        x = (inputString.substring(3)).toFloat();
        switch (char(inputString[2])) {
          case '0':
            ED = x;
            risposta = "F0_ED: " + String(ED, 6);
            break;
          case '1':
            ED_BASE = x;
            risposta = "F1_ED_BASE: " + String(ED_BASE, 6);
            break;
          case '2':
            BASELINE = x;
            risposta = "F2_BASELINE: " + String(BASELINE, 6);
            break;
          case '3':
            GIRO_RUOTA = x;
            risposta = "F3_GIRORUOTA: " + String(GIRO_RUOTA, 6);
            break;
          case '4':
            divLidar = (int)x;
            risposta = "div lidar: " + String(divLidar);
            break;
          case '5':
            offsetPan = (int)x;
            risposta = "offset Pan: " + String(offsetPan);
            break;
        }
        break;


      case 'G':   // ID_009
          modoGuida = x;
          risposta = "G: " + String(modoGuida);
        break;

      // K0 kpTeta
      // K1 kiTeta
      // K2 kp_guida
      // K3 kd_guida
      case 'K':
        x = (inputString.substring(3)).toFloat();
        switch (char(inputString[2])) {
          case '4':
//            kp = x;
//            risposta = "kp: " + String(kp, 6);
            break;
          case '0':
            kpTeta = x;
            risposta = "kpTeta: " + String(kpTeta, 6);
            break;
          case '1':
            kiTeta = x;
            risposta = "kiTeta: " + String(kiTeta, 6);
            break;
          case '2':
            kp_guida = x;
            risposta = "kp_guida: " + String(kp_guida, 6);
            break;
          case '3':
            kd_guida = x;
            risposta = "kd_guida: " + String(kd_guida, 6);
            break;
        }
        break;

      case 'H':
        xpos = ypos = teta = 0.0;
        risposta = "H: ";
      break;

      case 'L':
          Serial.println(x);
          if (x == 0.0){
            Serial.println("L0");
            digitalWrite(laserPin, LOW);
            digitalWrite(LED1, LOW);
            laser = 0;
          }
          else{
            Serial.println("L1");
            digitalWrite(laserPin, HIGH);
            digitalWrite(LED1, HIGH);
            laser = 1;
          }
          risposta = "L: " + String(x);
        break;

      case 'M':
          monitorDati = x;
          risposta = "M: " + String(monitorDati);
        break;

      case 'N':
        x = (inputString.substring(3)).toFloat();
        switch (char(inputString[2])) {
          case '0':
            ox = x;
            risposta = "N0: " + String(ox, 6);
            break;
          case '1':
            oy = x;
            risposta = "N1: " + String(oy, 6);
            break;
          case '2':
            ky = x;
            risposta = "N2: " + String(ky, 6);
            break;
        }
        break;

      case 'O':
        x = (inputString.substring(3)).toFloat();
        switch (char(inputString[2])) {
          case '0':
              distRef  =  (uint16_t)int(x);
              risposta = "distRef: " + String(distRef);
            break;
          case '1':
              distOstacolo  =  (uint16_t)int(x);
              risposta = "distOstacolo: " + String(distOstacolo);
            break;
        }
        break;

      case 'P':
          panAngle = x;
          risposta = "P: " + String(panAngle);
        break;

      case 'R':
          statoRun = x;
          risposta = "R: " + String(statoRun);
        break;

      case 'S':
          raggiorSterzo = x;
          risposta = "S: " + String( raggiorSterzo, 3);
        break;

      case 'T':
          tiltAngle = 180 - x;    // servomotore girato
          risposta = "T: " + String(x);
        break;

      case 'V':
          motorSpeedValue = x;
          risposta = "V: " + String(motorSpeedValue);
        break;

      case 'Z':
          MAX_S = x;
          risposta = "Z: " + String(MAX_S, 3);
        break;

      case '1':
          VA = x;
          risposta = "1: " + String(x, 3);
        break;

        default:
            risposta="mica capito";
        }
        sendAnswer2(port);
  }

/*!
    @brief  quando arriva una stringa da ESP la processa. Il protocollo usato è in ASCII permette di inviare comandi o richieste.
      Il primo carattere indica se richiesta (1) o comando (3).

      I caratteri successivi vengono passati alle routine dedicate che eseguono il comando e preparano la stringa "risposta".

      questa viene ritornata dalla funzione sendAnswer2
*/

void getCmd2(void){
static float x, y;
static int inByte;

  if (inputString!="")
  {
    Serial.print("getCmd2: ");
    Serial.println(inputString);
    switch (inputString.charAt(0)) {
      case '1':
      richieste();
      break;
      case '3':
      comandi();
      break;
      default:
      risposta="mica capito";
      sendAnswer2(port);    // l'argomento dovrebbe essere la porta su cui rispondere

    }

    // fine comandi
    inputString="";
  }

}


/*!
    @brief  riceve i caratteri dalla seriale dell'ESP. Il char '!' è l'inizio stringa, il '?' è il terminatore della stringa.
  il terminatore della stringa è ?

  All'arrivo dello start la stringa pulita e inizia l'accumolo dei caratteri

  All'arrivo del terminatore la stringa viene resa disponibile in "inputString"

  ! *flush*

  ? *terminatore*


  N.B. il codice su esp oltre a dispacciare i pacchetti giunti dal WiFi verso Arduino effettuata anche un filtraggio e una generazione di richieste.
  filtraggio: I pacchetti che iniziano !> sono destinati all'esp e sono quindi trattati sull'esp stesso.
  generazione di richieste: a tempo l'esp richiede a Arduino delle informazioni che poi invia al client.

  per dettagli su questa parte vedi codice esp..

*/
void rSeriale(void)
{
static unsigned long time, cmdTime;
static char c;
static int numero = 0;
  port = 0;
  if (ESPserial.available() ==0) return;

  c = (char)ESPserial.read();
  //Serial.write(c);

  // arrivano e non so da dove
    if (c == '\n' || c == '\r')   return;


  numero ++;

    if (c=='!')   // flush stringa
    {
      inputStringTmp = "";
      ESPserial.flush();
      okcomm  = 1;
      numero  = 0;
      Serial.println("flush");
    }
    if (c != '?' && c != '!' && okcomm==1)
    {
    cmdTime = millis();
      inputStringTmp += c;
    }
    if (c == '?') {
    inputString=inputStringTmp;
    Serial.println(inputString);
    Serial.print(millis());
    okcomm=0;
    }
}
