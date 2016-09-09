#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "tanteador.h"

void main(void) {

  inicializaCPU();
  inicializaDisplays();
  inicializaPulsadores();
  DisableInterrupts;


  for(;;) {
    __RESET_WATCHDOG(); /* feeds the dog */
  } /* loop forever */
  /* please make sure that you never leave main */
}
