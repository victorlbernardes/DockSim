# DockSim – Space Docking Simulator

---

## 1. Visão do Jogo

Simulador de pilotagem espacial focado em **controle técnico e acoplamento preciso (docking)** em 6 DOF com física newtoniana realista.

---

## 2. Core Gameplay Loop (Implementado)

1. **Selecionar nave** (Menu Principal → Ship Selection) 
2. **Navegar até a estação** (Voo livre com física 6 DOF)
3. **Entrar na zona de captura** (SensorShield 500m → sinal "Acquired")
4. **Ativar Modo Docking** (Tecla `Dock` → câmera CFTV + HUD dedicado)
5. **Alinhar e aproximar** (Velocidade relativa ≤ 2 m/s, Alinhamento ≥ 95%)
6. **Hard Lock Automático** (Acoplamento + Vitória)
7. **Game Over** se colidir com qualquer objeto (exceto porta de docking)

---

## 3. Mecânicas Principais

### Controle (6 DOF)
- **Translação**: W/S (Forward/Back), A/D (Strafe L/R), Space/C (Up/Down)
- **Rotação**: Mouse Y (Pitch), Mouse X + Shift (Yaw), Mouse X + Alt (Roll)
- **Throttle Mestre**: Scroll Wheel (0–100% multiplicador de força)
- **Precision Mode**: Tecla dedicada (reduz força para manobras finas)
- **SAS (Estabilizador)**: H/X – frena velocidade linear/angular automaticamente
- **Free Look**: Tecla dedicada – olha ao redor no cockpit (não gira a nave)

### Física (Newtoniana)
- **F = m · a** – Força aplicada em Newtons no StaticMeshComponent
- **Inércia real** – Sem atrito, movimento contínuo
- **Massa configurável** (2.000–5.000 kg) afeta aceleração
- **Angular Damping** simula giroscópios internos
- **SimulationMultiplier (100x)** – Física lenta/estável, HUD mostra valores altos

### Estados de Voo (`EShipFlightState`)
| Estado | Descrição |
|--------|-----------|
| `FreeFlying` | Voo livre, navegação até alvo |
| `LandingMode` | Modo docking ativo (CFTV + HUD docking) |
| `Docked` | Acoplado com sucesso (vitória) |
| `Disabled` | Destruído/Game Over (spin aleatório) |

---

## 4. Sistema de Docking

### Zonas de Detecção (SpaceDockingPort)
| Zona | Extensão | Função |
|------|----------|--------|
| `CaptureArea` | 1500m (Box) | Detecta aproximação → Sinal "Acquired", luzes laranja |
| `PrecisionDockingVolume` | 10×50×50m (Box) | Validação contínua a 10Hz para hard lock |

### Critérios de Sucesso (AND Lógico)
1. **Velocidade Relativa ≤ 2 m/s** (`MaxSafeDockingSpeed`)
2. **Nose Alignment ≥ 95%** (Forward da nave oposto ao Forward da porta)
3. **Roll Alignment ≥ 95%** (Up da nave alinhado com Up da porta)
4. **Distância ≤ 10m** (DockingPoint → DockingAnchor)
5. **Estado = LandingMode**

### Velocidade Relativa
```cpp
RelativeVelocity = ShipVelocity - StationVelocity  // Vetorial, magnitude em m/s
```
- Estação move-se (RotatingMovement + InterpToMovement para deriva ping-pong)
- Sistema compensa automaticamente – nave deve "acompanhar" o movimento

### Hard Lock
- Zera velocidades lineares/angulares
- Anexa ShipMesh ao DockingAnchor (SnapToTarget)
- Transição para `Docked` → Victory Screen

---

## 5. Sistema de Colisão / Game Over

**Qualquer colisão ou overlap com objeto NÃO-porta = Game Over Instantâneo**

```cpp
// OnSensorOverlap / OnShipHit
if (OtherActor is NOT ASpaceDockingPort) → TriggerGameOver()
```

### TriggerGameOver()
- Estado → `Disabled`
- Física: Velocidade zero + Spin aleatório (±30°/s em cada eixo)
- Controller → `EMissionState::GameOver` (Mostra tela Game Over)
- Audio → Som de falha
- Limpa timers de telemetria

### Exceção: Porta de Docking
Ignora colisões com todos os componentes da porta:
- `DockStructureMesh`, `CaptureArea`, `PrecisionDockingVolume`, `DockingAnchor`, `RunwayLights`, `SirenBulb`

---

## 6. HUD & Telemetria

### Widgets Ativos (ShipHUDWidget)
| Widget | Modo | Dados Exibidos |
|--------|------|----------------|
| `TelemetryMovementHUDWidget` | FreeFlying | Velocidades locais (F/R/U), Total, Throttle %, Modo Thrust |
| `TelemetryDockingHUDWidget` | LandingMode | Distância, Vel. Relativa, Alinhamento Nose/Roll %, Status Text |
| `CameraHUDWidget` | LandingMode | Feed SceneCapture2D (Câmera docking 1024×1024) |
| `AlertSwitcher` | Ambos | Sinal: Lost / Acquired |

