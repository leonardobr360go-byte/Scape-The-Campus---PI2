#ifndef JOGADOR_H
#define JOGADOR_H

#include <stdbool.h>
#include <allegro5/allegro.h>
#include "mapa.h"

/* A ordem bate com as LINHAS do spritesheet */
typedef enum {
    DIR_BAIXO    = 0,   /* linha 0: de frente   */
    DIR_CIMA     = 1,   /* linha 1: de costas   */
    DIR_ESQUERDA = 2,   /* linha 2: lado (mochila à direita) */
    DIR_DIREITA  = 3    /* linha 3: lado (mochila à esquerda) */
} Direcao;

typedef struct {
    float x, y;            /* posição dos PÉS no cenário */
    Direcao dir;
    bool  movendo;
    int   frame;           /* coluna do spritesheet (0 = parado) */
    float tempo_anim;
    ALLEGRO_BITMAP *sheet;
} Jogador;

bool jogador_carregar(Jogador *j);
void jogador_destruir(Jogador *j);
void jogador_posicionar(Jogador *j, float x, float y);
void jogador_parar(Jogador *j);

/* teclas = vetor indexado por ALLEGRO_KEY_* (true = pressionada) */
void jogador_atualizar(Jogador *j, const bool *teclas, const Mapa *mapa);
void jogador_desenhar(const Jogador *j);

#endif
