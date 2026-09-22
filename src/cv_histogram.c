// Projeto 1 (Proj1) - Computacao Visual - UPM FCI
// Modulo: calculo, analise e desenho do histograma.

#include <math.h>
#include "cv_histogram.h"

// Limiares adotados pelo grupo (justificados no README.md).
// Media de intensidade: faixa [0, 255] dividida em tres partes iguais.
#define CV_MEAN_DARK_MAX 85.0
#define CV_MEAN_MID_MAX 170.0
// Desvio padrao: referencia usual em processamento de imagens.
#define CV_STDDEV_LOW_MAX 40.0
#define CV_STDDEV_MID_MAX 70.0

bool cv_histogram_compute(CVHistogram *hist, SDL_Surface *gray_surface)
{
  if (!hist)
  {
    SDL_Log("*** Erro: histograma invalido (hist == NULL).");
    return false;
  }

  SDL_zerop(hist);

  if (!gray_surface)
  {
    SDL_Log("*** Erro: superficie invalida (gray_surface == NULL).");
    return false;
  }

  const SDL_PixelFormatDetails *format =
      SDL_GetPixelFormatDetails(gray_surface->format);
  if (!format)
  {
    SDL_Log("*** Erro ao obter detalhes do formato de pixel: %s",
            SDL_GetError());
    return false;
  }

  if (!SDL_LockSurface(gray_surface))
  {
    SDL_Log("*** Erro ao bloquear a superficie: %s", SDL_GetError());
    return false;
  }

  // Percorremos linha a linha usando o `pitch`, porque o numero de bytes de
  // uma linha nem sempre e igual a largura * bytes por pixel.
  for (int y = 0; y < gray_surface->h; ++y)
  {
    const Uint32 *row =
        (const Uint32 *)((const Uint8 *)gray_surface->pixels + (size_t)y * gray_surface->pitch);

    for (int x = 0; x < gray_surface->w; ++x)
    {
      Uint8 r = 0;
      Uint8 g = 0;
      Uint8 b = 0;
      SDL_GetRGB(row[x], format, NULL, &r, &g, &b);

      // A imagem ja esta em escala de cinza (R = G = B); usamos o canal R.
      hist->counts[r]++;
    }
  }

  SDL_UnlockSurface(gray_surface);

  hist->total = (Uint64)gray_surface->w * (Uint64)gray_surface->h;
  if (hist->total == 0)
  {
    SDL_Log("*** Erro: imagem sem pixels.");
    return false;
  }

  // Media: soma(i * h(i)) / total.
  double sum = 0.0;
  for (int i = 0; i < CV_HISTOGRAM_LEVELS; ++i)
  {
    sum += (double)i * (double)hist->counts[i];

    if (hist->counts[i] > hist->max_count)
      hist->max_count = hist->counts[i];
  }
  hist->mean = sum / (double)hist->total;

  // Desvio padrao: raiz da media dos quadrados dos desvios.
  double variance = 0.0;
  for (int i = 0; i < CV_HISTOGRAM_LEVELS; ++i)
  {
    const double diff = (double)i - hist->mean;
    variance += diff * diff * (double)hist->counts[i];
  }
  variance /= (double)hist->total;
  hist->stddev = SDL_sqrt(variance);

  return true;
}

const char *cv_histogram_brightness_label(const CVHistogram *hist)
{
  if (!hist)
    return "indefinida";

  if (hist->mean < CV_MEAN_DARK_MAX)
    return "escura";

  if (hist->mean <= CV_MEAN_MID_MAX)
    return "média";

  return "clara";
}

const char *cv_histogram_contrast_label(const CVHistogram *hist)
{
  if (!hist)
    return "indefinido";

  if (hist->stddev < CV_STDDEV_LOW_MAX)
    return "baixo";

  if (hist->stddev < CV_STDDEV_MID_MAX)
    return "médio";

  return "alto";
}

void cv_histogram_draw(const CVHistogram *hist, SDL_Renderer *renderer,
                       const SDL_FRect *area)
{
  if (!hist || !renderer || !area)
    return;

  // Fundo e moldura da area do histograma.
  SDL_SetRenderDrawColor(renderer, 24, 26, 32, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRect(renderer, area);

  SDL_SetRenderDrawColor(renderer, 90, 96, 110, SDL_ALPHA_OPAQUE);
  SDL_RenderRect(renderer, area);

  if (hist->max_count == 0)
    return;

  // Cada um dos 256 niveis ocupa uma fatia proporcional da largura disponivel.
  const float bar_width = area->w / (float)CV_HISTOGRAM_LEVELS;

  SDL_SetRenderDrawColor(renderer, 130, 190, 255, SDL_ALPHA_OPAQUE);

  for (int i = 0; i < CV_HISTOGRAM_LEVELS; ++i)
  {
    if (hist->counts[i] == 0)
      continue;

    // Altura proporcional a maior contagem do histograma.
    const float ratio = (float)hist->counts[i] / (float)hist->max_count;
    const float bar_height = ratio * (area->h - 2.0f);

    const SDL_FRect bar = {
      .x = area->x + (float)i * bar_width,
      .y = area->y + area->h - 1.0f - bar_height,
      .w = bar_width,
      .h = bar_height
    };

    SDL_RenderFillRect(renderer, &bar);
  }

  // Marca a posicao da media de intensidade sobre o histograma.
  const float mean_x = area->x + (float)(hist->mean / 255.0) * area->w;
  SDL_SetRenderDrawColor(renderer, 255, 170, 80, SDL_ALPHA_OPAQUE);
  SDL_RenderLine(renderer, mean_x, area->y + 1.0f, mean_x,
                 area->y + area->h - 1.0f);
}
