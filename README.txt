# Projeto de Compressão e Descompressão de Imagens PGM com Quadtree

Este projeto realiza a compressão e descompressão de imagens no formato PGM utilizando o método de Quadtree. A compressão é feita dividindo a imagem em blocos e verificando a homogeneidade dos pixels de cada bloco. Se o bloco for homogêneo, ele é representado por um único valor médio, caso contrário, ele é dividido em sub-blocos menores. O processo é recursivo até que os blocos sejam suficientemente pequenos ou homogêneos.

Passos:

1. Compilar o programa:
   - Para compilar o arquivo de compressão:
    gcc compressor.c -o compressor.exe

   - Para compilar o arquivo de descompressão:
    gcc descompressor.c -o descompressor.exe

2. Executar o programa de compressão:
   - Certifique-se de ter uma imagem no formato PGM (P5 ou P2) com dimensões 256x256.
   - Para executar o programa de compressão:
     ./compressor <imagemEntrada.pgm>

   - Isso gerará um arquivo de bitstream chamado `bitstream.bin`.

3. Executar o programa de descompressão:
   - Para descomprimir a imagem e reconstruí-la a partir do arquivo `bitstream.bin`:

     ./descompressor bitstream.bin

   - Isso gerará uma imagem chamada `reconstruida.pgm`.

Exemplo de Uso

gcc compressor.c -o compressor.exe
gcc descompressor.c -o descompressor.exe

./compressor exemplo.pgm  # Comprime a imagem PGM e gera o bitstream
./descompressor bitstream.bin  # Descomprime o bitstream e gera a imagem PGM reconstruída

Arquivos

- compressor.c: Código responsável pela compressão da imagem.
- descompressor.c: Código responsável pela descompressão do bitstream.
- README.md: Este arquivo com informações sobre o projeto.
- img01.pgm; img02.pgm; img03.pgm: Arquivos pgm para compressão disponibilizados pelo professor.
- bitstream.bin: Arquivo gerado pela compressão que contém o bitstream comprimido.
- reconstruida.pgm: Imagem reconstruída a partir do bitstream.