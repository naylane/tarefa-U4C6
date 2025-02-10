#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/i2c.h"
#include "hardware/clocks.h"
#include "lib/WS2812.h"
#include "lib/ssd1306.h"
#include "lib/font.h"
#include "WS2812.pio.h"

#define UART_ID uart0
#define BAUD_RATE 9600

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ADDRESS 0x3C

#define WS2812_PIN          7
#define LED_GREEN           11
#define LED_BLUE            12
#define BTN_A               5
#define BTN_B               6
#define DEBOUNCE_TIME       250

static uint32_t last_interrupt_time_A = 0; // Tempo da última interrupção do botão A
static uint32_t last_interrupt_time_B = 0; // Tempo da última interrupção do botão B

ssd1306_t ssd; // Inicializa a estrutura do display


/**
 * @brief Atualiza o display com o estado dos LEDs.
 *
 * @param ssd Ponteiro para a estrutura do display.
 */
void led_status_display(ssd1306_t *ssd) {
    // Limpa o display
    ssd1306_fill(ssd, false);

    bool estado_verde = gpio_get(LED_GREEN); // Verifica o estado do LED verde
    char *msg_verde = estado_verde ? "Verde ON" : "Verde OFF";
    ssd1306_draw_string(ssd, msg_verde, 0, 0);

    bool estado_azul = gpio_get(LED_BLUE); // Verifica o estado do LED azul
    char *msg_azul = estado_azul ? "Azul ON" : "Azul OFF";
    ssd1306_draw_string(ssd, msg_azul, 0, 15);

    ssd1306_send_data(ssd); // Atualiza o display
}


/**
 * @brief Função responsável por atualizar a matriz de LEDs (WS2812) e o display com base na entrada via UART.
 */
void show_char(char c_uart, PIO pio, uint sm){
    if ((c_uart >= '0') && (c_uart <= '9')) {
        uint8_t num = c_uart - '0';
        set_led_matrix(num, pio, sm);
    } else {
        clear_matrix(pio, sm);
    }

    // Atualiza e limpa o buffer do display
    ssd1306_fill(&ssd, false);

    // Se houver caractere c_uart via UART
    if (c_uart != '\0') {
        ssd1306_fill(&ssd, 0);
        ssd1306_send_data(&ssd); // Envia os dados para o display

        char str[2] = {c_uart, '\0'};
        ssd1306_draw_string(&ssd, "Caractere ", 0, 30); // Desenha uma string
        ssd1306_draw_string(&ssd, str, 80, 30); // Linha 3 (y=16)
        ssd1306_send_data(&ssd);
        c_uart = '\0'; // Limpa
    }
}


/**
 * @brief ISR dos botões.
 *
 * @param gpio GPIO que causou interrupção.
 * @param events evento que causou interrupção.
 */
void btn_interrupt(uint gpio, uint32_t events) {
    uint32_t now = to_ms_since_boot(get_absolute_time());

    if (gpio == BTN_A) { // Botão A: alterna o LED Verde
        if ((now - last_interrupt_time_A) > DEBOUNCE_TIME) {
            gpio_put(LED_GREEN, !gpio_get(LED_GREEN)); 
            printf("LED verde %s\n", gpio_get(LED_GREEN) ? "Ligado" : "Desligado");
            last_interrupt_time_A = now;  // Atualiza timestamp
        }
    }
    else if (gpio == BTN_B) { // Botão B: alterna o LED Azul
        if ((now - last_interrupt_time_B) > DEBOUNCE_TIME) {
            gpio_put(LED_BLUE, !gpio_get(LED_BLUE));
            printf("LED azul %s\n", gpio_get(LED_BLUE) ? "Ligado" : "Desligado");
            last_interrupt_time_B = now; // Atualiza timestamp
        }
    }
}


/**
 * @brief Função principal.
 */
int main() {
    stdio_init_all();

    // Inicializa o STDIO para comunicação via USB (Serial Monitor)
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(0, GPIO_FUNC_UART); // UART0 TX
    gpio_set_function(1, GPIO_FUNC_UART); // UART0 RX
    uart_set_format(UART_ID, 8, 1, UART_PARITY_NONE);

    // Inicializa o PIO para controlar a matriz de LEDs (WS2812)
    PIO pio = pio0;
    uint sm = 0;
    uint offset = pio_add_program(pio, &pio_matrix_program);
    pio_matrix_program_init(pio, sm, offset, WS2812_PIN);

    // Aguarda a conexão USB
    sleep_ms(2000);  

    // Inicialização dos LEDs
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_put(LED_GREEN, 0);
    gpio_init(LED_BLUE);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_put(LED_BLUE, 0);

    // Inicialização dos botões
    gpio_init(BTN_A);
    gpio_set_dir(BTN_A, GPIO_IN);
    gpio_pull_up(BTN_A);
    gpio_init(BTN_B);
    gpio_set_dir(BTN_B, GPIO_IN);
    gpio_pull_up(BTN_B);

    // Inicialização do I2C (400 kHz) para o display (SSD1306)
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); 
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); 
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicialização e configuração do display
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ADDRESS, I2C_PORT); 
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);

    // Configura as interrupções para os botões (borda de descida)
    gpio_set_irq_enabled_with_callback(BTN_A, GPIO_IRQ_EDGE_FALL, true, &btn_interrupt);
    gpio_set_irq_enabled_with_callback(BTN_B, GPIO_IRQ_EDGE_FALL, true, &btn_interrupt);

    while (1) {
        // Lê da USB CDC
        int c_usb = getchar_timeout_us(0);
        if (c_usb != PICO_ERROR_TIMEOUT) {
            sleep_ms(500);
            show_char(c_usb, pio, sm);
            sleep_ms(500);
        }

        led_status_display(&ssd);

        // Verifica tem dados na UART
        if (uart_is_readable(uart0)) {
            char c_uart = uart_getc(uart0);
            sleep_ms(500);
            show_char(c_uart, pio, sm);
            sleep_ms(500);
        }

        sleep_ms(200);
    }
    return 0;
}
