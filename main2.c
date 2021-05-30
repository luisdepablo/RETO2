#include "timer.h"
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
int max_count=10;

int alarma_time=0;
int luces_time=0;

int max_alarma_time=0;
int max_luces_time=0;

int mean_alarma_time=0;
int mean_luces_time=0;

struct timespec t1;
struct timespec t2;
struct timespec exec_time;

void
app_main(void)
{
    printf("Empieza el codigo\n");
    xTaskCreate(tareaAlarmaBasica, "tareaAlarmaBasica", 4096, NULL, 2, NULL);
    //xTaskCreate(entradas, "entradas", 4096, NULL, 1, NULL); 

    //xTaskCreate(tareaLuces,"tareaLuces",4096,NULL,2,NULL);

    
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
        //printf("boton pulsado");
        break;
    
    case 2:
        presencia_ISR();
        //printf("presencia");
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
    int t1s,t2s,exec_time_s;

    TickType_t xFrequency = pdMS_TO_TICKS(50);
    TickType_t actualTicks = xTaskGetTickCount();

    while (count<max_count)
    {
        clock_gettime(CLOCK_REALTIME,&t1);
        fsm_fire(alarmaBasica);
        clock_gettime(CLOCK_REALTIME,&t2);
        timespec_sub(&exec_time,&t2,&t1);
        t1s=(&t1)->tv_nsec;
        t2s=(&t2)->tv_nsec;
        exec_time_s=(&exec_time)->tv_nsec;
        printf("exec_time= %d, t1= %d, t2= %d \n",exec_time_s,t1s,t2s);
        if((&exec_time)->tv_nsec >max_alarma_time){
            max_alarma_time=(&exec_time)->tv_nsec;
        }
        alarma_time+=(&exec_time)->tv_nsec;
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
        int t1s,t2s,exec_time_s;

        TickType_t xFrequency = pdMS_TO_TICKS(25);
        TickType_t actualTicks = xTaskGetTickCount();

        while (count<max_count)
        {
            clock_gettime(CLOCK_REALTIME,&t1);
            fsm_fire(lucesFSM);
            clock_gettime(CLOCK_REALTIME,&t2);
            timespec_sub(&exec_time,&t2,&t1);
            t1s=(&t1)->tv_nsec;
            t2s=(&t2)->tv_nsec;
            exec_time_s=(&exec_time)->tv_nsec;
            printf("exec_time= %d, t1= %d, t2= %d \n",exec_time_s,t1s,t2s);
            if((&exec_time)->tv_nsec >max_luces_time){
            max_luces_time=(&exec_time)->tv_nsec;
            }
            luces_time+=(&exec_time)->tv_nsec;
            vTaskDelayUntil(&actualTicks, xFrequency);
            count++;
        }
        mean_luces_time=luces_time/max_count;
        printf("\r[%d] LUCES  EXEC TIME -> | MEAN: %dns | | MAX: %dns |\n",count,mean_luces_time,max_luces_time);

        vTaskDelete(NULL);
    } 
