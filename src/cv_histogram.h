// Projeto 1 (Proj1) - Computacao Visual - UPM FCI
// Modulo: calculo, analise e desenho do histograma.
#ifndef CV_HISTOGRAM_H
#define CV_HISTOGRAM_H

#include <stdbool.h>
#include <SDL3/SDL.h>

enum
{
  CV_HISTOGRAM_LEVELS = 256 // L (numero de niveis de intensidade, 8 bits)
};

/**
 * Histograma de uma imagem em escala de cinza, com as estatisticas derivadas
 * dele (media e desvio padrao das intensidades).
 */
typedef struct CVHistogram
{
  Uint32 counts[CV_HISTOGRAM_LEVELS];
  Uint32 max_count; // maior contagem, usada para normalizar o desenho
  Uint64 total;     // total de pixels
  double mean;      // media de intensidade [0, 255]
  double stddev;    // desvio padrao das intensidades
} CVHistogram;

/** Calcula o histograma e as estatisticas de uma superficie RGBA32 em cinza. */
bool cv_histogram_compute(CVHistogram *hist, SDL_Surface *gray_surface);

/** Classificacao da imagem pela media: "clara", "media" ou "escura". */
const char *cv_histogram_brightness_label(const CVHistogram *hist);

/** Classificacao do contraste pelo desvio padrao: "alto", "medio" ou "baixo". */
const char *cv_histogram_contrast_label(const CVHistogram *hist);

/** Desenha o histograma (barras proporcionais) dentro da area indicada. */
void cv_histogram_draw(const CVHistogram *hist, SDL_Renderer *renderer,
                       const SDL_FRect *area);

#endif // CV_HISTOGRAM_H
