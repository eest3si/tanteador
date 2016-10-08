/** ########################################################################
**     LIBRERIA DE FUNCIONES PARA EL PROYECTO 2016 TANTEADOR DE PING-PONG
**
**     Archivo    : tanteador.h
**     Procesador : MC68HC908AP32CFB (placa plugin AP)
**     Version    : 1.0
**     Fecha      : 20160911
**     Autor      : Juan Carlos Torres
**
** #########################################################################*/

#pragma MESSAGE DISABLE C5703 /* WARNING C5703: Parameter declared but not referenced */

/** ---------------- Seccion defines ------------------ **/

/* Alias para los leds */
#define LED1 PTA_PTA1
#define LED2 PTA_PTA2

/* Alias para los displays */
#define SEG_A  PTD_PTD0
#define SEG_B  PTD_PTD1
#define SEG_C  PTD_PTD2
#define SEG_D  PTD_PTD3
#define SEG_E  PTA_PTA4
#define SEG_F  PTA_PTA5
#define SEG_G  PTA_PTA6
#define SEG_DP PTA_PTA7
#define DISPLAY_1 PTC_PTC0
#define DISPLAY_2 PTC_PTC1
#define DISPLAY_3 PTB_PTB5
#define DISPLAY_4 PTB_PTB7
#define BARRER_POR_DISPLAY 0
#define BARRER_POR_SEGMENTO 1
#define PUNTO_EN_DISPLAY_1 1
#define PUNTO_EN_DISPLAY_2 2
#define PUNTO_EN_DISPLAY_3 3
#define PUNTO_EN_DISPLAY_4 4
#define DEMORA_SEGMENTOS_US 50     // demora en us para la barrida por segmentos
#define DEMORA_DISPLAY_MS 5        // demora en ms para la barrida por display entero

/* Alias para el LCD */
#define BACKLIGHT_LCD PTB_PTB7

/* Alias para los pulsadores */
#define SW1 PTD_PTD4
#define SW2 PTD_PTD5
#define SW3 PTD_PTD6
#define SW4 PTD_PTD7

/* Alias modulo SPI */
#define SPI_SS PTC_PTC4

/* Alias booleanos */
#define TRUE 1
#define FALSE 0
#define ON 1
#define OFF 0

/** ---------------- Seccion inicializacion ------------------ **/

void inicializaCPU(void)
/** Inicializacion de registros "write once" para el MC68HC908AP64_44 **/
{
  /* CONFIG1: COPRS=0,LVISTOP=0,LVIRSTD=1,LVIPWRD=1,LVIREGD=1,SSREC=0,STOP=1,COPD=1 */
  CONFIG1= 0x3B;   // deshabilita COP
  /* CONFIG2: STOP_ICLKDIS=1,STOP_RCLKEN=0,STOP_XCLKEN=0,OSCCLK1=1,OSCCLK0=0,??=0,??=0,SCIBDSRC=1 */
  CONFIG2= 0x91;   // fuente de clock XTAL

/*** Fin de inicializacion "after reset" ***/
}

void inicializaDisplays(void)
/** Configura los puertos asociados a los displays del Edukit **/
{
/** --------------------------------------------------------------
**  Segmento  |  Puerto_pin     ||      Display     |   Puerto_pin
**  --------------------------------------------------------------
**     a      |  PTD_PTD0       ||         1        |   PTC_PTC0
**     b      |  PTD_PTD1       ||         2        |   PTC_PTC1
**     c      |  PTD_PTD2       ||         3        |   PTB_PTB5
**     d      |  PTD_PTD3       ||         4        |   PTB_PTB7
**     e      |  PTA_PTA4       ||
**     f      |  PTA_PTA5       ||
**     g      |  PTA_PTA6       ||
**     d.p.   |  PTA_PTA7       ||
**  ---------------------------------------------------------------
**  PTA.0 como entrada SIEMPRE (usado por el MON08)
**/

  /* DDRA: DDRA7=1,DDRA6=1,DDRA5=1,DDRA4=1,DDRA0=0 */
  DDRA = 0xF0;
  /* DDRD: DDRD3=1,DDRD2=1,DDRD1=1,DDRD0=1 */
  DDRD = 0x0F;
  /* DDRB: DDRB7=1,DDRB5=1 */
  DDRB = 0xA0;
  /* INTSCR2: IMASK2=1 (mascara para IRQ2 pin PTC0) */
  INTSCR2 = 0x02;
  /* DDRC: DDRC1=1,DDRC0=1 */
  DDRC = 0x03;
}

