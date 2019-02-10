
/**
  @brief  sterzo con differenziale. Per curvare la velocità relativa delle due ruote viene modificata. Con le velocità uguali proseguo diritto, con una maggiore dell'altra curvo.

      raggiorSterzo (variabile globale) indica lo scorrimento che applichiamo alle ruote. E' un numero tra 0 e 2.
      ponendo s = raggiorSterzo abbiamo che le velocita' alle ruote valgono:

      VA = motorSpeed*(1-s)
      VB = motorSpeed*(1+s)

      In funzione di StatoRun si hanno diversi modi operativi

      StatoRun  .. : differenziale in funzione di raggiorSterzo

      StatoRun == 5: curva con una ruota ferma
      StatoRun == 6: curva con ruote controrotanti

    @param  motorSpeed. Il valore a cui limitare la velocita' massima applicata ai motori.

*/

void differenziale(float motorSpeed){
static float rs;
static float Vlimite;

  if ((statoRun != 5)&&(statoRun != 6)){

    rs = raggiorSterzo;

    /* quando viene fatta una curva con una ruota bloccata l'altra ruota va a 255.
       partendo da fermo ci può essere slittamento.
       In questo caso si limita la velocità della ruota che marcia.
    */
    Vlimite = 255;
    if (rs >  MAX_STERZO){      // ID_005
      rs =  MAX_STERZO;
      Vlimite = motorSpeed;
    }
    if (rs < -MAX_STERZO){
      rs = -MAX_STERZO;
      Vlimite = motorSpeed;
    }

    VA = motorSpeed*(1.0+rs);
    VB = motorSpeed*(1.0-rs);

    if (VA >  Vlimite) VA =  Vlimite;
    if (VA < -Vlimite) VA = -Vlimite;

    if (VB >  Vlimite) VB =  Vlimite;
    if (VB < -Vlimite) VB = -Vlimite;

    if (direzione == INDIETRO){
      if (VA >= 0 ){
      dirVA = -1;   // ID_005
      driver.motorAReverse(VA);
      }
      else{
      dirVA =  1;
      driver.motorAForward(VA);
      }
      if (VB >= 0 ){
      dirVB = -1;
      driver.motorBReverse(VB);
      }
      else{
      dirVB =  1;
      driver.motorBForward(VB);
      }
    }
    else{ // direzione == AVANTI

      if (VA >= 0 ){
      dirVA =  1;   // ID_005
      driver.motorAForward(VA);
      }
      else{
      dirVA = -1;
      driver.motorAReverse(VA);
      }
      if (VB >= 0 ){
      dirVB =  1;
      driver.motorBForward(VB);
      }
      else{
      dirVB = -1;
      driver.motorBReverse(VB);
      }
    }
  }
  else
    if (statoRun == 5){// con una ruota ferma ruota su se stesso

      if (direzione == AVANTI){
        VB = 0;
        VA = motorSpeed;
        dirVA = 1;
        dirVB = 0;
        driver.motorBForward(VB);
        driver.motorAForward(VA);
      }
      else{
        VA = 0;
        VB = motorSpeed;
        dirVB = 1;
        dirVA = 0;
        driver.motorBForward(VB);
        driver.motorAForward(VA);
      }
    }
    else
    if(statoRun == 6){// ruota su se stesso con ruote in direzioni opposte
      if (direzione == AVANTI){
        VA = motorSpeed;
        VB = motorSpeed;
        dirVB = -1;
        dirVA =  1;
        driver.motorAForward(motorSpeed);
        driver.motorBReverse(motorSpeed);
      }
      else{
        VA = motorSpeed;
        VB = motorSpeed;
        dirVB =  1;
        dirVA = -1;
        driver.motorBForward(motorSpeed);
        driver.motorAReverse(motorSpeed);
      }
    }

    // informazione per lettura encoder
    if (VA == 0)  VA_zero = 1;
    else      VA_zero = 0;
    if (VB == 0)  VB_zero = 1;
    else      VB_zero = 0;

}
