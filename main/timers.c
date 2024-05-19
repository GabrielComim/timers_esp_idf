#include "timers.h"

/* Handle do timer periódico */
gptimer_handle_t gptimer = NULL;            
/* Hnadle do timer one shot */
esp_timer_handle_t timer_one_shot;
/* Semáforo do timer periódico */
SemaphoreHandle_t smf_timer;
/* Variáveis para conferir o tempo temporizado */
uint64_t count_init, count_final = 0;
/* Flag para indicar o status do timer one shot */
int flag_status_one_shot;

/* ISR do timer*/
void timer_isr(void *arg)
{
    BaseType_t aux = false; 
    xSemaphoreGiveFromISR(smf_timer, &aux);
    #ifdef DEBUG
        count_final = esp_timer_get_time();
    #endif 
}

void task_gptimer(void *arg)
{
    while(true)
    {
        if(xSemaphoreTake(smf_timer, pdMS_TO_TICKS(portMAX_DELAY)) == pdTRUE)
        {
            flag_status_one_shot = TIMER_END;
            
            /* Atualiza a flag de status */
            #ifdef DEBUG
                uint64_t count_dif = count_final - count_init;
                // Após contabilizar mais de 10 segundos, para o timer periódico
                if(count_dif < 10000000)
                {
                    ESP_LOGI("GPTIMER", "COUNTING: %llu", count_dif);    
                }
                else
                {
                    ESP_LOGI("GPTIMER", "STOPED");
                    ESP_ERROR_CHECK(gptimer_stop(gptimer)); 
                    gptimer_disable(gptimer);
                    gptimer_del_timer(gptimer);
                    #ifdef DEBUG
                        ESP_LOGI("GPTIMER", "DELETE GPTIMER");
                    #endif                
                }
            #endif
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/* Configuração do timer */
void esp_gptimer_config(uint64_t timer_period_us, bool flag_reload)
{
    /* Inicializa o semáforo para a task */
    smf_timer = xSemaphoreCreateBinary();

    gptimer_config_t timer_cfg = 
    {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = RESOLUTION_TIMER, //1MHz = 1us
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_cfg, &gptimer));

    gptimer_alarm_config_t alarm_timer = 
    {
        .alarm_count = timer_period_us,         // período  = 1us 
        .reload_count = VALUE_INIT_TIMER,        // Regarrega o valor quando o timer reiniciar
        .flags.auto_reload_on_alarm = flag_reload,     // Habilita o reiniciar automático do timer
    };

    gptimer_event_callbacks_t cbs = 
    {
        .on_alarm = timer_isr,                  // informa a ISR
    };

    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_timer));
    ESP_ERROR_CHECK(gptimer_set_raw_count(gptimer, timer_period_us));           // Valor a ser alcançado
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));     // Registra a ISR da interrupção
    ESP_ERROR_CHECK(gptimer_enable(gptimer));                                   // Hailita o timer
    xTaskCreatePinnedToCore(task_gptimer, "Task_gptimer", 4096, NULL, 2, NULL, 1);
}

void start_timers()
{
    // Inicia os timers
    ESP_ERROR_CHECK(gptimer_start(gptimer));
    /* Flag indicando status do timer */
    flag_status_one_shot = TIMER_IN_PROGRESS;
    #ifdef DEBUG
        count_init = esp_timer_get_time();
    #endif
}

void wait_finished_one_shot()
{
    while(flag_status_one_shot == TIMER_IN_PROGRESS)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void timer_callback(void *arg)
{
    /* Atualiza a flag de status */
    flag_status_one_shot = TIMER_END;
    #ifdef DEBUG
        count_final = esp_timer_get_time();
        ESP_LOGI("TIMERS", "TIMER CALLBACK: %llu", (count_final - count_init));
    #endif
}

void timer_one_shot_init()
{
    esp_timer_create_args_t timer_args = 
    {
        .callback = &timer_callback,
        .name = "timer_one_shot",
    };
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &timer_one_shot));
}

void timer_one_shot_start(uint64_t timer_period_us)
{
    // Inicia o temporizador one shot. Configurado para 100 mili seg
    esp_timer_start_once(timer_one_shot, timer_period_us);  
    /* Flag indicando status do timer */
    flag_status_one_shot = TIMER_IN_PROGRESS;
    #ifdef DEBUG
        count_init = esp_timer_get_time();
    #endif
}

void gptimer_delete()
{
    gptimer_disable(gptimer);
    gptimer_del_timer(gptimer);
    #ifdef DEBUG
        ESP_LOGI("GPTIMER", "DELETE GPTIMER");
    #endif
}