void inicializaPulsadores(void)
{
  /* PTD4=SW1, PTD5=SW2, PTD6=SW3, PTD7=SW4 */
  DDRD_DDRD4 = 0;
  DDRD_DDRD5 = 0;
  DDRD_DDRD6 = 0;
  DDRD_DDRD7 = 0;

  /* Habilito las resistencias de pullup */
  KBIER = 0xF0;

  /* ACKK=1; IMASKK=1; MODEK=0; */
  KBSCR = 0x06;
}

void inicializaLeds(void)
/** Habilita los Led de proposito general del Edukit y los inicializa apagados **/
{
  /* LD1 (activo bajo) */
  DDRA_DDRA1 = 1; LED1 = 1;
  /* LD2 (activo alto) */
  DDRA_DDRA2 = 1; LED2 = 0;
}

/** ---------------- Seccion demoras ------------------ **/

void demora1us()
/** genera una demora aproximada de 1 useg por instrucciones en aseembler **/
{
    unsigned char trivial;
    asm
    {
        lda #$FF       /** 2 ciclos = 0.4 us **/
        sta trivial    /** 3 ciclos = 0.6 us **/
    }
}

void demora100us()
/** genera una demora aproximada de 100 useg. por ejecución de instrucciones en assembler **/
{
    unsigned char contador;
    asm
    {
        lda #$63        /** 63h = 99 veces  (2 ciclos = 0.4 us) **/
        sta contador    /** contador <- 99 (3 ciclos = 0.6 us) **/

        bucle:
            /** decrementa en 1 a contador y compara, si es != 0 salta a "bucle" **/
            dbnz contador, bucle    /** (5 ciclos = 1 us) **/
    }
}

void demoraEnus(unsigned int n)
/** genera una demora aproximada de n useg. **/
{
    while(n){
        demora1us();
        n--;
    }
}

void demora1ms()
/** genera una demora aproximada de 1 mseg. **/
{
    unsigned char i;

    for(i=1; i<=10; i++) demora100us();
}

void demoraEnms(unsigned int n)
/** genera una demora aproximada de n mseg. **/
{
    while(n){
        demora1ms();
        n--;
    }
}

void demoraEns(unsigned int n)
/** genera una demora aproximada de n seg. **/
{
    unsigned char i;
    n*=1000;   // convierto n a milisegundos

    for(i=1; i<=n; i++) demora1ms();
}

/** ----------- Seccion de funciones para display 7 segmentos --------- **/

unsigned char BCDa7seg(unsigned char numero)
/** devuelve la combinación para representar el numero en un display 7-seg **/
{
  switch(numero)
  {
    case  0:
        return  0x3f;
        break;
    case  1:
        return  0x6;
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
        return  0x7;
        break;
    case  8:
        return  0x7f;
        break;
    case  9:
        return  0x67;
        break;
   }
}

unsigned char hexa7seg(unsigned char digito)
/** devuelve la combinacion para representar el digito hexadecimal en un display 7-seg **/
{
    switch(digito)
    {
        case  'a':
        case  'A':
            return  0x00;
            break;
        case  'b':
        case  'B':
            return  0x00;
            break;
        case  'c':
        case  'C':
            return  0x00;
            break;
        case  'd':
        case  'D':
            return  0x00;
            break;
        case  'e':
        case  'E':
            return  0x00;
            break;
        case  'f':
        case  'F':
            return  0x00;
            break;
        default:
            return  BCDa7seg(digito);
            break;
    }
}

void enciendePuntoDecimal(void)
/** enciende el segmento "dp" en el display **/
{
    SEG_DP = 1;
}

