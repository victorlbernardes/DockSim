# 🎮 GDD – Space Docking Simulator (nome provisório)

---

## 1. 🎯 Visão do Jogo

Jogo de pilotagem espacial focado em **controle técnico e acoplamento preciso (docking)**.

Objetivo: entregar uma experiência baseada em habilidade, leitura de instrumentos e domínio da física.

---

## 2. 🎮 Core Gameplay Loop

1. Selecionar contrato
2. Navegar até o alvo
3. Entrar na zona de docking
4. Alternar para modo docking
5. Realizar acoplamento
6. Receber avaliação
7. Progredir

---

## 3. 🧠 Mecânicas Principais

### 🚀 Controle (6 DOF)

- Translação (X, Y, Z)
- Rotação (pitch, yaw, roll)
- Thrusters

### ⚙️ Física

- Inércia
- Sem atrito
- Movimento contínuo
- Velocidade relativa como base

---

## 4. 🎮 Modos de Pilotagem

- Assistido
- Padrão
- Simulação (sem assistência)

---

## 5. 🛰️ Modos de Operação

### 🧭 Pilotagem

- Navegação até o alvo
- Controle macro

### 🛰️ Docking

- Controle fino
- Ajustes de posição, rotação e velocidade

---

## 6. 🛰️ Sistema de Docking

### Requisitos para sucesso:

- Posição dentro da tolerância
- Rotação alinhada
- Velocidade segura

### Resultado:

- Docking automático ao atingir condições

---

## 7. 📊 HUD

> Principal interface de aprendizado e controle.
> 

### Cockpit

- Direção da nave
- Direção do movimento
- Velocidade relativa
- Alvo e distância

**🎯 Wireframe**

# Aplicar Glow no PNG

1. Crie Material:
    - **Material Domain**: User Interface
    - **Blend Mode**: Translucent
    - **Shading Model**: Unlit
2. Conecte sua `Texture2D` do PNG no `Emissive Color`
3. Adicione parâmetros de **TintColor e GlowPower** como discutimos
4. Aplique no **Brush → Material** do Image no Widget

# 🧱 HUD – Árvore de Componentes (sem Compass)

```
WBP_ShipHUD
└── CanvasPanel_Root
    ├── Overlay_Center
    │   ├── Image_Crosshair
    │   ├── Image_FlightVector
    │   └── CanvasPanel_TargetLayer
    │       ├── Text_TargetName
    │       └── Text_TargetDistance
    │
    ├── CanvasPanel_Attitude
    │   ├── Image_PitchLadder
    │   ├── Image_RollIndicator
    │   └── Text_YawHeading
    │
    ├── VerticalBox_TopInfo
    │   └── Text_Speed
    │
    ├── CanvasPanel_Velocity
    │   ├── ProgressBar_Forward
    │   ├── ProgressBar_Lateral
    │   └── ProgressBar_Vertical
    │
    ├── VerticalBox_RightPanel
    │   └── Text_GForce
    │
    └── LeftDebug (opcional)
        ├── Text_ForwardSpeed
        ├── Text_LateralSpeed
        └── Text_VerticalSpeed
```

---

# 📐 Wireframe do HUD (visão de tela)

```
┌─────────────────────────────────────────────┐
│                 Speed: 1250 m/s            │ <- VerticalBox_TopInfo
│                                             │
│        \      |      /                      │ <- RollIndicator
│          [  +  ]                            │ <- Crosshair
│             •                                │ <- FlightVector
│                                             │
│ Forward: [=======     ]                     │ <- ProgressBar_Forward
│ Lateral: [===         ]                     │ <- ProgressBar_Lateral
│ Vertical:[==          ]                     │ <- ProgressBar_Vertical
│                                             │
│ Target: ENEMY SHIP                           │ <- TargetLock / CanvasPanel_TargetLayer
│ Distance: 2.4 km                             │
│                                             │
│ G: 3.2                                       │ <- VerticalBox_RightPanel
│                                             │
│ Yaw: 270°                                    │ <- Text_YawHeading
│ Pitch: +10°                                  │ <- Image_PitchLadder
│                                             │
│ [Optional Debug Panel]                       │ <- LeftDebug
│ Fwd: 1234 m/s
│ Lat: 56 m/s
│ Vert: 12 m/s
└─────────────────────────────────────────────┘
```

---

# 🎯 Distribuição visual (para UMG)

