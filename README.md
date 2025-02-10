
<h2 align="center">  Tarefa 1 - U4, C6 </h3>
<h5 align="center"> Atividade promovida pelo programa EmbarcaTech. </h5>

### Sobre o projeto
Tarefa com objetivo de aplicar conhecimentos sobre Interfaces de Comunicação Serial com RP2040 (UART, SPI e I2C).

### Funcionalidades
- Modificação da Biblioteca font.h:
  - Inclusão de caracteres.
- Entrada de caracteres via PC:
  - É possível enviar caracteres pelo Serial Monitor do VS Code
- Interação com o Botão A:
  - Ao pressionar o botão A, o LED verde ligado/desligado e o display atualizado.
- Interação com o Botão B:
  - Ao pressionar o botão B, o LED azul ligado/desligado e o display atualizado.

### Instruções de uso
- É necessário instalar o SDK da Raspberry Pi Pico e realizar as configurações iniciais. [1]

Para ter acesso ao projeto, clone o repositório disponível na [plataforma GitHub](https://github.com/naylane/tarefa-U4C6).

Após clonar o repositório, no terminal, navegue até a pasta do projeto e execute os comandos:
```bash
mkdir build && cd build
cmake ..
make
```
Para executar a atividade na Raspberry Pi Pico, conecte a BitDogLab no computador enquanto pressiona o botão `BOOTSEL` e rode o código pelo VS Code.

⚠️ **Observação:** também é possível simular a atividade pelo Wokwi no Visual Studio Code. Basta instalar a extensão e executar o arquivo 'diagram.json'.

Ou, se preferir, [clique aqui para ver a simulação no Wokwi Web](https://wokwi.com/projects/421991045218041857).


### Tecnologias e Ferramentas utilizadas
- **Kit BitDogLab:**
  - Raspberry Pi Pico W
  - LEDs WS2812
  - Display SSD1306
  - Botões A e B
  - LED RGB
- **Linguagem de programação:** C
- **Ambiente de Desenvolvimento:** Visual Studio Code

### Referências
- [1] Aula sincrona do professor [Wilton Lacerda Silva](https://github.com/wiltonlacerda).
- [2] Documentação do Raspberry Pi Pico W. Disponível em [www.raspberrypi.com](https://www.raspberrypi.com/documentation/microcontrollers/raspberry-pi-pico.html)