void apagaSegmentos(void)
/** apaga todos los segmentos del display **/
{
  SEG_A = SEG_B = SEG_C = SEG_D = SEG_E = SEG_F = SEG_G = SEG_DP = 0;
}

void apagaDisplays(void)
/** desactiva los cátodos de los 4 displays del Edukit **/
{
  PTC_PTC0 = 0;  // display 1   (el de la derecha)
  PTC_PTC1 = 0;  // display 2
  PTB_PTB5 = 0;  // display 3
  PTB_PTB7 = 0;  // display 4
}

void activaDisplay(unsigned char display)
/** habilita el catodo del display indicado (display: 1..4) **/
{
  apagaDisplays();

  switch(display)
  {
    case  1:   // display 1 (---X)
      DISPLAY_1 = 1;
    break;
    case  2:   // display 2 (--X-)
      DISPLAY_2 = 1;
    break;
    case  3:   // display 3 (-X--)
      DISPLAY_3 = 1;
    break;
    case  4:   // display 4 (X---)
      DISPLAY_4 = 1;
    break;
  }
}

void formateaNumero4Digitos(unsigned int numero, unsigned char digitos[])
/** separa el nro recibido en 4 digitos para mostrarlo en los displays del Edukit **/
{
  unsigned int  temporal;

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
    PTD = valor & 0x0f; // nibble bajo
    PTA = valor & 0xf0; // nibble alto
}

void muestraNumeroEnDisplay(unsigned char numero, unsigned char puntoDecimal)
/** muestra el número recibido en algun display del Edukit **/
//  puntoDecimal = (OFF, ON)
{
    PTD = BCDa7seg(numero) & 0x0f; // nibble bajo
    PTA = BCDa7seg(numero) & 0xf0; // nibble alto

    if (puntoDecimal) enciendePuntoDecimal();
}


void barreDisplayPorSegmentos(unsigned char numero, unsigned char puntoDecimal)
/** muestra el numero recibido como parametro en un display, barriendolo por segmentos **/
//  puntoDecimal = (OFF, ON)
{
    unsigned char numeroEn7Segmentos, bitTesteado = 0;
    numeroEn7Segmentos = BCDa7seg(numero);

    // si hay punto decimal, lo agrego al numero codificado en 7 seg
    if (puntoDecimal) numeroEn7Segmentos = numeroEn7Segmentos | 0x80;

    // recorro bit a bit para encender el segmento correspondiente
    while (bitTesteado < 8) {
      apagaSegmentos();
      if (numeroEn7Segmentos & 0x01) {
         // enciendo el segmento correspondiente
         switch ( bitTesteado )
         {
           case 0: // segmento 'a'
                SEG_A = 1;
                demoraEnus(DEMORA_SEGMENTOS_US);
                break;
           case 1: // segmento 'b'
                SEG_B = 1;
                demoraEnus(DEMORA_SEGMENTOS_US);
                break;
           case 2: // segmento 'c'
                SEG_C = 1;
                demoraEnus(DEMORA_SEGMENTOS_US);
                break;
           case 3: // segmento 'd'
                SEG_D = 1;
                demoraEnus(DEMORA_SEGMENTOS_US);
                break;
           case 4: // segmento 'e'
                SEG_E = 1;
                demoraEnus(DEMORA_SEGMENTOS_US);
                break;
           case 5: // segmento 'f'
                SEG_F = 1;
                demoraEnus(DEMORA_SEGMENTOS_US);
                break;
           case 6: // segmento 'g'
                SEG_G = 1;
                demoraEnus(DEMORA_SEGMENTOS_US);
                break;
           case 7: // segmento 'd.p.'
                SEG_DP = 1;
                demoraEnus(DEMORA_SEGMENTOS_US);
                break;
         }
      }
      bitTesteado++;
      numeroEn7Segmentos = numeroEn7Segmentos >> 1;
    }
}

