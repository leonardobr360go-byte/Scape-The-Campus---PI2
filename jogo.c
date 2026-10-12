#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>
#include "jogo.h"
#include "recursos.h"

/* ============================================================
 *  Funções auxiliares
 * ============================================================ */

static void mostrar_mensagem(Jogo *j, const char *texto)
{
    snprintf(j->mensagem, sizeof(j->mensagem), "%s", texto);
    j->mensagem_tempo = 2.5f;
}

/* Começa (ou recomeça) uma partida do zero */
static void novo_jogo(Jogo *j)
{
    j->tempo_restante = (float)TEMPO_TOTAL_SEG;
    j->acum_pontos = 0.0f;
    j->pontuacao = PONTOS_INICIAIS;
    j->enigmas_resolvidos = 0;
    j->mensagem[0] = '\0';
    j->mensagem_tempo = 0.0f;
    memset(j->teclas, 0, sizeof(j->teclas));

    jogador_posicionar(&j->jogador, INICIO_X, INICIO_Y);
    enigma1_iniciar(&j->enigma1);

    j->estado = ESTADO_FASE1;
}

/* RN02: tempo 00:00 ou pontuação 0 = derrota */
static void verificar_derrota(Jogo *j)
{
    if (j->pontuacao < 0) j->pontuacao = 0;
    if (j->tempo_restante < 0.0f) j->tempo_restante = 0.0f;

    if (j->tempo_restante <= 0.0f || j->pontuacao <= 0) {
        j->estado = ESTADO_GAMEOVER;
        jogador_parar(&j->jogador);
    }
}

/* RF07: relógio e pontuação caem continuamente */
static void atualizar_relogio(Jogo *j)
{
    j->tempo_restante -= DT;
    j->acum_pontos += DT;
    while (j->acum_pontos >= 1.0f) {
        j->acum_pontos -= 1.0f;
        j->pontuacao -= PERDA_POR_SEGUNDO;
    }
    verificar_derrota(j);
}

static void aplicar_resultado_enigma1(Jogo *j, ResultadoE1 r)
{
    switch (r) {
    case E1_ACERTOU_PRIMEIRA:
        j->pontuacao += BONUS_PRIMEIRA;
        j->enigmas_resolvidos++;
        break;
    case E1_ACERTOU:
        j->enigmas_resolvidos++;
        break;
    case E1_ERROU:
        j->pontuacao -= PENALIDADE_ERRO;
        verificar_derrota(j);
        break;
    case E1_FECHAR:
        j->estado = ESTADO_FASE1;
        break;
    case E1_NADA:
    default:
        break;
    }
}

/* Tecla E na Fase 1 (RF03) */
static void interagir(Jogo *j)
{
    const Interagivel *it = mapa_interacao_em(&j->mapa, j->jogador.x, j->jogador.y);
    if (it == NULL) return;

    switch (it->id) {
    case INT_ENIGMA1:
        if (j->enigma1.resolvido) {
            mostrar_mensagem(j, "Enigma 1 ja resolvido.");
        } else {
            jogador_parar(&j->jogador);
            j->estado = ESTADO_ENIGMA1;
        }
        break;
    case INT_ENIGMA2:
        mostrar_mensagem(j, "Enigma 2: em construcao.");
        break;
    case INT_ENIGMA3:
        mostrar_mensagem(j, "Enigma 3: em construcao.");
        break;
    case INT_PORTA:
        if (j->enigmas_resolvidos < ENIGMAS_FASE1)
            mostrar_mensagem(j, "Porta trancada: resolva os 3 enigmas.");   /* RN04 */
        else
            mostrar_mensagem(j, "Fase 2: em construcao.");
        break;
    }
}

/* ============================================================
 *  Inicialização / encerramento
 * ============================================================ */

