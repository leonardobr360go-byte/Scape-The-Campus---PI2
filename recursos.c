#include <stdio.h>
#include <allegro5/allegro_ttf.h>
#include "recursos.h"

static const char *PREFIXOS[] = { "assets/", "../assets/", "../../assets/" };
#define N_PREFIXOS ((int)(sizeof(PREFIXOS) / sizeof(PREFIXOS[0])))

ALLEGRO_BITMAP *carregar_bitmap(const char *nome, bool suavizar)
{
    char caminho[256];
    ALLEGRO_BITMAP *bmp = NULL;
    int i;

    /* Pixel art: filtro "nearest" (padrão) mantém os pixels nítidos.
     * Para o cenário grande (que é reduzido), filtro linear fica melhor. */
    int flags_antigas = al_get_new_bitmap_flags();
    if (suavizar)
        al_set_new_bitmap_flags(flags_antigas | ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

    for (i = 0; i < N_PREFIXOS && bmp == NULL; i++) {
        snprintf(caminho, sizeof(caminho), "%s%s", PREFIXOS[i], nome);
        bmp = al_load_bitmap(caminho);
    }

    al_set_new_bitmap_flags(flags_antigas);

    if (bmp == NULL)
        fprintf(stderr, "ERRO: nao achei a imagem '%s' (pasta assets)\n", nome);
    return bmp;
}

ALLEGRO_FONT *carregar_fonte(int tamanho)
{
    char caminho[256];
    ALLEGRO_FONT *fonte = NULL;
    int i;

    for (i = 0; i < N_PREFIXOS && fonte == NULL; i++) {
        snprintf(caminho, sizeof(caminho), "%sfonte.ttf", PREFIXOS[i]);
        fonte = al_load_ttf_font(caminho, tamanho, 0);
    }
    if (fonte == NULL)  /* plano B: fonte do Windows */
        fonte = al_load_ttf_font("C:/Windows/Fonts/consolab.ttf", tamanho, 0);
    if (fonte == NULL) {
        fprintf(stderr, "AVISO: fonte.ttf nao encontrada, usando fonte embutida\n");
        fonte = al_create_builtin_font();
    }
    return fonte;
}
