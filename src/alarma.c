#include "fsm.h"
#include <stdio.h>



/*Estados*/
enum states{
    ENCENDIDA,
    APAGADA
};
typedef struct {
    int presencia : 1;
    int botonAlarma :1;

} EntradasAlarma_t;

typedef struct {
    int alarma:1;
    EntradasAlarma_t entradas;

} alarmaFSM_t;

static alarmaFSM_t alarmaFSM;


/*Comprobaciones*/
static int check_noBoton(fsm_t* this){
    alarmaFSM_t* p_this = this->user_data;
    /* mutex on */
    return !p_this->entradas.botonAlarma;
    /* mutex off */
    printf("No se ha pulsado boton");

}

/* static int check_botonSinPresencia(fsm_t* this){
    alarmaFSM_t* p_this = this->user_data;
    
    return p_this->entradas.botonAlarma;
   

} 
*/

static int check_boton(fsm_t* this){

    alarmaFSM_t* p_this = this->user_data;
    /* mutex on */
    return p_this->entradas.botonAlarma;
    /* mutex off */
    printf("Se ha pulsado el boton");

}

static int check_presenciaSinBoton(fsm_t* this){
    alarmaFSM_t* p_this = this->user_data;
    /* mutex on */
    return p_this->entradas.presencia && !p_this->entradas.botonAlarma;
    /* mutex off */

}

/*Acciones*/
static void noAlarma(fsm_t* this){
    alarmaFSM_t* p_this = this->user_data;

    p_this->alarma = 0;

    /* Mutex on */
    p_this->entradas.botonAlarma = 0;
    p_this->entradas.presencia = 0;
    /* Mutex off */
  //  printf("La alarma se mantiene apagada");
}

static void activarAlarma(fsm_t* this){
    alarmaFSM_t* p_this = this->user_data;

    p_this->alarma = 0;

    /* Mutex on */
    p_this->entradas.botonAlarma = 0;
    p_this->entradas.presencia = 0;
    /* Mutex off */
    printf("Se activa la alarma");

}

static void desactivarAlarma(fsm_t* this){
    alarmaFSM_t* p_this = this->user_data;

    p_this->alarma = 0;

    /* Mutex on */
    p_this->entradas.botonAlarma = 0;
    /* Mutex off */
    printf("Se desactiva la alarma");

}

static void mantieneAlarma(fsm_t* this){
    alarmaFSM_t* p_this = this->user_data;

    p_this->alarma = 1;

    /* Mutex on */
    p_this->entradas.presencia = 0;
    /* Mutex off */
     printf("La alarma está activada y sonando");

}

/*FSM*/

fsm_t* fsm_new_alarmaBasica (void)
{
	static fsm_trans_t alarma_tt[] = {
        { APAGADA, check_noBoton, APAGADA, noAlarma},
        { APAGADA, check_boton, ENCENDIDA, activarAlarma},
        { ENCENDIDA, check_boton, APAGADA, desactivarAlarma},
        { ENCENDIDA, check_presenciaSinBoton, ENCENDIDA, mantieneAlarma},
		{ -1, NULL, -1, NULL},
	};

	return fsm_new (APAGADA, alarma_tt, &alarmaFSM);
}

void boton_ISR(){
    alarmaFSM.entradas.botonAlarma=1;
}

void presencia_ISR(){
    alarmaFSM.entradas.presencia=1;
}