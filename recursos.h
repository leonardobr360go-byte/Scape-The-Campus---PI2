#ifndef RECURSOS_H
#define RECURSOS_H

#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

/* Carrega "assets/<nome>". Se não achar, tenta ../assets/ e ../../assets/
 * (útil quando o .exe é aberto direto da pasta x64/Debug).
 * Retorna NULL se falhar. */
ALLEGRO_BITMAP *carregar_bitmap(const char *nome, bool suavizar);

/* Carrega a fonte TTF em um tamanho. Se não achar, usa a fonte embutida
 * do Allegro (pequena e sem acentos) para o jogo não travar. */
ALLEGRO_FONT *carregar_fonte(int tamanho);

#endif
