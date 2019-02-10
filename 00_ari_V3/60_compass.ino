

void displaySensorDetails(void)
{
  sensor_t sensor;
  mag.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" uT");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" uT");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" uT");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void getSensorReading(void){
static String dato;
  /* Get a new sensor event */
  mag.getEvent(&event);

  /* Display the results (magnetic vector values are in micro-Tesla (uT)) */
  dato  = "X;Y;Z;";
  dato += event.magnetic.x;
  dato += ";";
  dato += event.magnetic.y;
  dato += ";";
  dato += event.magnetic.z;
  dato += ";";
  dato += "uT";
  /*
  Serial.print("X;Y;Z;");
  Serial.print(event.magnetic.x); Serial.print(";");
  Serial.print(event.magnetic.y); Serial.print(";");
  Serial.print(event.magnetic.z); Serial.print(";");
  Serial.println("uT");
*/
  /* Note: You can also get the raw (non unified values) for */
  /* the last data sample as follows. The .getEvent call populates */
  /* the raw values used below. */
  // Serial.print("X Raw: "); Serial.print(mag.raw.x); Serial.print("  ");
  // Serial.print("Y Raw: "); Serial.print(mag.raw.y); Serial.print("  ");
  // Serial.print("Z Raw: "); Serial.print(mag.raw.z); Serial.println("");
}


/*!
    @brief  Gets the most recent sensor values and transform in angle.

            This function reads only x and y magnetometer values. The coefficients for correction are applied.

      the output is tetaCompass which is a global variable.

      ox: offet on x axis

      oy: offset on y axis

      ky: gain on y axis


    @param  none
    @return none. The output valie is on tetaCompass global variable
*/

void compass(void){

  xc =  event.magnetic.x + ox;
  yc = (event.magnetic.y + oy)*ky;

//        tetaCompass = atan2 (yc, xc);     // arc tangent of y/x

  // ottengo angolo per integrazione
  // l'angolo andare oltre 2PI e può essere resettato
  // facilmente
  tetaCompassRead = atan2 (yc, xc);
  deltaCompass  = tetaCompassRead - tetaCompass_prev;
  tetaCompass_prev= tetaCompassRead;
  // considero che tra due campioni non ci siano oltre 3.28 rad
  // se ci sono c'è stato lo scaravolto dell'angolo
  if (deltaCompass >=  3.28) deltaCompass -= 6.28;
  if (deltaCompass <= -3.28) deltaCompass += 6.28;

  // meno per avere angolo positivo in senso antiorario
  // concorde con encoder

  tetaCompass -= deltaCompass;
}
