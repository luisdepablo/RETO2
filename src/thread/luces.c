#include "fsm.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"

#define LED 2
#define TIME 1000 //1 sec --> 1000

enum states{
    ON,
    OFF
};

typedef struct{
    int presencia : 1;
} luces_flags_t;

typedef struct
{
    luces_flags_t flag;
    uint32_t T;
    int luz :1;
} luzFSM_t;

luzFSM_t luzFSM;

static int check_presencia(fsm_t* this){
    luzFSM_t* p_this= this->user_data;
    int res;

    res=p_this->flag.presencia;

    return res;
}

static int check_T_no_presencia(fsm_t* this){
    luzFSM_t* p_this= this->user_data;
    int res;

    res=((p_this->T<=pdTICKS_TO_MS(xTaskGetTickCount()))&&(!p_this->flag.presencia));

    return res;

}

static void encender(fsm_t* this){
    luzFSM_t* p_this=this->user_data;

    

    p_this->flag.presencia=0;
    p_this->luz=1;
    gpio_set_level(LED,1);
    p_this->T=(pdTICKS_TO_MS(xTaskGetTickCount())+TIME);

}

static void apagar(fsm_t* this){
    luzFSM_t* p_this=this->user_data;

    gpio_set_level(LED,0);
    p_this->luz=0;
}

static void mantener(fsm_t* this){
    luzFSM_t* p_this = this->user_data;

    

    p_this->flag.presencia=0;
    p_this->luz=1;
    gpio_set_level(LED,1);
    p_this->T=(pdTICKS_TO_MS(xTaskGetTickCount()))+TIME;
}

fsm_t* fsm_new_luces(void){
    gpio_set_direction(LED,GPIO_MODE_OUTPUT);


    static fsm_trans_t luces_tt[]={
        {OFF,check_presencia,ON,encender},
        {ON,check_T_no_presencia,OFF,apagar},
        {ON,check_presencia,ON,mantener},
        {-1,NULL,-1,NULL},
    };
    return fsm_new(OFF,luces_tt,&luzFSM);
} 

void presencia_luces_ISR(){
    luzFSM.flag.presencia=1;
}

void desactiva_presencia_luces_ISR(){
    luzFSM.flag.presencia=0;
}