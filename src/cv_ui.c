// Projeto 1 (Proj1) - Computacao Visual - UPM FCI
// Modulo: fonte, textos e botoes desenhados com primitivas da SDL.

#include <stdarg.h>
#include "cv_ui.h"

enum
{
  CV_TEXT_BUFFER_MAX = 256
};

// Fonte distribuida junto do projeto (garante o mesmo resultado em qualquer
// sistema operacional).
static const char *CV_BUNDLED_FONT = "assets/fonts/DejaVuSans.ttf";

// Fontes do sistema usadas apenas como plano B, caso a pasta assets nao esteja
// junto do executavel.
static const char *const CV_SYSTEM_FONTS[] = {
#if defined(_WIN32)
  "C:/Windows/Fonts/segoeui.ttf",
  "C:/Windows/Fonts/arial.ttf",
  "C:/Windows/Fonts/tahoma.ttf",
  "C:/Windows/Fonts/verdana.ttf",
#elif defined(__APPLE__)
  "/System/Library/Fonts/Supplemental/Arial.ttf",
  "/Library/Fonts/Arial.ttf",
  "/System/Library/Fonts/Helvetica.ttc",
#else
  "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
  "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
  "/usr/share/fonts/TTF/DejaVuSans.ttf",
  "/usr/share/fonts/dejavu/DejaVuSans.ttf",
#endif
  NULL
};

TTF_Font *cv_font_open(float ptsize)
{
  // 1) Fonte que acompanha o executavel. SDL_GetBasePath() devolve a pasta do
  //    programa em qualquer sistema operacional, com o separador correto.
  const char *base_path = SDL_GetBasePath();

  if (base_path)
  {
    char path[1024];
    SDL_snprintf(path, sizeof(path), "%s%s", base_path, CV_BUNDLED_FONT);

    TTF_Font *font = TTF_OpenFont(path, ptsize);
    if (font)
      return font;

    SDL_Log("*** Aviso: nao foi possivel abrir a fonte do projeto (\"%s\"): %s",
            path, SDL_GetError());
  }

  // 2) Plano B: fontes conhecidas do sistema operacional.
  for (int i = 0; CV_SYSTEM_FONTS[i] != NULL; ++i)
  {
    TTF_Font *font = TTF_OpenFont(CV_SYSTEM_FONTS[i], ptsize);
    if (font)
    {
      SDL_Log("\tUsando fonte do sistema: %s", CV_SYSTEM_FONTS[i]);
      return font;
    }
  }

  SDL_Log("*** Erro: nenhuma fonte pode ser carregada.");
  return NULL;
}

/** Desenha uma string ja formatada na posicao indicada. */
static void cv_text_blit(SDL_Renderer *renderer, TTF_Font *font, float x,
                         float y, SDL_Color color, const char *text)
{
  if (!renderer || !font || !text || text[0] == '\0')
    return;

  // Em SDL_ttf 3, o tamanho do texto e um parametro; 0 significa "string
  // terminada em nulo".
  SDL_Surface *surface = TTF_RenderText_Blended(font, text, 0, color);
  if (!surface)
  {
    SDL_Log("*** Erro ao renderizar o texto \"%s\": %s", text, SDL_GetError());
    return;
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!texture)
  {
    SDL_Log("*** Erro ao criar a textura do texto: %s", SDL_GetError());
    SDL_DestroySurface(surface);
    return;
  }

  const SDL_FRect destination = {
    .x = x, .y = y, .w = (float)surface->w, .h = (float)surface->h
  };

  SDL_RenderTexture(renderer, texture, NULL, &destination);

  SDL_DestroyTexture(texture);
  SDL_DestroySurface(surface);
}

void cv_text_draw(SDL_Renderer *renderer, TTF_Font *font, float x, float y,
                  SDL_Color color, const char *fmt, ...)
{
  char buffer[CV_TEXT_BUFFER_MAX];

  va_list args;
  va_start(args, fmt);
  SDL_vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);

  cv_text_blit(renderer, font, x, y, color, buffer);
}

