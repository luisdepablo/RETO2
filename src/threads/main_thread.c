#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "fsm.h"
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"

#include "sdkconfig.h"

#define PRESENCIA 19
#define PULSADOR 18
#define LED 2



fsm_t* fsm_new_alarmaBasica (SemaphoreHandle_t mutexPresencia, SemaphoreHandle_t mutexCodigoOK, int* presencia, int* codigo_ok);
fsm_t* fsm_new_luces (SemaphoreHandle_t mutexPresencia, int* presencia);
fsm_t* fsm_new_codigo (SemaphoreHandle_t mutexPulsador, SemaphoreHandle_t mutexCodigoOK, int* pulsador, int* codigo_ok);



SemaphoreHandle_t mutexPresencia;
SemaphoreHandle_t mutexPulsador;
SemaphoreHandle_t mutexCodigoOK;

void tareaAlarma(void* ignore);
void tareaLuces(void* ignore);
void tareaCodigo(void* ignore);

int* pulsador;
int* codigo_ok;
int* presenciaAlarma;
int* presenciaLuces;


//void entradas(void* ignore);
/* void boton_ISR();
void presencia_ISR();
void presencia_luces_ISR();
void desactiva_presencia_luces_ISR();
void desactiva_presencia_alarma_ISR(); */


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

void
app_main(void)
{
    printf("Empieza el codigo\n");
    printf("Un tick son %d ms",pdTICKS_TO_MS(1));
    //xTaskCreate(tareaAlarma, "tareaAlarma", 4096, NULL, 4, NULL);
    //xTaskCreate(entradas, "entradas", 4096, NULL, 1, NULL);
    //xTaskCreate(tareaLuces,"tareaLuces",4096,NULL,2,NULL);
    //xTaskCreate(tareaCodigo,"tareaCodigo",4096,NULL,3,NULL);

    pulsador = malloc(sizeof(int));
    presenciaAlarma = malloc(sizeof(int));
    presenciaLuces = malloc(sizeof(int));
    codigo_ok = malloc(sizeof(int));

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
    
    int** presencias = malloc(2*sizeof(int*));
    presencias[0] = presenciaAlarma;
    presencias[1] = presenciaLuces;
    err = gpio_isr_handler_add(PRESENCIA, activa_presencia_ISR, (void*) presencias);
    if (err != 0)
        ESP_LOGE("app_main", "gpio_isr_handler_add: err %x", err);

    err = gpio_isr_handler_add(PULSADOR, pulsador_ISR, (void*) pulsador);
    if (err != 0)
        ESP_LOGE("app_main", "gpio_isr_handler_add: err %x", err);

    mutexPresencia = xSemaphoreCreateMutex();
    mutexPulsador = xSemaphoreCreateMutex();
    mutexCodigoOK = xSemaphoreCreateMutex();

    xTaskCreate(tareaAlarma, "tareaAlarma", 4096, NULL, configMAX_PRIORITIES-3, NULL);
    xTaskCreate(tareaLuces, "tareaLuces", 4096, NULL, configMAX_PRIORITIES-1, NULL);
    xTaskCreate(tareaCodigo, "tareaCodigo", 4096, NULL, configMAX_PRIORITIES-2, NULL);  

    
}


void
tareaAlarma(void* ignore){
    fsm_t* alarmaFSM = malloc(sizeof(fsm_t));
    alarmaFSM = fsm_new_alarmaBasica(mutexPresencia, mutexCodigoOK, presenciaAlarma, codigo_ok);

    TickType_t xFrequency = pdMS_TO_TICKS(100);
    TickType_t actualTicks = xTaskGetTickCount();

    while (1)
    {
        fsm_fire(alarmaFSM);
        //printf("fsm");
        vTaskDelayUntil(&actualTicks,xFrequency);
    }

    vTaskDelete(NULL);
}

void
tareaLuces(void* ignore){
    fsm_t* lucesFSM = malloc(sizeof(fsm_t));
    lucesFSM = fsm_new_luces(mutexPresencia,presenciaLuces);

    TickType_t xFrequency = pdMS_TO_TICKS(50);
    TickType_t actualTicks = xTaskGetTickCount();

    while (1)
    {
        fsm_fire(lucesFSM);
        //printf("fsm");
        vTaskDelayUntil(&actualTicks,xFrequency);
    }

    vTaskDelete(NULL);
}
void
tareaCodigo(void* ignore){
    fsm_t* codigoFSM = malloc(sizeof(fsm_t));
    codigoFSM = fsm_new_codigo(mutexPulsador, mutexCodigoOK, pulsador, codigo_ok);

    TickType_t xFrequency = pdMS_TO_TICKS(50);
    TickType_t actualTicks = xTaskGetTickCount();

    while (1)
    {
        fsm_fire(codigoFSM);
        //printf("fsm");
        vTaskDelayUntil(&actualTicks,xFrequency);
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