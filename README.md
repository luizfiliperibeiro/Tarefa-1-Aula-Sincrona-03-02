# 🚀 Projeto: Controle de LEDs e Display OLED com Raspberry Pi Pico W

Este projeto implementa a comunicação serial UART, controle de LEDs RGB e WS2812, e uso do display OLED SSD1306 via I2C na **Raspberry Pi Pico W**. Também inclui interação com botões utilizando interrupções (IRQ) e debounce por software.

---

## 🛠 **Funcionalidades**
✅ **Receber caracteres via UART** e exibir no **display SSD1306**  
✅ **Se for um número (0-9), exibir na matriz de LEDs WS2812**  
✅ **Botão A (GPIO 5)** → Alterna o **LED Verde (GPIO 12)**  
✅ **Botão B (GPIO 6)** → Alterna o **LED Azul (GPIO 13)**  
✅ **Usa interrupções (IRQ) para capturar o pressionamento dos botões**  
✅ **Implementa debounce via software para evitar múltiplos acionamentos**  
✅ **Código estruturado e bem documentado**  

---

## 🖥 **Configuração do Ambiente**
### 🔹 **Requisitos**
- **Raspberry Pi Pico W**
- **VS Code + Extensão CMake**
- **Pico SDK** instalado ([Guia de Instalação](https://github.com/raspberrypi/pico-sdk))
- **CMake** e **Make**
- **Python 3** e **Git**