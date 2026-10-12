#ifndef CONFIG_H
#define CONFIG_H

/* ============================================================
 * config.h - Todas as constantes do jogo ficam aqui.
 * Quer ajustar balanceamento, velocidade, tamanho? Mexa AQUI.
 * ============================================================ */

/* ---- Tela ----
 * O jogo é desenhado em uma resolução "virtual" fixa (igual à da arte
 * do cenário). A janela real é essa resolução multiplicada por
 * ESCALA_JANELA. Assim o jogo funciona em qualquer monitor. */
#define LARGURA_VIRTUAL   1408
#define ALTURA_VIRTUAL    768
#define ESCALA_JANELA     0.90f

/* ---- Tempo / FPS (RNF02) ---- */
#define FPS               60
#define DT                (1.0f / (float)FPS)   /* duração de 1 frame, em segundos */

/* ---- Regras de negócio (RN03) ---- */
#define TEMPO_TOTAL_SEG       600     /* 10 minutos (baixe para 20 p/ testar o Game Over) */
#define PONTOS_INICIAIS       10000
#define PERDA_POR_SEGUNDO     10
#define PENALIDADE_ERRO       500
#define BONUS_PRIMEIRA        1000
#define ENIGMAS_FASE1         3

/* ---- Jogador (spritesheet 7 colunas x 4 linhas, frames de 48x48) ---- */
#define SPRITE_LARGURA        48
#define SPRITE_ALTURA         48
#define SPRITE_FRAMES         7       /* frame 0 = parado, 1..6 = andando */
#define ESCALA_JOGADOR        1.8f
#define VELOCIDADE_JOGADOR    230.0f  /* pixels por segundo */
#define TEMPO_POR_FRAME_ANIM  0.09f   /* segundos entre frames da caminhada */

/* Hitbox de colisão = retângulo pequeno nos PÉS do personagem */
#define HITBOX_LARGURA        28.0f
#define HITBOX_ALTURA         14.0f

/* Posição inicial do jogador na Fase 1 (coordenadas dos PÉS) */
#define INICIO_X              320.0f
#define INICIO_Y              420.0f

#endif
