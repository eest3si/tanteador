/** ###################################################################
**    LIBRERIA DE FUNCIONES PARA EL PROYECTO TANTEADOR DE PING-PONG
**
**     Archivo    : tanteador.h
**     Version    : 1.7
**     Fecha      : 20161113
**     Micro      : MC68HC908AP8CFB (44 pines)
**     Autores    : 6to 1ra (A) 2016
**
** ###################################################################*/

#pragma MESSAGE DISABLE C5703 /* WARNING C5703: Parameter declared but not referenced */

/** ---------------- Seccion defines ------------------ **/

/* Macros booleanos */
#define TRUE   1
#define FALSE  0
#define ON     1
#define OFF    0
#define INPUT  0
#define OUTPUT 1

/* Macros para los leds de proposito general */
#define GUION       	      PTA_PTA2  		// activo alto
#define DOS_PUNTOS	    	  PTB_PTB0  		// activo bajo
#define EnciendeGuion		  GUION = ON;		// activo alto
#define ApagaGuion			  GUION = OFF;
#define Enciende2Puntos		  DOS_PUNTOS = ~ON;	// activo bajo
#define Apaga2Puntos		  DOS_PUNTOS = ~OFF;

/* Macros para los displays */
#define SEG_A                 PTD_PTD0
#define SEG_B                 PTD_PTD1
#define SEG_C                 PTD_PTD2
#define SEG_D                 PTD_PTD3
#define SEG_E                 PTA_PTA4
#define SEG_F                 PTA_PTA5
#define SEG_G                 PTA_PTA6
#define SEG_DP                PTA_PTA7
#define DISPLAY_1             PTC_PTC0
#define DISPLAY_2             PTC_PTC1
#define DISPLAY_3             PTB_PTB5
#define DISPLAY_4             PTB_PTB7
#define DISPLAYS_1_2          12
#define DISPLAYS_3_4          34
#define DEMORA_DISPLAY_MS     5    // demora en ms para la barrida por display 
#define EnciendePuntoDecimal  SEG_DP = ON;
#define ApagaPuntoDecimal     SEG_DP = OFF;

/* Macros para el LCD */
#define LCD_E         PTC_PTC0
#define LCD_RS        PTC_PTC1
#define LCD_RW        PTB_PTB5
#define LCD_BACKLIGHT PTB_PTB7
#define FILA_0 0
#define FILA_1 1
#define COL_0  0
#define EnciendeBacklight LCD_BACKLIGHT = ON;
#define ApagaBacklight    LCD_BACKLIGHT = OFF;

/* Macros para los pulsadores */
#define SW1 PTD_PTD4
#define SW2 PTD_PTD5
#define SW3 PTD_PTD6
#define SW4 PTD_PTD7
#define ANTIRREBOTE 3  // N x 100 ms de antirrebote

/* Macros modulo ADC */
#define ArrancarConversionAD  ADSCR = 0x03; // conversion unica, ADCH3/PTA3
#define ConversionCompleta    ADSCR_COCO
#define CELSIUS 1
#define KELVIN  0

/* Macros sensores */
#define SENSOR_IR         PTB_PTB4
#define SENSOR_TEMP       PTA_PTA3
#define HabilitarSensorIR DDRB_DDRB4 = INPUT;

// Macros para cambiar un bit de una variable (o registro)
#define bitset(var,bit) ((var) |= 1 << (bit))
#define bitclr(var,bit) ((var) &= ~(1 << (bit)))

/* Macros para la trama SIRC */
#define UNO_LOGICO  7000    // El "1" (1.2ms) se toma a 1.4ms para minimizar error (1.4ms = 7000 en el contador)
#define CERO_LOGICO 4000    // El "0" (0.6ms) se toma a 0.8ms para minimizar error (0.8ms = 4000 en el contador)

/** ---------------------- Variables globales --------------------------- **/
const char KBI_MASK[4] = { 16, 32, 64, 128 }; // mascara de bits para KBIER
const char DiasMeses[12+1] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
unsigned char Digitos[4+1];			// para los displays
unsigned char Antirrebote[4];       // contador antirrebote para los pulsadores
unsigned char SW[4];                // almacena el valor de cada pulsador
unsigned char P1mas = FALSE;            // Flag P1+
unsigned char P1menos = FALSE;          // Flag P1-
unsigned char P2mas = FALSE;            // Flag P2+
unsigned char P2menos = FALSE;          // Flag P2-
unsigned char Okey = FALSE;				// Flag boton OK del remoto
unsigned char ModoTest = FALSE; 		// Flag Modo Test
unsigned char ModoConfig = FALSE;		// Flag Modo Config
unsigned char ReiniciarPartida = FALSE;	// Flag Restart Game
unsigned char Timer5Seg = OFF;			// Flag que esta en "1" durante 5 segs
unsigned char ContadorTimer5Seg = 0;
unsigned char Timer1Min = OFF;			// Flag que esta en "1" durante 1 minuto
unsigned int  ContadorTimer1Min = 0;
unsigned char ContadorParpadeo = 0;
unsigned char Hora = 12;
unsigned char Minutos = 0;
unsigned char Segundos = 0;
unsigned char Dia = 1;
unsigned char Mes = 1;
unsigned char Anio = 16;
unsigned char EsBisiesto = FALSE;
unsigned char TickParpadeo = 0;     // Marca el parpadeo de 500ms para los displays
unsigned char ComandoIR = 129;      // Comando IR recibido (valor inicial para evitar lecturas espureas)
unsigned int  AnchoDePulso;         // Lapso de tiempo capturado por TIM1
unsigned char Comando = 0;          // Primeros 7 bits de la trama SIRC
unsigned char BitTrama = 0;         // Contador de bits recibidos trama SIRC
unsigned char i, j;                 // para los bucles for() de las ISR

