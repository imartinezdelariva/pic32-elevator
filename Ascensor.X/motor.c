#include <xc.h>

#define PIN_PWM_MOTOR_A 7
#define PIN_DIR_MOTOR_A 6
#define PERIODO 249 //100us

void inicializar_motor(void);
void parar_motor(void);
void girar_derecha(void);
void girar_izquierda(void);

void inicializar_motor(void) {
    ANSELC &= ~(1 << PIN_PWM_MOTOR_A | 1 << PIN_DIR_MOTOR_A); // Pines como digitales
    TRISC &= ~(1 << PIN_PWM_MOTOR_A | 1 << PIN_DIR_MOTOR_A); // Pines como salida
    LATC &= ~(1 << PIN_DIR_MOTOR_A); // en bajo por defecto
    
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    RPC7R = 5;            // OC1 ahora est� en RC7
    SYSKEY = 0x1CA11CA1;
    
    OC1CON = 0; // Deshabilitar OC1
    OC1R = 0;
    OC1RS = 0;
    OC1CON = 0x8006; // Habilitar OC1 en modo PWM
    
    T2CON = 0;
    TMR2 = 0;
    PR2 = PERIODO; // Establecer período de 50 us
    T2CON = 0x8000;
}

void parar_motor(void) {
    LATC &= ~(1 << PIN_DIR_MOTOR_A); // Asegurar dirección en bajo
    OC1RS = 0; // Detener PWM
}

void girar_derecha(void) {
    LATC &= ~(1 << PIN_DIR_MOTOR_A); //  derecha
    OC1RS = (80 * PERIODO) / 100; // 50% de ciclo útil
}

void girar_izquierda(void) {
    LATC |= (1 << PIN_DIR_MOTOR_A); //  izquierda
    OC1RS = (80 * PERIODO) / 100; // 50% de ciclo útil
}
