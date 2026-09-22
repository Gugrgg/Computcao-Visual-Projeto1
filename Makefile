# Projeto 1 (Proj1) - Computacao Visual - UPM FCI
#
# Compilacao multiplataforma:
#   Windows (MinGW-w64): mingw32-make
#   Linux / WSL:         make
#
# No Windows, informe onde as bibliotecas SDL foram extraidas. O caminho pode
# ser passado na linha de comando, sem precisar editar este arquivo:
#   mingw32-make SDL_DIR=d:/dev/libs/SDL3

TARGET   := imgproc
SRC_DIR  := src
OBJ_DIR  := build

CC       := gcc
CSTD     := -std=c17
WARNINGS := -Wall -Wextra -Wpedantic
CFLAGS   := $(CSTD) $(WARNINGS) -O2
LDLIBS   :=

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

ifeq ($(OS),Windows_NT)
  # ---------------------------------------------------------------- Windows
  SDL_DIR     ?= d:/dev/compvis/libs/SDL3
  CFLAGS      += -I"$(SDL_DIR)/include"
  LDFLAGS     += -L"$(SDL_DIR)/lib"
  LDLIBS      += -lSDL3 -lSDL3_image -lSDL3_ttf
  EXE         := $(TARGET).exe
  TEST_EXE    := test_processing.exe
  DLLS        := SDL3.dll SDL3_image.dll SDL3_ttf.dll
  RM_FILE      = del /Q
  MKDIR        = if not exist "$(subst /,\,$1)" mkdir "$(subst /,\,$1)"
  RUN_PREFIX   :=
else
  # ------------------------------------------------------------ Linux / WSL
  # As flags vem do pkg-config, evitando caminhos fixos no arquivo.
  SDL_CFLAGS  := $(shell pkg-config --cflags sdl3 sdl3-image sdl3-ttf)
  SDL_LDLIBS  := $(shell pkg-config --libs sdl3 sdl3-image sdl3-ttf)
  CFLAGS      += $(SDL_CFLAGS)
  LDLIBS      += $(SDL_LDLIBS) -lm
  EXE         := $(TARGET)
  TEST_EXE    := test_processing
  DLLS        :=
  RM_FILE      = rm -f
  MKDIR        = mkdir -p $1
  RUN_PREFIX   := ./
endif

.PHONY: all clean run test help

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)
ifeq ($(OS),Windows_NT)
	@echo Copiando DLLs da SDL para junto do executavel...
	@for %%f in ($(DLLS)) do @copy /Y "$(subst /,\,$(SDL_DIR))\bin\%%f" ".\%%f" >nul
endif
	@echo ">>> Build concluido: $(EXE)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(OBJ_DIR):
	@$(call MKDIR,$(OBJ_DIR))

run: all
	$(RUN_PREFIX)$(EXE) images/teste_colorida_baixo_contraste.png

$(TEST_EXE): tests/test_processing.c $(SRC_DIR)/cv_image.c $(SRC_DIR)/cv_histogram.c $(SRC_DIR)/cv_image.h $(SRC_DIR)/cv_histogram.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -o $@ tests/test_processing.c $(SRC_DIR)/cv_image.c $(SRC_DIR)/cv_histogram.c $(LDFLAGS) $(LDLIBS)

test: all $(TEST_EXE)
	$(RUN_PREFIX)$(TEST_EXE) images/teste_cinza_escura.png
	$(RUN_PREFIX)$(TEST_EXE) images/teste_colorida_baixo_contraste.png
	$(RUN_PREFIX)$(TEST_EXE) images/teste_colorida_gradiente.jpg

clean:
	-$(RM_FILE) $(subst /,$(if $(filter Windows_NT,$(OS)),\,/),$(OBJS) $(DEPS))
	-$(RM_FILE) $(EXE) $(TEST_EXE) $(DLLS)
	@echo ">>> Arquivos de build removidos."

help:
	@echo "Alvos disponiveis:"
	@echo "  make        - compila o projeto"
	@echo "  make run    - compila e executa com uma imagem de teste"
	@echo "  make test   - testa processamento e gravacao nas tres imagens"
	@echo "  make clean  - remove os arquivos gerados pela compilacao"
	@echo ""
	@echo "No Windows, informe o caminho das bibliotecas, por exemplo:"
	@echo "  mingw32-make SDL_DIR=d:/dev/libs/SDL3"

-include $(DEPS)