/** ---------------- Seccion inicializacion de modulos ------------------ **/

void configuraCPU(void)
/** Inicializacion de registros "write once" para el HC908 **/
{
  /* CONFIG1: COPRS=0,LVISTOP=0,LVIRSTD=1,LVIPWRD=1,LVIREGD=1,SSREC=0,STOP=1,COPD=1 */
  CONFIG1 = 0x3B;   // deshabilita COP
  /* CONFIG2: STOP_ICLKDIS=1,STOP_RCLKEN=0,STOP_XCLKEN=0,OSCCLK1=1,OSCCLK0=0,??=0,??=0,SCIBDSRC=1 */
  CONFIG2 = 0x91;   // fuente de reloj: XTAL
}

void inicializaPLL(void)
/************************************************************************
* seteo PLL para generar 4,9152 MHZ de FBUS desde cristal de 32,768 kHz *
* R=1    N=$258   P=0     E=2    L=$27 (valores de tabla manual AP32)   *
* PCTL: PLL Control Register                                            *
* PMS:  PLL Multiplier Select Register (High & Low)                     *
* PBWC: PLL Bandwith Control Register                                   *
* PMDS: PLL Reference Divider Select Register                           *
* PMRS: PLL VCO Range Select Register                                   *
*************************************************************************/
{
  PCTL_PLLON = OFF;   // PLL:OFF
  PCTL_PRE = 0;       // P=0
  PCTL_VPR = 2;       // E=2
  PMSH = 2;           // N_HIGH = 2
  PMSL = 0x58;        // N_LOW = 58H
  PMDS_RDS0 = 1;      // R=1 (default)
  PMRS = 0x27;        // L=27H
  PCTL_PLLON = ON;    // PLL:ON
  PBWC_AUTO = ON;     // Enganche automatico
  while(!PBWC_LOCK);  // esperar a LOCK = 1
  PCTL_BCS = 1;       // cambio a frecuencia del PLL
}

void habilitaDisplays(void)
/** Configura los pines compartidos por el LCD y los 4 displays del Edukit **/
{
/** --------------------------------------
**  Segmento |  Linea LCD |  Linea MCU    
**  --------------------------------------
**     a     |     DO     |   PTD_PTD0    
**     b     |     D1     |   PTD_PTD1    
**     c     |     D2     |   PTD_PTD2    
**     d     |     D3     |   PTD_PTD3    
**     e     |     D4     |   PTA_PTA4    
**     f     |     D5     |   PTA_PTA5    
**     g     |     D6     |   PTA_PTA6    
**     d.p.  |     D7     |   PTA_PTA7    
**  --------------------------------------
**  display1 |  E negado  |   PTC_PTC0
**  display1 | RS negado  |   PTC_PTC1
**  display1 |    R/W     |   PTB_PTB5
**  display1 | Backlight  |   PTB_PTB7
**  --------------------------------------
**
**  IMPORTANTE: PTA0 como entrada SIEMPRE (usado por el MON08)
**/
  // Port A
  DDRA_DDRA4 = OUTPUT;
  DDRA_DDRA5 = OUTPUT;
  DDRA_DDRA6 = OUTPUT;
  DDRA_DDRA7 = OUTPUT;
  // Port B
  DDRB_DDRB5 = OUTPUT;
  DDRB_DDRB7 = OUTPUT;
  // Port C
  INTSCR2_IMASK2 = ON; // mascara para IRQ2 (pin PTC0)
  DDRC_DDRC0 = OUTPUT;
  DDRC_DDRC1 = OUTPUT;
  // Port D
  DDRD_DDRD0 = OUTPUT;
  DDRD_DDRD1 = OUTPUT;
  DDRD_DDRD2 = OUTPUT;
  DDRD_DDRD3 = OUTPUT;
}

