# iot-esp32-robo-espacial

## Nome do Projeto
Robo Lab ESP32 - Exploracao Espacial com telemetria, alerta e persistencia em Supabase

## Autores
- Luiz Gustavo Cunha
- Rafael Crusoé
- Manuella Serravalle

## Visao Geral
Este projeto simula um robo espacial com ESP32 capaz de:

- medir luminosidade com LDR;
- detectar proximidade com sensor ultrassonico HC-SR04;
- mover um servo com joystick;
- sinalizar status com LEDs;
- enviar alerta via CallMeBot;
- salvar leituras no Supabase;
- consultar os dados salvos por um backend Python simples.

## Estrutura do Repositorio
- `src/main.cpp`: firmware principal do ESP32.
- `diagram.json`: circuito base para simulacao no Wokwi.
- `backend/app.py`: API Python para consultar leituras do Supabase.
- `backend/requirements.txt`: dependencias do backend.

## Como Montar o Robo

### Componentes

| Componente | Quantidade | Pinos usados no ESP32 |
|---|---:|---|
| ESP32 DevKit | 1 | - |
| LDR | 1 | GPIO 2 |
| HC-SR04 | 1 | TRIG GPIO 12, ECHO GPIO 13 |
| LED verde | 1 | GPIO 5 |
| LED vermelho | 1 | GPIO 3 |
| Servo motor | 1 | GPIO 18 |
| Joystick analogico | 1 | X GPIO 4, Y GPIO 1 |
| Protoboard + jumpers | 1 kit | - |

### Ligacoes
1. Ligue o `LDR` ao `GPIO 2` e ao circuito de alimentacao conforme o modulo ou divisor resistivo adotado.
2. Ligue o `HC-SR04` com `TRIG` no `GPIO 12`, `ECHO` no `GPIO 13`, `VCC` em `5V` e `GND` no terra.
3. Ligue o `LED verde` ao `GPIO 5` com resistor em serie e o catodo no `GND`.
4. Ligue o `LED vermelho` ao `GPIO 3` com resistor em serie e o catodo no `GND`.
5. Ligue o sinal do `servo` no `GPIO 18`, alem de alimentacao e `GND`.
6. Ligue o `joystick` com eixo `X` no `GPIO 4`, eixo `Y` no `GPIO 1`, `VCC` em `3V3` e `GND` no terra.
7. Garanta `GND` comum entre ESP32, servo e sensores.

### Observacoes de hardware
- `GPIO 1` e `GPIO 3` costumam ser compartilhados com a serial USB em varias placas ESP32. O codigo atual usa esses pinos porque essa foi a revisao informada do projeto. Se houver conflito com upload ou monitor serial em hardware real, remapeie esses sinais.
- Servo motor pode exigir fonte separada se houver oscilacao ou reinicio do ESP32.

## Como Rodar o Firmware
1. Instale o `PlatformIO`.
2. Abra o projeto na pasta raiz.
3. Compile e grave com:

```powershell
pio run
pio run --target upload
pio device monitor
```

4. O firmware conecta no Wi-Fi, mede sensores a cada 2 segundos e envia leituras ao Supabase.

## Como Rodar o Backend Python
O backend serve para consultar as leituras salvas no Supabase por uma API HTTP local.

1. Entre na pasta `backend`:

```powershell
cd backend
```

2. Crie e ative um ambiente virtual:

```powershell
python -m venv .venv
.\.venv\Scripts\Activate.ps1
```

3. Instale as dependencias:

```powershell
pip install -r requirements.txt
```

4. Configure as variaveis de ambiente:

```powershell
copy .env.example .env
```

5. Se quiser sobrescrever os valores padrao, edite o arquivo `.env` ou exporte as variaveis manualmente:

```powershell
$env:SUPABASE_URL="https://SEU-PROJETO.supabase.co/rest/v1/leituras"
$env:SUPABASE_KEY="SUA_SUPABASE_KEY"
```

6. Inicie a API:

```powershell
python app.py
```

7. A API ficara disponivel em `http://localhost:8000`.

## Como Consultar Dados Salvos

### Pelo backend local
Verificar se a API esta online:

```powershell
curl http://localhost:8000/health
```

Listar as ultimas leituras:

```powershell
curl "http://localhost:8000/leituras?limit=10"
```

### Diretamente no Supabase
Tambem e possivel consultar o endpoint REST:

```powershell
curl "https://SEU-PROJETO.supabase.co/rest/v1/leituras?select=*&order=id.desc&limit=10" `
  -H "apikey: SUA_SUPABASE_KEY" `
  -H "Authorization: Bearer SUA_SUPABASE_KEY"
```

## Regras de Funcionamento
- `probabilidade_vida = 40%` quando a luminosidade supera o limite.
- `probabilidade_vida = 60%` quando ha objeto dentro de `30 cm`.
- `ALERTA` ocorre quando a probabilidade total passa de `75%`.
- Em alerta, o projeto envia mensagem via WhatsApp usando `CallMeBot`.
- Em falha de Wi-Fi, o LED vermelho permanece aceso e o firmware tenta reconectar.

## Simulacao no Wokwi
O projeto usa `diagram.json` e `wokwi.toml` na raiz. Compile o firmware primeiro e depois rode a simulacao no Wokwi ou no fluxo configurado pelo editor.
