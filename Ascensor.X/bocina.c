#include <xc.h>
#include "Pic32Ini.h"

#define PIN_BOCINA 1  // RA0
#define PERIODO 7999  // Calculado para 5 kHz, sin prescaler

void inicializar_bocina(void);
void sonar(void);
void parar_bocina(void);

void inicializar_bocina(void) {
    ANSELA &= ~(1 << PIN_BOCINA);  // RA0 digital
    TRISA &= ~(1 << PIN_BOCINA);   // RA0 salida

    // Remapear OC2 a RA1
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    RPA1R = 5; // OC2 en RA1
    SYSKEY = 0x1CA11CA1;

    // Configurar Timer3 sin prescaler
    T3CON = 0;          // Stop y reset Timer3
    TMR3 = 0;           // Reset contador
    PR3 = PERIODO;      // PWM periodo (para 5 kHz)
    T3CON = 0x8000;     // Prescaler 1:1, Timer ON

    // Configurar OC2 para usar Timer3
    OC2CON = 0;
    OC2R = 0;
    OC2RS = 0;
    OC2CON = 0x800E; // PWM sin sincronización, usa Timer3
}

void parar_bocina(void) {
    OC2RS = 0;
}

void sonar(void) {
    OC2RS = (PERIODO * 50) / 100;  // 50% ciclo útil
}