void habilitaPulsadores(void)
{
  DDRD_DDRD4 = INPUT; // SW1
  DDRD_DDRD5 = INPUT; // SW2
  DDRD_DDRD6 = INPUT; // SW3
  DDRD_DDRD7 = INPUT; // SW4

  /* Habilito las resistencias de pullup para SW1..SW4 */
  KBIER_KBIE4 = ON;
  KBIER_KBIE5 = ON;
  KBIER_KBIE6 = ON;
  KBIER_KBIE7 = ON;

  /* ACKK=1; IMASKK=1; MODEK=0; */
  KBSCR = 0x06;
}

void habilitaGuionY2Puntos(void)
/** Habilita los Led de proposito general del Edukit y los inicializa apagados **/
{
  /* LD2 (activo alto) */
  DDRA_DDRA2 = OUTPUT; ApagaGuion;
  /* LD10 (activo bajo) */
  DDRB_DDRB0 = OUTPUT; Apaga2Puntos; 
}

void inicializaKBI(void)
/** Configura los registros para el modulo KBI **/
{
  /* Habilito IRQ y las resistencias de pullup para SW1..SW4 */
  KBIER = 0xF0;                
  // ACKK=1   (limpio cualquier IRQ fantasma) 
  // IMASKK=0 (habilito interrupcion por KBI)
  // MODEK=0  (sensible solo al flanco descendiente)
  KBSCR = 0x04;
}

void configuraADC(void)
/** Configura el modulo ADC para las practicas de la guia */
{
  /* deshabilito funcion AutoScan */
  ADASCR_ASCAN = 0; 
  
  /* configuracion de clock interno del ADC: */                     
  /* ADICLK=1: ADC Input Clock Select Bit (1 = internal bus clock) */
  /* ADIV2=0; ADIV1=1; ADIV0=0 : ADC input CLK / 4  (1,25 MHz) */
  /* MODE1=0, MODE0=1: Right justified mode c/ 10 bits resolucion */
  ADICLK = 0x54;  // ADIV1=1, ADICLK=1, MODE0=1
}

void inicializaTIM1(void)
/** para el control remoto IR **/
{
  T1SC = 0x20;        // Timer detenido y configurado a fBus (1/f = 200 ns)
  T1SC0 = 0x4C;       // Interrupcion habilitada, modo Input Capture en ambos flancos (CH0)
  T1MODH = 0xFF;      // Lmite de conteo maximo (default after reset)
  T1MODL = 0xFF;
  T1SC_TRST = 1;      // Prescaler and TIM counter cleared 
  T1SC_TSTOP = 0;     // Arranca el timer
}

void inicializaTIM2(void)
/** para la base de tiempo de 100 ms **/
/** fBus=5Mhz (T=200ns); PS = 32; MOD = 15625 **/
/** T x MOD x PS = 100 ms **/
{
  T2SC = 0x55;        // Timer configurado a fBus/32. Se habilita interrupcion por overflow
  T2MODH = 0x3D;      // Modulo del contador en 15625 = 0x3d09  
  T2MODL = 0x09;
}

/** ---------------- Seccion demoras ------------------ **/

void demora1us()
/** genera una demora aproximada de 1 useg por instrucciones en assembler **/
{
    unsigned char trivial;
    asm
    {
        lda #$FF       /** 2 ciclos = 0.4 us **/
        sta trivial    /** 3 ciclos = 0.6 us **/
    }
}

void demora100us()
/** genera una demora aproximada de 100 useg por ejecucion de instrucciones en assembler **/
{
    unsigned char contador;
    asm
    {
        lda #$63        /** 63h = 99 veces  (2 ciclos = 0.4 us) **/
        sta contador    /** contador <- 99 (3 ciclos = 0.6 us) **/

        bucle:
            /** decrementa en 1 a contador y compara, si != 0 salta a "bucle" **/
            dbnz contador, bucle    /** (5 ciclos = 1 us) **/
    }
}

void demoraEnus(unsigned int n)
/** genera una demora aproximada de n microsegundos **/
{
    while (n) {
        demora1us();
        n--;
    }
}

void demora1ms()
/** genera una demora aproximada de 1 milisegundo **/
{
    unsigned char i;

    for (i = 0; i < 10; i++) demora100us();
}

void demoraEnms(unsigned int n)
/** genera una demora aproximada de n milisegundos **/
{
    while (n) {
        demora1ms();
        n--;
    }
}

void demoraEns(unsigned int n)
/** genera una demora aproximada de n segundos **/
{
    unsigned int i;
    n *= 1000;   // convierto n a milisegundos

    for (i = 0; i < n; i++) demora1ms();
}

/** ----------- Seccion de funciones para display 7 segmentos --------- **/

unsigned char BCDa7seg(unsigned char numero)
/** devuelve el valor para representar el numero en un display 7-seg **/
{
  switch(numero)
  {
    case  0:
        return  0x3f;
        break;
    case  1:
        return  0x06;
        break;
    case  2:
        return  0x5b;
        break;
    case  3:
        return  0x4f;
        break;
    case  4:
        return  0x66;
        break;
    case  5:
        return  0x6d;
        break;
    case  6:
        return  0x7c;
        break;
    case  7:
        return  0x07;
        break;
    case  8:
        return  0x7f;
        break;
    case  9:
        return  0x67;
        break;
    default:
        return  0x40; // guion (0100 0000)
        break;

   }
}

