#define V_FW_ATMEGA  "3.05.09"
/**@file ariPi_2DC_esp_08.ino */
/* stuffcube.wordpress.com

	02feb19 3.05.9
	in cfg.ino agginta define LIDAR_PRESENT per usare con/senza lidar
	il file cfg.ino è per lasciarlo separato su git
	se il lidar non è presente con la libreria attuale si blocca.
	va modificata.
	rotazione sul posto. motorSpeed =  abs(raggiorSterzo)*motorSpeedValue;//110.0; 
	ridotto MIN_TIME_TRA_PULSE da 9 a 1. Nuovi opto encoder con nuove rotelle a fessura più stretta. Il tempo arrivava a 5 ms.
	default kpTeta = 2.0; // era 8.0;

	23gen19 3.05.8
	su blocco per ostacolo azzerare distanza residua #9, 
	soluzione: distanza = odometro 
	lettura  'm': ritorna "mis:vl53dist; lidarDistance


	20gen19 3.05.7
		la registrazione, una volta attivata, parte quando R diventa diverso da zero.
		Cioe quando ARI inizia a muoversi

	13gen19 3.05.6
		introdotto regolatore PI anche su modo R6
		introdotto terminatore su risposta registratore

	06gen19 3.05.5
		inserito offset su PAN, F5. Salvato in E2prom
		test OscilloSscope
		motorSpeedRef =  108; //MODERATA; troppo veloce eccede. 
		se tropo pesante non si muove........
		modificata temporizzazione task.
			la base è la rampa a 10 ms che incrementa cnt10ms
			ogni volta che arriva a TEMPO_CONTROLLO viene eseguito

		03gen19 3.05.4
		errato separatore, corretto. String(inputString.substring(2)) +";"+\

    03gen19 3.05.03
		corretto baco su divLidar
		// preparata aggiunta lidarDistance alla risposta 'p'
		rimossa E4 e E5 (set dei parametri predefinito)

    27dic18 3.05.02
		valore default KpTeta da 0.8 a 8
		baco: caricava KpTeta con valore di default in firstRun
		rimossa risposta "mis:"
		rimosso cmd 'B', il sensore ostacolo si autodetecta
		aggiornata stringa di risposta a comando 'p'
		campo ID msg 'p' in posizione 2

	24dic18 3.05.00
		aggiunto vl53 per la gestione ostacoli, ritorna ost come segnale  per non creare confusione con lidar, aggiunto parametro lidar

	09dic18  3.04.01
		gestione lidar con ritorno in mm divido il valore per 10
		aggiunto divisore divLidar come parametro in E2prom per vecchi lidar
		modificati valori di default per meccanica corrente
		corretto errore in load default
		caricando questa versione la prima volta vanno ripristinati i valori meccanici
*/

 #include "cfg.h"


/*!
    @brief  setup della parte hardware e caricamento coefficienti da E2prom

*/

// setup vedi file 20_setup.ino

