
//ID_002 *********************ale

  void sendAnswer2(char port){
    //*********ale
    ESPserial.println("!"+risposta+"?");
    //Serial.println   ("!"+risposta+"?"); ID_010
    //*********ale
    risposta="";
  }

/** @brief richieste: 
    viene fatta una richiesta. riorna una o più variabili in risposta
    @param comando

    p: 
    risposta = "pos:"     +\
    String(millis())    +";"+\
    String(xpos)      +";"+\ 
    String(ypos)      +";"+\
    String(teta)      +";"+\
    String(tetaCompass)   +";"+\
    String(statoRun)    +";"+\
    String(raggiorSterzo)   +";"+\
    String(errore)      +";"+\
    String(vl53dist)    +";"+\
    String(inputString.substring(2)) ;

    
  case 'm':
    risposta = "mis:"     +\
    String(vl53dist)      +";"+\
    String(lidarDistance);
    break;
    
*/
void richieste(void)
{
static float x, y;
        Serial.println("richiesta");
        Serial.println(inputString);
    
        switch (char(inputString[1])) {
          case 'a':
              risposta = "a: " + String(xpos);
            break;

          case 'b':
              risposta = "b: " + String(ypos);
            break;

          case 'c':
              risposta = "c: " + String(teta);
            break;


          case 'd':
              risposta = "d: " + String(odometro);
            break;

          case 'e':
              risposta = "e: " + String(errore);
            break;

          case 'f':
              risposta = "f: ";   // ID_009
              risposta += event.magnetic.x;
              risposta += ";";
              risposta += event.magnetic.y;
              risposta += ";";
              risposta += event.magnetic.z;
            break;

          case 'g':
              risposta = "g: " + String(tetaCompass);
            break;

          case 'h':
              risposta = "h-V FW: " + String(V_FW_ATMEGA);
            break;

      // comandi registratore (digital scope)
      // ixyyyy
      // x: comando
      // y: argomento comando
      case 'i':
        scope( char(inputString[2]), inputString.substring(3).toInt(),0,0,0,0);
        
            break;

      case 'm':
        risposta = "m:"     +\
        String(vl53dist)      +";"+\
        String(lidarDistance);
        break;

      case 'p':
        if (monitorDati) return;
        digitalWrite(LED2, !digitalRead(LED2));
        
        risposta = "pos:"     +\
        String(millis())    +";"+\
        String(inputString.substring(2)) +";"+\
        String(xpos)      +";"+\ 
        String(ypos)      +";"+\
        String(teta)      +";"+\
        String(tetaCompass)   +";"+\
        String(statoRun)    +";"+\
        String(raggiorSterzo)   +";"+\
        String(errore)      +";"+\
        String(vl53dist);//     +";"+\
//        String(lidarDistance);
        break;

      case 'q':
              // eseguo una scansione
              int range_ang1;
              int range_ang2;
              int passo_ang;

              // ricevo una stringa da sezionare
              range_ang1 = String(inputString.substring(2, 5)).toInt();
              range_ang2 = String(inputString.substring(5, 8)).toInt();
              passo_ang  = String(inputString.substring(8,10)).toInt();

              // limito il movimento causa limiti servo
              if (range_ang1 > 170) range_ang1 = 170;
              if (range_ang2 > 170)   range_ang2 = 170;
              if (range_ang1 < 10)  range_ang1 = 10;
              if (range_ang2 < 10)  range_ang2 = 10;

              // gestisce scan in accordo alla direzione
              if ( range_ang1 > range_ang2){

                passo_ang=passo_ang*-1;

                for (int ang=range_ang1; ang>=range_ang2; ang=ang+passo_ang)
                {
                  //settare angolo pan
                  servoPan.write ( ang);
                  delay(200);
                  risposta = lidarMisuraAmb(10,ang) ;
                  sendAnswer2(port);
                }
              }
              else{
                for (int ang=range_ang1; ang<=range_ang2; ang=ang+passo_ang){
                  //settare angolo pan
                  servoPan.write ( ang);
                  delay(200);
                  risposta = lidarMisuraAmb(10,ang) ;
                  sendAnswer2(port);
                }
              }
              risposta = "echf";
              sendAnswer2(port);
            break;

          case 'r':
              risposta = "r: " + String(statoRun)+";"+String(inputString.substring(2)); // ID_008
            break;

          case 's':
              risposta = "s: " + String(raggiorSterzo, 3);
            break;

          case 'v':
              risposta = "v: " + String(motorSpeed);
            break;

          case 'w':
              risposta = "w: " + String(VB);
            break;

          case 'z':
              risposta = "z: " + String(VA);
            break;

          case '2':
              risposta = "Sx: " + String(odometroSxCnt);
            break;

          case '3':
              risposta = "Dx: " + String(odometroDxCnt);
            break;
        }

    sendAnswer2(port);


  }