unsigned char hexa7seg(unsigned char digito)
/** devuelve la combinacion en 7 segmentos para representar el digito hexadecimal **/
{
    switch(digito)
    {
        case  'a':
        case  'A':
            return  0x77;
            break;
        case  'b':
        case  'B':
            return  0x7c;
            break;
        case  'c':
        case  'C':
            return  0x39;
            break;
        case  'd':
        case  'D':
            return  0x5e;
            break;
        case  'e':
        case  'E':
            return  0x79;
            break;
        case  'f':
        case  'F':
            return  0x71;
            break;
        default:
            return  BCDa7seg(digito);
            break;
    }
}

void apagaSegmentos(void)
/** apaga todos los segmentos del display **/
{
  SEG_A = SEG_B = SEG_C = SEG_D = SEG_E = SEG_F = SEG_G = SEG_DP = OFF;
}

void apagaDisplays(void)
/** desactiva los catodos de los 4 displays del Edukit **/
{
  DISPLAY_1 = DISPLAY_2 = DISPLAY_3 = DISPLAY_4 = OFF;
}

void activaDisplay(unsigned char display)
/** habilita solamente el catodo del display indicado **/
/** display= (1, 2, 3, 4) **/
{
  apagaDisplays();

  switch(display)
  {
    case  1:   // [ ][ ][ ][X]
      DISPLAY_1 = ON;
      break;
    case  2:   // [ ][ ][X][ ]
      DISPLAY_2 = ON;
      break;
    case  3:   // [ ][X][ ][ ]
      DISPLAY_3 = ON;
      break;
    case  4:   // [X][ ][ ][ ]
      DISPLAY_4 = ON;
      break;
  }
}

void formateaNumero2Digitos(unsigned char numero, unsigned char displays)
/** separa el numero recibido en 2 digitos para mostrarlo en los displays **/
{
	// Displays 1 y 2: [ ][ ][X][X]
	if (displays == 12) {
		Digitos[1] = numero % 10; // unidades
		Digitos[1] = BCDa7seg(Digitos[1]);
		Digitos[2] = numero / 10; // decenas
		Digitos[2] = BCDa7seg(Digitos[2]);
	}
	// Displays 3 y 4: [X][X][ ][ ]
	else {
		Digitos[3] = numero % 10; // unidades
		Digitos[3] = BCDa7seg(Digitos[3]);
		Digitos[4] = numero / 10; // decenas
		Digitos[4] = BCDa7seg(Digitos[4]);
	}

}

void formateaNumero4Digitos(unsigned int numero)
/** separa el numero recibido en 4 digitos para mostrarlo en los displays **/
{
  unsigned int aux;
  unsigned char i;

  Digitos[4] = numero / 1000;   // unidades de mil
  aux = numero % 1000;

  Digitos[2] = aux % 100;
  Digitos[3] = aux / 100;  // centenas

  Digitos[1] = Digitos[2] % 10; // unidades
  Digitos[2] = Digitos[2] / 10; // decenas

  // guardo los numeros en formato 7 segmentos
  for (i = 1; i <= 4; i++) {
	Digitos[i] = BCDa7seg(Digitos[i]);
  } 
}

void muestraEnDisplay(unsigned char dato7Seg)
/** muestra el parametro recibido en algun display **/
{
    // escribo solamente en los pines conectados al display
    PTD_PTD0 = dato7Seg;
    PTD_PTD1 = dato7Seg>>=1;
    PTD_PTD2 = dato7Seg>>=1;
    PTD_PTD3 = dato7Seg>>=1; 
    PTA_PTA4 = dato7Seg>>=1;
    PTA_PTA5 = dato7Seg>>=1;
    PTA_PTA6 = dato7Seg>>=1;
    PTA_PTA7 = dato7Seg>>1;
}

void muestraNumero2Digitos(unsigned char numero, unsigned char posicionPuntoDecimal, unsigned char displays, unsigned char parpadear)
/** muestra el nro recibido en 2 displays del Edukit **/
/** posicionPuntoDecimal = (0 -apagado-, 1, 2, 3, 4) **/
/** displays = (DISPLAYS_1_2, DISPLAYS_3_4) **/
/** parpadear = (OFF, ON) **/
{
  apagaDisplays();

  if (!parpadear || TickParpadeo) {
    unsigned char i, inicio, fin;

    formateaNumero2Digitos(numero, displays);

	// Displays 1 y 2: [ ][ ][X][X]
    if (displays == 12) {
      inicio = 1;
      fin = 2;
    }
   	// Displays 3 y 4: [X][X][ ][ ]
	else {
      inicio = 3;
      fin = 4;
    }
    
    // barrida de displays
    for (i = inicio; i <= fin; i++) {
        muestraEnDisplay(Digitos[i]);
        activaDisplay(i);
        if (posicionPuntoDecimal == i) EnciendePuntoDecimal;
        demoraEnms(DEMORA_DISPLAY_MS);
    }
    apagaDisplays();
  }
}

