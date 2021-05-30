#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "fsm.h"
#include "time.h"
#include <stdio.h>

#define ESPERA_CORTA 1000
#define ESPERA_LARGA 10000
#define STATE 1

static int codigo_usado[3]={3,3,3};
void codigo_ok_ISR(void* args);

typedef struct{
    int* pulsador;
} flags_t;



typedef struct{
    SemaphoreHandle_t mutexCodigoOK;
    SemaphoreHandle_t mutexPulsador; 
    flags_t flags;
    uint32_t esperaCorta;
    uint32_t esperaLarga;
    int codigo[3];
    int* codigo_ok;
    int count;
    int index;
} codigoFSM_t;

static codigoFSM_t codigoFSM;

static int check_pulsacion(fsm_t* this){
    codigoFSM_t* p_this= this->user_data;
    int res=0;
    if(xSemaphoreTake(p_this->mutexPulsador,0)){
        res=((*p_this->flags.pulsador)&&!(p_this->esperaCorta<=pdTICKS_TO_MS(xTaskGetTickCount()))&&!(p_this->esperaLarga<=pdTICKS_TO_MS(xTaskGetTickCount())));
        xSemaphoreGive(p_this->mutexPulsador);
    }
    return res;
}

static int check_esperaCorta(fsm_t* this)
{
    codigoFSM_t* p_this= this->user_data;
    int res;

    res=((p_this->esperaCorta<=pdTICKS_TO_MS(xTaskGetTickCount()))&&((p_this->index)<2));
    
    return res;
}

static int check_codigo_correcto(fsm_t* this){
    codigoFSM_t* p_this= this->user_data;
    int res;

    res=((p_this->esperaCorta<=pdTICKS_TO_MS(xTaskGetTickCount()))&&((p_this->index)==2)&&
    ((p_this->codigo[0]==codigo_usado[0])&&(p_this->codigo[1]==codigo_usado[1])&&(p_this->codigo[2]==codigo_usado[2])));

    return res;
}

static int check_codigo_no_correcto(fsm_t* this){
    codigoFSM_t* p_this= this->user_data;
    int res;

    res=((p_this->esperaCorta<=pdTICKS_TO_MS(xTaskGetTickCount()))&&((p_this->index)==2)&&
    !((p_this->codigo[0]==codigo_usado[0])&&(p_this->codigo[1]==codigo_usado[1])&&(p_this->codigo[2]==codigo_usado[2])));

    return res;
}

static int check_esperaLarga(fsm_t* this){
    codigoFSM_t* p_this= this->user_data;
    int res;

    res=((p_this->esperaLarga<=pdTICKS_TO_MS(xTaskGetTickCount()))&&!(p_this->esperaCorta<=pdTICKS_TO_MS(xTaskGetTickCount())));
    
    return res;
}

static void pulsacion(fsm_t* this){
    codigoFSM_t* p_this= this->user_data;
    static int aux;
    if(xSemaphoreTake(p_this->mutexPulsador,0)){
        p_this->flags.pulsador=0;
        xSemaphoreGive(p_this->mutexPulsador);
    }

    p_this->esperaCorta=((pdTICKS_TO_MS(xTaskGetTickCount()))+ESPERA_CORTA);
    p_this->esperaLarga=((pdTICKS_TO_MS(xTaskGetTickCount()))+ESPERA_LARGA);

    aux=(p_this->count)+1;
    aux=aux%10;

    (p_this->count)=10;
    p_this->codigo[p_this->index]=p_this->count;
}

static void incrementa_digito(fsm_t* this){
    codigoFSM_t* p_this=this->user_data;

    p_this->esperaCorta=((pdTICKS_TO_MS(xTaskGetTickCount()))+ESPERA_CORTA);
    p_this->esperaLarga=((pdTICKS_TO_MS(xTaskGetTickCount()))+ESPERA_LARGA);
    
    p_this->count=0;
    p_this->index++;
}

static void codigo_correcto(fsm_t* this){
    codigoFSM_t* p_this= this->user_data;

    p_this->esperaCorta=((pdTICKS_TO_MS(xTaskGetTickCount()))+ESPERA_CORTA);
    p_this->esperaLarga=((pdTICKS_TO_MS(xTaskGetTickCount()))+ESPERA_LARGA);
    
    p_this->count=0;
    p_this->index=0;
    p_this->codigo[0]=0;
    p_this->codigo[1]=0;
    p_this->codigo[2]=0;
    
    if(xSemaphoreTake(p_this->mutexCodigoOK,0)){
        *p_this->codigo_ok=1;
        xSemaphoreGive(p_this->mutexCodigoOK);
    }
    //codigo_ok_ISR((void*) p_this->codigo_ok);

    
}

static void codigo_no_correcto(fsm_t* this){
    codigoFSM_t* p_this= this->user_data;

    p_this->esperaCorta=((pdTICKS_TO_MS(xTaskGetTickCount()))+ESPERA_CORTA);
    p_this->esperaLarga=((pdTICKS_TO_MS(xTaskGetTickCount()))+ESPERA_LARGA);
    
    p_this->count=0;
    p_this->index=0;
    p_this->codigo[0]=0;
    p_this->codigo[1]=0;
    p_this->codigo[2]=0;

    p_this->codigo_ok=0;

    
}

static void reinicio(fsm_t* this){
    codigoFSM_t* p_this=this->user_data;

    p_this->esperaCorta=((pdTICKS_TO_MS(xTaskGetTickCount()))+ESPERA_CORTA);
    p_this->esperaLarga=((pdTICKS_TO_MS(xTaskGetTickCount()))+ESPERA_LARGA);
    p_this->count=0;
    p_this->index=0;
    p_this->codigo[0]=0;
    p_this->codigo[1]=0;
    p_this->codigo[2]=0;

    p_this->codigo_ok=0;


}

fsm_t* fsm_new_codigo(SemaphoreHandle_t mutexPulsador,SemaphoreHandle_t mutexCodigoOK,int* pulsador, int* codigo_ok){

    static fsm_trans_t codigo_tt[]={
        {STATE,check_pulsacion,STATE,pulsacion},
        {STATE,check_esperaCorta,STATE,incrementa_digito},
        {STATE,check_codigo_correcto,STATE,codigo_correcto},
        {STATE,check_codigo_no_correcto,STATE,codigo_no_correcto},
        {STATE,check_esperaLarga,STATE,reinicio},
        {-1,NULL,-1,NULL},
    };
    codigoFSM.esperaCorta=pdTICKS_TO_MS(xTaskGetTickCount());
    codigoFSM.esperaLarga=pdTICKS_TO_MS(xTaskGetTickCount());
    codigoFSM.flags.pulsador=pulsador;
    codigoFSM.codigo_ok=codigo_ok;
    codigoFSM.mutexCodigoOK=mutexCodigoOK;
    codigoFSM.mutexPulsador=mutexPulsador;

    return fsm_new(STATE,codigo_tt,&codigoFSM);

}