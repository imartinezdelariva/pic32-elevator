#include "Pic32Ini.h"
#include "UART.h"
#include <stdio.h>
#include <xc.h>
#include "motor.h"

#define TAM_COM 1 //UART
#define TAM_COLA 3
char comando[TAM_COM]={0};
char pos;
char mensaje[TAM_COLA]={0};

int PRIMERA_LLAMADA=-1; //Por si estás en el primero saber dónde ir primero
int PISOS[3]= {0,0,0}; //Para saber si hay que ir a un piso
int PISO_ACTUAL=0;

typedef enum {PARADO_ESTADO, SUBIENDO, BAJANDO} EstadoAscensor;
EstadoAscensor estado = PARADO_ESTADO;

int T_PARADO=0;
int PARADO=0; //Variable para saber si el ascensor está parado(1) o no(0)
int EMERGENCIA=0;

#define PIN_PISO0 5
#define PIN_PISO1 8
#define PIN_PISO2 9
#define SENSOR_PISO0 7
#define SENSOR_PISO1 0
#define SENSOR_PISO2 9

//Variables para detectores de flanco
int prev_piso0, prev_piso1, prev_piso2;
int post_piso0, post_piso1, post_piso2;
int prev_sensor0, prev_sensor1, prev_sensor2;
int post_sensor0, post_sensor1, post_sensor2;

void init_temp4(void){
    T4CON=0;
    TMR4=0;
    PR4=19531; 
    IPC4bits.T4IP=4;
    IPC4bits.T4IS=0;
    IFS0bits.T4IF=0;
    IEC0bits.T4IE=1;
    T4CON=0x0070;
}

void __attribute__ (( vector ( _TIMER_4_VECTOR ) , interrupt ( IPL4SOFT ) , nomips16 ) )
InterrupcionTimer4 ( void ){
    IFS0bits.T4IF = 0;
    T_PARADO++;
    if(T_PARADO==5){
        PARADO=0;
        T_PARADO=0;
        T4CON=0x0070;
        if(EMERGENCIA==1){//tras esperar 5 segundos en caso de emergencia vamos al piso 0
            girar_derecha();
            while(((PORTA>>SENSOR_PISO0)&1)==1){}//bajamos al piso 0 en caso de emergencia
            parar_motor();
            EMERGENCIA=0;
            parar_bocina();
        }
    }
}

void leerSensores(void) {
    post_sensor0=(PORTA>>SENSOR_PISO0)&1;
    post_sensor1=(PORTA>>SENSOR_PISO1)&1;
    post_sensor2=(PORTA>>SENSOR_PISO2)&1;
    if ((post_sensor0!=prev_sensor0)&&(post_sensor0==0)&&(PISOS[0])){
        PISOS[0]=0;
        PISO_ACTUAL=0;
        if(PRIMERA_LLAMADA==0){
            PRIMERA_LLAMADA=-1;
        }
        parar_motor();
        estado = PARADO_ESTADO;
        T4CON=0x8070;
        PARADO=1;
    }
    if ((post_sensor1!=prev_sensor1)&&(post_sensor1==0)&&(PISOS[1])){
        PISOS[1]=0;
        PISO_ACTUAL=1;
        parar_motor();
        estado = PARADO_ESTADO;
        T4CON=0x8070;
        PARADO=1;
    }
    if ((post_sensor2!=prev_sensor2)&&(post_sensor2==0)&&(PISOS[2])){
        PISOS[2]=0;
        PISO_ACTUAL=2;
        if(PRIMERA_LLAMADA==2){
            PRIMERA_LLAMADA=-1;
        }
        parar_motor();
        estado = PARADO_ESTADO;
        T4CON=0x8070;
        PARADO=1;
    }
    prev_sensor0 = post_sensor0;
    prev_sensor1 = post_sensor1;
    prev_sensor2 = post_sensor2;
}

void leerBotones(void){
    post_piso0=(PORTC>>PIN_PISO0)&1;
    post_piso1=(PORTC>>PIN_PISO1)&1;
    post_piso2=(PORTC>>PIN_PISO2)&1;
    if((post_piso0!=prev_piso0)&&(post_piso0==0)){
        PISOS[0]=1;            
    }
    else if ((post_piso1!=prev_piso1)&&(post_piso1==0)){
        PISOS[1]=1;            
    }
    else if((post_piso2!=prev_piso2)&&(post_piso2==0)){
        PISOS[2]=1;            
    }
    prev_piso0=post_piso0;
    prev_piso1=post_piso1;  
    prev_piso2=post_piso2;
}