void muestraNumero4Digitos(unsigned int numero, unsigned char posicionPuntoDecimal, unsigned char parpadear)
/** muestra el nro recibido en los 4 displays del Edukit **/
// posicionPuntoDecimal = (0 -apagado-, 1, 2, 3, 4)
// parpadear = (OFF, ON)

{
  apagaDisplays();

  if (!parpadear || TickParpadeo) {
    unsigned char i;

    formateaNumero4Digitos(numero);

    for (i = 1; i <= 4; i++) {
        muestraEnDisplay(Digitos[i]);
        activaDisplay(i);
        if (posicionPuntoDecimal == i) EnciendePuntoDecimal;
        demoraEnms(DEMORA_DISPLAY_MS);
    }
    apagaDisplays();
  }
}

/** ------------ Funciones modulo ADC ----------------- **/

unsigned int lecturaAD(void)
/** Devuelve el valor de la conversion AD con 10 bits de resolucion **/
{
    unsigned int valor = 0;

    // se debe leer primero la parte alta y luego la baja (ver guia v1.0)
    // leo la parte alta (2 bits de mayor peso) 
    valor = ADR0H;
    // hago lugar para los 8 bits restantes
    valor <<= 8;
    // leo la parte baja y guardo el resultado
    valor |= ADR0L;

    return valor;
}

unsigned int convierteEnmV(unsigned int valor)
/** Devuelve el valor en mV con 4 cifras significativas **/
/** valor: 0 (0mV) a 1023 (4,999mV) */
{
    unsigned long aux;
    
    // convierto a mV (FE=5V)
    aux = (unsigned long) valor*5000/1024;
    return (unsigned int) aux;
}

unsigned int convierteEnTemp(unsigned int valor, unsigned char escala)
/** Devuelve el valor en la escala de temperatura seleccionada **/
/** escala = (CELSIUS, KELVIN) **/
{
    // con precision en decimas de grado
    if (escala == CELSIUS)
      return (convierteEnmV(valor) - 2730);
    else
      return convierteEnmV(valor);
}

/** ----------------- Funciones propias del tanteador ------------------ **/

void limpiaFlagsGlobales(void)
/** Apaga los todos los flags de los botones **/
{
	P1mas = 	FALSE;	// Flag P1+
	P1menos = 	FALSE;	// Flag P1-
	P2mas = 	FALSE;	// Flag P2+
	P2menos = 	FALSE;	// Flag P2-
	Okey = 		FALSE;	// Flag boton OK del remoto
	ModoTest = FALSE;
	ModoConfig = FALSE;
	ReiniciarPartida = FALSE;
}

void modoTest(void)
/** enciende TODOS los Leds (Juarez) **/
{
    
    EnciendeGuion;		// activo alto
    Enciende2Puntos;	// activo bajo 

    muestraEnDisplay(0xFF);		// activo todos los segmentos incluido el dp 
    DISPLAY_1 = DISPLAY_2 = DISPLAY_3 = DISPLAY_4 = ON;	// enciendo los 4 diplays

    demoraEnms(DEMORA_DISPLAY_MS);

    apagaDisplays();
    ApagaGuion;		// activo alto
	  Apaga2Puntos;	// activo bajo 

}

void walk(unsigned char sentido)
/** hace un barrido rapido de los displays para mostrar/ocultar el numero **/
/** sentido = 0 (INPUT)  barre de izquierda a derecha mostrando el numero **/
/** sentido = 1 (OUTPUT) barre de derecha a izquierda ocultando el numero **/
/** IMPORTANTE: verificar el contenido de Digitos[] antes de llamar a esta funcion (!) **/
{
	unsigned char cont;

	if (sentido == INPUT)
	  cont = 1;
	else
	  cont = 42;	// equivale a 400 ms aprox

	while(cont) {
	
		muestraEnDisplay(Digitos[4]);
		activaDisplay(4);                   
		demoraEnms(DEMORA_DISPLAY_MS);
		DISPLAY_4 = OFF;  // apago para evitar la escritura anticipada

		// ~100 ms
		if (cont > 20) { 
			muestraEnDisplay(Digitos[3]);
			activaDisplay(3);                       
			demoraEnms(DEMORA_DISPLAY_MS);
			DISPLAY_3 = OFF;                
		}

		// ~200 ms
		if (cont > 30) {
			muestraEnDisplay(Digitos[2]);
			activaDisplay(2);
			demoraEnms(DEMORA_DISPLAY_MS);
			DISPLAY_2 = OFF;
		}

		// ~300 ms
		if (cont > 37) {
			muestraEnDisplay(Digitos[1]);
			activaDisplay(1);
			demoraEnms(DEMORA_DISPLAY_MS);
			DISPLAY_1 = OFF;
		}

		// ~400 ms
		if (cont > 42)
			break;

		(sentido == INPUT) ? cont++ : cont--;
	}
  
  // solamente en el walkout
  apagaDisplays();
}