| Local | Elemento | Função |
| --- | --- | --- |
| Top Center | VerticalBox_TopInfo | Velocidade total |
| Center | Overlay_Center | Crosshair, Flight Vector, Target Lock |
| Center / Middle | CanvasPanel_Attitude | Roll Indicator, Pitch Ladder, Yaw Heading |
| Center / Bottom | CanvasPanel_Velocity | Barras de Forward, Lateral, Vertical |
| Right Center | VerticalBox_RightPanel | G-Force |
| Left Side (opcional) | LeftDebug | Debug de velocidades individuais |

---

## 8. ⚙️ Sistema de Cálculo

Baseado em valores relativos ao alvo.

- Posição relativa → offset X/Y + distância
- Velocidade relativa → forward + lateral
- Rotação relativa → pitch/yaw/roll
- Direção → vetor até o alvo

---

## 9. ⭐ Sistema de Avaliação

Avaliação baseada em:

- Precisão (offset)
- Estabilidade (oscilações)
- Velocidade (controle de aproximação)

Resultado:

- Score por categoria

Objetivo:

- Feedback
- Progressão

---

## 10. 🔄 Sistema de Feedback ao Jogador

### Visual

- HUD
- Indicadores de risco

### Sonoro

- Alertas de velocidade
- Confirmações de alinhamento
- Docking concluído

### Crítico

- Avisos de erro
- Feedback de risco

---

## 11. 🛰️ Sistema de Assistência

Opcional

- Linha de aproximação
- Indicação de correção

Mais presente no modo assistido

---

## 12. 📈 Progressão

Sistema de licenças:

- D → básico
- C → intermediário
- B → avançado
- A → especialista

Baseado em performance

---

## 13. 🧩 Variações de Desafio

### Docking Ports

- Fixo
- Rotacional
- Desalinhado
- Danificado

### Ambiente

- Baixa visibilidade
- Iluminação reduzida
- Movimento da estação

---

## 14. 🧭 Tipos de Missão

- Comercial
- Resgate
- Rebocador
- Manutenção
- Transporte sensível

---

## 15. 🎮 MVP

- Controle da nave
- Física básica
- Sistema de docking
- HUD essencial
- Sistema de avaliação

---

## 16. 🧱 Roadmap

1. Movimento
2. Física
3. Assistência
4. HUD
5. Docking
6. Avaliação
7. Missões

---

## 17. 🚧 Escopo

Sem combate

Sem multiplayer

Foco:

- Pilotagem
- Precisão

---

## 18. 🎯 Critério de Sucesso

- Jogador entende o HUD
- Evolui com prática
- Docking é consistente e satisfatório

---

## 19. 🧠 Filosofia

Domínio vem da leitura de instrumentos e execução precisa.

---

# 🛰️ Documento de Evolução Técnica: Projeto DockSim

## 1. Sistema de Controle e Máquina de Estados (Backend)

Em vez de variáveis booleanas isoladas, implementamos uma **Máquina de Estados de Contexto de Voo** via `enum` para gerenciar como o input do jogador é interpretado.

### Enum: `ESpaceControlContext`

Define os modos:

- `PitchOnly`
- `YawMode`
- `RollMode`
- `FullPrecision`
- `Stabilizing`

### Lógica de Override

O sistema foi configurado para que **qualquer input manual de movimento ou rotação desative automaticamente o modo de estabilização**, devolvendo o controle imediato ao piloto.

### Joystick Virtual

Implementamos o **acúmulo de rotação (Clamp de -1.0 a 1.0)** para que o mouse funcione como um manche físico, mantendo a inclinação até ser:

- zerado manualmente, ou
- resetado via auto-centralização

---

## 2. Enhanced Input e Gerenciamento de Potência

Abandonamos o sistema de input legado em favor do **Enhanced Input Plugin da Unreal Engine 5**.

### Master Throttle (Potência Mestre)

- Variável: `MasterPowerScale`
- Controle via **Scroll do Mouse**

### Escalonamento de Força

- A potência ajustada pelo jogador (0% a 100%)
- Atua como **multiplicador direto no empuxo dos motores**
- Permite manobras de docking de alta precisão

### Feedback Visual

- O valor do Throttle é enviado em tempo real para o HUD
- Faixa: **0% – 100%**

---

## 3. Componente de Telemetria Avançada (`SpaceTelemetryComponent`)

Desenvolvemos um componente desacoplado que processa a física bruta e a transforma em dados legíveis para a interface.

### Decomposição de Vetores

