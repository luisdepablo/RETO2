#include "fsm.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"




/*Estados*/
enum states{
    ENCENDIDA,
    APAGADA
};
typedef struct {
    int* presencia;
    int* botonAlarma;

} alarma_flags_t;

typedef struct {
    SemaphoreHandle_t mutexPresencia;
    SemaphoreHandle_t mutexCodigoOk;
    int alarma:1;
    alarma_flags_t flags;

} alarmaFSM_t;

static alarmaFSM_t alarmaFSM;


/*Comprobaciones*/
static int check_noBoton(fsm_t* this){
    alarmaFSM_t* p_this = this->user_data;
    int res=0;
    /* mutex on */
    if(xSemaphoreTake(p_this->mutexCodigoOk,0)){
    res= !*p_this->flags.botonAlarma;
    /* mutex off */
    xSemaphoreGive(p_this->mutexCodigoOk);
    
    printf("No se ha pulsado boton\n");
    }
    

    return res;

}

/* static int check_botonSinPresencia(fsm_t* this){
    alarmaFSM_t* p_this = this->user_data;
    
    return p_this->flags.botonAlarma;
   

} 
*/

static int check_boton(fsm_t* this){

    alarmaFSM_t* p_this = this->user_data;
    int res=0;
    /* mutex on */
    if(xSemaphoreTake(p_this->mutexCodigoOk,0)){
    res= *p_this->flags.botonAlarma;
    xSemaphoreGive(p_this->mutexCodigoOk);
    /* mutex off */
    printf("Se ha pulsado el boton\n");
    }
    return res;

}

static int check_presenciaSinBoton(fsm_t* this){
    alarmaFSM_t* p_this = this->user_data;
    int res=0;
    if(xSemaphoreTake(p_this->mutexCodigoOk,0)&&xSemaphoreTake(p_this->mutexPresencia,0)){
    /* mutex on */
    res= *p_this->flags.presencia && !*p_this->flags.botonAlarma;

    /* mutex off */
    xSemaphoreGive(p_this->mutexPresencia);
    xSemaphoreGive(p_this->mutexCodigoOk);
    }
    return res;
}

/*Acciones*/
static void noAlarma(fsm_t* this){
    alarmaFSM_t* p_this = this->user_data;

    p_this->alarma = 0;

    if(xSemaphoreTake(p_this->mutexCodigoOk,0)&&xSemaphoreTake(p_this->mutexPresencia,0)){
    /* Mutex on */
    *p_this->flags.botonAlarma = 0;
    *p_this->flags.presencia = 0;
    /* Mutex off */
    xSemaphoreGive(p_this->mutexPresencia);
    xSemaphoreGive(p_this->mutexCodigoOk);

    printf("La alarma se mantiene apagada\n");
    }
}

static void activarAlarma(fsm_t* this){
    alarmaFSM_t* p_this = this->user_data;

    p_this->alarma = 0;

    if(xSemaphoreTake(p_this->mutexCodigoOk,0)&&xSemaphoreTake(p_this->mutexPresencia,0)){
    /* Mutex on */
    *p_this->flags.botonAlarma = 0;
    *p_this->flags.presencia = 0;
    /* Mutex off */
    printf("Se activa la alarma\n");
    xSemaphoreGive(p_this->mutexPresencia);
    xSemaphoreGive(p_this->mutexCodigoOk);
    }

}

static void desactivarAlarma(fsm_t* this){
    alarmaFSM_t* p_this = this->user_data;

    p_this->alarma = 0;

    if(xSemaphoreTake(p_this->mutexCodigoOk,0)){
    /* Mutex on */
    *p_this->flags.botonAlarma = 0;
    /* Mutex off */
    xSemaphoreGive(p_this->mutexCodigoOk);
    printf("Se desactiva la alarma\n");
    }

}

static void mantieneAlarma(fsm_t* this){
    alarmaFSM_t* p_this = this->user_data;

    p_this->alarma = 1;
    
    if(xSemaphoreTake(p_this->mutexPresencia,0)){
    /* Mutex on */
    *p_this->flags.presencia = 0;
    /* Mutex off */
     printf("La alarma está activada y sonando\n");
    xSemaphoreGive(p_this->mutexPresencia);
    }
}

/*FSM*/

fsm_t* fsm_new_alarmaBasica (SemaphoreHandle_t mutexPresencia, SemaphoreHandle_t mutexCodigoOK, int* presencia, int* codigo_ok)
{
	static fsm_trans_t alarma_tt[] = {
        { APAGADA, check_noBoton, APAGADA, noAlarma},
        { APAGADA, check_boton, ENCENDIDA, activarAlarma},
        { ENCENDIDA, check_boton, APAGADA, desactivarAlarma},
        { ENCENDIDA, check_presenciaSinBoton, ENCENDIDA, mantieneAlarma},
		{ -1, NULL, -1, NULL},
	};
    alarmaFSM.mutexCodigoOk=mutexCodigoOK;
    alarmaFSM.mutexPresencia=mutexPresencia;
    alarmaFSM.flags.botonAlarma=codigo_ok;
    alarmaFSM.flags.presencia=presencia;

	return fsm_new (APAGADA, alarma_tt, &alarmaFSM);
}

