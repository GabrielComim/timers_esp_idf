#ifndef TIMERS_H
#define TIMERS_H

#include <stdint.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include <freertos/task.h>
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "driver/gptimer.h"
#include "esp_timer.h"

#define RESOLUTION_TIMER    1000000
#define VALUE_INIT_TIMER    0
#define TIMER_COUNT         3000000

#define DEBUG

typedef enum
{
    TIMER_END = 0,
    TIMER_IN_PROGRESS,
}status_timer_t;

/**
 * @brief Configura o gptimer
 * 
 * @param timer_period_us   valor a ser alcançado pela temporização
 * @param flag_reload       se for true: timer periódico
 *                          se for false: timer one shot
 */
void esp_gptimer_config(uint64_t timer_period_us, bool flag_reload);

/**
 * @brief Inicia o gptimer após a configuração
 * 
 */
void start_timers();

/**
 * @brief Trava o firmware para aguardar o fim do timer one shot
 * 
 */
void wait_finished_one_shot();

/**
 * @brief Inicializa o timer one shot
 * 
 */
void timer_one_shot_init();

/**
 * @brief Start o timer one shot
 * 
 * @param timer_period_us valor a ser alcançado pela temporização
 */
void timer_one_shot_start(uint64_t timer_period_us);

/**
 * @brief Deleta o timer
 * 
 */
void gptimer_delete();

#endif      // TIMERS_H