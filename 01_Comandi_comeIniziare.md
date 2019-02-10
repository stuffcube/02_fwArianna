# Come muovere ARI V3.

Ari non si muove da sola, si muove su commando. I comandi vengono inviati tramite una riga di commando (Command Line Interface), una GUI oppure un sw.
 Per iniziare a capire usare la CLI è una buona soluzione.

 

[TOC]

# i comandi e le richieste

il dialogo con il robot segue il semplice protocollo qui illustrato.  Ad ARI è possibile inviare scritture di valori o comandi (ad esempio dire di quanto avanzare) oppure inviare richieste di lettura di valori (esempio lo stato del movimento). Nel primo caso la risposta è una conferma e l'azione viene effettuata. Nel secondo caso la risposta contiene il valore (ad esempio lo stato è uguale a 0, il che significa sono fermo).

La differenza tra scritture e letture viene indicata con il primo numero del messaggio, 1 lettura, 3 scrittura. Il secondo carattere specifica l'oggetto dell'invio. Ad esempio 3L1 significa 3 scrivi, L il soggetto è il Laser, 1 l'argomento 1 acceso, 0 spento. 3L1 accendi laser, 3L0 spegni laser.
1r richiede lo stato del movimento. Il ritorno è il valore dello stato di Run attuale.

Per ragioni "storiche" le scritture hanno lettere maiuscuole, le letture minuscole.

Nei paragrafi iniziali i comandi verranno scritti per esteso comprendendo 1 e 3 iniziali. questo verrà via via tralasciato.

# Primi movimenti

 i primi comandi da inviare possono essere:

**3L1**, 3**L0**: accendo e spengo il puntatore

**3P45**: oriento pan a sinistra, 3P90 lo metto al centro

visto che la comunicazione funziona passo ai primi movimenti.

## l'orientamento di ARI

il sistema di coordinate di ARI_V3 è cartesiano assoluto con angoli crescenti in senso orario. La posizione del robot è identificata da una terna (x, y, teta). Il sistema è assoluto e stabilito all'atto dell'accensione. L'asse x coincide con la direzione dove guarda Ari al momento dell'accensione, l'asse y punta a 90 antiorari. Gli angoli sono crescenti in direzione oraria con lo zero allineato con l'asse x.

Il sistema non è quindi relativo alla posizione corrente del robot. Angoli e direzioni si rifanno al'atto dell'accensione.

## spostamento verso una direzione 

si vuole muovere il robot di una certa distanza in una direzione Alfa. 

**3A0**		programmo l'angolo di avanzamento *Alfa* a zero gradi. Lo zero è la direzione verso cui punta il robot all'atto dell'accensione. L'angolo è assoluto ed è quindi stabilito all'accensione.

**3D1000**	programmo un avanzamento di 1000 mm nella direzione di Alfa

**3R4**		metto il robot in stato Run di tipo 4. Cioè avanzare per la distanza **D** lungo la direzione Alfa.

durante il movimento si arresta il robot inviando 3R99.

## rotazione sul posto verso una direzione

per far puntare ARI verso un angolo usare il comando R6

**3A90**		angolo 90 gradi

**3R6**		ARI si orienta a 90 gradi ruotando sul posto

### esercizi:

#### percorre un quadrato

percorre un quadrato di lato 1000 in senso orario, al termine orientarsi nella direzione di partenza.

3A0, 3D1000, 3R4
al temine del movimento proseguire con
3A90, 3D1000, 3R4
......
......

al termine fare 3A360, 3R6. Cosa accade facendo 3A0, 3R6. Perchè?

#### l'orologio

muoversi nelle varie direzioni ruotando sul posto. 

## movimento parallelo a una parete

i modi Run R1 e R3 attivano un controllo PD (proporzionale Derivativo) che regola lo sterzo per mantenere costante la misura fatta dal Lidar. Puntando il Lidar a 45 gradi dalla parete il robot viaggia parallelo alla parete seguendone le curvature.

i parametri necessari sono:

 **Onxx**: impostazione deistanza di riferimento e distanza ostacolo

​        O0: distRef           [cm]. riferimento per guida a distanza fissa nei modi R1 e R3
​        O1: distOstacolo [cm]. distanza minima sotto la quale ARI si arresta

O0 è la distanza che si vuole mantenere dalla parete. O1 va messo maggiore di O0 per evitare che la parete venga riconosciuta come ostacolo.

**Pxxx**:   movimento Pan

orienta il servo PAN della testa. xxx è in gradi, 90° guarda in avanti, 0 a sinistra e 180 a destra.
In alcuni casi, a seconda del servo usato, è bene limitare l'escursione ad esempio tra 10 e 170°.

### viaggiare lungo la parete

appoggio il robot circa parallelo alla parete alla sua sinistra. Con il comando 3P135 oriento la testa verso la parete. Leggo la misura della distanza dalla parete.  
Faccio questo leggendo il valore "1p" . La risposta è simile a "m:65;110". m: è l'identificatore della risposta, il primo numero è la distanza letta dal sensore frontale, il secondo è la distanza misurata dal lidar montato sulla testa. Il promo sensore a un angolo di lettura più ampio e guarda sempre in avanti, questo permette di rilevare ostacoli durante il movimento. A noi ora interessa il secondo numero.

