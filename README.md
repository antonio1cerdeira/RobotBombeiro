Robô Bombeiro (RB_2023 1.0)

Descrição

O Robô Bombeiro (RB_2023 1.0) é um projeto desenvolvido para a disciplina de Robótica, com o objetivo de criar um robô autônomo capaz de navegar em um ambiente, identificar a presença de fogo e extingui-lo utilizando um sistema de ventilação. O projeto é baseado em uma máquina de estados finitos (FSM) e utiliza sensores de chama, sensores ultrassônicos e odometria para navegação precisa.

Funcionalidades

Navegação autônoma utilizando sensores ultrassônicos para evitar obstáculos.

Detecção de chama com sensor infravermelho.

Extinção do fogo através de um ventilador embutido.

Sistema de odometria para medição precisa de deslocamento.

Interface OLED para exibição de informações do robô.

Componentes Utilizados

Hardware:

Arduino Mega 2560

Módulos de Motor (Ponte H)

Sensores ultrassônicos (NewPing)

Sensor de chama (FlameSensor)

Display OLED (Adafruit SH1106)

Encoders para odometria

Ventilador para extinguir o fogo

Software:

Linguagem: C++ (Arduino)

Bibliotecas:

Motor.h

NewPing.h

Adafruit_GFX.h

Adafruit_SH1106.h

FlameSensor.h

Wire.h

Estrutura do Código

|-- src/
|   |-- Motor.h      # Classe para controle dos motores
|   |-- main.ino     # Código principal
|-- README.md       # Documentação do projeto

Como Usar

1. Instalar Dependências

Certifique-se de instalar as bibliotecas necessárias na IDE do Arduino:

#include "Motor.h"
#include <NewPing.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#include <FlameSensor.h>
#include <Wire.h>

2. Configurar Hardware

Conectar os sensores e motores conforme os pinos definidos no código.

Carregar o código no Arduino Mega 2560.

3. Executar o Robô

Ligar o robô e pressionar o botão de Start.

O robô iniciará a navegação e procurará por chamas.

Ao detectar fogo, se aproximará e ligará o ventilador.

Contribuição

Se deseja contribuir para o projeto, siga os passos:

Faça um fork do repositório.

Crie uma nova branch: git checkout -b minha-feature

Faça commit das suas alterações: git commit -m 'Adicionei uma nova funcionalidade'

Envie para o repositório remoto: git push origin minha-feature

Abra um Pull Request.

Licença

Este projeto é distribuído sob a licença MIT. Consulte o arquivo LICENSE para mais detalhes.

Desenvolvido para a disciplina de Robótica - 2023.