void cv_text_draw_centered(SDL_Renderer *renderer, TTF_Font *font,
                           const SDL_FRect *area, SDL_Color color,
                           const char *text)
{
  if (!renderer || !font || !area || !text)
    return;

  int width = 0;
  int height = 0;
  if (!TTF_GetStringSize(font, text, 0, &width, &height))
  {
    SDL_Log("*** Erro ao medir o texto \"%s\": %s", text, SDL_GetError());
    return;
  }

  const float x = area->x + (area->w - (float)width) * 0.5f;
  const float y = area->y + (area->h - (float)height) * 0.5f;

  cv_text_blit(renderer, font, x, y, color, text);
}

void cv_button_init(CVButton *button, float x, float y, float w, float h,
                    const char *label)
{
  if (!button)
    return;

  button->rect.x = x;
  button->rect.y = y;
  button->rect.w = w;
  button->rect.h = h;
  button->state = CV_BUTTON_NORMAL;
  button->armed = false;

  cv_button_set_label(button, label);
}

void cv_button_set_label(CVButton *button, const char *label)
{
  if (!button)
    return;

  if (!label)
  {
    button->label[0] = '\0';
    return;
  }

  SDL_strlcpy(button->label, label, sizeof(button->label));
}

bool cv_button_handle_event(CVButton *button, const SDL_Event *event)
{
  if (!button || !event)
    return false;

  bool clicked = false;

  switch (event->type)
  {
  case SDL_EVENT_MOUSE_MOTION:
  {
    const SDL_FPoint point = { event->motion.x, event->motion.y };
    const bool inside = SDL_PointInRectFloat(&point, &button->rect);

    if (button->armed)
      button->state = inside ? CV_BUTTON_PRESSED : CV_BUTTON_NORMAL;
    else
      button->state = inside ? CV_BUTTON_HOVER : CV_BUTTON_NORMAL;
    break;
  }

  case SDL_EVENT_MOUSE_BUTTON_DOWN:
  {
    if (event->button.button != SDL_BUTTON_LEFT)
      break;

    const SDL_FPoint point = { event->button.x, event->button.y };
    if (SDL_PointInRectFloat(&point, &button->rect))
    {
      button->armed = true;
      button->state = CV_BUTTON_PRESSED;
    }
    break;
  }

  case SDL_EVENT_MOUSE_BUTTON_UP:
  {
    if (event->button.button != SDL_BUTTON_LEFT)
      break;

    const SDL_FPoint point = { event->button.x, event->button.y };
    const bool inside = SDL_PointInRectFloat(&point, &button->rect);

    // O clique so e considerado quando o botao do mouse e pressionado e
    // solto dentro do mesmo retangulo (comportamento usual de interfaces).
    if (button->armed && inside)
      clicked = true;

    button->armed = false;
    button->state = inside ? CV_BUTTON_HOVER : CV_BUTTON_NORMAL;
    break;
  }

  case SDL_EVENT_WINDOW_MOUSE_LEAVE:
    button->armed = false;
    button->state = CV_BUTTON_NORMAL;
    break;

  default:
    break;
  }

  return clicked;
}

void cv_button_draw(const CVButton *button, SDL_Renderer *renderer,
                    TTF_Font *font)
{
  if (!button || !renderer)
    return;

  SDL_Color fill;
  switch (button->state)
  {
  case CV_BUTTON_HOVER: // azul claro
    fill = (SDL_Color){ 96, 165, 250, SDL_ALPHA_OPAQUE };
    break;
  case CV_BUTTON_PRESSED: // azul escuro
    fill = (SDL_Color){ 23, 58, 120, SDL_ALPHA_OPAQUE };
    break;
  case CV_BUTTON_NORMAL: // azul (estado neutro)
  default:
    fill = (SDL_Color){ 37, 99, 195, SDL_ALPHA_OPAQUE };
    break;
  }

  SDL_SetRenderDrawColor(renderer, fill.r, fill.g, fill.b, fill.a);
  SDL_RenderFillRect(renderer, &button->rect);

  SDL_SetRenderDrawColor(renderer, 225, 230, 240, SDL_ALPHA_OPAQUE);
  SDL_RenderRect(renderer, &button->rect);

  const SDL_Color text_color = { 255, 255, 255, SDL_ALPHA_OPAQUE };
  cv_text_draw_centered(renderer, font, &button->rect, text_color,
                        button->label);
}
