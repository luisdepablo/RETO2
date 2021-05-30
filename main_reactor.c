#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "fsm.h"
#include "freertos/semphr.h"
#include "reactor.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include <string.h>

#include "sdkconfig.h"

int count_alarm=0;
int max_count_alarm=10;

int count_luces=0;
int max_count_luces=10;

int count_codigo=0;
int max_count_codigo=10;

int t1,t2,exec_time;

int alarma_time=0;
int luces_time=0;
int codigo_time=0;

int max_alarma_time=0;
int max_luces_time=0;
int max_codigo_time=0;

int mean_alarma_time=0;
int mean_luces_time=0;
int mean_codigo_time=0;

#define PRESENCIA 19
#define PULSADOR 18

fsm_t* fsm_new_alarmaBasica (SemaphoreHandle_t mutexPresencia, SemaphoreHandle_t mutexCodigoOK, int* presencia, int* codigo_ok);
fsm_t* fsm_new_luces (SemaphoreHandle_t mutexPresencia, int* presencia);
fsm_t* fsm_new_codigo (SemaphoreHandle_t mutexPulsador, SemaphoreHandle_t mutexCodigoOK, int* pulsador, int* codigo_ok);

static fsm_t* alarmaFSM;
static fsm_t* lucesFSM;
static fsm_t* codigoFSM;

SemaphoreHandle_t mutexPresencia;
SemaphoreHandle_t mutexPulsador;
SemaphoreHandle_t mutexCodigoOK;

//void tareaAlarmaBasica(void* ignore);
//void tareaLuces(void* ignore);
//void tareaCodigo(void* ignore);
void tarea_reactor(void* ignore);

static void IRAM_ATTR activa_presencia_ISR(void* args){
    int** presencias = (int**) args;

    if(xSemaphoreTake(mutexPresencia, 0)){
        *presencias[0] = 1;
        *presencias[1] = 1;
        xSemaphoreGive(mutexPresencia);
    }

}

static void IRAM_ATTR pulsador_ISR(void* args){
    int* pulsador = (int*) args;

    if(xSemaphoreTake(mutexPresencia, 0)){
        *pulsador = 1;
        xSemaphoreGive(mutexPresencia);
    }
}

static void tareaAlarma(EventHandler* this){
    
    
    while (count_alarm<max_count_alarm)
    {
        t1=pdTICKS_TO_MS(xTaskGetTickCount());
        fsm_fire(alarmaFSM);
        t2=pdTICKS_TO_MS(xTaskGetTickCount());
        exec_time=t2-t1;
        if(exec_time>max_alarma_time){
            max_alarma_time=exec_time;
        }
        printf("exec_time= %d, t1= %d, t2= %d \n",exec_time,t1,t2);
        alarma_time+=exec_time;
        //printf("fsm");
        count_alarm++;
        this->next_activation += pdMS_TO_TICKS(50);

    }
    mean_alarma_time=alarma_time/max_count_alarm;
    if(count_alarm==max_count_alarm&&count_luces==max_count_luces&&count_codigo==max_count_codigo){
    printf("\r[%d] LUCES  EXEC TIME -> | MEAN: %dns | | MAX: %dns |\n",count_luces,mean_luces_time,max_luces_time);
    printf("\r[%d] ALARM  EXEC TIME -> | MEAN: %dns | | MAX: %dns |\n",count_alarm,mean_alarma_time,max_alarma_time);

    printf("\r[%d] CODIGO  EXEC TIME -> | MEAN: %dns | | MAX: %dns |\n",count_codigo,mean_codigo_time,max_codigo_time);
    }
}

static void tareaLuces(EventHandler* this){
   
    

    while (count_luces<max_count_luces)
    {
        t1=pdTICKS_TO_MS(xTaskGetTickCount());
        fsm_fire(lucesFSM);
        t2=pdTICKS_TO_MS(xTaskGetTickCount());
        exec_time=t2-t1;
        if(exec_time>max_luces_time){
            max_alarma_time=exec_time;
        }
        luces_time+=exec_time;
        //printf("fsm");
        count_luces++;
        this->next_activation += pdMS_TO_TICKS(50);
    }
    mean_luces_time=luces_time/max_count_luces;
    if(count_alarm==max_count_alarm&&count_luces==max_count_luces&&count_codigo==max_count_codigo){
        
    printf("\r[%d] LUCES  EXEC TIME -> | MEAN: %dns | | MAX: %dns |\n",count_luces,mean_luces_time,max_luces_time);
    printf("\r[%d] ALARM  EXEC TIME -> | MEAN: %dns | | MAX: %dns |\n",count_alarm,mean_alarma_time,max_alarma_time);
    printf("\r[%d] CODIGO  EXEC TIME -> | MEAN: %dns | | MAX: %dns |\n",count_codigo,mean_codigo_time,max_codigo_time);
    }
}

static void tareaCodigo(EventHandler* this){

    while (count_codigo<max_count_codigo)
    {
        t1=pdTICKS_TO_MS(xTaskGetTickCount());
        fsm_fire(codigoFSM);
        t2=pdTICKS_TO_MS(xTaskGetTickCount());
        exec_time=t2-t1;
        //printf("fsm");
        if(exec_time<max_codigo_time){
            max_codigo_time=exec_time;

        }
        codigo_time+=exec_time;
        count_codigo++;
        this->next_activation += pdMS_TO_TICKS(50);
    }
    mean_codigo_time=codigo_time/max_count_codigo;
    if(count_alarm==max_count_alarm&&count_luces==max_count_luces&&count_codigo==max_count_codigo){
    printf("\r[%d] LUCES  EXEC TIME -> | MEAN: %dns | | MAX: %dns |\n",count_luces,mean_luces_time,max_luces_time);
    printf("\r[%d] ALARM  EXEC TIME -> | MEAN: %dns | | MAX: %dns |\n",count_alarm,mean_alarma_time,max_alarma_time);
    printf("\r[%d] CODIGO  EXEC TIME -> | MEAN: %dns | | MAX: %dns |\n",count_codigo,mean_codigo_time,max_codigo_time);
    }
    
}

void entradas(void* ignore);
void boton_ISR();
void presencia_ISR();
void presencia_luces_ISR();
void desactiva_presencia_luces_ISR();
void desactiva_presencia_alarma_ISR();

void
a_main(void)
{
    printf("Empieza el codigo\n");
    printf("Un tick son %d ms",pdTICKS_TO_MS(1));
    //xTaskCreate(tareaAlarma, "tareaAlarma", 4096, NULL, 4, NULL);
    //xTaskCreate(entradas, "entradas", 4096, NULL, 1, NULL);
    //xTaskCreate(tareaLuces,"tareaLuces",4096,NULL,2,NULL);
    //xTaskCreate(tareaCodigo,"tareaCodigo",4096,NULL,3,NULL);

    xTaskCreate(tarea_reactor, "tarea_reactor", 4096, NULL, configMAX_PRIORITIES-1, NULL);
    
}



void tarea_reactor(void* ignore){

    int* pulsador = malloc(sizeof(int));
    int* presenciaAlarma=malloc(sizeof(int));
    int* presenciaLuces=malloc(sizeof(int));
    int* codigo_ok = malloc(sizeof(int));

    memset(pulsador, 0, sizeof(int));
    memset(presenciaAlarma, 0, sizeof(int));
    memset(presenciaLuces, 0, sizeof(int));
    memset(codigo_ok, 0, sizeof(int)); 

    /* Init GPIO */
        gpio_config_t io_conf;

        io_conf.intr_type = GPIO_PIN_INTR_POSEDGE;
        io_conf.pin_bit_mask = ((1ULL<<PRESENCIA)|(1ULL<<PULSADOR));
        io_conf.mode = GPIO_MODE_INPUT;
        io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
        io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        gpio_config(&io_conf);

        /* Init interrupt for given signum */
        esp_err_t err;

        err = gpio_intr_enable(PRESENCIA);
        if(err != 0)
            ESP_LOGE("app_main", "gpio_intr_enable: err %d", err);

        err = gpio_intr_enable(PULSADOR);
        if(err != 0)
            ESP_LOGE("app_main", "gpio_intr_enable: err %d", err);        
        
        err = gpio_install_isr_service(0);
        if(err != 0)
            ESP_LOGE("app_main", "gpio_install_isr_service: err %x", err);
        
        int* presencias[2] = {presenciaAlarma, presenciaLuces};
        err = gpio_isr_handler_add(PRESENCIA, activa_presencia_ISR, (void*) presencias);
        if (err != 0)
            ESP_LOGE("app_main", "gpio_isr_handler_add: err %x", err);

        err = gpio_isr_handler_add(PULSADOR, pulsador_ISR, (void*) pulsador);
        if (err != 0)
            ESP_LOGE("app_main", "gpio_isr_handler_add: err %x", err);

    // Inicia mutex
    mutexPresencia = xSemaphoreCreateMutex();
    mutexPulsador = xSemaphoreCreateMutex();
    mutexCodigoOK = xSemaphoreCreateMutex();


    EventHandler alarmaBasica;
    EventHandler luces;
    EventHandler codigo;
    reactor_init();

    alarmaFSM = fsm_new_alarmaBasica(mutexPresencia, mutexCodigoOK, presenciaAlarma, codigo_ok);
    lucesFSM = fsm_new_luces(mutexPresencia, presenciaLuces);
    codigoFSM = fsm_new_codigo(mutexPulsador, mutexCodigoOK, pulsador, codigo_ok);

    event_handler_init(&alarmaBasica, 2, tareaAlarma);
    event_handler_init(&luces, 3, tareaLuces);
    event_handler_init(&codigo, 1, tareaCodigo);

    reactor_add_handler(&alarmaBasica);
    reactor_add_handler(&luces);
    reactor_add_handler(&codigo);

    while(1){
        reactor_handle_events();
        vTaskDelay(1);
        if(count_alarm==max_count_alarm&&count_luces==max_count_luces&&count_codigo==max_count_codigo){
            vTaskDelete(NULL);
        }
    }

    vTaskDelete(NULL);
}

/* void entradas(void* ignore){
    

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

} */