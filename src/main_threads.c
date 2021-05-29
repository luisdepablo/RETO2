#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "fsm.h"

#include "sdkconfig.h"

fsm_t* fsm_new_alarmaBasica (void);
fsm_t* fsm_new_codigo(void);
fsm_t* fsm_new_luces(void);

void tareaAlarmaBasica(void* ignore);
void tareaLuces(void* ignore);
void tareaCodigo(void* ignore);

void entradas(void* ignore);
void boton_ISR();
void presencia_ISR();
void presencia_luces_ISR();
void desactiva_presencia_luces_ISR();
void desactiva_presencia_alarma_ISR();

void
app_main(void)
{
    printf("Empieza el codigo\n");
    xTaskCreate(tareaAlarmaBasica, "tareaAlarmaBasica", 4096, NULL, 4, NULL);
    xTaskCreate(entradas, "entradas", 4096, NULL, 1, NULL);
    xTaskCreate(tareaLuces,"tareaLuces",4096,NULL,2,NULL);
    xTaskCreate(tareaCodigo,"tareaCodigo",4096,NULL,3,NULL);
    
}

void entradas(void* ignore){
    

    TickType_t xFrequency = pdMS_TO_TICKS(500);
    TickType_t actualTicks = xTaskGetTickCount();

    while (1)
    {
        int num=rand()%7;

    switch (num)
    {
    case 0:
        break;

    case 1:
        boton_ISR();
        printf("boton pulsado\n");
        break;
    
    case 2:
        presencia_ISR();
        presencia_luces_ISR();
        printf("presencia\n");
        break;
    case 3:
        desactiva_presencia_luces_ISR();
        desactiva_presencia_alarma_ISR();
        printf("no presencia\n");
    case 4:
        break;
    
    case 5:
        break;
    case 6:
        break;

    
    default:
        break;
    }
       
        vTaskDelayUntil(&actualTicks,xFrequency);
    }

    vTaskDelete(NULL);

}
void
tareaAlarmaBasica(void* ignore){
    fsm_t* alarmaBasica = malloc(sizeof(fsm_t));
    alarmaBasica = fsm_new_alarmaBasica();

    TickType_t xFrequency = pdMS_TO_TICKS(100);
    TickType_t actualTicks = xTaskGetTickCount();

    while (1)
    {
        fsm_fire(alarmaBasica);
        //printf("fsm");
        vTaskDelayUntil(&actualTicks,xFrequency);
    }

    vTaskDelete(NULL);
}

void
tareaLuces(void* ignore){
    fsm_t* luces = malloc(sizeof(fsm_t));
    luces = fsm_new_luces();

    TickType_t xFrequency = pdMS_TO_TICKS(150);
    TickType_t actualTicks = xTaskGetTickCount();

    while (1)
    {
        fsm_fire(luces);
        //printf("fsm");
        vTaskDelayUntil(&actualTicks,xFrequency);
    }

    vTaskDelete(NULL);
}
void
tareaCodigo(void* ignore){
    fsm_t* codigo = malloc(sizeof(fsm_t));
    codigo = fsm_new_codigo();

    TickType_t xFrequency = pdMS_TO_TICKS(100);
    TickType_t actualTicks = xTaskGetTickCount();

    while (1)
    {
        fsm_fire(codigo);
        //printf("fsm");
        vTaskDelayUntil(&actualTicks,xFrequency);
    }

    vTaskDelete(NULL);
}