#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "tanteador.h"

/*-------------Declaración de funciones------------*/

void MuestraModoConfig(unsigned char, unsigned char);
unsigned char ModoConfig(void);
void ModoPlay(unsigned char );

/*-------------Variables globales-------------*/

unsigned char pepe;

/**-------------Main-------------**/

void main(void){
    unsigned char i=0,antirrebote=0;                              
    DisableInterrupts;
    inicializaCPU();
    inicializaPulsadores();
    inicializaDisplays();
  
    for(;;){
        pepe=ModoConfig();
     
        ModoPlay(pepe+1);
    } /* loop forever */
} /* fin main */

/** --------- Sección de funciones -------------- **/

void ModoPlay(unsigned char modoConfig){
    char player1=0;
    char player2=0;
    unsigned int numero=0;
    unsigned char i=0,antirrebote=0;
  
    while(1){
        numero=player1*100+player2;               
        muestraNumero4Digitos(numero,0,BARRER_POR_DISPLAY);
        
                
   //ModoConfig
   
   if(SW3 == 0 && SW1 == 0) { 
   demoraEnms(500);
   break;
   }
    
    
   //RESET SCORE 
    
   if(!antirrebote){
   if(SW4 == 0 && SW2 == 0){
      player2=0;
      player1=0;
      antirrebote=1;
      i=0;
    }
    } else {
      
      i++;
      if(i==150) antirrebote=0;
    }
        
        
        
                           
        if(!antirrebote){
            if(SW1==0){
            player1++;
            antirrebote=1;
            i=0;
            }
        } 
        else{
            i++;
            if(i==50) antirrebote=0;
        }
   
        if(!antirrebote){
            if(SW2==0){
                if (player1==0) i++;
                else{
                    player1--;
                    antirrebote=1;
                    i=0;
                }
            }
         }
         else{
             i++;
             if(i==50) antirrebote=0;
         }
   
         if(!antirrebote){
             if(SW3==0){
                 player2++;
                 antirrebote=1;
                 i=0;   
             }     
         }
         else{
             i++;
             if(i==50) antirrebote=0;
         }
     
         if(!antirrebote){
             if(SW4==0){
                 if (player2==0) i++;
                 else{
                     player2--;
                     antirrebote=1;
                     i=0;
                 }
      
             }    
         }
         else{
             i++;
             if(i==50) antirrebote=0;
         }
      
         if (player1==modoConfig) break;
    
         if (player2==modoConfig) break;
 
         
     }
}


unsigned char ModoConfig(void){

    unsigned char antirrebote=0,i=0,retorno=0;
    unsigned int numero=0;

    int pulso=0;
    int modo=0;

    while(1){
        if(!antirrebote){
            if(SW1==0 && SW2==0){           //Modo Test
                while(SW1==0 && SW2==0){       
                    modoTest();
                }
                antirrebote=1;
                i=0;
            } 
      
            if(SW4==0){           //P1+
                pulso++;   
                demoraEnms(50);
                antirrebote=1;
                i=0;
            }
        } 
        else{    
            i++;
            if(i==50) antirrebote=0;
        }
  
        if(pulso==4) pulso=0;    //P1 limite
  
        if(SW3==0){              //P1-  Retorno del modo de juego
            pulso=0;
            demoraEnms(150);
            return (numero);
        }

        if(pulso==0) numero=5;
        if(pulso==1) numero=7;
        if(pulso==2) numero=14;
        if(pulso==3) numero=21;

        MuestraModoConfig(numero,modo);

    }
}

void MuestraModoConfig(unsigned char puntos, unsigned char modo){
    unsigned char i, digitos[4];
  
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