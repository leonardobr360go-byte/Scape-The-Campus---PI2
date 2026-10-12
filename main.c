/* ============================================================
 *  ESCAPE THE CAMPUS: CORE RUNTIME
 *  main.c - só o Game Loop. Toda a lógica está em jogo.c
 *
 *  Ciclo de cada iteração:
 *     1) espera um evento
 *     2) evento do timer (60x/s)  -> UPDATE (lógica)
 *        outros eventos           -> teclado / mouse / janela
 *     3) se a fila esvaziou e houve update -> DRAW (desenho)
 * ============================================================ */
#include <stdio.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include "jogo.h"

int main(void)
{
    Jogo jogo;
    bool redesenhar = false;

    if (!jogo_iniciar(&jogo)) {
        fprintf(stderr, "Nao foi possivel iniciar o jogo.\n");
        jogo_encerrar(&jogo);
        return 1;
    }

    while (jogo.estado != ESTADO_SAIR) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(jogo.fila, &ev);

        if (ev.type == ALLEGRO_EVENT_TIMER) {
            jogo_atualizar(&jogo);
            redesenhar = true;
        } else {
            jogo_tratar_evento(&jogo, &ev);
        }

        if (redesenhar && al_is_event_queue_empty(jogo.fila)) {
            redesenhar = false;
            jogo_desenhar(&jogo);
        }
    }

    jogo_encerrar(&jogo);
    return 0;
}
