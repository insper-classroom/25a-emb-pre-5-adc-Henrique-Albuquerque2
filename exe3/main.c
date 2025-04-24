#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "data.h"
QueueHandle_t xQueueData;

// não mexer! Alimenta a fila com os dados do sinal
void data_task(void *p) {
    vTaskDelay(pdMS_TO_TICKS(400));

    int data_len = sizeof(sine_wave_four_cycles) / sizeof(sine_wave_four_cycles[0]);
    for (int i = 0; i < data_len; i++) {
        xQueueSend(xQueueData, &sine_wave_four_cycles[i], 1000000);
    }

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void process_task(void *p) {
    int data = 0;

    // janela de 5 valores
    int window[5] = {0};
    int index = 0;       // posição atual de escrita no vetor
    int count = 0;       // número de amostras já recebidas
    int sum = 0;         // soma acumulada

    while (true) {
        if (xQueueReceive(xQueueData, &data, 100)) {
            // implementar filtro aqui!
            // remove valor antigo da soma
            sum -= window[index];

            // adiciona novo valor
            window[index] = data;
            sum += data;

            // avança circularmente no vetor
            index = (index + 1) % 5;
        
            // conta inicial (antes de preencher as 5 posições)
            if (count < 5) count++;

            // calcula a média
            int media = sum / count;

            printf("%d\n", media);

            // manter o delay
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}


int main() {
    stdio_init_all();

    xQueueData = xQueueCreate(64, sizeof(int));

    xTaskCreate(data_task, "Data task ", 4096, NULL, 1, NULL);
    xTaskCreate(process_task, "Process task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