void actualizar_primera_llamada(void){
    if(PISOS[2]&&(!PISOS[0])){
        PRIMERA_LLAMADA=2;
    }
    else if(PISOS[0]&&(!PISOS[2])){
        PRIMERA_LLAMADA=0;
    }
}

void leerUART(void){
    char c=getcUART();//Practica6
    if(c!='\0'){ //comprobamos que se ha recibido algo
        if(c!='\r'){ //comando no completo
            comando[pos]=c;
            pos++;
            if(pos>10){
                pos = 0;
                putsUART("\n\rError\n");
            }
        }else{ //comando completo
            comando[pos]='\0'; //fin
            pos=0; //ponemos a 0 para el proximo
            char respuesta[20];
            if ((comando[0]!='0')&&(comando[0]!='1')&&(comando[0]!='2')&&(comando[0]!='e')){
                sprintf(respuesta,"\n\rPiso no existe\n");
                putsUART(respuesta);
                return;
            }else{
                if (comando[0]=='0'){
                    PISOS[0]=1;
                }
                else if (comando[0]=='1'){
                    PISOS[1]=1;
                }
                else if (comando[0]=='2'){
                    PISOS[2]=1;
                }
                else if(comando[0]=='e'){
                    putsUART("\n\rEmer\n");
                    parar_motor();
                    sonar();
                    T4CON=0x8070;
                    PARADO=1;
                    EMERGENCIA=1;
                    PISOS[0]=0;
                    PISOS[1]=0;
                    PISOS[2]=0;
                }
            }            
        }
        mensaje[0]=c;
        putsUART(mensaje);
    }
}

void main(void){
    ANSELC &=~((1<<PIN_PISO0)|(1<<PIN_PISO1)|(1<<PIN_PISO2));
    TRISC |=(1<<PIN_PISO0)|(1<<PIN_PISO1)|(1<<PIN_PISO2);
    ANSELA &=~((1<<SENSOR_PISO0)|(1<<SENSOR_PISO1)|(1<<SENSOR_PISO2));
    TRISA |=(1<<SENSOR_PISO0)|(1<<SENSOR_PISO1)|(1<<SENSOR_PISO2);
    inicializar_motor();
    inicializar_bocina();
    init_temp4();
    InicializarUART1(9600);
    cola_tx.cabeza = cola_tx.cola = 0;
    cola_rx.cabeza = cola_rx.cola = 0;

    INTCONbits.MVEC = 1;//multivector
    asm("ei");

    prev_piso0 = (PORTC >> PIN_PISO0) & 1;//inicio detectores de flanco
    prev_piso1 = (PORTC >> PIN_PISO1) & 1;
    prev_piso2 = (PORTC >> PIN_PISO2) & 1;
    prev_sensor0 = (PORTA >> SENSOR_PISO0) & 1;
    prev_sensor1 = (PORTA >> SENSOR_PISO1) & 1;
    prev_sensor2 = (PORTA >> SENSOR_PISO2) & 1;
    
    girar_derecha();
    while(((PORTA>>SENSOR_PISO0)&1)==1){}//bajamos al piso 0 en caso de emergencia
    parar_motor();

    while(1){
        leerBotones();
        leerUART();
        actualizar_primera_llamada();
        leerSensores();
        
        if(!PARADO){
            if((PISO_ACTUAL==0)){
                if(PISOS[1]||PISOS[2]){
                    estado=SUBIENDO;
                    girar_izquierda();
                }
            }
            else if((PISO_ACTUAL==1)){
                if (PISOS[0]||PISOS[2]){
                    if(PRIMERA_LLAMADA==0){
                        estado=BAJANDO;
                        girar_derecha();
                    }
                    else{
                        estado=SUBIENDO;
                        girar_izquierda();
                    }
                }
            }
            else if((PISO_ACTUAL==2)){
                if(PISOS[0]||PISOS[1]){
                    estado=BAJANDO;
                    girar_derecha();
                }
            }
        }
    }
}
