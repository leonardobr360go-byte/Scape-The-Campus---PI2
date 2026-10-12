# Escape The Campus: Core Runtime — Game Loop + Movimentação + Enigma 1

## Como montar o projeto no Visual Studio 2022
1. **Criar projeto:** Arquivo → Novo → Projeto → **Projeto Vazio** (C++). Nome: `EscapeTheCampus`.
2. **Copiar arquivos:** copie todos os `.c`, `.h` e a pasta `assets` para a **pasta do projeto**
   (a mesma onde fica o `.vcxproj`).
3. **Adicionar ao projeto:** Gerenciador de Soluções → botão direito no projeto →
   Adicionar → Item Existente → selecione todos os `.c` e `.h`.
4. **Compilar como C:** botão direito no projeto → Propriedades → C/C++ → Avançado →
   **Compilar como: Compilar como Código C (/TC)**. (Configuração: Todas as Configurações.)
5. **Allegro:** Ferramentas → Gerenciador de Pacotes NuGet → Gerenciar Pacotes NuGet para a Solução →
   instale **Allegro** (Allegro Team). Se der erro de link com PNG/TTF, instale também **AllegroDeps**.
   Cada integrante precisa fazer isso na sua máquina.
6. **Rodar:** F5 (Debug, x64 ou x86 — o mesmo para todos). O diretório de trabalho padrão do VS é a
   pasta do projeto, então `assets/` é encontrado. O jogo também tenta `../assets` e `../../assets`.

> Strings na tela estão **sem acentos de propósito**: o MSVC só trata UTF-8 em strings se o projeto tiver
> a opção `/utf-8` (C/C++ → Linha de Comando → Opções Adicionais). Com ela ligada dá para usar acentos.

## Controles
| Tecla | Ação |
|---|---|
| WASD / setas | mover |
| E | interagir (aparece "[E] ..." quando estiver na zona) |
| ENTER (no menu / game over) | jogar / reiniciar |
| Enigma 1: mouse nas chaves ou teclas 1–8 | liga/desliga chave |
| Enigma 1: ENTER ou botão CONFIRMAR | verifica |
| ESC | fecha o enigma / volta ao menu |
| **F1** | **modo debug**: mostra colisões (vermelho), zonas de interação (verde), hitbox e coordenadas |

## Arquitetura (arquivos)
| Arquivo | Papel |
|---|---|
| `main.c` | só o **Game Loop** (evento → update → draw) |
| `jogo.c/.h` | **FSM** (menu, fase1, enigma1, vitória, game over), HUD, relógio, pontuação |
| `config.h` | **todas as constantes** (tempo, pontos, velocidade, escala...) — balanceamento é aqui |
| `jogador.c/.h` | movimentação WASD, colisão por eixo (desliza na parede), animação do spritesheet |
| `mapa.c/.h` | cenário, **retângulos de colisão** e **zonas de interação** da Fase 1 |
| `enigma1.c/.h` | Enigma 1: lógica separada do desenho |
| `recursos.c/.h` | carrega imagens e fonte (com plano B) |

## Regras já implementadas (GDD)
RN03 (10.000 pts, -10/s, -500 por erro, +1.000 de primeira) · RN02 (derrota por tempo ou pontos) ·
RN05 (enigma resolvido fica inativo) · RN06 (relógio **não pausa** no enigma) · RN04 (porta trancada até 3 enigmas) ·
RF08/RF10 (Game Over "Kernel Panic") · reiniciar rápido após Game Over.

## Como cada integrante pode mexer
- **Ajustar colisão do cenário:** `mapa.c`, funções `add_obstaculo` / `add_interagivel`. Use o F1 para enxergar.
- **Testar o Game Over rápido:** em `config.h` ponha `TEMPO_TOTAL_SEG 20`.
- **Deixar o Enigma 1 mais difícil:** `enigma1.c`, `E1_MOSTRAR_PESOS 0` (esconde 128 64 32...).
- **Novo enigma:** copie o padrão do `enigma1.c/.h` (`iniciar`, `clique`, `tecla`, `atualizar`, `desenhar`),
  crie `ESTADO_ENIGMA2` em `jogo.h` e ligue em `jogo.c` (3 pontos: `interagir`, `tratar_evento`, `desenhar`).

## Observações da arte
- O `fase1.png` tem textos embutidos ("Phase 1...", "ESCAPE THE CAMPUS...", "Enigma 1/2/3"). O HUD cobre a faixa
  de cima, mas os rótulos "Enigma N" aparecem no jogo. Peçam à arte uma versão **sem texto**.
- A mesa do Enigma 1 tem **7 chaves** desenhadas; o GDD e o código usam **8**. O painel do enigma é desenhado
  por código, então funciona, mas vale alinhar o GDD com a arte.
- Spritesheet: 7 colunas × 4 linhas, frames de 48×48. Linhas: 0 frente, 1 costas, 2 esquerda, 3 direita.
  Coluna 0 = parado, 1–6 = andando.
