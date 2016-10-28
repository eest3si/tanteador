/** ###################################################################
**     LIBRERIA DE FUNCIONES PARA LA PLACA PLUGIN AP_32 DEL EDUKIT
**
**     Archivo    : edukit.h
**     Micro      : MC68HC908AP32CFB (44 pines)
**     Version    : 1.3
**     Fecha      : 20161023
**     Autor      : Ing. Juan Carlos Torres
**
** ###################################################################*/

#pragma MESSAGE DISABLE C5703 /* WARNING C5703: Parameter declared but not referenced */

/** ---------------- Seccion defines ------------------ **/

/* Macros para los leds de proposito general */
//TODO: definir GUION y DOS_PUNTOS
#define LED1            PTA_PTA1  // activo bajo
#define LED2            PTA_PTA2  // activo alto
#define LED10           PTB_PTB0  // activo bajo
#define EnciendeLED1    LED1 = 0; 
#define ApagaLED1       LED1 = 1;
#define EnciendeLED2    LED2 = 1;
#define ApagaLED2       LED2 = 0;
#define EnciendeLED10   LED10 = 0;
#define ApagaLED10      LED10 = 1;

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
#define ANTIRREBOTE 10

/* Macros modulo SPI */
#define SPI_SS PTC_PTC4

/* Macros modulo ADC */
#define ArrancarConversionAD  ADSCR = 0x03; // conversion unica, ADCH3/PTA3
#define ConversionCompleta    ADSCR_COCO
#define CELSIUS 1
#define KELVIN  0

/* Macros sensores */
#define SENSOR_IR   PTB_PTB4
#define SENSOR_TEMP PTA_PTA3

/* Macros booleanos */
#define TRUE   1
#define FALSE  0
#define ON     1
#define OFF    0
#define INPUT  0
#define OUTPUT 1

/** ---------------------- Variables globales --------------------------- **/
const char KBI_MASK[4] = { 16, 32, 64, 128 };
unsigned char Antirrebote[4];
unsigned char SW[4];
unsigned char P1mas = 0;
unsigned char P1menos = 0;
unsigned char P2mas = 0;
unsigned char P2menos = 0;
unsigned char ModoTest = 0;
unsigned char ModoConfig = 0;
unsigned char ReiniciarPartida = 0;
unsigned char contador = 0;

/** ---------------- Seccion inicializacion de modulos ------------------ **/

void configuraCPU(void)
/** Inicializacion de registros "write once" para el HC908 **/
{
  /* CONFIG1: COPRS=0,LVISTOP=0,LVIRSTD=1,LVIPWRD=1,LVIREGD=1,SSREC=0,STOP=1,COPD=1 */
  CONFIG1= 0x3B;   // deshabilita COP
  /* CONFIG2: STOP_ICLKDIS=1,STOP_RCLKEN=0,STOP_XCLKEN=0,OSCCLK1=1,OSCCLK0=0,??=0,??=0,SCIBDSRC=1 */
  CONFIG2= 0x91;   // fuente de reloj: XTAL
}

