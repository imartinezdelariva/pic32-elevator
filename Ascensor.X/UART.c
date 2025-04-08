#include <xc.h>
#include "Pic32Ini.h"




#define TAM_COLA 100
typedef struct{
    int cabeza; //Marca el indice de la cabeza
    int cola;
    char datos[TAM_COLA];
} cola_circ;


//////////////////////////////////////////
static cola_circ cola_tx, cola_rx;


void InicializarUART1(int baudios)
{
    ANSELB &= ~((1<<13)|(1<<7)); // Pines digitales
    TRISB &=~(1<<7);
    TRISB |=(1<<13);
    LATB |= 1<<7; // A 1 si el transmisor est? inhabilitado.


    SYSKEY=0xAA996655; // Desbloquear registros de configuraci?n
    SYSKEY=0x556699AA;
    U1RXR = 3; // Conectar U1RX a RB13
    RPB7R = 1; // Conectar U1TX a RB7
    SYSKEY=0x1CA11CA1; // Bloquear registros de configuraci?n


    if (baudios > 38400) {
        U1MODEbits.BRGH = 1; // Alta velocidad, divisor por 4
        U1BRG = (5000000 / (4 * baudios)) - 1;
    } else {
        U1MODEbits.BRGH = 0; // Velocidad est?ndar, divisor por 16
        U1BRG = (5000000 / (16 * baudios)) - 1;
    }


    U1STAbits.URXISEL = 0; // Interrupci?n al recibir un car?cter
    U1STAbits.UTXISEL = 2; // Interrupci?n cuando FIFO est? vac?a


    IFS1bits.U1RXIF = 0; // Limpiar flag de interrupci?n del receptor
    IEC1bits.U1RXIE = 1; // Habilitar interrupci?n del receptor
    IFS1bits.U1TXIF = 0; // Limpiar flag de interrupci?n del transmisor
    IPC8bits.U1IP = 3;   // Prioridad de interrupci?n
    IPC8bits.U1IS = 1;   // Subprioridad


    U1STAbits.URXEN = 1; // Habilitar receptor
    U1STAbits.UTXEN = 1; // Habilitar transmisor


    INTCONbits.MVEC = 1; // Modo multivector


    U1MODE = 0x8000;     // Encender UART
}


//Interrupciones
__attribute__((vector(32),interrupt(IPL3SOFT),nomips16))
void InterupcionUART(){
   
    //Interrupcion transmisor
    if(IFS1bits.U1TXIF==1){
        if(cola_tx.cabeza != cola_tx.cola){//Hay cosas que enviar
            U1TXREG= cola_tx.datos[cola_tx.cola];
            cola_tx.cola++;
            if(cola_tx.cola==TAM_COLA){
                cola_tx.cola=0;
            }
        }else{
            //No hay nada que enviar
            IEC1bits.U1TXIE = 0;
        }
        IFS1bits.U1TXIF=0; //flag a 0
    }
    //Interrupcion receptor
    if(IFS1bits.U1RXIF==1){
        if((cola_rx.cabeza +1 ==cola_rx.cola) || (cola_rx.cabeza +1 ==TAM_COLA && cola_rx.cola==0)){
        }else{
            cola_rx.datos[cola_rx.cabeza]=U1RXREG; //Lee dato
            cola_rx.cabeza ++;
            if (cola_rx.cabeza==TAM_COLA){
                cola_rx.cabeza=0;
            }
        }
        IFS1bits.U1RXIF = 0; //flag a 0
    }
}




void putsUART(char* ps ){
    while(*ps!='\0'){ //Cuando se lea /0 se acaba la secuencia a escribir
        if((cola_tx.cabeza +1 ==cola_tx.cola)||
                (cola_tx.cabeza +1 ==TAM_COLA && cola_tx.cola==0)){
            break ;//Cola llena, abortar
        }else{
            cola_tx.datos[cola_tx.cabeza]= *ps;//Escribe el elemento al que apunta
            ps++; //Apunta al siguiente
            cola_tx.cabeza++;
            if(cola_tx.cabeza==TAM_COLA){
                cola_tx.cabeza=0;
            }
        }
    }
    IEC1bits.U1TXIE = 1;//Se habilita interrupcion para enviar


}




char getcUART(void)
{
    char c;


    if(cola_rx.cabeza != cola_rx.cola){ // Hay datos nuevos
        c = cola_rx.datos[cola_rx.cola];
        cola_rx.cola++;
        if(cola_rx.cola == TAM_COLA){
            cola_rx.cola=0;
        }
    } else { // No ha llegado nada
        c = '\0';
    }


    return c;
}
