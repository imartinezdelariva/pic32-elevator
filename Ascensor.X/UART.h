

#ifndef UART_H
#define	UART_H

#ifdef	__cplusplus
extern "C" {
#endif
#define TAMANO_COLA 100
typedef struct {
    int cabeza; // Marca el ?ndice de la cabeza
    int cola;
    char datos[TAMANO_COLA];
} cola_circ;
static cola_circ cola_tx, cola_rx;
void InicializarUART1(int baudios);
char getcUART(void);
void putsUART(char* ps);
void InterupcionUART();

#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */
