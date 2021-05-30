#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "fsm.h"
#include <string.h>
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_err.h"

#include "sdkconfig.h"

#define PRESENCIA 19
#define PULSADOR 18
#define LED 2

fsm_t* fsm_new_alarmaBasica (SemaphoreHandle_t mutexPresencia, SemaphoreHandle_t mutexCodigoOK, int* presencia, int* codigo_ok);
fsm_t* fsm_new_codigo(SemaphoreHandle_t mutexPulsador,SemaphoreHandle_t mutexCodigoOK,int* pulsador, int* codigo_ok);
fsm_t* fsm_new_luces(SemaphoreHandle_t mutexPresencia, int* presencia);

void tareaAlarmaBasica(void* ignore);
void tareaLuces(void* ignore);
void tareaCodigo(void* ignore);

SemaphoreHandle_t mutexPresencia;
SemaphoreHandle_t mutexPulsador;
SemaphoreHandle_t mutexCodigoOK;

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







void tarea_ec(void* ignore);




void
app_main(void)
{
    printf("Empieza el codigo\n");
    xTaskCreate(tarea_ec, "tarea_ec", 4096, NULL, 1, NULL);
}


void tarea_ec(void* ignore){

    int* pulsador = malloc(sizeof(int));
    int* presenciaAlarma = malloc(sizeof(int));
    int* presenciaLuces = malloc(sizeof(int));
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

        gpio_pad_select_gpio(LED);
        gpio_set_direction(LED, GPIO_MODE_OUTPUT);



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

    //Iniciar mutex
    mutexCodigoOK=xSemaphoreCreateMutex();
    mutexPresencia=xSemaphoreCreateMutex();
    mutexPulsador=xSemaphoreCreateMutex();
    //iniciar fsm
    fsm_t* alarmaFSM=malloc(sizeof(fsm_t));
    fsm_t* lucesFSM=malloc(sizeof(fsm_t));
    fsm_t* codigoFSM=malloc(sizeof(fsm_t)); 

    alarmaFSM= fsm_new_alarmaBasica(mutexPresencia,mutexCodigoOK,presenciaAlarma,codigo_ok);
    lucesFSM= fsm_new_luces(mutexPresencia,presenciaLuces);
    codigoFSM= fsm_new_codigo(mutexPulsador,mutexCodigoOK,pulsador,codigo_ok);

    /*EJECUTIVO CICLICO     P
    CODIGO_FSM  ->T=50MS    1
    LUCES_FSM   ->T=50MS    3
    ALARMA_FSM  ->T=50MS    2


    */
   TickType_t H=pdMS_TO_TICKS(50);
   TickType_t actualTicks;
    while(1){
        fsm_fire(lucesFSM);
        fsm_fire(alarmaFSM);
        fsm_fire(codigoFSM);
        printf("codigo_ok= %d\n",*codigo_ok);
        vTaskDelayUntil(&actualTicks,H);


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
    
        break;
    
    case 2:
        
       // activa_presencia_ISR((void*) presencia);
        printf("presencia\n");
        break;
    case 3:
        break;
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