void loop() {

static long exeTime, tInit;


  mode = TEST_CONTROLLO;

  getCmd2();              /// gestione comunicazione con ESP
  rSeriale();             /// gestione comunicazione con ESP

  servoPan.write ( panAngle + offsetPan );     /// assegna angoli a pan e tilt
  servoTilt.write(tiltAngle);


  /**
    funzionamento corrente con anelli di controllo attivi
  */
    if (mode == TEST_CONTROLLO){

    if (firstRun){
      Serial.println("start TEST_CONTROLLO mode");
      Serial.println("controlloAttivo");
      raggiorSterzo = 0.0;
      servoPan.write( panAngle);
      servoTilt.write(tiltAngle);
      digitalWrite( ENB_L_SIDE_FRONT, HIGH);
      timeLidar=millis();
      firstRun = 0;
    }

    /*
      test e azione ostacoli frontali
    */

    testOstacoli();
    if (sensore_ost == 1) 	occlusoDavanti = vl53dist		< distOstacolo;
    else					occlusoDavanti = lidarDistance	< distOstacolo;
    

        if (occlusoDavanti && (statoRun != 5) &&(statoRun != 6) &&(statoRun != 0))
        {
			//motorSpeedRef = FERMO;
			statoRun      	= 99;   // senza rampa
			motorSpeed    	= 0;    // annullo rampa
			motorSpeedRef   = 0;
			distanza 		= odometro;
			risposta    	= "ostacolo";
			sendAnswer2(port);
			if (sensore_ost==1){
				risposta = "ost;" + String(vl53dist);
			}
			else{
				risposta = "ost;" + String(lidarDistance);
			}
			sendAnswer2(port);
        }


    /** statoRun

      il movimento è gestito da una macchina a stati. Lo stato iniziale è fermo.


      0: fermo
      1: controllo sterzo da sensore dx e distanza automatico (non attivo)
      1: controllo sterzo da sensore sx e distanza automatico (non attivo)
      2: controllo distanza, sterzo da parametro
      4: controllo distanza, sterzo da parametro da tetaRef vs teta odometria
      5: rotazione sul posto con una ruota ferma
      6: rotazione sul posto con ruote controrotanti

      99: comando arresto
    */

      // parte a 30 ms
      if ( cnt10ms >= TEMPO_CONTROLLO){

		cnt10ms = 0;
		
        exeTime = micros() - tInit;
        //Serial.println(lidarDistance);
        tInit = micros();

		if (LIDAR_PRESENT)	lidarDistance = tfmini.getDistance()/divLidar;
		else				lidarDistance = 9999;


        updatePosition();
        // ID_009 bussola

        if (bussola) compass();

        if (modoGuida == 0) tetaMisura = teta;      // ID_001
        if (modoGuida == 1) tetaMisura = tetaCompass; // ID_009
        if (modoGuida == 3) tetaMisura = tetaCompass; // ID_009
        if (modoGuida == 2) {
          tetaCompass = 0;
          modoGuida   = 1;
        }



        // controllo sterzo da lidar
        // R3: PAN  10 muro su lato DX
        // R1: PAN 170 muro su lato SX
        // kp usato 0.5
        // Se errore troppo alto andarci con S limitato
        if ((statoRun == 1)||(statoRun == 3)){

          errore = float(distRef - lidarDistance);
          // per come sono messi i motori inverto segno dell
          // deltaErrore
          deltaErrore = - errore + erroreK_1;
          erroreK_1   = errore;
          //tauD = 100e-3;
          //ts   = 25e-3
          //0.7788008 ktD  = exp(-ts/tauD)
          DerActive = DerActive*0.7788008 + kd_guida*deltaErrore*0.2211992;

          if (statoRun == 1) {
            raggiorSterzo =   kp_guida*errore  - DerActive;
          }
          else{
            raggiorSterzo =  -kp_guida*errore  + DerActive;
          }
          // errore +/- 0.5


          if (raggiorSterzo < -MAX_S) raggiorSterzo = -MAX_S;
          if (raggiorSterzo >  MAX_S) raggiorSterzo =  MAX_S;
        }

        if (statoRun == 2){
          motorSpeedRef = motorSpeedValue;
          direzione     = AVANTI;
        }

        // controllo sterzo da teta
        if (statoRun == 4){

          // regolatore PI

          errore = tetaRef - tetaMisura;
          raggiorSterzo =   kpTeta*errore;
          i_part += raggiorSterzo*kiTeta;

          // anti windup
          // limito la parte integrale se la parte proporzionale è già alta
          // questo evita di accumulare errore da scaricare
          // se la somma di parte P e I supera il massimo, la parte integrale viene
          // limitata a MAX - parte P
          if  ((raggiorSterzo + i_part) >  MAX_STERZO){
            i_part = MAX_STERZO - raggiorSterzo;
            if (i_part < 0) i_part = 0;
          }
          if  ((raggiorSterzo + i_part) < -MAX_STERZO){
            i_part = -MAX_STERZO - raggiorSterzo; // -2 - (-1) = -1, -2 -(-3) = 1
            if (i_part > 0) i_part = 0;
          }
          if (i_part>  0.5) i_part =  0.5;
          if (i_part< -0.5) i_part = -0.5;

          raggiorSterzo += i_part;

          // errore +/- 0.5

          if (raggiorSterzo < -2.0) raggiorSterzo = -2.0;   // ID_005
          if (raggiorSterzo >  2.0) raggiorSterzo =  2.0;   // ID_005
        }


        /** gestione raggiungimento angolo target
          in questo caso il robot deve ruotare per orientarsi con l'angolo dato.
          Questo vale per i modi 5 e 6.

			viene generato una rampa sull'angolo sino al target
			un controllo PI genera la velocità del motore
			
        */
        if ((statoRun == 5)||(statoRun == 6)){
		
			switch (SM_R5){
				case 0:
					posTarget = tetaRef;
					posRef    = tetaMisura;		// init rampa posizione
					
					step 		= 3.14/(1.5/0.03);
					if (posTarget < posRef)	step = -step;
					counterArresto = 0;
					SM_R5   = 1;
					break;
		
				case 1:
			
					posRef 	+= step;
					
					if (abs(posRef - posTarget) < 2*(3.14/(1.5/0.03))){
						posRef = posTarget;
						counterArresto++;
						// giunto al target lascio un secondo per stabilizzare posizione
						// trascorso il tempo fermo il movimento
						if (counterArresto > 30){
							SM_R5     = 0;
							statoRun    = 0;  // arresto senza rampa
							motorSpeed  = 0;  // annullo rampa
							motorSpeedRef = 0;
							break;
						}
					}

					//Serial.println(posRef);
					
					// regolatore PI

					errore = posRef - tetaMisura;
					raggiorSterzo =   kpTeta*errore;
					i_part += raggiorSterzo*kiTeta;

					// anti windup
					// limito la parte integrale se la parte proporzionale è già alta
					// questo evita di accumulare errore da scaricare
					// se la somma di parte P e I supera il massimo, la parte integrale viene
					// limitata a MAX - parte P
					if  ((raggiorSterzo + i_part) >  MAX_STERZO){
						i_part = MAX_STERZO - raggiorSterzo;
						if (i_part < 0) i_part = 0;
					}
					if  ((raggiorSterzo + i_part) < -MAX_STERZO){
						i_part = -MAX_STERZO - raggiorSterzo; // -2 - (-1) = -1, -2 -(-3) = 1
						if (i_part > 0) i_part = 0;
					}
					if (i_part>  0.5) i_part =  0.5;
					if (i_part< -0.5) i_part = -0.5;

					raggiorSterzo += i_part;

					// errore +/- 0.5

					if (raggiorSterzo < -2.0) raggiorSterzo = -2.0;   // ID_005
					if (raggiorSterzo >  2.0) raggiorSterzo =  2.0;   // ID_005
					
	                motorSpeed =  abs(raggiorSterzo)*motorSpeedValue;//110.0; //MODERATA;
					if (raggiorSterzo > 0) 	direzione = AVANTI;
					else					direzione = INDIETRO;

					break;
			}
        }// SM_R5


        /** gestione raggiungimento distanza target
          in questo caso il robot deve arrivare alla distanza impostata.
          Questo vale per i modi (1, 3, 2, 4).
          Sono esclusi i 5 e 6 che ruotano il robot e sono gestiti nei modi 5 e 6 da una propria macchina a stati.

          All'approsimarsi della destinazione la velocità viene diminuita per raggiungere il target senza superarlo.

        */


        if (((statoRun != 5)&&(statoRun != 6))&&(statoRun != 0)){
          if ( distanza > odometro )  direzione = AVANTI;
          else            direzione = INDIETRO;

            if ( abs(distanza - odometro) > E_APPROCCIO){
              motorSpeedRef = motorSpeedValue;
            }
            else
              if ( abs(distanza - odometro) > E_POSIZIONAMENTO){
                motorSpeedRef = APPROCCIO;
              }
              else{
                motorSpeedRef = FERMO;
                statoRun      = 99;
              }
        }
        else{ // statoRun == 5 or 6 o zero (ruota su se stessa)
          ;
        }

		scope (-1, 0, 1, 2, 3, 4);	// chiamo con cmd=-1, serve per attivare funzione
      }// fine parte temporizzata TEMPO_CONTROLLO ms


		/** questa parte genera le rampe sulla tensione del motore. Sia in accelerazione che decelerazione.
		*   Viene fatta girare con un tempo piu' veloce per poter usare gradini di riferimento più piccoli.
		*   All'arrivo a velocità zero la macchina a stati del movimento viene messa nello stato iniziale.
		*   Genera il tick per il cilco di controllo
		*/
		if ((millis()-lastTimeFast) > 10){
			lastTimeFast = millis();
			
			cnt10ms++; // counter per ciclo controllo
			
			if (statoRun == 99) motorSpeedRef = 0;

			if ((statoRun != 6)&&(statoRun != 5)){
				// rampa sulla velocita'
				if (motorSpeedRef > motorSpeed) motorSpeed += 2;
				if (motorSpeedRef < motorSpeed) motorSpeed -= 4;  // 15

				if (motorSpeed > 250) motorSpeed = 250;
				if ((motorSpeed <   1)&&(statoRun == 99)){
					motorSpeed = 0;
					statoRun   = 0;
				}
			}
				
			differenziale(motorSpeed);	
		}




  }


  /**
    modo di test. viene usato durante il debug dell'hw. viene configurato alla bisogna.
    I comandi arrivano diretti dalla seriale e codificati comenecessario.
  */
    if (mode == TEST_MOTORE){
        Serial.println("start TEST_MOTORE mode, enter motorSpeed, direzione");
    while (1){
      while (Serial.available() > 0) {

        // format x, y
        // look for the next valid integer in the incoming serial stream:
        motorSpeedRef = Serial.parseInt();
        // do it again:
        direzione = Serial.parseInt();

        //PDURATION = Serial.parseInt();

        // look for the newline. That's the end of your
        // sentence:
        if (Serial.read() == '\n') {
          // constrain the values to -50 - 50
      //                    x = constrain(x, -150, 150);
      //                    y = constrain(y, -200, 200);

          Serial.print(motorSpeedRef);
          Serial.print(',');
          Serial.println(direzione);

        }
      }

      if (motorSpeedRef < 0){ // == INDIETRO){
        driver.motorBForward(motorSpeedRef);
        driver.motorAReverse(motorSpeedRef);
      }
      else{
        driver.motorAForward(motorSpeedRef);
        driver.motorBReverse(motorSpeedRef);
      }

    }
  }

}

//------------------- fine main --------------------------------