in questo caso porre la distanza pari a 65  inviando 3O065.
mettere *distOstacolo* maggiore di *distRef* (O1), per esempio maggiore di 30 cm. 3O195

scrivo la distanza da percorre a 2000 mm con 3D2000 e avvio la marcia con 3R1. Con qualche oscillazione il robot dovrebbe marciare parallelo al muro.

gli angoli pan (P) sono P45  seguo parete dx con R3
gli angoli pan (P) sono P135 seguo parete sx con R1



### Esercizi

#### marcia con la parete a destra

con le opportune modifiche faccio muovere il robot mantenendolo parallelo alla parete alla sua destra

#### La sentinella

percorro la parete, arrivo al fondo. mi giro e la ripercorro in senso contrario. Ripeto la cosa più volte.

#### cambio della distanza durante la marcia

come nella sentinella cambiando però la distanza dalla parete ad ogni percorrenza. Modificare la distanza poco alla volta per evitare perdite di orientamento. La distanza può essere cambiata durante la marcia.



# Elenco comandi

### scritture

le scitture vogliono un "3" iniziale e un argomento, esempio 3A90.

#### Axxx: Alfa. assegna alfa 

Alfa. assegna Alfa, la direzione del robot. xxx è l'angolo in radianti, es 3.14. La direzione zero è definita all'accensione del robot. E' la direzione in avanti. Coincide con l'asse x.
Alfa è positivo in senso antiorario.

#### Cxxx: assegna raggio di sterzo

assegna "raggiorSterzo" in [m]. La variabile è usata nella movimento R2, stabilisce il raggio della circonferenza su cui ruota il robot.

#### Dxxx: distanza da percorrere
Distance. La distanza che viene percorsa nel prossimo Run espressa in [mm]. La distanza è sempre incrementale. 

#### Ex: EEprom
EEprom.  Esegue operazioni su dei parametri di taratura. Vedi procedura DataEEprom. 

​E0 SCRIVI i parametri in E2prom,            
​E1 LEGGI i parametri in E2prom,             
​E2 rispristina in valori di DEFAULT,            
​E3 mostra i parametri CORRENTI,             



#### Fnxxx: impostazione dei parametri meccanici del robot
imposta dei parametri del robot. "n" indica quale parametro, "xxx" è il valore. 

​F0xx ED             
​F1xx ED_BASE            
​F2xx BASELINE       mm
​F3xx GIRO_RUOTA     mm  = sviluppo ruota[mm]/(4*ppr)
F4xx Divisore lidar
F5xx offest servo Pan [°]

​N.B. questi valori vanno attivati con un 3E3

 

#### Gxxx: Guide mode
"Guide Mode". definisce il modo di guida nel run "R4". Il teta di feedback, tetaMisura, può provenire dall'odometria, dalla bussola, o altri mix possibili.
Nel codice sotto teta è l'angolo ricavato dagli encoder. Il modo zero si affida alle misure fatte dagli encoder. Il modo 1 e 3 ricavano il teta direttamente dalla bussola. Nel modo 3 anche le posizioni x e y  sono calcolate dal teta della bussola.con il modo 2 è possibile imporre l'angolo corrente come angolo zero. Altrimenti la direzione è    assoluta come determinata dalla bussola.

`​if (modoGuida == 0) tetaMisura = teta;`
`	if (modoGuida == 1) tetaMisura = tetaCompass;                
    if (modoGuida == 3) tetaMisura = tetaCompass;               
​    if (modoGuida == 2) {                   `
`​   	tetaCompass = 0;    // definisco la direzione corrente come zero.                   
​        modoGuida   = 1;`
`   }`

​ in updatePosition integro posizioni in funzione del modoGuida attivo
`if (modoGuida == 3) {                   
	xpos    +=  deltaC*cos(tetaCompass);                    
	ypos    +=  deltaC*sin(tetaCompass);                    
}`
`else{                   
	xpos    +=  deltaC*cos(teta);                   
	ypos    +=  deltaC*sin(teta);                   
}` 

#### Knxx:   assegnazione dei guadagni dei regolatori
K0 kpTeta   il guadagno proporzionale kp usato nel modo Run 4
​K1 kiTeta
​K2 kp_guida il guadagno proporzionale kp usato nel modo Run 1 e 3 (sensore di distanza laterale)
​K3 kd_guida il guadagno derivativo    kd usato nel modo Run 1 e 3 (sensore di distanza laterale)

#### H0: Homing
Homing, assegna lo posizione corrente (x, y, teta) = (0, 0, 0) 

#### Lx: accende puntatore
Led. puntatore a Led acceso, x=1, o spento x=0.

#### Mx: monitor dati
Monitor. Attiva, 1, o disattiva, 0, l'invio continuo di misure da parte del robot. le msiure sono inviate nella routine "updatePosition" ogni 100 ms. La stringa ha l'header "mon:"

