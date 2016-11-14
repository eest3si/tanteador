#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "tanteador.h"

/*-------------Prototipos------------*/

void muestraModoConfig(unsigned char);
unsigned char modoConfig(void);
unsigned char modoPlay(unsigned char);
unsigned char ModoMenu(unsigned char );

/**-------------Main-------------**/

void main(void){
    unsigned char pepe,A;                              
    
    configuraCPU();
    habilitaPulsadores();
    inicializaKBI();
    habilitaLCDyDisplays();
    habilitaGuionY2Puntos();
    inicializaTIM1();       // Input-capture para el control remoto IR
    inicializaTIM2();       // base de tiempo de 100 ms
    configuraADC();         // para el sensor de temperatura (ADCH3/PTA3)
    HabilitarSensorIR;      // conectado a TIM1CH0/PTB4 
    EnableInterrupts;
  
      configuraFechaHora();
    for(;;){
    	// mensaje introductorio (Bejarano) 	 
    	
    	
    	// menu principal (Carbajal)
    	 
        ModoMenu(A);
 
  
        pepe=modoConfig();
     
        A = modoPlay(pepe+1);
    
    } /* loop forever */
} /* fin main */

/** --------- Seccion de funciones -------------- **/

unsigned char modoPlay(unsigned char modoConfig){
    char player1=0;
    char player2=0;
    unsigned int numero=0;
  
    while(1){
        numero=player1*100+player2;               
        muestraNumero4Digitos(numero, OFF, OFF);
                
		//ModoConfig
	   
	   	if(ModoConfig) {
	   		ModoConfig = OFF;	// limpio flag
	   		break;
	   	}
	   
	    //RESET SCORE
	  	if(ReiniciarPartida) {
	    	ReiniciarPartida = OFF;	// limpio flag
	    	player2=0;
	    	player1=0;
	    }
	 
		if(P1mas) {
			P1mas = OFF;	// limpio flag
			player1++;
		}

	    if(P1menos) {
	    	P1menos = OFF;	// limpio flag
	    	if(player1>0) player1--;
	    }

	    if(P2mas) {
	    	P2mas = OFF;	// limpio flag
	    	player2++;
	    }

	    if(P2menos) {
	    	P2menos = OFF;	// limpio flag
	    	if(player2>0) player2--;
	    }

	    if (player1==modoConfig) break;

	    if (player2==modoConfig) break; 
    }
}


unsigned char modoConfig(void){

    unsigned char numero=0, pulso=0;

    while(1){
    	//Modo Test
        if(ModoTest) {
        	ModoTest = OFF;	// limpio flag
        	while(Timer3Seg) modoTest();
        }
      
    	// P1+
        if(P1mas) {
        	P1mas = OFF;	// limpio flag
        	pulso++;
        }   
  
        //P1 limite
        if(pulso==4) pulso=0;
  
        
        if(P1menos) {              //P1-  Retorno del modo de juego
            P1menos = OFF;	// limpio flag
            pulso=0;
            demoraEnms(150);
            return (numero);
        }

        if(pulso==0) numero=5;
        if(pulso==1) numero=7;
        if(pulso==2) numero=14;
        if(pulso==3) numero=21;

        muestraModoConfig(numero);

    }
}

void muestraModoConfig(unsigned char puntos){
  
    muestraCaracterEnDisplay(0x39);
    activaDisplay(4);
    demoraEnms(DEMORA_DISPLAY_MS);
    muestraCaracterEnDisplay(0x73);
    activaDisplay(3);
    demoraEnms(DEMORA_DISPLAY_MS);
    muestraNumeroEnDisplay(puntos/10, OFF);
    activaDisplay(2);
    demoraEnms(DEMORA_DISPLAY_MS);
    muestraNumeroEnDisplay(puntos%10, OFF);
    activaDisplay(1);
    demoraEnms(DEMORA_DISPLAY_MS);
            
}  
/* Fin ModoConfig */

unsigned char ModoMenu(unsigned char A) {

unsigned char pulso=0;

  while(1){
    
         if(P1mas) {
        	P1mas = OFF;	
        	pulso++;
         }
        	
        	if (pulso>=2){  
          pulso=0;
        } 
        
         
         
  if(pulso==0){
    
    muestraCaracterEnDisplay(0x73);
    activaDisplay(4);
    demoraEnms(DEMORA_DISPLAY_MS);
    muestraCaracterEnDisplay(0x73);
    activaDisplay(3);
    demoraEnms(DEMORA_DISPLAY_MS);
    muestraCaracterEnDisplay(OFF);
    activaDisplay(2);
    demoraEnms(DEMORA_DISPLAY_MS);
    muestraCaracterEnDisplay(OFF);
    activaDisplay(1);
    demoraEnms(DEMORA_DISPLAY_MS);
  }
  
  if(P1menos){
   P1menos = OFF;
   
  return A;
    
  }
  
   
    if (pulso==1){
      
    muestraNumeroEnDisplay(1,OFF);
    activaDisplay(4);
    demoraEnms(DEMORA_DISPLAY_MS);
    muestraCaracterEnDisplay(0x54);
    activaDisplay(3);
    demoraEnms(DEMORA_DISPLAY_MS);
    muestraCaracterEnDisplay(0x71);
    activaDisplay(2);
    demoraEnms(DEMORA_DISPLAY_MS);
    muestraCaracterEnDisplay(0x5C);
    activaDisplay(1);
    demoraEnms(DEMORA_DISPLAY_MS);
    }
   
    if(P1menos){     
    P1menos = OFF;   
    
    configuraFechaHora();
      
    }
    
}
}