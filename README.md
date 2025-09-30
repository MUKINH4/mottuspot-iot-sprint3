# 🏍️ Mottu Spot - Sistema IoT de Gestão de Pátios e Motos

## Integrantes
- Samuel Heitor – RM 556731
- Lucas Nicolini – RM 557613
- Renan Olivi – RM 557680

## 📋 Sobre o Projeto

O **Mottu Spot** é um sistema completo de Internet das Coisas (IoT) desenvolvido para gestão inteligente de pátios e controle de motocicletas. O projeto integra uma aplicação web Flask com dispositivos ESP32 para permitir o monitoramento e controle remoto de veículos através de comunicação MQTT.

### 🎯 Funcionalidades Principais

- **Interface Web Responsiva**: Dashboard intuitivo para visualização e controle das motos
- **Comunicação IoT**: Integração ESP32 via protocolo MQTT
- **Controle de Localização**: Sistema de ativação/desativação de dispositivos de rastreamento
- **Alertas Visuais e Sonoros**: LED e buzzer para indicação de status
- **Banco de Dados**: Armazenamento persistente do status das motocicletas
- **Simulação Wokwi**: Ambiente de desenvolvimento e testes virtuais

## 🏗️ Arquitetura do Sistema

```
┌─────────────────┐    MQTT     ┌─────────────────┐
│   Aplicação     │◄────────────┤     ESP32       │
│     Flask       │     WiFi    │   (Wokwi)       │
│                 │             │                 │
│ • Interface Web │             │ • LED (Pin 2)   │
│ • Banco SQLite  │             │ • Buzzer (Pin 4)│
│ • Cliente MQTT  │             │ • Cliente MQTT  │
└─────────────────┘             └─────────────────┘
         │                               │
         │        broker.hivemq.com      │
         └───────────────┬───────────────┘
                         │
                 ┌───────▼───────┐
                 │  Broker MQTT  │
                 │   (HiveMQ)    │
                 └───────────────┘
```

## 🚀 Como Usar o Sistema

### 📋 Pré-requisitos

- Python 3.7+
- Bibliotecas Python: Flask, paho-mqtt, sqlite3
- Navegador web moderno
- Conexão com internet

### 🔧 Instalação e Configuração

1. **Clone o repositório**:
   ```bash
   git clone <url-do-repositorio>
   cd mottuspot-iot-sprint3
   ```

2. **Instale as dependências**:
   ```bash
   pip install flask paho-mqtt
   ```

3. **Execute a aplicação**:
   ```bash
   cd app
   flask run
   ```

4. **Acesse a interface web**:
   - Abra o navegador em: `http://localhost:5000`

### 🎮 Passo a Passo de Uso

#### 1. **Acessando o Dashboard**
- A página inicial exibe todas as motos cadastradas no sistema
- Cada moto é apresentada em um card com informações:
  - **Placa**: Identificação única do veículo
  - **Descrição**: Detalhes como cor e modelo
  - **Status**: Estado atual do dispositivo de localização

#### 2. **Controlando o Dispositivo de Localização**

##### 🟢 **Ativar Localização**
- Clique no botão **"Ativar"** (verde) na moto desejada
- **O que acontece**:
  - Status muda para "Ativado" (texto verde)
  - Comando MQTT é enviado para o ESP32
  - LED vermelho acende no dispositivo
  - Buzzer emite sons intermitentes (300ms ligado, 700ms desligado)
  - Banco de dados é atualizado

##### 🔴 **Desativar Localização**
- Clique no botão **"Desativar"** (vermelho) na moto desejada
- **O que acontece**:
  - Status muda para "Desativado" (texto vermelho)
  - Comando MQTT é enviado para o ESP32
  - LED vermelho apaga no dispositivo
  - Buzzer para de tocar
  - Banco de dados é atualizado

## 🔧 Como Funciona o Dispositivo de Localização

### 📡 **Comunicação MQTT**

O sistema utiliza o protocolo MQTT para comunicação bidirecional:

