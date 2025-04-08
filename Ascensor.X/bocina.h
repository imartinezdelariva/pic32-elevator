#ifndef BOCINA_H
#define BOCINA_H

#include <xc.h>

// Definiciones de pines y parámetros
#define PIN_BOCINA 9
#define PERIODO 498 // 100 us

// Prototipos de funciones
void inicializar_bocina(void);
void sonar(void);
void parar_bocina(void); // <- Este nombre parece confuso, ¿quieres cambiarlo por parar_bocina?
                         // Si sí, te dejo también la versión corregida:


#endif // BOCINA_H