void modoIntro(void)
/** Mensaje introductorio (Bejarano) **/
{
	unsigned int cont = 0;
	unsigned char H = 4, O = 3, L = 2, A = 1;        


	for(;;) {
		
		muestraEnDisplay(0x76); // H
		activaDisplay(H);                   
		demoraEnms(DEMORA_DISPLAY_MS);

		if (cont > 50) { 
			muestraEnDisplay(0x3F); // O
			activaDisplay(O);                       
			demoraEnms(DEMORA_DISPLAY_MS);                
		}

		if (cont > 75) {
			muestraEnDisplay(0x38); // L
			activaDisplay(L);
			demoraEnms(DEMORA_DISPLAY_MS);
		}

		if (cont > 100) {
			muestraEnDisplay(0x77); // A
			activaDisplay(A);
			demoraEnms(DEMORA_DISPLAY_MS);
		}

		// DESPLAZA A LA DERECHA
		if (cont == 200) {
			H = 3;
			O = 2;
			L = 1;
			A = 0;
		}

		if (cont == 225) {
			H = 2;
			O = 1;
			L = 0;
		}
    
		if (cont == 250) {
			H = 1;
			O = 0;
		}

		if(cont==275) {  
			H = 0;
			break;
		}
  
  		cont++;
	}
}

void muestraModoConfig(unsigned char puntos)
/** muestra el tope de la partida (Juarez) **/
{  
    muestraEnDisplay(0x39); // P
    activaDisplay(4);
    demoraEnms(DEMORA_DISPLAY_MS);
    muestraEnDisplay(0x73); // C
    activaDisplay(3);
    demoraEnms(DEMORA_DISPLAY_MS);
    muestraEnDisplay(BCDa7seg(puntos/10));
    activaDisplay(2);
    demoraEnms(DEMORA_DISPLAY_MS);
    muestraEnDisplay(BCDa7seg(puntos%10));
    activaDisplay(1);
    demoraEnms(DEMORA_DISPLAY_MS);
}  

unsigned char modoConfig(void)
/** Setea el tope de la partida (por Juarez) **/
{
    unsigned char numero = 0, pulso = 0;

    while(1){
    	
        //Modo Test
        if(ModoTest) {
        	ModoTest = FALSE;
        	while(Timer5Seg) modoTest();
        }
      
    	// P1+
        if(P1mas) {
        	P1mas = FALSE;
        	pulso++;
        }   
  
        //P1 limite
        if(pulso == 4) pulso = 0;
         
        //P1-  Retorno al modo play
        if(P1menos) {
            limpiaFlagsGlobales();
            pulso = 0;
            demoraEnms(150);
            return (numero);
        }

        if(pulso == 0) numero = 5;
        if(pulso == 1) numero = 7;
        if(pulso == 2) numero = 14;
        if(pulso == 3) numero = 21;

        muestraModoConfig(numero);
    }
}