void inicializaPLL(void)
/************************************************************************
* seteo PLL para generar 4,9152 MHZ de FBUS desde cristal de 32K768     *
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
  PBWC_AUTO = ON;     // Enganche automático
  while(!PBWC_LOCK);  // esperar a LOCK = 1
  PCTL_BCS = 1;       // cambio a frecuencia del PLL
}

void habilitaLCDyDisplays(void)
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

void habilitaLeds(void)
/** Habilita los Led de proposito general del Edukit y los inicializa apagados **/
{
  /* LD1 (activo bajo) */
  DDRA_DDRA1 = OUTPUT; LED1 = 1;
  /* LD2 (activo alto) */
  DDRA_DDRA2 = OUTPUT; LED2 = OFF;
  /* LD10 (activo bajo) */
  DDRB_DDRB0 = OUTPUT; LED10 = 1; 
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

void inicializaSPI(void)
/** Inicializa el modulo SPI **/
{
  char dummy;  // para lectura trivial

  /* SPCR: SPRIE=0,??=0,SPMSTR=0,CPOL=0,CPHA=0,SPWOM=0,SPE=0,SPTIE=0 */
  SPCR = 0x00;      /* Disable the SPI module */
  dummy = SPSCR;    /* Dummy read of the SPSCR registr to clear flags */
  dummy = SPDR;     /* Dummy read of the SPDR registr to clear flags */

  /* SPSCR: SPRF=0,ERRIE=0,OVRF=0,MODF=0,SPTE=0,MODFEN=0,SPR1=1,SPR0=1 */
  SPSCR = 0x03;

  /* SPCR: SPRIE=0,??=0,SPMSTR=1,CPOL=0,CPHA=1,SPWOM=0,SPE=0,SPTIE=0 */
  SPCR = 0x28;

  //SPCR_SPE = 1;
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

void inicializaTIM2(void)
/** para la base de tiempo de 20ms **/
// fBus=5Mhz (T=200ns); PS = 8; MOD = 12500 
// T * MD * PS = 20ms
{
  T2SC=0x53;    // PS2=0, PS1=1, PS0=1 (fBus/8); se habilita interrupción por overflow
  T2MODH=0x30;  // Módulo del contador en 12500  
  T2MODL=0xD4;
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
    while(n){
        demora1us();
        n--;
    }
}

void demora1ms()
/** genera una demora aproximada de 1 milisegundo **/
{
    unsigned char i;

    for(i=1; i<=10; i++) demora100us();
}

void demoraEnms(unsigned int n)
/** genera una demora aproximada de n milisegundos **/
{
    while(n){
        demora1ms();
        n--;
    }
}

void demoraEns(unsigned int n)
/** genera una demora aproximada de n segundos **/
{
    unsigned int i;
    n*=1000;   // convierto n a milisegundos

    for(i=1; i<=n; i++) demora1ms();
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
/** desactiva los cátodos de los 4 displays del Edukit **/
{
  DISPLAY_1 = OFF;
  DISPLAY_2 = OFF;
  DISPLAY_3 = OFF;
  DISPLAY_4 = OFF;
}

void activaDisplay(unsigned char display)
/** habilita el catodo del display indicado (display: 1..4) **/
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

void formateaNumero2Digitos(unsigned char numero, unsigned char digitos[])
/** separa el numero recibido en 4 digitos para mostrarlo en los displays del Edukit **/
{
  digitos[0] = numero % 10; // unidades
  digitos[1] = numero / 10; // decenas
}

void formateaNumero4Digitos(unsigned int numero, unsigned char digitos[])
/** separa el numero recibido en 4 digitos para mostrarlo en los displays del Edukit **/
{
  unsigned int temporal;

  digitos[3] = numero / 1000;   // unidades de mil
  temporal = numero % 1000;

  digitos[1] = temporal % 100;
  digitos[2] = temporal / 100;  // centenas

  digitos[0] = digitos[1] % 10; // unidades
  digitos[1] = digitos[1] / 10; // decenas
}

void muestraCaracterEnDisplay(unsigned char valor)
/** usada para mostrar caracteres personalizados **/
{    
    // escribo sólo en los pines conectados al display
    PTD_PTD0 = valor;
    PTD_PTD1 = valor>>=1;
    PTD_PTD2 = valor>>=1;
    PTD_PTD3 = valor>>=1; 
    PTA_PTA4 = valor>>=1;
    PTA_PTA5 = valor>>=1;
    PTA_PTA6 = valor>>=1;
    PTA_PTA7 = valor>>1;
}

void muestraNumeroEnDisplay(unsigned char numero, unsigned char puntoDecimal)
/** muestra el número recibido en algun display del Edukit **/
/** puntoDecimal = (OFF, ON)  **/
{
    unsigned char dato7Seg = BCDa7seg(numero); 
    
    // escribo sólo en los pines conectados al display
    PTD_PTD0 = dato7Seg;
    PTD_PTD1 = dato7Seg>>=1;
    PTD_PTD2 = dato7Seg>>=1;
    PTD_PTD3 = dato7Seg>>=1; 
    PTA_PTA4 = dato7Seg>>=1;
    PTA_PTA5 = dato7Seg>>=1;
    PTA_PTA6 = dato7Seg>>=1;
    PTA_PTA7 = dato7Seg>>1;

    if (puntoDecimal) EnciendePuntoDecimal;
}

void muestraNumero2Digitos(unsigned char numero, unsigned char posicionPuntoDecimal, unsigned char displays, unsigned char parpadear)
/** muestra el nro recibido en 2 displays del Edukit **/
// posicionPuntoDecimal = (0 -apagado-, 1, 2)
// displays = (DISPLAYS_1_2, DISPLAYS_3_4)
// parpadear = (OFF, ON)
{
  if(!parpadear){
    unsigned char i, inicio, fin, digitos[2];

    formateaNumero2Digitos(numero, digitos);
    apagaDisplays();

    if(displays == 12){
      inicio = 1;
      fin = 2;
    }

    if(displays == 34){
      inicio = 3;
      fin = 4;
    }
    
    for(i = inicio; i <= fin; i++){
        muestraNumeroEnDisplay(digitos[i-1], OFF);
        activaDisplay(i);
        if(posicionPuntoDecimal == i) EnciendePuntoDecimal;
        demoraEnms(DEMORA_DISPLAY_MS);
    }
  }
}

void muestraNumero4Digitos(unsigned int numero, unsigned char posicionPuntoDecimal, unsigned char parpadear)
/** muestra el nro recibido en los 4 displays del Edukit **/
// posicionPuntoDecimal = (0 -apagado-, 1, 2, 3, 4)
// parpadear = (OFF, ON)
{
  if(!parpadear){
    unsigned char i, digitos[4];

    formateaNumero4Digitos(numero, digitos);
    apagaDisplays();

    for(i = 1; i <= 4; i++){
        muestraNumeroEnDisplay(digitos[i-1], OFF);
        activaDisplay(i);
        if(posicionPuntoDecimal == i) EnciendePuntoDecimal;
        demoraEnms(DEMORA_DISPLAY_MS);
    }
  }
}

/** ----------- Seccion de funciones para display LCD 16x2 --------- **/

void datoLCD(unsigned char dato)
/** descompone el byte (dato) y lo pone en el bus de datos del LCD **/
{
    // escribo sólo en los pines conectados al LCD
    PTD_PTD0 = dato;
    PTD_PTD1 = dato>>=1;
    PTD_PTD2 = dato>>=1;
    PTD_PTD3 = dato>>=1; 
    PTA_PTA4 = dato>>=1;
    PTA_PTA5 = dato>>=1;
    PTA_PTA6 = dato>>=1;
    PTA_PTA7 = dato>>1;
}

void comandoLCD(unsigned char comando)
/** descompone el byte (comando) y lo pone en el bus de datos del LCD **/
{
    // vuelco el comando en el bus de datos
    datoLCD(comando);

    // operacion de escritura del comando
    LCD_RW = 0; // Write = 0
    LCD_RS = 0; // Register Select = 0 (comando)

    // provocamos un pulso en la linea E para que el comando sea leido por el LCD
    LCD_E = 1;
    demoraEnms(2);
    LCD_E = 0;
    demoraEnms(2);
}

void inicializaLCD(void)
/** Comandos de inicializacion del LCD 16x2 del Edukit **/
{
    demoraEnms(20);   // demora para el 'booteo' del LCD

    // comandos de configuracion
    comandoLCD(0x02);   // CURSOR HOME          (setea el cursor al comienzo de linea 1)
    comandoLCD(0x38);   // FUNCTION SET         (modo 8 bits, 2 lineas, formato 5x7 dots)    
    comandoLCD(0x0c);   // DISPLAY & CURSOR     (display ON, cursor underline OFF, cursor blink OFF)
    comandoLCD(0x06);   // CHARACTER ENTRY MODE (increment, display shift OFF)
    comandoLCD(0x01);   // CLEAR DISPLAY
}

void muestraCaracterEnLCD(unsigned char caracter)
/** envia el caracter para mostrarlo en el LCD **/
{
    // pongo el caracter en el bus de datos del LCD
    datoLCD(caracter);
    
    // operacion de escritura del caracter
    LCD_RW = 0;   // write = 0
    LCD_RS = 1;   // Register Select = 1 (dato)

    // provocamos un flanco en E para que el comando sea leido por el LCD
    LCD_E = 1;
    demoraEnms(2);
    LCD_E = 0;
    demoraEnms(2);
}

void posicionaCursorLCD(unsigned char fila, unsigned char columna)
/** posiciona el cursor en la coordenada indicada **/
{
    unsigned char comando;

    switch (fila)
    {
        case 0:
            comando = 0x80; // SET DISPLAY ADDRESS
            break;
        case 1:
            comando = 0xc0; // SET DISPLAY ADDRESS 
            break;
    }
    // desplazo a la columna indicada
    comando += columna;
    // envio el comando al LCD
    comandoLCD(comando);
}

void muestraTextoEnLCD(unsigned char fila, unsigned char columna, char *texto)
/** muestra el texto en la posicion especificada **/
/** fila: 0, 1; columna: 0..15 **/
{
    unsigned char i=0;
    posicionaCursorLCD(fila, columna);

    // recorro la cadena de texto para mostrarla
    while(texto[i] != 0){
        muestraCaracterEnLCD(texto[i]);
        i++;
    }
}

unsigned char enviaSPI(unsigned char unByte)
/** El byte recibido se envia por SPI, producto de la trasaccion se recibe otro byte que la funcion retorna **/
{
    SPI_SS = 0;
    SPDR = unByte;
    // espero al buffer de recepcion
    while(!SPSCR_SPRF);

    return SPDR;
}

unsigned int lecturaAD(void)
/** Devuelve el valor de la conversion a 10 bits **/
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
// escala = (CELSIUS, KELVIN)
{
    // con precision en decimas de grado
    if(escala==CELSIUS)
      return (convierteEnmV(valor) - 2730);
    else
      return convierteEnmV(valor);
}

void modoTest(void)
/** enciende TODOS los segmentos en los 4 Displays **/
{
    unsigned char i, digitos[4];
    digitos[0]=8;
    digitos[1]=8;
    digitos[2]=8;
    digitos[3]=8;

    // barro los displays para mostrar el mumero
    for(i = 0; i < 4; i++)
    {
      apagaDisplays();
      muestraNumeroEnDisplay(digitos[i], OFF);
      EnciendePuntoDecimal;
      activaDisplay(i+1);
      // dejo encendido el display un instante para persistencia visual
      demoraEnms(DEMORA_DISPLAY_MS);
    }
    apagaDisplays();
}

/** ----------- Seccion de funciones de atencion de interrupcion (ISR) --------- **/
void interrupt irqTIM2OF(void)
/** interrupcion periodica cada 20ms para antirrebote **/
{
  char i;
 
   for (i = 0; i < 4; i++) {                   // Test all keys
      if (Antirrebote[i]) {
          Antirrebote[i]--;
          if (Antirrebote[i] == 0)             // Debounce timeout occurred
            KBIER |= KBI_MASK[i];              // Habilito IRQ para ese pulsador
      }
    } 

  if(contador==50){
    LED10 = ~LED10;
    contador = 0;
  }
  else 
    contador++;

  T2SC_TOF=0;
}

void interrupt irqPulsadores(void)
/** Contiene la logica de los pulsadores (ver Descripcion Funcional para detalles) **/
{
   char i;
   
   // Guardo el estado actual de los pulsadores
   SW[0] = SW1;  // P2- (PTD4)
   SW[1] = SW2;  // P2+ (PTD5)
   SW[2] = SW3;  // P1- (PTD6)
   SW[3] = SW4;  // P1+ (PTD7)

   for (i = 0; i < 4; i++) {            // chequeo cada pulsador
       if (SW[i] == 0) {                // pulsador activo
          KBIER &= ~KBI_MASK[i];        // deshabilito IRQ para ese pulsador
          Antirrebote[i] = ANTIRREBOTE; // y le asigno un antirrebote
          
          // Modo Test (P2+ y P2-)
          if (!KBIER_KBIE5 & !KBIER_KBIE4)
            ModoTest = 1;

          // Reiniciar partida (P1+ y P2+)
          if (!KBIER_KBIE7 & !KBIER_KBIE5) 
            ReiniciarPartida = 1;

          // Modo Config (P1- y P2-)
          if (!KBIER_KBIE6 & !KBIER_KBIE4)
            ModoConfig = 1;
          
          // P1+
          if (!KBIER_KBIE7)
            P1mas = 1; 

          // P1-
          if (!KBIER_KBIE6)
            P1menos = 1; 

          // P2+
          if (!KBIER_KBIE5)
            P2mas = 1; 

          // P2-
          if (!KBIER_KBIE4)
            P2menos = 1; 
       }
    }

  KBSCR_ACK = 1;
}

void interrupt irqTIM1OF(void)
/**  Reestablece el timer por overflow **/
{
  T1SC_TOF=0;   // Limpio el TIM Overflow Flag Bit 
  T1SC_TRST=1;  // Limpio el Prescaler y el contador del TIM
}

void interrupt irqTIM1CH0(void)
/** ISR del TIM1 por el PTB4 en flancos de subida **/
{
  /*
  if(SENSOR_IR==0){        // Caso Flanco de bajada (el receptor invierte la señal)
    T1SC_TRST=1;          // Limpio el Prescaler y el contador del TIM
  } else{                 // Caso flanco de subida
    Tiempo=T1CH0;         // Guarda el valor del evento.
    if(Tiempo>UNO_LOGICO) // Lo considero como pulso de inicio.
    {       
      i=0;                // Prepara i para recibir los 7 bits.
      Dato1=0;
      Dato2=0;
    }
    if(Tiempo>CERO_LOGICO) // Lo considero como "1".
    {      
      if(i<8){
        bitset(Dato1,i);
        i++;
      }
      if(7<i<11){
        bitset(Dato2,i);
      }
    } 
    else{               // Lo considero como "0".
      if(i<8){
        bitclr(Dato1,i);
        i++;
      }
      if(7<i<11){
        bitclr(Dato2,i);
      }
    }
  }
  if(i>=7){
    Dato=Dato1;
  }
  */ 
  T1SC0_CH0F=0;
}