- Converte velocidade global (`FVector`)
- Em velocidades locais:
    - Forward
    - Lateral
    - Vertical
- Unidade: **m/s**

### Cálculo de G-Force

- Medição de aceleração em Gs
- Separação por eixos
- Estados:
    - `Normal`
    - `Warning`
    - `Critical`

### Escala de Simulação (`SimulationMultiplier`)

Para evitar erros de precisão em altas velocidades:

- Física real → permanece lenta (estável)
- HUD → exibe valores multiplicados (ex: `100x`)

✔ Permite sensação de **~19.000 m/s sem jitter de renderização**

---

## 4. Modelo de Voo e Física (`SpaceMovementComponent`)

A movimentação foi baseada na **Segunda Lei de Newton**:

```
F = m · a
```

### Aplicação de Empuxo (Thrust)

- Força aplicada em Newtons
- Direcionada ao `StaticMeshComponent`
- Respeita a massa configurada da nave

### SAS (Stability Augmentation System)

Sistema de estabilização automática que:

- Aplica contra-força proporcional à:
    - velocidade linear
    - torque atual

✔ Resultado: nave estabilizada automaticamente no espaço

### Angular Damping Dinâmico

- Simula resistência de giroscópios internos
- Evita rotação infinita descontrolada

---

## 5. Otimização de Performance e Renderização

### Tick Gerenciado

- O `SpaceMovementComponent` só executa Tick de estabilização quando:
    - modo `Stabilizing` está ativo

✔ Economia de CPU

### Cockpit-Centric Design

Foco total na visão interna:

- Uso de **FOV Dinâmico**
- FOV aumenta conforme a velocidade simulada cresce

✔ Cria efeito de velocidade extrema (tipo dobra)

✔ Sem mover o ator além dos limites do mapa

---

## 📝 Resumo de Variáveis Críticas para Ajuste Fino (Editor)

| Variável | Localização | Valor Recomendado | Efeito |
| --- | --- | --- | --- |
| `ThrustForce` | MovementComp | 25.000 – 50.000 | Força de aceleração linear |
| `RotationForce` | MovementComp | 50.000+ | Resposta de torque/rotação |
| `AngularDamping` | ShipMesh | 0.8 – 2.0 | Resistência à rotação contínua |
| `MassInKg` | ShipMesh | 2.000 – 5.000 | Peso e inércia da nave |
| `SimulationMultiplier` | TelemetryComp | 100.0 | Escala entre física real e HUD |

# ⌨️ Mapeamento de Comandos (Input Mapping)

Este mapeamento foi projetado para equilibrar a complexidade de um simulador espacial com a acessibilidade de um jogo em cockpit.

---

## 1. Translação (Movimento Linear)

| Comando | Tecla / Input | Ação no Código | Descrição |
| --- | --- | --- | --- |
| Frente / Trás | `W / S` | `HandleMove (Y)` | Aplica empuxo no eixo longitudinal |
| Esquerda / Direita | `A / D` | `HandleMove (X)` | Aplica empuxo lateral (Strafe) |
| Cima / Baixo | `Space / C` | `HandleMove (Z)` | Aplica empuxo vertical |

---

## 2. Rotação (Atitude)

| Comando | Tecla / Input | Ação no Código | Descrição |
| --- | --- | --- | --- |
| Pitch (Bico) | `Mouse Y` | `HandleLook (Y)` | Inclina a nave para cima ou para baixo |
| Yaw (Guinada) | `Mouse X (Modo Yaw)` | `HandleLook (Z)` | Gira o nariz da nave para os lados |
| Roll (Rolagem) | `Mouse X (Modo Roll)` | `HandleLook (X)` | Gira a nave em torno do próprio eixo |

---

## 3. Sistemas de Voo e Precisão

| Comando | Tecla / Input | Ação no Código | Descrição |
| --- | --- | --- | --- |
| Ajustar Potência | `Mouse Wheel` | `HandleAdjustThrottle` | Ajusta o `MasterPowerScale` (0% – 100%) |
| Toggle Yaw | `Shift` *(ou configurável)* | `ToggleYawMode` | Alterna eixo X do mouse para controle de Yaw |
| Toggle Roll | `Alt` *(ou configurável)* | `ToggleRollMode` | Alterna eixo X do mouse para controle de Roll |
| Estabilizador (SAS) | `H / X` | `ToggleStabilizer` | Ativa/desativa frenagem automática (computador de bordo) |