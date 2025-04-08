#ifndef MOTOR_H
#define MOTOR_H

#include <xc.h>

#define PIN_PWM_MOTOR_A 7
#define PIN_DIR_MOTOR_A 6 
#define PERIODO 249 // 50us

void inicializar_motor(void);
void parar_motor(void);
void girar_derecha(void);
void girar_izquierda(void);

#endif // MOTOR_H
