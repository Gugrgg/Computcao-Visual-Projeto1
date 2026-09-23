// Teste de processamento sem interface grafica. Execute com make test.
#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "../src/cv_image.h"
#include "../src/cv_histogram.h"

int main(int argc, char *argv[])
{
  if (argc != 2) { printf("uso: test_processing <imagem>\n"); return 1; }
  SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy");
  if (!SDL_Init(SDL_INIT_VIDEO)) { printf("erro SDL: %s\n", SDL_GetError()); return 1; }

  int result = 1;
  SDL_Window *w = NULL;
  SDL_Renderer *r = NULL;
  CVImage img = {0};
  CVHistogram h, h2;
  bool existed = false;
  if (!SDL_CreateDirectory("build")) goto cleanup;
  if (!SDL_CreateWindowAndRenderer("teste", 64, 64, 0, &w, &r)) goto cleanup;
  if (!cv_image_load(&img, argv[1], r)) goto cleanup;
  if (!cv_histogram_compute(&h, img.gray)) goto cleanup;
  printf("GRAY mean=%.6f stddev=%.6f max_count=%u total=%llu brilho=%s contraste=%s\n",
         h.mean, h.stddev, h.max_count, (unsigned long long)h.total,
         cv_histogram_brightness_label(&h), cv_histogram_contrast_label(&h));
  if (!cv_image_save_png(&img, img.width, img.height, "build/out_gray.png", &existed)) goto cleanup;
  if (!cv_image_build_equalized(&img)) goto cleanup;
  if (!cv_image_show(&img, img.equalized, r)) goto cleanup;
  if (!cv_image_save_png(&img, img.width, img.height, "build/out_eq.png", &existed)) goto cleanup;
  if (!cv_histogram_compute(&h2, img.equalized)) goto cleanup;
  printf("EQ   mean=%.6f stddev=%.6f brilho=%s contraste=%s\n",
         h2.mean, h2.stddev, cv_histogram_brightness_label(&h2), cv_histogram_contrast_label(&h2));
  if (!cv_image_save_png(&img, 1024, 768, "build/out_scaled.png", &existed)) goto cleanup;
  result = 0;
cleanup:
  if (result) fprintf(stderr, "FALHA: %s\n", SDL_GetError());
  cv_image_destroy(&img);
  SDL_DestroyRenderer(r);
  SDL_DestroyWindow(w);
  SDL_Quit();
  if (!result) puts("OK");
  return result;
}
