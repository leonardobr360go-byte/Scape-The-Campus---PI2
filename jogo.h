#ifndef JOGO_H
#define JOGO_H

#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "config.h"
#include "mapa.h"
#include "jogador.h"
#include "enigma1.h"

/* Máquina de Estados Finitos (RNF03): o jogo está SEMPRE em um destes estados */
typedef enum {
    ESTADO_MENU,
    ESTADO_FASE1,
    ESTADO_ENIGMA1,     /* painel do enigma aberto (o relógio NÃO pausa - RN06) */
    ESTADO_VITORIA,
    ESTADO_GAMEOVER,
    ESTADO_SAIR
} Estado;

typedef struct {
    /* Allegro */
    ALLEGRO_DISPLAY     *display;
    ALLEGRO_EVENT_QUEUE *fila;
    ALLEGRO_TIMER       *timer;
    ALLEGRO_FONT *fonte_p, *fonte_m, *fonte_g;   /* pequena, média, grande */

    /* Estado geral */
    Estado estado;
    bool   teclas[ALLEGRO_KEY_MAX];
    bool   debug;
    unsigned long frames;

    /* Regras */
    float tempo_restante;       /* segundos */
    float acum_pontos;          /* acumula frames até completar 1 segundo */
    int   pontuacao;
    int   enigmas_resolvidos;

    /* Mensagem temporária na tela */
    char  mensagem[96];
    float mensagem_tempo;

    /* Objetos do jogo */
    Mapa     mapa;
    Jogador  jogador;
    Enigma1  enigma1;
} Jogo;

bool jogo_iniciar(Jogo *j);
void jogo_encerrar(Jogo *j);
void jogo_tratar_evento(Jogo *j, const ALLEGRO_EVENT *ev);
void jogo_atualizar(Jogo *j);   /* chamado 60x por segundo */
void jogo_desenhar(Jogo *j);

#endif