- **Tópico de Comando**: `iot/mottuspot/comando`
  - Aplicação Flask → ESP32
  - Formato JSON: `{"placa": "ABC-1234", "descricao": "Moto vermelha", "status": "ativar"}`

- **Tópico de Status**: `iot/mottuspot/status`
  - ESP32 → Aplicação Flask
  - Confirma execução dos comandos

### ⚡ **Hardware ESP32**

#### Componentes Conectados:
- **LED Vermelho (Pino 2)**: Indicador visual de ativação
- **Buzzer (Pino 4)**: Alerta sonoro
- **Resistor 220Ω**: Proteção do LED

#### Comportamento do Dispositivo:

**Estado ATIVADO**:
- ✅ LED vermelho permanece aceso
- 🔊 Buzzer toca em padrão intermitente (500Hz)
- 📡 Confirma status via MQTT

**Estado DESATIVADO**:
- ❌ LED apagado
- 🔇 Buzzer silencioso
- 📡 Confirma status via MQTT

### 🌐 **Simulação Wokwi**

Para testar o dispositivo ESP32:

1. **Acesse**: [Wokwi Simulator](https://wokwi.com)
2. **Importe o projeto**: Use os arquivos da pasta `/wokwi`
3. **Execute a simulação**: Clique em "Start Simulation"
4. **Monitor Serial**: Observe as mensagens de debug
5. **Teste MQTT**: Use a aplicação Flask para enviar comandos

## 📊 Banco de Dados

### Estrutura da Tabela `motos`:
```sql
CREATE TABLE motos (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    placa TEXT,
    descricao TEXT,
    status TEXT
);
```

### Dados Pré-cadastrados:
- **ABC-1234**: Moto vermelha
- **XYZ-5678**: Moto azul  
- **JKL-9999**: Moto preta

## 🔍 Logs e Monitoramento

### Aplicação Flask:
```
📥 Status recebido do ESP32: {"placa":"ABC-1234","status":"ativado"}
📡 Comando MQTT enviado: {"placa":"ABC-1234","descricao":"Moto vermelha","status":"ativar"}
```

### ESP32 (Monitor Serial):
```
🚨 Comando recebido:
Placa: ABC-1234
Descrição: Moto vermelha
Status: ativar
🔊 Alerta ativado
```

## 🛠️ Configurações Técnicas

### MQTT Broker:
- **Servidor**: `broker.hivemq.com`
- **Porta**: `1883`
- **Protocolo**: TCP/IP

### Rede WiFi (Wokwi):
- **SSID**: `Wokwi-GUEST`
- **Senha**: *(sem senha)*

## 🚨 Solução de Problemas

### Problemas Comuns:

1. **Dispositivo não responde**:
   - Verificar conexão WiFi do ESP32
   - Confirmar conexão com broker MQTT
   - Validar formato JSON dos comandos

2. **Interface web não carrega**:
   - Verificar se o Flask está rodando na porta 5000
   - Confirmar instalação das dependências

3. **Status não atualiza**:
   - Verificar logs MQTT na aplicação
   - Conferir tópicos de publicação/subscrição

## 👨‍💻 Desenvolvimento

### Estrutura de Arquivos:
```
📁 mottuspot-iot-sprint3/
├── 📁 app/
│   ├── app.py              # Aplicação Flask principal
│   └── 📁 templates/
│       └── index.html      # Interface web
├── 📁 wokwi/
│   ├── sketch.ino          # Código ESP32
│   ├── diagram.json        # Circuito Wokwi
│   ├── libraries.txt       # Bibliotecas Arduino
│   └── wokwi-project.txt   # Configurações Wokwi
└── README.md               # Este arquivo
```

### Tecnologias Utilizadas:
- **Backend**: Python Flask
- **Frontend**: HTML5, Bootstrap 5
- **IoT**: ESP32, Arduino IDE
- **Comunicação**: MQTT Protocol
- **Banco**: SQLite
- **Simulação**: Wokwi Platform