void configuraFechaHora(void)
/** Setea los valores iniciales de Fecha/Hora **/
{
  TBCR_TBIE = OFF; 	// Deshabilito temporalmente las interrupciones por TBM
  ApagaGuion;
  Apaga2Puntos;

  // seteo el anio (parpadeando hasta que el usuario confirme)  
  while(1) {

    muestraNumero4Digitos((2000 + Anio), OFF, ON);

    // incremento anio
    if (P1mas) {
      P1mas = FALSE; // limpio el flag
      (Anio >= 99) ? Anio = 0 : Anio++;
    }

    // decremento anio
    if (P1menos) {
      P1menos = FALSE;  // limpio el flag
      (Anio == 0) ? Anio = 99 : Anio--;
    }
    
    // confirmacion del usuario    
    if (P2menos || Okey) {
    	P2menos = FALSE;	// limpio flags
    	Okey = FALSE;
    	break;
    }
  }

  if ((Anio % 4) == 0)
  	EsBisiesto = TRUE;
  else
  	EsBisiesto = FALSE;

  limpiaFlagsGlobales();
  EnciendeGuion; // para indicar que estoy configurando fecha 
  
  // seteo el mes (parpadeando hasta que el usuario confirme)
  while(1) {

    muestraNumero2Digitos(Mes, OFF, DISPLAYS_1_2, ON);

    // incremento mes
    if (P1mas) {
      P1mas = FALSE; // limpio el flag
      (Mes >= 12) ? Mes = 1 : Mes++;
    }

    // decremento mes
    if (P1menos) {
      P1menos = FALSE;  // limpio el flag
      (Mes == 1) ? Mes = 12 : Mes--;
    }
    
    // confirmacion del usuario    
    if (P2menos || Okey) {
    	P2menos = FALSE;	// limpio flags
    	Okey = FALSE;
    	break;
    }
  }
  
  limpiaFlagsGlobales();
  
  // seteo el dia (parpadeando hasta que el usuario confirme)  
  while(1) {

    muestraNumero2Digitos(Dia, OFF, DISPLAYS_3_4, ON);

    // incremento dia
    if (P1mas) {
      P1mas = FALSE; // limpio el flag
      
      // meses de 31 dias
      if (DiasMeses[Mes] == 31) 
      	(Dia >= 31) ? Dia = 1 : Dia++;
      // resto de los meses (y febrero)
      else {
      	if (Mes == 2) {
      		if (EsBisiesto)
      			(Dia >= 29) ? Dia = 1 : Dia++;
  			else
  				(Dia >= 28) ? Dia = 1 : Dia++;
      	}
      	else
      		(Dia >= 30) ? Dia = 1 : Dia++;
      }
    }

    // decremento dia
    if (P1menos) {
      P1menos = FALSE;  // limpio el flag
      
      // meses de 31 dias
      if (DiasMeses[Mes] == 31) 
      	(Dia == 1) ? Dia = 31 : Dia--;
      // resto de los meses (y febrero)
      else {
      	if (Mes == 2) {
      		if (EsBisiesto)
      			(Dia == 1) ? Dia = 29 : Dia--;
  			else
  				(Dia == 1) ? Dia = 28 : Dia--;
      	}
      	else
      		(Dia == 1) ? Dia = 30 : Dia--;
      }    
  	}
    
    // confirmacion del usuario   
    if (P2menos || Okey) {
    	P2menos = FALSE;	// limpio flags
    	Okey = FALSE;
    	break;	
    } 
  }

  ApagaGuion;
  Enciende2Puntos;	// para indicar que estoy seteando la hora
  limpiaFlagsGlobales();  
  
  // seteo Hora (parpadeando hasta que el usuario confirme)
  while(1) {

    muestraNumero2Digitos(Hora, OFF, DISPLAYS_3_4, ON);

    // incremento Hora
    if (P1mas) {
      P1mas = FALSE; // limpio el flag
      (Hora >= 23) ? Hora = 0 : Hora++;
    }

    // decremento Hora
    if (P1menos) {
      P1menos = FALSE;  // limpio el flag
      (Hora == 0) ? Hora = 23 : Hora--;
    }
    
    // confirmacion del usuario    
    if (P2menos || Okey) {
    	P2menos = FALSE;
    	Okey = FALSE;
    	break;
    }
  }

  limpiaFlagsGlobales();
  
  // seteo minutos (parpadeando hasta que el usuario confirme)
  while(1) {
  	muestraNumero2Digitos(Minutos, OFF, DISPLAYS_1_2, ON);

  	// Incremento minutos
	if (P1mas) {
	  P1mas = FALSE;	// limpio el flag
      (Minutos >= 59) ? Minutos = 0: Minutos++;
	}
                        
    // Decremento Minutos
	if (P1menos) {
      P1menos = FALSE;	// limpio el flag
      (Minutos == 0) ? Minutos = 59: Minutos--;
    }

    // confirmacion del usuario
    if (P2menos || Okey) {
    	P2menos = FALSE;
    	Okey = FALSE;
    	break;
    }
  }

  limpiaFlagsGlobales();
  Apaga2Puntos;
  Segundos = 0;		    // empieza a contar desde ahora
  TBCR_TBIE = ON;	    // habilito nuevamente las interrupciones por TBM
}

/** ----------- Seccion de funciones de atencion de interrupcion (ISR) --------- **/

void interrupt irqTIM2OF(void)
/** interrupcion periodica cada 100ms **/
{
   // para el antirrebote SW1..SW4
   for (i = 0; i < 4; i++) {                   // Recorro los 4 pulsadores
      if (Antirrebote[i]) {
          Antirrebote[i]--;
          if (Antirrebote[i] == 0)             // timeout antirrebote
            KBIER |= KBI_MASK[i];              // Habilito IRQ para ese pulsador
      }
    } 

  // para el timer de 5 segundos
  if (Timer5Seg) {
    if (ContadorTimer5Seg == 50) {
      ContadorTimer5Seg = 0;
      Timer5Seg = OFF;
    }
    else
      ContadorTimer5Seg++;
  }

  // para el timer de 1 minuto (idle timeout)
  if (Timer1Min) {
  	if (ContadorTimer1Min = 600) {
  		ContadorTimer1Min = 0;
  		Timer1Min = OFF;
  	}
  	else
  		ContadorTimer1Min++;
  }

  // para el parpadeo de los displays c/ 500 mseg
  if (ContadorParpadeo == 5) {
    ContadorParpadeo = 0;
    TickParpadeo = ~TickParpadeo;
  }
  else 
    ContadorParpadeo++;

  T2SC_TOF = 0;   // interrupcion atendida
}

