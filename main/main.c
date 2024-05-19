#include <stdio.h>
#include "timers.h"

void app_main(void)
{
    ESP_LOGI("MAIN", "-----------CONFIGURANDO TIMERS-----------");
    
    ESP_LOGI("MAIN", "TIMER ONE SHOT");
    timer_one_shot_init();
    timer_one_shot_start(TIMER_COUNT);
    wait_finished_one_shot();
    
    ESP_LOGI("MAIN", "TIMER ONE SHOT RESTART");
    timer_one_shot_start(TIMER_COUNT);
    wait_finished_one_shot();

    ESP_LOGI("MAIN", "TIMER PERIODIC WITH GPTIMER");
    esp_gptimer_config(TIMER_COUNT, true);
    start_timers();

    vTaskDelete(NULL);
}
