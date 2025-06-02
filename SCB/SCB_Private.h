#ifndef SCB_PRIVATE_H
#define SCB_PRIVATE_H

#define SCB_AIRCR (*((volatile unsigned long *)0xE000ED0C))

#define AIRCR_VECTKEY 0x5FA0000
#define SYS_RESET_REQ 0x4

#endif //SCB_PRIVATE_H
