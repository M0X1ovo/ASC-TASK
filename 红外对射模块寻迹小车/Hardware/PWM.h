#ifndef __PWM_H
#define __PWM_H

void PWM_Init(void);
void PWM_Stop(void);
void PWM_SetCompareleftfront(uint16_t Compare);
void PWM_SetCompareleftback(uint16_t Compare);
void PWM_SetComparerightfront(uint16_t Compare);
void PWM_SetComparerightback(uint16_t Compare);

#endif
