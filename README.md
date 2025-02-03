# Robô Bombeiro (RB_2023 1.0)

## Descrição

O **Robô Bombeiro (RB_2023 1.0)** é um projeto desenvolvido para a disciplina de Robótica, com o objetivo de criar um robô autônomo capaz de navegar em um ambiente, identificar a presença de fogo e extingui-lo utilizando um sistema de ventilação. O projeto é baseado em uma **máquina de estados finitos (FSM)** e utiliza **sensores de chama, sensores ultrassônicos e odometria** para navegação precisa.

## Funcionalidades

- 🚀 **Navegação autônoma** utilizando sensores ultrassônicos para evitar obstáculos.
- 🔥 **Detecção de chama** com sensor infravermelho.
- 💨 **Extinção do fogo** através de um ventilador embutido.
- 📏 **Sistema de odometria** para medição precisa de deslocamento.
- 📟 **Interface OLED** para exibição de informações do robô.

## Componentes Utilizados

### 🔧 Hardware:

- **Arduino Mega 2560**
- **Módulos de Motor (Ponte H)**
- **Sensores ultrassônicos (NewPing)**
- **Sensor de chama (FlameSensor)**
- **Display OLED (Adafruit SH1106)**
- **Encoders para odometria**
- **Ventilador para extinguir o fogo**

### 💻 Software:

- **Linguagem:** C++ (Arduino)
- **Bibliotecas:**
  - `Motor.h`
  - `NewPing.h`
  - `Adafruit_GFX.h`
  - `Adafruit_SH1106.h`
  - `FlameSensor.h`
  - `Wire.h`

## 📁 Estrutura do Código

```
|-- src/
|   |-- Motor.h      # Classe para controle dos motores
|   |-- main.ino     # Código principal
|-- README.md       # Documentação do projeto
```

## 🚀 Como Usar

### 1️⃣ Instalar Dependências

Certifique-se de instalar as bibliotecas necessárias na IDE do Arduino:

```cpp
#include "Motor.h"
#include <NewPing.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#include <FlameSensor.h>
#include <Wire.h>
```

### 2️⃣ Configurar Hardware

1. Conectar os sensores e motores conforme os pinos definidos no código.
2. Carregar o código no **Arduino Mega 2560**.

### 3️⃣ Executar o Robô

1. Ligar o robô e pressionar o botão de **Start**.
2. O robô iniciará a navegação e procurará por chamas.
3. Ao detectar fogo, se aproximará e ligará o ventilador.

## 🤝 Contribuição

Se deseja contribuir para o projeto, siga os passos:

1. Faça um **fork** do repositório.
2. Crie uma nova branch:
   ```sh
   git checkout -b minha-feature
   ```
3. Faça commit das suas alterações:
   ```sh
   git commit -m 'Adicionei uma nova funcionalidade'
   ```
4. Envie para o repositório remoto:
   ```sh
   git push origin minha-feature
   ```
5. Abra um **Pull Request**.

## 📜 Licença

Este projeto é distribuído sob a licença **MIT**. Consulte o arquivo `LICENSE` para mais detalhes.