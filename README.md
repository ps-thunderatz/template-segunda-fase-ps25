# ⚡ 🪨 Pedrinha Semi-Auto 🪨⚡

- [🎈 Introdução](#-introdução)
- [📂 Arquivos do projeto](#-arquivos-do-projeto)
- [📝 O Projeto](#-o-projeto)
  - [💡 **LED**](#-led)
  - [⚙️ **Motores**](#️-motores)
  - [🚗​ **Locomotion**](#-locomotion)
  - [🧠 **Controller and Strategy**](#-controller)
  - [✅ **Testes**](#-testes)
- [🔍 Observações finais](#-observações-finais)
- [📎 APÊNDICE A - Exemplo de máquina de estados](#-apêndice-a---exemplo-de-máquina-de-estados)
- [📎 APÊNDICE B - Configuração do STM32CubeMX](#-apêndice-b---configuração-do-stm32cubemx)
    - [⚙️ Motores](#️-motores-1)
    - [💡 LED](#-led-1)

# 🎈 Introdução

Olá e bem vindos à 2ª ~~e ultima~~ fase do PS!

Nesta fase, vocês irão projetar o primeiro robô de vocês!  Vocês deverão implementar o firmware de um robô de sumo que deverá ser capaz de ter uma estratégia inicial, como em um  round real de sumos.

As ferramentas a serem utilizadas já foram passadas nas últimas tarefinhas, então não será necessário instalar mais nada.



# 📂 Arquivos do projeto

- **Pedrinha/**
  - **inc/**
    - **utils.hpp** - Arquivo com funções uteis
    - **target.hpp** - Arquivo com as definições dos targets
    - **Class headers**
  - **src/**
    - **led.cpp** - Arquivo que contém a lógica do led
    - **rc_receiver.cpp** - Arquivo que contém a lógica de funcionamento dos sensores de distância
    - **motor.cpp** - Arquivo que contém a lógica dos motores
    - **locomotion.cpp** - Arquivo que contém a lógica da locomoção
    - **controller.cpp** - Arquivo que contém a fsm do Sumo
  - **tests/** - Pasta que contém a implementação dos testes

# 📝 O Projeto

Nosso sumo possui um led, 2 motores DC e um receptor de sinal de rádio controle.

Nesse projeto vocês deverão fazer a configuração do microcontrolador no STM32CubeMx e implementar o firmware do Sumo. Para isso, vocês desenvolverão as seguintes funcionalidades e testes (lembrando que vocês podem implementar mais funções além das fornecidas):

## 💡 **LED**

No arquivo `led.cpp` vocês deverão implementar a lógica de funcionamento do led, também parecido com o que foi feito na primeira tarefinha de embarcados.

```c
#define LED_PORT GPIOA
#define LED_PIN GPIO_PIN_4
```

Na função `void Led::on()` `void Led::off()` `void Led::toggle()` vocês deverão implementar a lógica de acender, apagar e inverter o estado do led, respectivamente. Para isso, vocês deverão escrever no pino PA04.

## ⚙️ **Motores**

É no `motors.cpp` em que será necessário fazer a implementação do código referente ao funcionamento dos motores. Nela há 1 timer, 2 canais e dois pinos

```c
#define  MOTORS_TIM_HANDLER (htim2)

#define  MOTOR_LEFT_TIM_CH  TIM_CHANNEL_1
#define  MOTOR_LEFT_PORT GPIOA
#define  MOTOR_LEFT_PIN  GPIO_PIN_2

#define  MOTOR_RIGHT_TIM_CH  TIM_CHANNEL_2
#define  MOTOR_RIGHT_PORT GPIOA
#define  MOTOR_RIGHT_PIN  GPIO_PIN_3
```

Para a configuração da PWM no CubeMX, é recomendado um Counter Period de 1000 e 1 de Prescaler.

No construtor vocês devem iniciar o timer e todos os canais de PWM. Além disso vocês devem setar o valor do duty cycle de todos os canais para 0%. Os pinos utilizados para a geração das PWMs são os pinos PA00 e PA01. Para a direção, vocês utilizarão os pinos digitais PA02 e PA03.

```c
Motor::Motor(TIM_HandleTypeDef* motor_tim_handle, uint8_t tim_ch, GPIO_TypeDef*  direction_port, uint16_t  direction_pin)
``` 

Vocês deverão implementar a função `void Motor::set_speed(int8_t speed)` que recebe como parâmetro a velocidade de -100 a 100.

De maneira geral, cada motor tem duas entradas principais (in1 e in2), uma recebe uma PWM, e a outra recebe um sinal alto ou baixo. Uma entrada corresponde a velocidade (PWM) e a outra corresponde ao sentido de rotação (GPIO).

Dessa forma, se o valor de velocidade que a funçao `void set_speed` recebe for maior que zero, o motor deve girar no sentido horário e se o valor for menor que zero, o motor deve girar no sentido anti-horário.

Por conta dessas propridades do driver, os valores de entrada, que estarão entre **-100** e **100**, devem ter um tratamento para serem tranformados em porcentagens do duty cycle da onda. Não basta passar o valor cru para o _COMPARE_ da PWM.

Por fim, o método `void Motors::stop()` deverá setar o valor do duty cycle da PWM dos motores para 0%.

Para isso, é utilizado um circuito específico denominado Ponte H, que permite, com um pequeno consumo energético, a rotação dos motores tanto no sentido horário quanto no anti-horário. Não iremos nos estender na definição do circuito, mas quem tiver interesse pode saber mais [aqui](https://www.manualdaeletronica.com.br/ponte-h-o-que-e-como-funciona/)

## 🚗​ **Locomotion**

Na `locomotion.cpp` que efetivamente trabalharemos com dois motores. O intuito aqui é basicamente implementar o que funciona pra um motor mas pra dois.

Aqui a classe receberá dois motores, já declarados antes.

## 🧠 **Controller**

A lógica principal do robô deve ser implementada nos arquivos controller.cpp. A função do controller é ser o "cérebro" do robô, ou seja, nele estará contida a lógica relacionada à quando o robô deve inicializar, a seleção de estratégias e quando parar.

### Funcionamento do Controller

A implementação da lógica do controller se dá por meio da confecção de uma FSM (Finite State Machine). Um exemplo de FSM pode ser encontrado no [Apêndice A](#apêndice-a---exemplo-de-máquina-de-estados).

 O arquivo possuirá, então, 2 métodos:

- `void Controller::init()` -> Responsável por inicializar a máquina de estados.
- `void Controller::run()` -> Responsável por conter e alternar os estados durante a execução do Sumo.

Ao todo, sugere-se que vocês utilizem 2 estados definidos no `enum State` da classe:

- **_STRATEGY_CHOOSER_**:  Nesse estado, ele deve esperar um movimento de alguma manopla e assim alterar o current state. Ou seja, 3 estados pro movimento da manopla e um movimento pra mudar a estratégia pra run

- **_RUN_**: Nesse estado é rodado a estratégia selecionada. Deve-se também checar a leitura de algum movimento específico, como os dois em -100 para ele realizar um "drible".

![Drible](/media/Drible.gif)

Nós encorajamos que vocês coloquem uma forma de ligar o led diferente para cada um dos estados mencionados, pois irá ajudá-los a debuggar possíveis problemas dos seus controllers.

## 📜 Fases

As estratégias deveram ser caṕazes de passar por 3 fases.

### 🌱 1° Fase

A primeira fase, os sumos serão posicionados frente a frente um do outro. Seu objetivo será movimentar o robo para frente para empurrar o adversário para fora do dojo. E parar antes de cair.

![Fase01](/media/Fase01.gif)

### 🌿 2° Fase

Na segunda fase, ele deve rodear o dojo inteiro e posicionar nas costas do inimigo.

![Fase02](/media/Fase02.gif)

### 🌳 3° Fase

Na última fase, temos como objetivo que vocês criem uma estratégia que aparenta ser boa, **usem a criatividade**. Recomendo dar uma olhada nos vídeos do nosso canal do youtube para ter ideias.

#### Lembrando que após cada fase, ele deve ser capaz de ser controlado pelo controle e ainda capaz de driblar.

## ✅ **Testes**

Uma prática muito comum na equipe e no desenvolvimento de projetos embarcados no geral é a criação e utilização de testes.

Na pasta `tests/bin` vocês devem implementar testes para os leds e motores serem usados em bancada para validar o código de vocês. Vocês devem implementar testes para o led, motores e locomotion.

# 🔍 **Observações finais**

Na pasta inc, está os headers com as definições das classes. Vocês não precisam alterá-los, mas devem olhar as definições feitas, como as declarações das funções, que vocês irão implementar, e as variáveis declaradas, que vocês utilizarão nos metodos. O arquivo utils.hpp também possui funções que podem ser utilizadas em suas implementações.

## **IMPORTANTE**
Caso a sua versão do STM32CubeMX seja a versão 6.14.X, o cubemx acaba gerando o arquivo de forma diferente. Para resolver esse problema, por favor, instale a versão 6.13.X do cube.  

# 📎 APÊNDICE A - Exemplo de máquina de estados

 ```c
    #include <stdio.h>

    typedef enum estado {
        INICIO,  
        MEIO,   
        FIM,  
    } estado_t;

    static estado_t estado_atual;
    
    int main() {
        fsm_init();

        for(;;) {
            fsm_troca_estado();
        }
    }

    void fsm_init(void) {
        estado_atual = INICIO;
    }


    void fsm_troca_estado() {
        switch (estado_atual) {
            case INICIO:
                printf("Olá você está no inicio. Aperte o botão para ir para o meio");

                if (botao_esta_apertado()) {
                    estado_atual = MEIO;
                }

                break;

            case MEIO:
                printf("Olá você está no meio. Aperte o botão para ir para o fim");

                if (botao_esta_apertado()) {
                    estado_atual = FIM;
                }

                break;

            case FIM:
                printf("Olá você está no fim. Aperte o botão para voltar para o inicio");

                if (botao_esta_apertado()) {
                    estado_atual = INICIO;
                }

                break;
        }
    } 
```

## 📎 APÊNDICE B - Configuração do STM32CubeMX

### ⚙️ Motores

- Motor direito:
	- PWM:
		- **Timer**: tim 2
		- **Pino**: PA00
		- **Canal**: channel 1
		- **Prescaler**: 1 (recomendado)
		- **Counter Period**: 1000 (recomendado)
	- Gpio output:
		- **Pino**: PA02

- Motor Esquerdo:
	- PWM:
		- **Timer**: tim 2
		- **Pino**: PA01
		- **Canal**: channel 1
		- **Prescaler**: 1 (recomendado)
		- **Counter Period**: 1000 (recomendado)
	- Gpio output:
		- **Pino**: PA03

### 💡 LED

  - LED:
    - **Pino**: PA04 - Gpio output