void muestraNumero4Digitos(unsigned int numero, unsigned char posicionPuntoDecimal, unsigned char modo)
/** muestra el nro recibido en los 4 displays del Edukit **/
// posicionPuntoDecimal = (0 -apagado-, 1, 2, 3, 4)
// modo = (BARRER_POR_DISPLAY, BARRER_POR_SEGMENTO)
{
    unsigned char i, digitos[4];

    formateaNumero4Digitos(numero, digitos);
    apagaDisplays();

    if(modo == BARRER_POR_SEGMENTO){
        // barro por segmentos
        for(i = 1; i <= 4; i++){
            activaDisplay(i);
            barreDisplayPorSegmentos(digitos[i-1], OFF);
            if(posicionPuntoDecimal == i){
              enciendePuntoDecimal();
              demoraEnus(DEMORA_SEGMENTOS_US);
              apagaSegmentos(); // para evitar parpadeo espureo en el proximo ciclo
            }
        }
    }
    else{
        // barro por display entero
        for(i = 1; i <= 4; i++){
            muestraNumeroEnDisplay(digitos[i-1], OFF);
            activaDisplay(i);
            if(posicionPuntoDecimal == i) enciendePuntoDecimal();
            demoraEnms(DEMORA_DISPLAY_MS);
        }
    }
}

/** ----------- Seccion de funciones para display LCD 16x2 --------- **/

void datoLCD(unsigned char dato)
/** descompone el byte (dato) y lo pone en el bus de datos del LCD **/
{
    PTD = dato & 0x0f; // nibble bajo
    PTA = dato & 0xf0; // nibble alto
}

void controlLCD(unsigned char comando)
/** Recibe un byte como parámetro para comandar el LCD **/
{
    // vuelco el comando en el bus de datos
    datoLCD(comando);

    // operación de escritura
    PTB_PTB5 = 0; // Write
    PTC_PTC1 = 0; //

    // provocamos un flanco en E para que el comando sea leido por el LCD
    PTC_PTC0 = 1;
    demoraEnms(2);
    PTC_PTC0 = 0;
    demoraEnms(2);
}

void initLCD(void)
/** encapsula todos los comandos de inicialización del LCD del Edukit **/
{
    demoraEnms(20); // demora para el encendido del LCD

    // comandos de configuración
    controlLCD(0x02);
    controlLCD(0x38);
    controlLCD(0x0c);
    controlLCD(0x06);
    controlLCD(0x01);
}

void enviaCharLCD(unsigned char caracter)
/** envia el caracter para mostrarlo en el LCD **/
{
    datoLCD(caracter);
    // operación de escritura
    PTB_PTB5 = 0; // r/w = 0
    PTC_PTC1 = 1; // rs = 1

    // provocamos un flanco en E para que el comando sea leido por el LCD
    PTC_PTC0 = 1;
    demoraEnms(2);
    PTC_PTC0 = 0;
    demoraEnms(2);
}

void setLCDCursor(unsigned char fila, unsigned char columna)
/** posiciona el cursor en la coordenada indicada **/
{
    unsigned char comando;

    switch(fila)
    {
        case 0:
            comando = 0x80;
            break;
        case 1:
            comando = 0xc0;
            break;
    }
    // desplazo a la columna indicada
    comando+=columna;
    // envio el comando armado al LCD
    controlLCD(comando);
}

void printLCD(unsigned char fila, unsigned char columna, char *texto)
/** muestra el texto en la posicion especificada **/
{
    unsigned char i=0;
    setLCDCursor(fila, columna);

    // recorro la cadena de texto para mostrarla
    while(texto[i] != 0){
        enviaCharLCD(texto[i]);
        i++;
    }
}
void modoTest(void)
/** muestra el numero recibido en los 4 displays del Edukit **/
{
    unsigned char i, digitos[4];
    digitos[0]=8;
    digitos[1]=8;
    digitos[2]=8;
    digitos[3]=8;

    //formateaNumero4Digitos(numero, digitos);

    // barro los displays para mostrar el mumero
    for(i = 0; i < 4; i++)
    {
      apagaDisplays();
      muestraNumeroEnDisplay(digitos[i], OFF);
      enciendePuntoDecimal();
      // activo el punto separador de segundos
      //if(display == 3) muestraPuntoEnDisplay;

      activaDisplay(i+1);
      // dejo encendido el display un instante para persistencia visual
      demoraEnms(5);
    }
    apagaDisplays();
}
