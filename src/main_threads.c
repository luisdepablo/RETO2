#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "fsm.h"

#include "sdkconfig.h"

fsm_t* fsm_new_alarmaBasica (void);

void tareaAlarmaBasica(void* ignore);
void entradas(void* ignore);
void boton_ISR();
void presencia_ISR();

void
app_main(void)
{
    printf("Empieza el codigo");
    xTaskCreate(tareaAlarmaBasica, "tareaAlarmaBasica", 4096, NULL, 2, NULL);
    xTaskCreate(entradas, "entradas", 4096, NULL, 1, NULL);

    
}

void entradas(void* ignore){
    

    TickType_t xFrequency = pdMS_TO_TICKS(200);
    TickType_t actualTicks = xTaskGetTickCount();

    while (1)
    {
        int num=rand()%3;

    switch (num)
    {
    case 0:
        break;

    case 1:
        boton_ISR();
        printf("boton pulsado");
        break;
    
    case 2:
        presencia_ISR();
        printf("presencia");
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

    TickType_t xFrequency = pdMS_TO_TICKS(50);
    TickType_t actualTicks = xTaskGetTickCount();

    while (1)
    {
        fsm_fire(alarmaBasica);
        //printf("fsm");
        vTaskDelayUntil(&actualTicks,xFrequency);
    }

    vTaskDelete(NULL);
}