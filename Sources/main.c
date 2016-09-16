#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "tanteador.h"

unsigned char ModoConfig(void);

void main(void) {

  DisableInterrupts;
  inicializaCPU();
  inicializaPulsadores();
  inicializaDisplays();

  for(;;) {
    ModoConfig();
  } /* loop forever */
} /* fin main */


/** --------- seccion funciones -------------- **/
unsigned char ModoConfig(void)
{

unsigned char antirrebote=0,  i=0, retorno=0;
unsigned int numero=0;

int pulso=0;
int enter=0;

while(1){

PTB_PTB7=1;             //Letra C
PTB_PTB5=0;
PTC_PTC1=0;
PTC_PTC0=0;

PTD_PTD0=1;
PTD_PTD1=0;
PTD_PTD2=0;
PTD_PTD3=1;
PTA_PTA4=1;
PTA_PTA5=1;
PTA_PTA6=0;
PTA_PTA7=0;
demoraEnms(10);

PTB_PTB7=0;             //Letra P
PTB_PTB5=1;
PTC_PTC1=0;
PTC_PTC0=0;

PTD_PTD0=1;
PTD_PTD1=1;
PTD_PTD2=0;
PTD_PTD3=0;
PTA_PTA4=1;
PTA_PTA5=1;
PTA_PTA6=1;
PTA_PTA7=0;
demoraEnms(10);


         if(!antirrebote){
      
      if(SW4==0){         //P1+
pulso++;   
demoraEnms(50);
        antirrebote=1;
        i=0;
      }
    } else{    
      i++;
      if(i==15) antirrebote=0;
    }
  
      
      if(pulso==4){        //P1 limite
pulso=0;
    }
  
  
  
if(pulso==0) {  //Modo de juego en 05  
PTB_PTB7=0;
PTB_PTB5=0;
PTC_PTC1=0;
PTC_PTC0=1;

PTD_PTD0=1;
PTD_PTD1=0;
PTD_PTD2=1;
PTD_PTD3=1;
PTA_PTA4=0;
PTA_PTA5=1;
PTA_PTA6=1;
PTA_PTA7=0;
demoraEnms(10);

PTB_PTB7=0;
PTB_PTB5=0;
PTC_PTC1=1;
PTC_PTC0=0;

PTD_PTD0=1;
PTD_PTD1=1;
PTD_PTD2=1;
PTD_PTD3=1;
PTA_PTA4=1;
PTA_PTA5=1;
PTA_PTA6=0;
PTA_PTA7=0;
demoraEnms(10);
retorno = 5;
}


if(pulso==1){   //Modo de juego en 07
PTB_PTB7=0;
PTB_PTB5=0;
PTC_PTC1=0;
PTC_PTC0=1;

PTD_PTD0=1;
PTD_PTD1=1;
PTD_PTD2=1;
PTD_PTD3=0;
PTA_PTA4=0;
PTA_PTA5=0;
PTA_PTA6=0;
PTA_PTA7=0;
demoraEnms(10);
PTB_PTB7=0;
PTB_PTB5=0;
PTC_PTC1=1;
PTC_PTC0=0;

PTD_PTD0=1;
PTD_PTD1=1;
PTD_PTD2=1;
PTD_PTD3=1;
PTA_PTA4=1;
PTA_PTA5=1;
PTA_PTA6=0;
PTA_PTA7=0;
demoraEnms(10);
retorno=7;  
}


if(pulso==2){   //Modo de juego en 14 
PTB_PTB7=0;
PTB_PTB5=0;
PTC_PTC1=0;
PTC_PTC0=1;

PTD_PTD0=0;
PTD_PTD1=1;
PTD_PTD2=1;
PTD_PTD3=0;
PTA_PTA4=0;
PTA_PTA5=1;
PTA_PTA6=1;
PTA_PTA7=0;
demoraEnms(10);

PTB_PTB7=0;
PTB_PTB5=0;
PTC_PTC1=1;
PTC_PTC0=0;

PTD_PTD0=0;
PTD_PTD1=1;
PTD_PTD2=1;
PTD_PTD3=0;
PTA_PTA4=0;
PTA_PTA5=0;
PTA_PTA6=0;
PTA_PTA7=0;
demoraEnms(10);
retorno=14;
}


if(pulso==3){   //Modo de juego en 21
PTB_PTB7=0;
PTB_PTB5=0;
PTC_PTC1=0;
PTC_PTC0=1;

PTD_PTD0=0;
PTD_PTD1=1;
PTD_PTD2=1;
PTD_PTD3=0;
PTA_PTA4=0;
PTA_PTA5=0;
PTA_PTA6=0;
PTA_PTA7=0;
demoraEnms(10);

PTB_PTB7=0;
PTB_PTB5=0;
PTC_PTC1=1;
PTC_PTC0=0;

PTD_PTD0=1;
PTD_PTD1=1;
PTD_PTD2=0;
PTD_PTD3=1;
PTA_PTA4=1;
PTA_PTA5=0;
PTA_PTA6=1;
PTA_PTA7=0;
demoraEnms(10);
retorno=21;
}


if(SW3==0){        //P1-  Retorno del modo de juego
pulso=0;
demoraEnms(50);
return (retorno);
}

}




} /* fin ModoConfig */