### TelemetryComponent (Atualização 60Hz via HUD Timer)
- Decomposição vetorial: Global → Local (Forward/Right/Up) em m/s
- Velocidade relativa calculada vs alvo
- `ApplyPsychologicalZero(0.05)` – Limpa ruído visual
- `IsReadyForHardLock()` – Validação client-side espelhada

---

## 7. Componentes da Nave (Todos Ativos no Pawn)

| Componente | Responsabilidade |
|------------|------------------|
| `SpaceMovementComponent` | Input → Força/Torque, Throttle, Precision Mode, SAS, Consumo combustível |
| `SpaceTelemetryComponent` | Cálculos vetoriais, Alignment %, Relative Velocity |
| `SpaceResourceComponent` | Combustível (consumo por translação/rotação), Status Normal/Low/Empty |
| `SpaceHullComponent` | **Legado** – ProcessImpact não é mais chamado (Game Over instantâneo) |
| `CFTVComponent` | Gerencia SceneCapture2D + RenderTarget para câmera docking |
| `SpaceAudioComponent` | Sons: Thrust, Crash, Docking Success/Fail, Alertas |

---

## 8. Input Mapping (Enhanced Input - Todos Funcionando)

| Ação | Binding | Triggers |
|------|---------|----------|
| Move (3D Vector) | `MoveAction` | Triggered/Completed/Canceled |
| Look (2D) | `LookAction` | Triggered/Completed/Canceled |
| Roll | `RollAction` | Triggered/Completed/Canceled |
| Throttle | `ThrottleAction` | Triggered (float) |
| Precision Mode | `PrecisionAction` | Started/Completed |
| Free Look | `FreeLookAction` | Started (Toggle) |
| Dock Button | `DockAction` | Started |
| Headlights | `ToggleHeadlightsAction` | Started |
| Pause | `PauseAction` | Started |

---

## 9. Estrutura de Missão (SpacePlayerController)

### Estados (`EMissionState`)
`Cinematic` → `Playing` → `Paused` / `Victory` / `GameOver`

### Fluxo Implementado
1. **BeginPlay** → Cria HUD, adiciona MappingContext
2. **Cinematic** (se `bStartInCinematic`) → LevelSequence → `OnIntroCinematicFinished` → `Playing`
3. **Playing** → Input GameOnly, HUD visível
4. **Pause** (P) → `Paused` → Cria `MissionMenuWidget` (Restart/Main Menu/Continue)
5. **Victory/GameOver** → UI Only, Mouse Cursor, Tela correspondente

---

## 10. GameMode & GameInstance

### DockSimGameMode (Funcional)
- `PlayerControllerClass = ASpacePlayerController`
- `DefaultPawnClass` → Vem de `DockSimGameInstance.SelectedShipData.ActualShipClass` (fallback: `ASpaceShipPawn`)
- Liga eventos: `Ship.OnDockingCompleted` → Victory

### DockSimGameInstance (Existe, não configurado no .uproject)
- Persiste `SelectedShipData` (UShipDataAsset) entre menus/jogo
- **Pendente**: Adicionar `"GameInstanceClass": "/Script/DockSim.DockSimGameInstance"` no `.uproject`

---

## 11. Sistema de Menu (Parcial)

| Classe | Status                            | Notas |
|--------|-----------------------------------|-------|
| `MissionMenuWidget` | ✅ Funcional                      | Pause/Victory/GameOver – Restart/MainMenu/Continue |
| `MenuManagerWidget` | ✅ Funcional                      | Menu principal (Ship/Stage/Controls/Settings) |
| `ShipCarouselManager` | ✅ Funcional                      | Actor para seleção visual de naves |
| `ResourceWidget` | ✅ Funcional | HUD combustível/throttle/headlights |

---

## 12. Variáveis de Ajuste (Editor)

| Variável | Componente | Valor Típico |
|----------|------------|--------------|
| `ThrustForce` | MovementComp | 25.000 – 50.000 |
| `TorqueForce` | MovementComp | 50.000+ |
| `AngularDamping` | ShipMesh | 0.8 – 2.0 |
| `MassInKg` | ShipMesh | 2.000 – 5.000 |
| `SimulationMultiplier` | TelemetryComp | 100.0 |
| `MaxSafeDockingSpeed` | DockingPort | 2.0 m/s |
| `LinearInputSmoothing` | MovementComp | 5.0–10.0 |
| `InputCurveExponent` | MovementComp | 1.5–2.5 |

---

## 13. Pendências Conhecidas

- [ ] Instanciar `MenuManagerWidget` + `ResourceWidget` no HUD
- [ ] Spawnar `ShipCarouselManager` no nível do menu
- [ ] Configurar `GameInstanceClass` no `.uproject`
- [ ] Sistema de missões/contratos (DataAssets)
- [ ] Progressão de licenças
- [ ] Variações de porta (Rotacional, Deriva, Danificado)
- [ ] Salvamento de progresso

---

## 14. Fora de Escopo

- Combate
- Multiplayer
- EVA / FPS
- Economia / Trading