
void setup() {
    // initialize serial:
    Serial.begin  (115200);
    Serial.print  ("setup starting! FW version: ");
    Serial.println  (V_FW_ATMEGA);

    pinMode(LED1,         OUTPUT);
    pinMode(LED2,         OUTPUT);
    pinMode(LED3,         OUTPUT);
    
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, HIGH);
    
    
    if (LIDAR_PRESENT){
      TFserial.begin  (TFMINI_BAUDRATE);  // seriale lidar
      tfmini.begin    (&TFserial);
    }
    
      ESPserial.begin (115200);  // ID_002
    
      // LSM303_magn unit
      /* Enable auto-gain */
      mag.enableAutoRange(true);
    
      /* Initialise the sensor */
      Serial.println("init LSM");
    bussola = 1;
    if(!mag.begin()){
      /* There was a problem detecting the LSM303 ... check your connections */
      Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
      bussola = 0;
    }
    else{
      Serial.println("init LSM done");
      /* Display some basic information on this sensor */
      displaySensorDetails();
    }
    
      sensore_ost=1;
    if (!lox.begin()) {
      Serial.println(F("Failed to boot VL53L0X"));
      sensore_ost=0;
    }
    else{
      Serial.println("init VL53L0X done");
    }
    pinMode(ledPin,   OUTPUT);
    pinMode(laserPin,   OUTPUT);
    
    // setup digital inputs for IR sensors
    pinMode(R_SIDE_FRONT,     INPUT);     // non usato
    pinMode(L_SIDE_FRONT,     INPUT);
    pinMode(ENB_R_SIDE_FRONT,   OUTPUT);    // non usato
    pinMode(ENB_L_SIDE_FRONT,   OUTPUT);
    
    
    pinMode(GIRO_DX_PIN,      INPUT);
    pinMode(GIRO_SX_PIN,      INPUT);
    
    
    digitalWrite(laserPin, LOW);
    
    // attaches the servo on pin .. to the servo object
    servoPan.attach  (SERVO_PAN_PIN );
    servoTilt.attach (SERVO_TILT_PIN);
    
    servoPan.write ( 90);
    servoTilt.write( 90);
    
    
    // Attach motors to the input pins:
    driver.attachMotorA(MTR_A_DX_P1, MTR_A_DX_P2);
    driver.attachMotorB(MTR_B_SX_P1, MTR_B_SX_P2);
    
    differenziale(0);
    
    // odometro
    attachInterrupt(digitalPinToInterrupt(GIRO_DX_PIN), odometroDxMisuraHW, CHANGE);
    attachInterrupt(digitalPinToInterrupt(GIRO_SX_PIN), odometroSxMisuraHW, CHANGE);
    
    Serial.flush();
    ESPserial.flush();  // ID_002
    
    
    DataEEprom(LEGGI);  // carica coefficienti da e2prom
    //DataEEprom(5);    /// 5 ARI3, 4 ARI2 carica coefficienti da e2prom
    
    Serial.println( risposta   );
    
    firstRun = 1;
    
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    
    Serial.println("setup done!");

}