void interrupt irqPulsadores(void)
/** Contiene la logica de los pulsadores (ver Descripcion Funcional para detalles) **/
{   
   // Guardo el estado actual de los pulsadores
   SW[0] = SW1;  // P2- (PTD4)
   SW[1] = SW2;  // P2+ (PTD5)
   SW[2] = SW3;  // P1- (PTD6)
   SW[3] = SW4;  // P1+ (PTD7)

   for (j = 0; j < 4; j++) {            // chequeo cada pulsador
       if (SW[j] == 0) {                // pulsador activo
          KBIER &= ~KBI_MASK[j];        // deshabilito IRQ para ese pulsador
          Antirrebote[j] = ANTIRREBOTE; // y le asigno un antirrebote
          
          // Modo Test (P2+ y P2-)
          if (!KBIER_KBIE5 & !KBIER_KBIE4) {
            ModoTest = TRUE;   // seteo la variable global
            Timer5Seg = ON; // "arranco" el timer de 5 segundos
          }   

          // Reiniciar partida (P1+ y P2+)
          if (!KBIER_KBIE7 & !KBIER_KBIE5) 
            ReiniciarPartida = TRUE;

          // Modo Config (P1- y P2-)
          if (!KBIER_KBIE6 & !KBIER_KBIE4)
            ModoConfig = TRUE;
          
          // P1+
          if (!KBIER_KBIE7)
            P1mas = TRUE; 

          // P1-
          if (!KBIER_KBIE6)
            P1menos = TRUE; 

          // P2+
          if (!KBIER_KBIE5)
            P2mas = TRUE; 

          // P2-
          if (!KBIER_KBIE4)
            P2menos = TRUE; 
       }
    }

  KBSCR_ACK = 1;  // interrupcion atendida
}

void interrupt irqTBM(void)
/**  Interrupcion periodica cada 1 seg **/
{
  if (Segundos == 59) {
    Segundos = 0;
  
    if (Minutos == 59) {
      Minutos = 0;
    
      if (Hora == 23) {
        if (Dia >= 28) {
        	switch (Dia)
        	{
			    case  28:   
			      if (Mes == 2 && !EsBisiesto) {
		    		Dia = 1;
		    		Mes++;
		    	  }
			      break;
			    case  29:
			      if (Mes == 2 && EsBisiesto) {
		    		Dia = 1;
		    		Mes++;
		    	  }
			      break;
			    case  30:
			      if (DiasMeses[Mes] == 30) {
			        Dia = 1;
	           		Mes++;	
			      }
			      break;
			    case  31:
			      if (Mes == 12) {
	        		Anio++;
					  if ((Anio % 4) == 0)
						  EsBisiesto = TRUE;
					  else
						  EsBisiesto = FALSE;

					  Mes = 1;
	        	}
	        	else
	        		Mes++;

	        	Dia = 1;
			      break;
			}	// fin switch
		  } else
			  Dia++;

		  Hora = 0;
      } else
         Hora++;
    } else
      	Minutos++;
  } else
      Segundos++;
    
  DOS_PUNTOS = ~DOS_PUNTOS;		// Parpadeo

  TBCR_TACK = 1;    // Interrupcion atendida
}

void interrupt irqTIM1OF(void)
/**  Reestablece el timer por overflow **/
{
  T1SC_TOF = 0;   // Limpio Flag TIM Overflow 
  T1SC_TRST = 1;  // Limpio Prescaler y contador del Timer1
}

void interrupt irqTIM1CH0(void)
/** Manejo del control remoto IR (por Ramos) **/
{
  if (SENSOR_IR == 0)          // Flanco de bajada (recordar que el receptor invierte la señal!)
      T1SC_TRST = 1;          // limpio el contador para medir el ancho del pulso
  else
  {                                     // Flanco de subida
      AnchoDePulso = T1CH0;             // Guarda el valor medido.
      if (AnchoDePulso > UNO_LOGICO)     // Pulso de inicio (nueva trama) 
      {       
          BitTrama = 0;                 // limpio las variables para recibir los datos
          Comando = 0;
      }
      if (AnchoDePulso > CERO_LOGICO)    // Recibi un "1".
      {      
          if (BitTrama < 8) {
            bitset(Comando, BitTrama);  // seteo el bit correspondiente
            BitTrama++;
          }
      }
      else
      {                 // Recibi un "0".
          if (BitTrama < 8) {
            bitclr(Comando, BitTrama);  // limpio el bit correspondiente
            BitTrama++;
          }
      }
  }

  if (BitTrama >= 7) ComandoIR = Comando;  // si recibi al menos 7 bits guardo el comando

  // la logica del manejo de botones (Facundo)
  // setear flag del boton presionado
  // setear flag timer1seg
  // enmascarar T1SC0_CH0IE = 0;
  


  T1SC0_CH0F=0;   // interrupcion atendida
}