bool jogo_iniciar(Jogo *j)
{
    memset(j, 0, sizeof(*j));   /* tudo zerado: ponteiros NULL, bools false */
    srand((unsigned int)time(NULL));

    if (!al_init())            { fprintf(stderr, "Falha: al_init\n");          return false; }
    if (!al_install_keyboard()){ fprintf(stderr, "Falha: teclado\n");          return false; }
    if (!al_install_mouse())   { fprintf(stderr, "Falha: mouse\n");            return false; }
    if (!al_init_primitives_addon()) { fprintf(stderr, "Falha: primitives\n"); return false; }
    if (!al_init_image_addon())      { fprintf(stderr, "Falha: image\n");      return false; }
    if (!al_init_font_addon())       { fprintf(stderr, "Falha: font\n");       return false; }
    if (!al_init_ttf_addon())        { fprintf(stderr, "Falha: ttf\n");        return false; }

    j->display = al_create_display((int)(LARGURA_VIRTUAL * ESCALA_JANELA),
                                   (int)(ALTURA_VIRTUAL * ESCALA_JANELA));
    if (!j->display) { fprintf(stderr, "Falha: display\n"); return false; }
    al_set_window_title(j->display, "Escape The Campus: Core Runtime");

    j->timer = al_create_timer(1.0 / FPS);
    j->fila  = al_create_event_queue();
    if (!j->timer || !j->fila) { fprintf(stderr, "Falha: timer/fila\n"); return false; }

    al_register_event_source(j->fila, al_get_display_event_source(j->display));
    al_register_event_source(j->fila, al_get_timer_event_source(j->timer));
    al_register_event_source(j->fila, al_get_keyboard_event_source());
    al_register_event_source(j->fila, al_get_mouse_event_source());

    j->fonte_p = carregar_fonte(18);
    j->fonte_m = carregar_fonte(28);
    j->fonte_g = carregar_fonte(52);
    if (!j->fonte_p || !j->fonte_m || !j->fonte_g) { fprintf(stderr, "Falha: fontes\n"); return false; }

    if (!mapa_carregar_fase1(&j->mapa)) return false;
    if (!jogador_carregar(&j->jogador)) return false;

    novo_jogo(j);
    j->estado = ESTADO_MENU;

    al_start_timer(j->timer);
    return true;
}

/* Todo al_create/al_load tem seu al_destroy. Cada função aceita NULL,
 * então dá para chamar mesmo se a inicialização falhou no meio. */
void jogo_encerrar(Jogo *j)
{
    jogador_destruir(&j->jogador);
    mapa_destruir(&j->mapa);
    if (j->fonte_g) al_destroy_font(j->fonte_g);
    if (j->fonte_m) al_destroy_font(j->fonte_m);
    if (j->fonte_p) al_destroy_font(j->fonte_p);
    if (j->fila)    al_destroy_event_queue(j->fila);
    if (j->timer)   al_destroy_timer(j->timer);
    if (j->display) al_destroy_display(j->display);
}

/* ============================================================
 *  Eventos (teclado, mouse, janela)
 * ============================================================ */

void jogo_tratar_evento(Jogo *j, const ALLEGRO_EVENT *ev)
{
    switch (ev->type) {

    case ALLEGRO_EVENT_DISPLAY_CLOSE:
        j->estado = ESTADO_SAIR;
        break;

    case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:      /* janela perdeu o foco */
        memset(j->teclas, 0, sizeof(j->teclas));
        break;

    case ALLEGRO_EVENT_KEY_UP:
        j->teclas[ev->keyboard.keycode] = false;
        break;

    case ALLEGRO_EVENT_KEY_DOWN: {
        int k = ev->keyboard.keycode;
        j->teclas[k] = true;

        if (k == ALLEGRO_KEY_F1) j->debug = !j->debug;

        switch (j->estado) {
        case ESTADO_MENU:
            if (k == ALLEGRO_KEY_ENTER) novo_jogo(j);
            else if (k == ALLEGRO_KEY_ESCAPE) j->estado = ESTADO_SAIR;
            break;
        case ESTADO_FASE1:
            if (k == ALLEGRO_KEY_E) interagir(j);
            else if (k == ALLEGRO_KEY_ESCAPE) j->estado = ESTADO_MENU;
            break;
        case ESTADO_ENIGMA1:
            aplicar_resultado_enigma1(j, enigma1_tecla(&j->enigma1, k));
            break;
        case ESTADO_VITORIA:
        case ESTADO_GAMEOVER:
            if (k == ALLEGRO_KEY_ENTER) novo_jogo(j);          /* reiniciar rápido */
            else if (k == ALLEGRO_KEY_ESCAPE) j->estado = ESTADO_MENU;
            break;
        default:
            break;
        }
        break;
    }

    case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
        if (j->estado == ESTADO_ENIGMA1 && ev->mouse.button == 1) {
            /* Converte pixels da janela -> coordenadas virtuais */
            float mx = ev->mouse.x / ESCALA_JANELA;
            float my = ev->mouse.y / ESCALA_JANELA;
            aplicar_resultado_enigma1(j, enigma1_clique(&j->enigma1, mx, my));
        }
        break;

    default:
        break;
    }
}

