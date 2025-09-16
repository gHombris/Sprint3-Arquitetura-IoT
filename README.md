Projeto Skill Her - Sprint 3: Arquitetura IoT
Este repositório contém os artefatos de software para a entrega da Sprint 3 do projeto Skill Her, referente à disciplina de Edge Computing and Computer Systems.

Integrantes
Gabriel Henrique Borges Hombris - RM: 566553 

Felipe Kolarevic Santos - RM: 565230 

1. Descrição do Projeto
O objetivo do projeto Skill Her é resolver a falta de um ambiente de treino acolhedor e didático para mulheres iniciantes no futebol, oferecendo uma plataforma focada no desenvolvimento de habilidades individuais de forma gamificada. Esta entrega foca na prova de conceito da arquitetura IoT, responsável por coletar dados de desempenho durante os treinos. 


2. Arquitetura Proposta
A arquitetura planejada para a solução completa consiste em três componentes principais que se comunicam via internet.

Explicação do Fluxo
Dispositivo IoT (Coleta): Um microcontrolador (ESP32) acoplado a um sensor (acelerômetro MPU-6050) é posicionado no equipamento da jogadora. Ele é responsável por detectar os toques na bola e contar o total durante uma sessão de treino.

Comunicação (MQTT): Ao final do treino, o dispositivo envia o resultado (ex: total de toques) para um servidor na nuvem (Broker MQTT) usando o protocolo MQTT. Este protocolo é leve e ideal para dispositivos com recursos limitados.

Backend (Recepção e Processamento): Nossa aplicação de servidor, desenvolvida em Python com Flask, se inscreve no tópico do Broker MQTT para receber os dados enviados pelo dispositivo. Ao receber, ele processa a informação para calcular o XP da jogadora e atualizar seu dashboard de progresso.

Diagrama de Fluxo
+------------------+           +------------------+           +------------------+
|                  |           |                  |           |                  |
| Dispositivo IoT  |  ----->   |  Broker MQTT     |  ----->   |   Backend        |
| (ESP32 + Sensor) |  (Publica|   (Nuvem)          | (Recebe |  (Python/Flask)  |
|                  |   dados)  |                  |  dados)  |                  |
+------------------+           +------------------+           +------------------+
3. Recursos Necessários
Software
Python 3.x

Biblioteca Flask (pip install Flask)

Um simulador de terminal para executar o comando curl (CMD, PowerShell, Git Bash, etc.)

Hardware (Simulado)
Simulador online Wokwi ([https://wokwi.com/](https://wokwi.com/projects/442279286784339969))

Componentes simulados: ESP32, Acelerômetro MPU-6050, LCD 16x2 I2C, LED, Push Button.

4. Instruções de Uso
Para testar a prova de conceito da Sprint 3, siga os passos abaixo para rodar o backend e a simulação do dispositivo. 

Parte 1: Executando o Backend
Navegue até a pasta do projeto no seu terminal.

(Opcional) Crie e ative um ambiente virtual: python -m venv venv e source venv/bin/activate.

Instale as dependências: pip install Flask.

Execute o servidor: python backend.py.

O terminal mostrará uma mensagem indicando que o servidor está rodando e esperando por requisições na porta 5000. Mantenha este terminal aberto.

Parte 2: Executando a Simulação do Dispositivo
Abra o projeto no Wokwi (incluir o link do seu projeto aqui).

O código sketch.ino e o diagrama diagram.json já estarão carregados.

Clique no botão verde "Play" (▶️) para iniciar a simulação.

Para iniciar um treino, clique no botão verde no circuito. O LED verde acenderá.

Para simular toques, clique e arraste rapidamente o sensor MPU-6050 (o chip azul). O contador no LCD irá incrementar.

Para finalizar o treino, clique no botão verde novamente. O LCD mostrará o total de toques.

Parte 3: Simulando o Envio de Dados
Para demonstrar a comunicação entre o dispositivo (simulado) e o backend, usaremos curl para enviar manualmente o resultado obtido na simulação.

Abra um novo terminal.

Anote o número total de toques que apareceu no LCD do Wokwi (ex: 32).

Execute o comando abaixo, substituindo N pelo número anotado:

Bash

curl -X POST -H "Content-Type: application/json" -d "{\"toques\": N}" http://127.0.0.1:5000/treino/resultado
Observe o terminal onde o backend.py está rodando. A mensagem de sucesso ✅ Resultado recebido do dispositivo! Toques: N será exibida.

Isso conclui a demonstração da arquitetura ponta-a-ponta, validando a coleta de dados no dispositivo e a recepção no servidor. A conexão automática via MQTT será o foco da implementação da Sprint 4. 
