#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "tanteador.h"

void main(void) {

  inicializaCPU();
  inicializaDisplays();
  inicializaPulsadores();
  DisableInterrupts;

// CP
while(1){

PTB_PTB7=1;
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

PTB_PTB7=0;
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
/*
 if(!antirrebote)
   {
    if(SW4==0)
    {
     pulso++;
     if(pulso==4||SW3==0) 
     pulso=0;
     antirrebote=1;
     i=0;
    }
   } 
   else
    {
     i++;
     if(i==15) antirrebote=0;
    }
   }
*/


if(SW4==0){
pulso++;   
demoraEnms(50);
}

if(pulso==4){
pulso=0;     
}

CallMode(pulso);

}
}





void CallMode(int pulso) {


//05
if(pulso==0) {
  
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
}

//07
if(pulso==1){

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
}

//14
if(pulso==2){
  
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
}

//21

if(pulso==3){
  
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

}

}
  
  
  