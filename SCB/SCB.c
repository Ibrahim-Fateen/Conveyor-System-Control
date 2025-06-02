#include "SCB.h"
#include "SCB_Private.h"

void SCB_Reset(void) {
    SCB_AIRCR = AIRCR_VECTKEY | SYS_RESET_REQ;

    // wait for everything to reset properly
    while (1) {}
}