/* ============================================================
 *  UPDATE (lógica) - 60x por segundo
 * ============================================================ */

void jogo_atualizar(Jogo *j)
{
    j->frames++;
    if (j->mensagem_tempo > 0.0f) j->mensagem_tempo -= DT;

    switch (j->estado) {
    case ESTADO_FASE1:
        jogador_atualizar(&j->jogador, j->teclas, &j->mapa);
        atualizar_relogio(j);
        break;
    case ESTADO_ENIGMA1:
        if (enigma1_atualizar(&j->enigma1)) j->estado = ESTADO_FASE1;
        atualizar_relogio(j);       /* RN06: o tempo continua correndo */
        break;
    default:
        break;
    }
}

/* ============================================================
 *  DRAW
 * ============================================================ */

static void desenhar_hud(const Jogo *j)
{
    int seg = (int)(j->tempo_restante + 0.999f);
    ALLEGRO_COLOR cor_tempo = (seg <= 60) ? al_map_rgb(255, 80, 80) : al_map_rgb(0, 255, 200);

    al_draw_filled_rectangle(0, 0, LARGURA_VIRTUAL, 56, al_map_rgb(8, 14, 26));
    al_draw_line(0, 56, LARGURA_VIRTUAL, 56, al_map_rgb(0, 255, 200), 2);

    al_draw_text(j->fonte_m, al_map_rgb(200, 220, 240), 20, 12, 0, "FASE 1: LAB DE HARDWARE");
    al_draw_textf(j->fonte_m, cor_tempo, LARGURA_VIRTUAL / 2, 12, ALLEGRO_ALIGN_CENTRE,
                  "TEMPO %02d:%02d", seg / 60, seg % 60);
    al_draw_textf(j->fonte_m, al_map_rgb(255, 220, 90), LARGURA_VIRTUAL - 20, 12, ALLEGRO_ALIGN_RIGHT,
                  "SCORE %d   ENIGMAS %d/%d", j->pontuacao, j->enigmas_resolvidos, ENIGMAS_FASE1);
}

static void desenhar_mundo(const Jogo *j)
{
    const Interagivel *it;

    mapa_desenhar(&j->mapa, j->debug, j->fonte_p);
    jogador_desenhar(&j->jogador);

    if (j->debug) {   /* hitbox dos pés + coordenadas, para ajustar o mapa */
        al_draw_rectangle(j->jogador.x - HITBOX_LARGURA / 2, j->jogador.y - HITBOX_ALTURA,
                          j->jogador.x + HITBOX_LARGURA / 2, j->jogador.y,
                          al_map_rgb(255, 255, 0), 2);
        al_draw_textf(j->fonte_p, al_map_rgb(255, 255, 0), 20, 70, 0,
                      "x=%.0f y=%.0f  (F1 = debug)", j->jogador.x, j->jogador.y);
    }

    /* Aviso "[E] Interagir" quando estiver perto de algo */
    if (j->estado == ESTADO_FASE1) {
        it = mapa_interacao_em(&j->mapa, j->jogador.x, j->jogador.y);
        if (it != NULL) {
            al_draw_filled_rectangle(LARGURA_VIRTUAL / 2 - 300, ALTURA_VIRTUAL - 64,
                                     LARGURA_VIRTUAL / 2 + 300, ALTURA_VIRTUAL - 18, al_map_rgba(8, 14, 26, 220));
            al_draw_textf(j->fonte_m, al_map_rgb(255, 255, 255), LARGURA_VIRTUAL / 2, ALTURA_VIRTUAL - 58,
                          ALLEGRO_ALIGN_CENTRE, "[E] %s", it->rotulo);
        }
    }

    if (j->mensagem_tempo > 0.0f) {
        al_draw_filled_rectangle(LARGURA_VIRTUAL / 2 - 380, 70, LARGURA_VIRTUAL / 2 + 380, 116, al_map_rgba(8, 14, 26, 230));
        al_draw_text(j->fonte_m, al_map_rgb(255, 220, 90), LARGURA_VIRTUAL / 2, 76, ALLEGRO_ALIGN_CENTRE, j->mensagem);
    }
}

