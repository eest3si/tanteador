#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "tanteador.h"

/*-------------Prototipos------------*/
void modoPlay(unsigned char);
void modoInfo(unsigned char);
unsigned char modoMenu(void);
void muestraFecha(unsigned char,unsigned char);

/**-------------Main-------------**/

void main(void){

    unsigned char opcion = 0;

    configuraCPU();
    //inicializaPLL();
    habilitaPulsadores();
    inicializaKBI();
    habilitaDisplays();
    habilitaGuionY2Puntos();// Guion: PTA2, 2Puntos: PTB0 
    inicializaTIM1();       // Input-capture para el control remoto IR
    inicializaTIM2();       // base de tiempo de 100 ms
    configuraADC();         // para el sensor de temperatura (ADCH3/PTA3)
    HabilitarSensorIR;      // conectado a TIM1CH0/PTB4 
    EnableInterrupts;

    modoIntro();
    configuraFechaHora();
   
    for(;;){
    	// menu principal (Carbajal)
    	   	 
        opcion = modoMenu();

        if (opcion)
            modoPlay(modoConfig());
        else
            modoInfo(FALSE);
    } /* loop forever */
} /* fin main */


/** --------- Seccion funciones -------------- **/

void modoPlay(unsigned char topePartida)
/** version inicial por Bourgez **/
/** manejo del empate por Torres **/
{
      
    char player1 = 0, player2 = 0;
    unsigned char LastSt = 0, empate = 0;  
    unsigned int numero = 0;
  
    while(1) {

        // manejo de la ventaja
        if(player1 >= (topePartida-1) && player2 >= (topePartida-1)) {

            empate = topePartida - 1; 
    
            // P2 tenia la ventaja y P1 se la saca
            if(LastSt == 2)
                if(player1 < topePartida && player2 >= topePartida) {
                    LastSt = 1;
                    player2 = empate;
                }
            
            // P1 tenia la ventaja y P2 se la saca
            if(LastSt == 1)
                if(player1 >= topePartida && player2 < topePartida) {
                    LastSt = 2;
                    player1 = empate;
                }
            
            // ventaja P1
            if(player1 >= topePartida && player2 < topePartida)
                LastSt = 1;
            
            if(LastSt == 1) {
                numero = empate*100 + empate; 
                muestraNumero4Digitos(numero, 3, OFF);
                player2 = empate;  
            }
               
            // ventaja P2
            if(player2 >= topePartida)
                LastSt = 2;

            if (LastSt == 2) {
                numero = empate*100 + empate; 
                muestraNumero4Digitos(numero, 1, OFF);
                player1 = empate;  
            } 
               
            // ventaja iguales
            if(player1 == empate || player2 == empate ) {
                numero = empate*100 + empate;
                muestraNumero4Digitos(numero, OFF, OFF);  
            }
                 
        } else {
            numero = player1*100 + player2;
            muestraNumero4Digitos(numero, OFF, OFF);  
        }
                
		  //ModoConfig (P1- y P2-)	   
	   	if(ModoConfig) {
	   		limpiaFlagsGlobales();
	   		break;
	   	}
	   
	    //RESET SCORE (P1+ y P2+)
	  	if(ReiniciarPartida) {
	    	limpiaFlagsGlobales();
	    	player2 = 0;
	    	player1 = 0;
	    }
	 
		if(P1mas) {
			P1mas = FALSE;
			player1++;
		}

	    if(P1menos) {
	    	P1menos = FALSE;
	    	if(player1>0) player1--;
	    }

	    if(P2mas) {
	    	P2mas = FALSE;
	    	player2++;
	    }

	    if(P2menos) {
	    	P2menos = FALSE;
	    	if(player2>0) player2--;
	    }

	    // fin de la partida
        if (player1 == topePartida || player2 == topePartida){
	      limpiaFlagsGlobales();
	      break;
	    }
    }
}

unsigned char modoMenu(void)
/** menu principal (Carbajal) **/
/** permite elegir entre "modo play" y "modo info" **/
/** va al modo info tras 1 minuto de inactividad **/
{
    unsigned char pulso = 2;
    
    Timer1Min = ON;
    
    // lo reestablece la ISR
    while(Timer1Min) {

        if (pulso > 2)
            pulso = 1;

        // modo play (muestra PP)
        if (pulso == 2) {
            muestraEnDisplay(0x73); // P
            activaDisplay(4);
            demoraEnms(DEMORA_DISPLAY_MS);
            activaDisplay(3);
            demoraEnms(DEMORA_DISPLAY_MS);            
        }

        // modo info
        if (pulso == 1) {
            muestraEnDisplay(0x06); // I
            activaDisplay(4);
            demoraEnms(DEMORA_DISPLAY_MS);
            DISPLAY_4 = OFF;
            
            muestraEnDisplay(0x54); // n
            activaDisplay(3);
            demoraEnms(DEMORA_DISPLAY_MS);
            DISPLAY_3 = OFF;
    
            muestraEnDisplay(0x71); // F
            activaDisplay(2);
            demoraEnms(DEMORA_DISPLAY_MS);
            DISPLAY_2 = OFF;
    
            muestraEnDisplay(0x5C); // o
            activaDisplay(1);
            demoraEnms(DEMORA_DISPLAY_MS);
            DISPLAY_1 = OFF;
        }

        if (P1mas) {
        	P1mas = OFF;	
        	pulso++;
          ContadorTimer1Min = 0;  // reinicio el timeout por inactividad
        }

        // confirmacion del usuario
        if (P1menos) {     
            limpiaFlagsGlobales();
            Timer1Min = OFF;        // apago el flag del timer
            ContadorTimer1Min = 0;  // reinicio el timeout por inactividad
            
            if (pulso == 1)
                return FALSE;
            if (pulso == 2)
                return TRUE;
        }
    }
    // salgo por timeout (va al modo Info)
    return FALSE;
}

void modoInfo(unsigned char exit) {
/** muestra hora, fecha y temperatura ciclicamente cada 5 seg **/
/** por Juarez **/
// 5 sec muestraHora(); muestraFecha(); muestraTemperatura();
// cualquier tecla lo saca de este modo y va al menu ppal
unsigned int cont=0;
 
while(!exit){
demoraEnms(500);
GUION = ~GUION;
if(ReiniciarPartida) break;
/*
cont++;

  if(cont>0 && cont<500){
  muestraHora();
  }
  if(cont>500 && cont<1000){
  muetraFecha();
  }
  if(cont>1000 && cont<1500){
  muestraTemperatura();
  }
*/
}
}

void muestraFecha(void){

  unsigned int numero1;
  
  numero1=Dia*100+Mes;               
  muestraNumero4Digitos(numero1, OFF, OFF);
  GUION = ON;       //Habilito guion de fecha
  DOS_PUNTOS = ON;  //Deshabilito Dos puntos de hora
}