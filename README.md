# Proj1 — Processamento de Imagens (SDL3 + C)

Projeto 1 da disciplina **Computação Visual** — Ciência da Computação, Faculdade de Computação e Informática, Universidade Presbiteriana Mackenzie. Prof. André Kishimoto.

Repositório público: [Computcao-Visual-Projeto1](https://github.com/Gugrgg/Computcao-Visual-Projeto1).

## Integrantes

| Nome completo | RA |
|---|---|
| Gustavo Galhardo Rodrigues | 10403091 |
| Guilherme Rainho Geraldo | 10418251 |
| Gabriel Vieira de Sousa | 10410264 |
| Guilherme Gomes Arantes Teles | 10364065 |

As contribuições de cada pessoa estão descritas na seção [Contribuições](#contribuições).

---

## O que é o projeto

Um programa de linha de comando escrito em **C**, usando **SDL3**, **SDL_image** e **SDL_ttf**, que carrega uma imagem, converte para escala de cinza e permite analisá-la e processá-la por meio de uma interface com duas janelas:

- **Janela principal** — exibe a imagem que está sendo processada.
- **Janela secundária** (filha da principal) — exibe o histograma da imagem, informações estatísticas e dois botões de ação, todos desenhados com primitivas da SDL.

O projeto foi construído a partir do exemplo `05-filter_image` do [repositório da disciplina](https://github.com/profkishimoto/CompVis262).

## Funcionalidades

1. **Carregamento de imagem** via `IMG_Load`, com tratamento de arquivo inexistente e de formato não suportado.
2. **Detecção e conversão para escala de cinza** — o programa informa no terminal se a imagem de entrada é colorida ou já está em escala de cinza. Se for colorida, aplica `Y = 0.2125·R + 0.7154·G + 0.0721·B`.
3. **Interface com duas janelas** — principal em 1024x768 centralizada no monitor principal; secundária de tamanho fixo (416x520) no canto superior esquerdo da tela.
4. **Histograma e análise** — histograma de 256 níveis desenhado proporcionalmente, com média de intensidade (classificada em clara / média / escura) e desvio padrão (contraste alto / médio / baixo).
5. **Equalização do histograma** — botão que alterna entre a imagem equalizada e a original em escala de cinza, sem recarregar o arquivo.
6. **Alternância de resolução** — botão que alterna entre a resolução original da imagem e 1024x768, redimensionando e reposicionando a janela principal.
7. **Salvar imagem** — a tecla `S` grava a imagem exibida em `output_image.png`, informando no terminal se o arquivo foi criado ou sobrescrito.
8. **Exibição de textos** com SDL_ttf, usando uma fonte distribuída junto do projeto.

---

## Requisitos

### Bibliotecas

| Biblioteca | Versão usada | Origem |
|---|---|---|
| SDL3 | **3.4.16** | https://github.com/libsdl-org/SDL/releases |
| SDL_image | **3.4.6** | https://github.com/libsdl-org/SDL_image/releases |
| SDL_ttf | **3.2.2** | https://github.com/libsdl-org/SDL_ttf/releases |

> A SDL_ttf **não** acompanha a numeração da SDL3: na verificação de 22/09/2026, a versão estável mais recente dela ainda é a 3.2.2.

### Ambientes

| Ambiente | Sistema operacional | Compilador | Situação |
|---|---|---|---|
| Windows | Windows 10 Home 22H2, build 19045, x64 | GCC 15.1.0 / WinLibs MinGW-w64 UCRT | Compilado sem avisos; testes de processamento e integração aprovados |
| WSL (ambiente de avaliação) | Ubuntu 26.04 | GCC 15.2.0 | Instruções fornecidas; ainda não validado neste ambiente |

O código usa apenas C padrão (`-std=c17`) e a API da SDL3, sem extensões específicas de compilador.

---

## Instalação das bibliotecas

### Windows (MinGW-w64 / MSYS2)

Instale o [GCC 15.1.0 para Windows x64 (WinLibs/MinGW-w64 UCRT)](https://github.com/brechtsanders/winlibs_mingw/releases/tag/15.1.0posix-12.0.0-ucrt-r1). Extraia, por exemplo, em `C:/dev/mingw64`. Para usar os comandos `gcc` e `mingw32-make` diretamente, adicione a pasta `bin` ao PATH. Prefira caminhos sem espaços ou acentos para instalar o compilador.

1. Baixe os pacotes de desenvolvimento para MinGW nas páginas de releases:
   - `SDL3-devel-3.4.16-mingw.zip` (ou `.tar.gz`)
   - `SDL3_image-devel-3.4.6-mingw.zip` (ou `.tar.gz`)
   - `SDL3_ttf-devel-3.2.2-mingw.zip` (ou `.tar.gz`). Os pacotes ZIP podem ser extraídos pelo Windows; para `.tar.gz`, use `tar -xzf arquivo.tar.gz`.
2. De cada pacote, abra a pasta `x86_64-w64-mingw32`.
3. Copie o conteúdo das três (`bin/`, `include/`, `lib/`, `share/`) para **uma única pasta**, por exemplo `d:\dev\compvis\libs\SDL3`. As três bibliotecas convivem na mesma árvore.
4. Ao final você deve ter `include/SDL3`, `include/SDL3_image`, `include/SDL3_ttf` e, em `bin/`, as três DLLs: `SDL3.dll`, `SDL3_image.dll` e `SDL3_ttf.dll`.

### Linux / WSL (Ubuntu)

Os repositórios da distribuição podem trazer versões mais antigas do que a última estável. Confira antes:

```bash
pkg-config --modversion sdl3 sdl3-image sdl3-ttf
```

Se as versões não baterem com as da tabela acima, compile a partir do código-fonte:

```bash
sudo apt install build-essential cmake pkg-config \
  libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxi-dev \
  libxfixes-dev libxtst-dev libwayland-dev libxkbcommon-dev libdecor-0-dev \
  libasound2-dev libpulse-dev libudev-dev libdbus-1-dev \
  libfreetype-dev libharfbuzz-dev libpng-dev libjpeg-dev

# SDL3
git clone --depth 1 --branch release-3.4.16 https://github.com/libsdl-org/SDL.git
cmake -S SDL -B SDL/build -DCMAKE_BUILD_TYPE=Release -DSDL_TESTS=OFF -DSDL_EXAMPLES=OFF
cmake --build SDL/build -j$(nproc) && sudo cmake --install SDL/build

# SDL_image
git clone --depth 1 --branch release-3.4.6 https://github.com/libsdl-org/SDL_image.git
cmake -S SDL_image -B SDL_image/build -DCMAKE_BUILD_TYPE=Release -DSDLIMAGE_VENDORED=OFF \
  -DSDLIMAGE_SAMPLES=OFF -DSDLIMAGE_TESTS=OFF
cmake --build SDL_image/build -j$(nproc) && sudo cmake --install SDL_image/build

# SDL_ttf
git clone --depth 1 --branch release-3.2.2 https://github.com/libsdl-org/SDL_ttf.git
cmake -S SDL_ttf -B SDL_ttf/build -DCMAKE_BUILD_TYPE=Release -DSDLTTF_VENDORED=OFF -DSDLTTF_SAMPLES=OFF
cmake --build SDL_ttf/build -j$(nproc) && sudo cmake --install SDL_ttf/build

sudo ldconfig
```

---

## Compilação

### Windows

Abra o terminal na pasta que contém o Makefile. Com GCC e Make no PATH:

```bat
mingw32-make SDL_DIR=d:/dev/compvis/libs/SDL3
```

O `SDL_DIR` pode ser omitido se o caminho padrão do Makefile já for o seu. As três DLLs são copiadas automaticamente para junto do executável ao final do build.

Também é possível usar PowerShell sem alterar o PATH permanente:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\compilar-windows.ps1 -GccPath C:/dev/mingw64/bin/gcc.exe -SdlDir C:/dev/SDL3
```

O script exige GCC 15.1.0. Os caminhos devem apontar para as instalações da sua máquina. A pasta `.toolchain` usada na preparação local não faz parte do repositório e não é necessária quando os caminhos são informados. Veja também [BUILD-WINDOWS.md](BUILD-WINDOWS.md).

### Linux / WSL

```bash
make
```

No Linux as flags de compilação e ligação vêm do `pkg-config`, então não há caminho fixo no Makefile.

### Outros alvos

```bash
make run     # compila e executa com uma imagem de teste
make test    # testa as três imagens, incluindo gravação em PNG
make clean   # remove os arquivos gerados pela compilação
make help    # mostra os alvos disponíveis
```

---

## Execução

```bash
./imgproc caminho_da_imagem.ext        # Linux / WSL
imgproc.exe caminho_da_imagem.ext      # Windows
```

Exemplos incluídos em `images/`:

```bash
./imgproc images/teste_colorida_baixo_contraste.png   # colorida, contraste baixo
./imgproc images/teste_cinza_escura.png               # já em escala de cinza, escura
./imgproc images/teste_colorida_gradiente.jpg         # JPEG colorido
```

### Controles

| Ação | Como |
|---|---|
| Equalizar / voltar ao original | Botão superior da janela secundária |
| Alternar resolução original / 1024x768 | Botão inferior da janela secundária |
| Salvar `output_image.png` | Tecla `S` |
| Sair | Tecla `ESC` ou fechar qualquer janela |

---

## Estrutura do projeto

```
.
├── Makefile                 # build multiplataforma (Windows/MinGW e Linux)
├── README.md
├── assets/fonts/            # fonte distribuída com o projeto + licença
├── images/                  # imagens de teste
├── src/
│   ├── main.c               # argumentos de linha de comando e ponto de entrada
│   ├── cv_app.c/.h          # estado da aplicação, laço principal, eventos, render
│   ├── cv_window.c/.h       # janela + renderizador, centralização e posicionamento
│   ├── cv_image.c/.h        # carregamento, escala de cinza, equalização, gravação
│   ├── cv_histogram.c/.h    # cálculo, estatísticas, classificação e desenho
│   └── cv_ui.c/.h           # fonte, textos e botões desenhados com primitivas
├── tests/
│   └── test_processing.c    # teste headless do processamento (não faz parte do programa)
└── tools/
    └── lsan.supp            # supressões do LeakSanitizer (alocações internas da SDL)
```

---

## Decisões de implementação

### Fonte

O programa usa a família **DejaVu Sans** (arquivo `assets/fonts/DejaVuSans.ttf`), distribuída junto do projeto.

O carregamento é independente do sistema operacional: `cv_font_open()` monta o caminho a partir de `SDL_GetBasePath()`, que devolve a pasta do executável com o separador correto em qualquer SO. Caso a pasta `assets` não esteja presente, o programa tenta, como plano B, uma lista de fontes conhecidas do sistema (Segoe UI/Arial no Windows, DejaVu/Liberation no Linux, Arial/Helvetica no macOS). Só falha se nenhuma delas existir.

A DejaVu foi escolhida por ter licença permissiva (permite redistribuição), cobrir os caracteres acentuados do português e estar disponível na maioria das distribuições Linux — o que torna o plano B provável de funcionar.

### Equalização do histograma

Implementada conforme GONZALES & WOODS (cap. 3):

```
s_k = round( (L-1) · Σ(j=0..k) p_r(r_j) ),  onde p_r(r_j) = n_j / n  e  L = 256
```

A transformação é calculada uma única vez, armazenada em uma tabela de 256 posições e aplicada pixel a pixel.

> Existe uma variante comum (usada pelo OpenCV) que subtrai o menor valor não nulo da CDF, fazendo a imagem usar toda a faixa [0, 255]. Optamos pela formulação do livro-texto por ser a apresentada na disciplina.

### Volta ao original sem recarregar

A estrutura `CVImage` mantém três ponteiros: `gray` (imagem em escala de cinza, **nunca modificada**), `equalized` (criada sob demanda) e `current`, que apenas aponta para uma das duas. Alternar entre as versões significa recriar a textura a partir da superfície escolhida — o arquivo nunca é lido novamente.

### Limiares de classificação

O enunciado não define os limiares, então adotamos:

| Critério | Faixa | Classificação |
|---|---|---|
| Média de intensidade | `< 85` | escura |
| | `85 – 170` | média |
| | `> 170` | clara |
| Desvio padrão | `< 40` | contraste baixo |
| | `40 – 70` | contraste médio |
| | `≥ 70` | contraste alto |

A média divide a faixa [0, 255] em três partes iguais. Os limiares do desvio padrão seguem a referência usual em processamento de imagens e foram validados com as imagens de teste: `teste_colorida_baixo_contraste.png` tem desvio 17,97 (baixo) e, após a equalização, 73,51 (alto).

### Posicionamento da janela secundária

O enunciado pede a janela secundária na coordenada (0,0) da tela. Como a SDL posiciona a **área de cliente** da janela, usar (0,0) diretamente deixaria a barra de título fora da área visível. Seguimos a mesma abordagem do exemplo 04 da disciplina: `SDL_GetWindowBordersSize()` fornece a espessura da borda superior e esquerda, e a janela é posicionada nesse deslocamento, de modo que a janela inteira (incluindo a barra de título) fique visível a partir do canto da tela.

### Salvar a imagem exibida

A tecla `S` grava exatamente o que está na janela principal. Se a exibição estiver em 1024x768 e a imagem original tiver outra resolução, a superfície é redimensionada com `SDL_ScaleSurface()` antes de gravar. A existência prévia do arquivo é verificada com `SDL_GetPathInfo()`, para distinguir as mensagens "criado" e "sobrescrito".

---

## Verificação

### Corretude do processamento

O arquivo `tests/test_processing.c` executa os módulos de imagem e histograma sem interface gráfica (driver de vídeo `dummy`), gravando as saídas em PNG. Os resultados foram comparados com uma implementação de referência independente (NumPy):

| Verificação | Resultado |
|---|---|
| Conversão para escala de cinza | **0 pixels de diferença** em 393.216 |
| Equalização do histograma | **0 pixels de diferença** em 393.216 |
| Média / desvio padrão | compatíveis com os valores registrados nos testes |

```bash
gcc -std=c17 -Wall -Wextra $(pkg-config --cflags sdl3 sdl3-image sdl3-ttf) \
    tests/test_processing.c src/cv_image.c src/cv_histogram.c \
    -o test_proc $(pkg-config --libs sdl3 sdl3-image sdl3-ttf) -lm
./test_proc images/teste_colorida_baixo_contraste.png
```

### Testes e limites da verificação

No Windows, execute `mingw32-make test SDL_DIR=C:/dev/SDL3` com GCC/Make no PATH.
Alternativamente, use `./compilar-windows.ps1 -GccPath C:/dev/mingw64/bin/gcc.exe -SdlDir C:/dev/SDL3 -MakeArgs test` no PowerShell.
Os testes usam o driver de vídeo `dummy`, criam `build/` e verificam os retornos das operações de carregamento, histograma, equalização e gravação. Uma falha encerra o teste com código diferente de zero.

A verificação de integração no Windows também exercitou, com janelas ocultas e eventos simulados, a relação entre as janelas, os estados dos botões, a alternância de resolução, o retorno ao original e a criação/sobrescrita do PNG. Essa verificação não substitui uma conferência visual manual nem uma execução no WSL.

O código possui rotinas de liberação de imagens, texturas, renderizadores, janelas e fontes, inclusive na falha de inicialização. Não foi executado um detector de vazamentos nesta verificação do Windows. Para verificar memória em um ambiente Linux/WSL configurado, use:

```bash
gcc -std=c17 -g -O1 -fsanitize=address,undefined $(pkg-config --cflags sdl3 sdl3-image sdl3-ttf) \
    src/*.c -o imgproc_asan $(pkg-config --libs sdl3 sdl3-image sdl3-ttf) -lm
./imgproc_asan images/teste_colorida_baixo_contraste.png
```

---

## Contribuições

Divisão de responsabilidades adotada para revisão e integração dos módulos. Cada integrante deve revisar sua parte e registrar os próprios commits. A divisão dos arquivos não substitui o histórico das alterações efetivamente realizadas.

- **Gustavo Galhardo Rodrigues (RA 10403091)** — Módulos `cv_app` e `cv_window`: integração dos componentes, laço de eventos, posicionamento e redimensionamento das janelas; configuração de compilação e documentação técnica.
- **Guilherme Rainho Geraldo (RA 10418251)** — Módulo `cv_histogram`: cálculo e desenho do histograma, média de intensidade, desvio padrão e classificação de brilho e contraste; verificação das estatísticas antes e depois da equalização.
- **Gabriel Vieira de Sousa (RA 10410264)** — Módulo `cv_image`: carregamento de imagens, identificação e conversão para escala de cinza, equalização e gravação em PNG; testes com diferentes formatos de entrada.
- **Guilherme Gomes Arantes Teles (RA 10364065)** — Módulo `cv_ui`: carregamento de fontes com SDL_ttf, apresentação dos textos e implementação dos botões e seus estados; revisão da interface e das instruções de uso.

---

## Licenças e créditos

- Código-base adaptado do [repositório da disciplina](https://github.com/profkishimoto/CompVis262) — Copyright (c) 2026 André Kishimoto, licença Apache-2.0.
- SDL3, SDL_image e SDL_ttf — licença zlib.
- Fonte **DejaVu Sans** — licença livre permitindo redistribuição; o texto completo está em `assets/fonts/LICENSE-DejaVu.txt`.
- Imagens de teste em `images/` foram geradas pelo próprio grupo.