void jogo_desenhar(Jogo *j)
{
    ALLEGRO_TRANSFORM t;
    float cx = LARGURA_VIRTUAL / 2.0f;

    /* Tudo é desenhado em coordenadas virtuais; a transformação ajusta à janela */
    al_identity_transform(&t);
    al_scale_transform(&t, ESCALA_JANELA, ESCALA_JANELA);
    al_use_transform(&t);

    al_clear_to_color(al_map_rgb(8, 12, 24));

    switch (j->estado) {

    case ESTADO_MENU:
        al_draw_text(j->fonte_g, al_map_rgb(0, 255, 200), cx, 190, ALLEGRO_ALIGN_CENTRE, "ESCAPE THE CAMPUS");
        al_draw_text(j->fonte_m, al_map_rgb(180, 120, 255), cx, 262, ALLEGRO_ALIGN_CENTRE, "C O R E   R U N T I M E");
        al_draw_text(j->fonte_p, al_map_rgb(160, 180, 200), cx, 360, ALLEGRO_ALIGN_CENTRE,
                     "WASD / setas: mover   |   E: interagir   |   Mouse: enigmas   |   F1: debug");
        al_draw_text(j->fonte_p, al_map_rgb(160, 180, 200), cx, 390, ALLEGRO_ALIGN_CENTRE,
                     "Voce tem 10 minutos. Resolva os enigmas antes do Hard Reset!");
        if ((j->frames / 30) % 2 == 0)
            al_draw_text(j->fonte_m, al_map_rgb(255, 255, 255), cx, 480, ALLEGRO_ALIGN_CENTRE, "PRESSIONE ENTER PARA JOGAR");
        al_draw_text(j->fonte_p, al_map_rgb(100, 120, 140), cx, 560, ALLEGRO_ALIGN_CENTRE, "ESC: sair");
        break;

    case ESTADO_FASE1:
        desenhar_mundo(j);
        desenhar_hud(j);
        break;

    case ESTADO_ENIGMA1:
        desenhar_mundo(j);
        enigma1_desenhar(&j->enigma1, j->fonte_p, j->fonte_m, j->fonte_g);
        desenhar_hud(j);           /* HUD por cima: o relógio continua visível */
        break;

    case ESTADO_GAMEOVER:
        al_clear_to_color(al_map_rgb(40, 0, 0));
        al_draw_text(j->fonte_g, al_map_rgb(255, 70, 70), cx, 220, ALLEGRO_ALIGN_CENTRE, "KERNEL PANIC");
        al_draw_text(j->fonte_m, al_map_rgb(255, 200, 200), cx, 300, ALLEGRO_ALIGN_CENTRE,
                     (j->tempo_restante <= 0.0f) ? "Hard Reset: o tempo acabou." : "Hard Reset: a pontuacao zerou.");
        al_draw_textf(j->fonte_m, al_map_rgb(255, 220, 90), cx, 360, ALLEGRO_ALIGN_CENTRE,
                      "Enigmas resolvidos: %d", j->enigmas_resolvidos);
        al_draw_text(j->fonte_m, al_map_rgb(255, 255, 255), cx, 460, ALLEGRO_ALIGN_CENTRE, "ENTER: tentar de novo   |   ESC: menu");
        break;

    case ESTADO_VITORIA:
        al_clear_to_color(al_map_rgb(0, 30, 20));
        al_draw_text(j->fonte_g, al_map_rgb(60, 255, 120), cx, 220, ALLEGRO_ALIGN_CENTRE, "VOCE ESCAPOU!");
        al_draw_textf(j->fonte_m, al_map_rgb(255, 220, 90), cx, 320, ALLEGRO_ALIGN_CENTRE, "Pontuacao final: %d", j->pontuacao);
        al_draw_text(j->fonte_m, al_map_rgb(255, 255, 255), cx, 460, ALLEGRO_ALIGN_CENTRE, "ENTER: jogar de novo   |   ESC: menu");
        break;

    default:
        break;
    }

    al_flip_display();
}
