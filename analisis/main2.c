#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "fsm.h"
#include "time.h"

#include "sdkconfig.h"

fsm_t* fsm_new_alarmaBasica (void);
fsm_t* fsm_new_luces(void);

void tareaAlarmaBasica(void* ignore);
void tareaLuces(void* ignore);
void entradas(void* ignore);
void boton_ISR();
void presencia_ISR();

int count=0;
int max_count=100;

uint32_t alarma_time=0;
uint32_t luces_time=0;

uint32_t max_alarma_time=0;
uint32_t max_luces_time=0;

uint32_t mean_alarma_time=0;
uint32_t mean_luces_time=0;

uint32_t t1;
uint32_t t2;
uint32_t exec_time;

void
app_main(void)
{
    printf("Empieza el codigo");
    xTaskCreate(tareaAlarmaBasica, "tareaAlarmaBasica", 4096, NULL, 2, NULL);
    //xTaskCreate(tareaLuces,"tareaLuces",NULL,2,NULL);
    xTaskCreate(entradas, "entradas", 4096, NULL, 1, NULL);

    
}

void entradas(void* ignore){
    

    TickType_t xFrequency = pdMS_TO_TICKS(100);
    TickType_t actualTicks = xTaskGetTickCount();

    while (count<max_count)
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

    while (count<max_count)
    {
        
        t1=(pdTICKS_TO_MS(xTaskGetTickCount()));
        fsm_fire(alarmaBasica);
        t2=(pdTICKS_TO_MS(xTaskGetTickCount()));
        exec_time=t2-t1;
        printf("exec_time= %d, t1= %d, t2= %d \n",exec_time,t1,t2);
        if(exec_time>max_alarma_time){
            max_alarma_time=exec_time;
        }
        alarma_time+=exec_time;
        //printf("fsm");
        vTaskDelayUntil(&actualTicks,xFrequency);
        printf("cuenta= %d",count);
        count++;
        
        
    }
    mean_alarma_time=alarma_time/max_count;
    printf("\r[%d] ALARM  EXEC TIME -> | MEAN: %dns | | MAX: %dns |\n",count,mean_alarma_time,max_alarma_time);

    vTaskDelete(NULL);
}

void
    tareaLuces(void* ignore){
        fsm_t* lucesFSM = malloc(sizeof(fsm_t));
        lucesFSM = fsm_new_luces();

        TickType_t xFrequency = pdMS_TO_TICKS(25);
        TickType_t actualTicks = xTaskGetTickCount();

        while (count<max_count)
        {
            t1=(pdTICKS_TO_MS(xTaskGetTickCount()));
            fsm_fire(lucesFSM);
            t2=(pdTICKS_TO_MS(xTaskGetTickCount()));
            exec_time=t2-t1;
            if(exec_time>max_luces_time){
            max_luces_time=exec_time;
            }
            alarma_time+=exec_time;
            vTaskDelayUntil(&actualTicks, xFrequency);
        }
        mean_alarma_time=alarma_time/max_count;
        printf("\r[%d] LUCES  EXEC TIME -> | MEAN: %dns | | MAX: %dns |\n",count,mean_luces_time,max_luces_time);

        vTaskDelete(NULL);
    }