`if (monitorDati && (counter >=4) ){`
`​        counter = 0;`
`​        risposta  = "mon;";`
`​        risposta += (dDxCnt);`
`​        risposta += (";");`
`​        risposta += (dSxCnt);`
`​        risposta += (";");`
`​        risposta += (deltaC);`
`​        risposta += (";");`
`​        risposta += (teta);`
`​        risposta += (";");`
`​        risposta += (xpos);`
`​        risposta += (";");`
`​        risposta += (ypos);`
`​        risposta += (";");`
`​        risposta += (i_part);// raggiorSterzo`
`​        risposta += (";");`
`​        risposta += event.magnetic.x;`
`​        risposta += ";";`
`​        risposta += event.magnetic.y;`
`​        risposta += ";";`
`​        risposta += event.magnetic.z;`
`​        risposta += ";";`
`​        sendAnswer2(port);`
`}`

 

#### Nnxxx:  assegna i coefficienti di correzione della bussola.

coefficienti di correzione bussola. 

​        **N0**: ox    sommato lungo asse x
​	**N1**: oy	sommato lungo asse y
​	**N2**: ky	moltiplicatore asse z 

 

#### Onxx:  impostazione distanze di riferimento   

​        **O0**: distRef      	[cm]. riferimento per guida a distanza fissa nei modi R1 e R2
​        **O1**: distOstacolo 	[cm]. distanza minima sotto la quale ARI si arresta

#### Pxxx:   movimento Pan

orienta il servo PAN della testa. xxx è in gradi, 90° guarda in avanti, 0 a sinistra e 180 a destra.
In alcuni casi, a seconda del servo usato, è bene limitare l'escursione ad esempio tra 10 e 170°.

#### Rx:     Run. 

Definisce in che modo effettuare la corsa "Run". Il run è definito con un obbiettivo (target).        Il raggiungimento del target termina il run.

​         **x=4: R4**. Il robot si muove sino a percorrere la distanza impostata con "D" nella direzione alfa impostata con "A".

​        **x=5: R5**. Il robot si muove sino a orientarsi nella direzione alfa impostata con "A". La rotazione fa perno su una ruota che rimane ferma.

​        **x=6: R6**. Il robot si muove sino a orientarsi nella direzione alfa impostata con "A". La rotazione avviene con perno al centro del robot con ruote controrotanti.

​        **x=2: R2**. Il robot effettua un percorso circolare con raggio pari al parametro "C". Il raggiorSterzo può anche essere imposto direttamente con il comando "S". In questo caso sarà lo scorrimento tra le ruote dx e sx.

​         **x=1 o 3: R1 e 3**. i modi 1 e 3 fanno viaggiare il robot a una distanza fissa dalla parete laterale. la misura è fatta orientando il LIDAR.
​		R1 lato sinistro, P135 (pan a 135 gradi)
​		R3 lato destro,   P45   (pan a   45 gradi)  

​        **x=99:** comanda l'arresto del movimento

​         Il movimento si arresta quando il lidar vede un ostacolo troppo vicino. La distanza è impostata con "O1"

 

#### Sxxx:   Scorrimento tra le ruote

Imposta la variabile "raggioDiSterzo". Questa è un numero tra -1 e 1. 
Zero significa scorrimento nullo, cioè le due ruote hanno la stessa velocità comandata.

​        vedi procedura "differenziale"
​        ponendo s = raggiorSterzo abbiamo che le velocita' alle ruote valgono:

​		VA = motorSpeed*(1-s)           
​		*VB = motorSpeed*(1+s)       

#### Txxx:   Tilt

orienta il servo TILT della testa. xxx è in gradi, 90° guarda in avanti, 0 a sinistra e 180 a destra.
In alcuni casi, a seconda del servo usato, è bene limitare l'escursione ad esempio tra 10 e 170°.

#### Vxxx:   Velocità. 

Definisce la velocità del robot. é il valore assegnato al PWM.  Valori tra 0 e 255.   

#### Zxxx:   limite allo scorrimento

definisce MAX_S. è il limite dello scorrimento (positivo e negativo) applicabile dal controllo.

 

### Letture

le letture vogliono un "1" iniziale, esempio 1m

#### m: richiede distanze lette da sensore frontale e lidar

la risposta è m:distanzaSensoreFrontale;distanzaLidar. il sensore frontale ritorna 999 se non rileva nulla. il lidar nello stesso caso ritorna -1 

#### p: stato del robot

ritorna una stringa separata da ";" con i seguenti campi

				risposta = "pos:"			+\
				String(millis())		+";"+\
				String(inputString.substring(2)) +";"+\
				String(xpos)			+";"+\ 
				String(ypos)			+";"+\
				String(teta) 			+";"+\
				String(tetaCompass) 	+";"+\
				String(statoRun) 		+";"+\
				String(raggiorSterzo) 	+";"+\
				String(errore)			+";"+\
				String(vl53dist);//	    +";"+\
#### r: ritorna lo stato di Run del robot

0: fermo
x: tipo di run comandato (es 4 a seguito di un R4)

