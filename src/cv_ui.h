// Projeto 1 (Proj1) - Computacao Visual - UPM FCI
// Modulo: fonte, textos e botoes desenhados com primitivas da SDL.
#ifndef CV_UI_H
#define CV_UI_H

#include <stdbool.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

enum
{
  CV_BUTTON_LABEL_MAX = 64
};

/** Estados visuais do botao, conforme exigido pelo enunciado. */
typedef enum CVButtonState
{
  CV_BUTTON_NORMAL = 0, // azul
  CV_BUTTON_HOVER,      // azul claro (mouse sobre o botao)
  CV_BUTTON_PRESSED     // azul escuro (botao pressionado)
} CVButtonState;

typedef struct CVButton
{
  SDL_FRect rect;
  char label[CV_BUTTON_LABEL_MAX];
  CVButtonState state;
  bool armed; // o botao do mouse foi pressionado dentro deste retangulo
} CVButton;

/**
 * Abre a fonte do programa de forma independente de sistema operacional:
 * procura primeiro a fonte distribuida junto do executavel (via
 * SDL_GetBasePath) e, se nao encontrar, tenta fontes conhecidas do sistema.
 */
TTF_Font *cv_font_open(float ptsize);

/** Desenha texto (aceita formatacao no estilo printf). */
void cv_text_draw(SDL_Renderer *renderer, TTF_Font *font, float x, float y,
                  SDL_Color color, const char *fmt, ...);

/** Desenha texto centralizado horizontalmente dentro de um retangulo. */
void cv_text_draw_centered(SDL_Renderer *renderer, TTF_Font *font,
                           const SDL_FRect *area, SDL_Color color,
                           const char *text);

void cv_button_init(CVButton *button, float x, float y, float w, float h,
                    const char *label);
void cv_button_set_label(CVButton *button, const char *label);

/** Processa um evento de mouse. Retorna true quando o botao foi acionado. */
bool cv_button_handle_event(CVButton *button, const SDL_Event *event);

void cv_button_draw(const CVButton *button, SDL_Renderer *renderer,
                    TTF_Font *font);

#endif // CV_UI_H
