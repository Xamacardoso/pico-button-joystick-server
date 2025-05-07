# 🛜 BitDogLab - HTTP Client com Servidor Flask

Este projeto demonstra a comunicação entre um **BitDogLab (Raspberry Pi Pico W)**, programado em **C bare metal**, e um servidor **Flask** rodando localmente em um computador. O microcontrolador coleta dados de entrada (botões A e B, joystick analógico e sensor de temperatura interno) e envia esses dados periodicamente para o servidor via **requisições HTTP**.

## 📌 Descrição

* O **Raspberry Pi Pico W** atua como um cliente HTTP.

* Ele envia requisições `POST` para a rota `/update` de um servidor Flask local.

* As informações enviadas incluem:

  * Estado de dois botões (A e B)
  * Direção (X/Y) do joystick analógico
  * Temperatura medida pelo sensor interno do Raspberry (Utilizando conversor ADC)

* O **servidor Flask**:

  * Recebe e armazena os dados enviados pelo Raspberry na rota `/update`
  * Exibe os dados recebidos em tempo real por meio de uma interface HTML simples na rota raiz `/`

## 🧱 Estrutura do Projeto

```plaintext
.
├── flask_server/
│   ├── app.py            # Servidor Flask com rotas /update e /
│   └── templates/
│       └── index.html    # Interface web para visualizar os dados
│
├── main.c     # Código C bare-metal do Raspberry Pi Pico W
├── README.md
```

## ⚙️ Requisitos

### Raspberry Pi Pico W

* SDK C/C++ do Pico instalado
* Configuração bare-metal (sem FreeRTOS ou MicroPython)
* Wi-Fi configurado para acesso local

### PC com Servidor Flask

* Python 3.8+ (Verifique a instalação e o PATH)
* Flask (`pip install flask`)

## 🚀 Como Executar

### 1. Compilar e Programar o Pico

Compile o código C para o Pico usando o CMake, com a biblioteca `lwIP`, que fornece suporte HTTP client via Wi-Fi.

### 2. Rodar o servidor Flask no PC

```bash
cd flask_server
python app.py
```

Por padrão, o servidor escuta em `http://localhost:5000`.

### 3. Iniciar o Pico

Assim que o programa do Pico iniciar, ele começará a enviar requisições `POST` para `http://<IP_DO_PC>:5000/update`.

### ⚠️ Nota Importante

Certifique-se de substituir `WIFI_SSID` e `WIFI_PASSWORD` no código do Pico pelo SSID e senha reais da sua rede Wi-Fi antes de compilar e carregar o programa. Além disso, atualize o valor de `SERVER_IP` no código para o endereço IP do computador onde o servidor Flask está rodando. Isso é essencial para garantir que o dispositivo consiga se conectar corretamente ao servidor.

### 4. Visualizar os dados

Abra `http://localhost:5000` no navegador para acompanhar os dados em tempo real.

## 🛠️ Tecnologias Utilizadas

* **Raspberry Pi Pico W (BitDogLab)**
* **C (Bare Metal)**
* **HTTP (Cliente)**
* **Flask (Servidor HTTP e HTML)**
* **HTML (Interface Web)**


## 📄 Licença

Este projeto está licenciado sob a [MIT License](LICENSE).

---