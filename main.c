#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/adc.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/ip4_addr.h"

// Configuração do servidor (substitua pelo seu SSID real e senha)
const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";

// IP do servidor e porta (substitua pelo IP real do servidor)
#define SERVER_IP "255.255.255.255"
#define SERVER_PORT 5000

// Variáveis globais para o callback de conexão de rede
char request_buffer[256];
struct tcp_pcb* global_pcb = NULL;

// Configuração do joystick
#define JOYSTICK_VRX 27
#define JOYSTICK_VRY 26
#define JOYSTICK_MAX_VALUE 99
#define JOYSTICK_MIN_VALUE 0

// Configuração dos botões
#define BUTTON_A 5
#define BUTTON_B 6

// Estrutura para armazenar os dados do joystick
typedef struct {
    uint8_t x;
    uint8_t y;
} joystick_data_t;

// Estrutura para armazenar os dados dos dois botões
typedef struct {
    uint8_t button_a;
    uint8_t button_b;
} button_data_t;


uint8_t read_joystick_x(){
    adc_select_input(1);

    return adc_read() * 100 / 4095;
}

uint8_t read_joystick_y(){
    adc_select_input(0);

    return adc_read() * 100 / 4095;
}

float read_temperature(){
    adc_select_input(4); // Seleciona o canal do sensor de temperatura
    uint16_t raw_value = adc_read(); // Lê o valor bruto do ADC
    float voltage = (raw_value * 3.3) / 4095; // Converte para tensão (0-3.3V)
    float temperature = (voltage - 0.5) * 100; // Converte para temperatura em Celsius
    return temperature;
}

int wifi_init(){
    printf("[WIFI INIT]: Conectando ao wifi...\n");

    // Initialise the Wi-Fi chip
    if (cyw43_arch_init()) {
        printf("[WIFI INIT]: Falha ao conectar ao WiFi...\n");
        return -1;
    }

    // Enable wifi station
    cyw43_arch_enable_sta_mode();

    if (cyw43_arch_wifi_connect_timeout_ms(ssid, password, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("[WIFI INIT]: Falha ao conectar ao WiFi\n");
        return 1;
    } else {

        printf("[WIFI INIT]: Conectado ao WiFi!!!\n");
        // Read the ip address in a human readable way
        uint8_t *ip_address = (uint8_t*)&(cyw43_state.netif[0].ip_addr.addr);
        printf("Endereço IP: %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    }

    return 0;
}


// Callback após conexão estabelecida
err_t on_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
    if (err != ERR_OK) return err;

    printf("Conectado ao servidor!\n");

    tcp_write(tpcb, request_buffer, strlen(request_buffer), TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);
    tcp_close(tpcb); // Fecha a conexão após envio

    return ERR_OK;
}

// Envia requisição POST com dados do botão e temperatura
void send_http_post(joystick_data_t joystick, button_data_t buttons, float temp) {
    uint8_t joy_x = joystick.x;
    uint8_t joy_y = joystick.y;
    uint8_t btn_a = buttons.button_a;
    uint8_t btn_b = buttons.button_b;

    ip4_addr_t dest_ip;
    if (!ip4addr_aton(SERVER_IP, &dest_ip)) {
        printf("Erro no IP\n");
        return;
    }

    snprintf(request_buffer, sizeof(request_buffer),
        "POST /update?joy_x=%d&joy_y=%d&btn_a=%d&btn_b=%d&temp=%.2f HTTP/1.0\r\nHost: %s\r\n\r\n",
        joy_x, joy_y, btn_a, btn_b, temp, SERVER_IP);

    global_pcb = tcp_new_ip_type(IPADDR_TYPE_V4);
    if (!global_pcb) {
        printf("Erro criando pcb\n");
        return;
    }

    // Conecta ao servidor e define o callback de conexão
    err_t err = tcp_connect(global_pcb, &dest_ip, SERVER_PORT, on_connected);
    if (err != ERR_OK) {
        printf("Erro ao conectar: %d\n", err);
        tcp_abort(global_pcb);
    }
}

void setup(){
    stdio_init_all();
    wifi_init();

    // Inicializa joystick e sensor de temperatura
    adc_init();
    adc_gpio_init(JOYSTICK_VRX);
    adc_gpio_init(JOYSTICK_VRY);
    adc_set_temp_sensor_enabled(true); // Habilita o sensor de temperatura

    // Inicializa os pinos dos botões
    gpio_init(BUTTON_A);
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_A); // Habilita pull-up interno
    gpio_pull_up(BUTTON_B);
}

int main()
{
    setup();
    joystick_data_t joystick;
    button_data_t buttons;
    float temp; // Temperatura sensor interno

    while (true) {
        buttons.button_a = !gpio_get(0); // Lê o botão A
        buttons.button_b = !gpio_get(1); // Lê o botão B
        joystick.x = read_joystick_x();
        joystick.y = read_joystick_y();
        temp = read_temperature(); // Lê a temperatura do sensor interno

        printf("[MAIN]: Joy X: %d || Joy Y: %d || Btn A: %d ||  Btn B: %d || Temp: %.2f C\n", joystick.x, joystick.y, buttons.button_a, buttons.button_b, temp);
        send_http_post(joystick, buttons, temp);
        sleep_ms(500);
    }
}
