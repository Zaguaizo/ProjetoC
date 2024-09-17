//*******************************************************************************************/
// Alunos: Felipe Haiashida, João Matheus Albuquerque, Isaac Alves, Fernando de Freitas
// Avaliação 04 - Trabalho Final
// 04.505.23 − 2024.2 − Prof. Daniel Ferreira
// Compilador: gcc.exe (MinGW.org GCC-6.3.0-1) 6.3.0
//*******************************************************************************************/

#include <stdio.h>
#include <stdlib.h>

// Estrutura da imagem PGM
struct pgm {
    int tipo;      // Tipo da imagem (P2 ou P5)
    int c;         // Largura (colunas)
    int r;         // Altura (linhas)
    int mv;        // Valor máximo de intensidade
    unsigned char *pDados;  // Dados dos pixels
};

// Funções para ler o bitstream binário e reconstruir a imagem
void descomprimir(struct pgm *img, FILE *bitstream, int x, int y, int tamanho);

// Funções para exibir e escrever a imagem PGM
void exibirImagemPGM(struct pgm *img);
void escreverImagemPGM(struct pgm *img, char *nomeArquivo);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso correto: %s <bitstream.bin>\n", argv[0]);
        exit(1);
    }

    // Abrir o arquivo de bitstream em modo binário
    FILE *bitstream = fopen(argv[1], "rb");
    if (!bitstream) {
        perror("Erro ao abrir o arquivo de bitstream");
        exit(1);
    }

    // Inicializar a estrutura da imagem PGM
    struct pgm img;
    img.tipo = 5;  // Assumindo que vamos gerar um PGM binário (P5)
    img.c = 256;   // Ajustar para 256x256 (tamanho correto da imagem original)
    img.r = 256;
    img.mv = 255;  // Valor máximo de intensidade (ajustar conforme necessário)
    img.pDados = (unsigned char *)malloc(img.r * img.c * sizeof(unsigned char));
    if (img.pDados == NULL) {
        perror("Erro ao alocar memória para a imagem");
        exit(1);
    }

    // Descompressão
    descomprimir(&img, bitstream, 0, 0, img.c);

    // Fechar o bitstream
    fclose(bitstream);

    // Salvar a imagem reconstruída
    escreverImagemPGM(&img, "reconstruida.pgm");

    // Liberação da memória da imagem
    free(img.pDados);

    return 0;
}

// Função recursiva para descomprimir o bitstream binário e reconstruir a imagem
void descomprimir(struct pgm *img, FILE *bitstream, int x, int y, int tamanho) {
    unsigned char flag;
    fread(&flag, sizeof(unsigned char), 1, bitstream);  // Lê a flag (1 para homogêneo, 0 para não homogêneo)

    if (flag == 1) {
        // Bloco homogêneo: ler a média e preencher o bloco
        unsigned char media;
        fread(&media, sizeof(unsigned char), 1, bitstream);  // Lê o valor médio
        for (int i = x; i < x + tamanho && i < img->r; i++) {
            for (int j = y; j < y + tamanho && j < img->c; j++) {
                img->pDados[i * img->c + j] = media;
            }
        }
    } else {
        // Bloco não homogêneo: dividir o bloco em 4 quadrantes
        int metadeTamanho = tamanho / 2;
        if (metadeTamanho >= 1) {
            descomprimir(img, bitstream, x, y, metadeTamanho);                // Quadrante superior esquerdo
            descomprimir(img, bitstream, x, y + metadeTamanho, metadeTamanho);     // Quadrante superior direito
            descomprimir(img, bitstream, x + metadeTamanho, y, metadeTamanho);     // Quadrante inferior esquerdo
            descomprimir(img, bitstream, x + metadeTamanho, y + metadeTamanho, metadeTamanho);  // Quadrante inferior direito
        }
    }
}

// Função para salvar a imagem PGM no formato binário
void escreverImagemPGM(struct pgm *img, char *nomeArquivo) {
    FILE *fp;

    if (!(fp = fopen(nomeArquivo, "wb"))) {
        perror("Erro ao criar o arquivo.");
        exit(1);
    }

    // Escreve o cabeçalho PGM
    fprintf(fp, "P5\n");
    fprintf(fp, "%d %d\n", img->c, img->r);
    fprintf(fp, "%d\n", img->mv);

    // Escreve os dados da imagem
    fwrite(img->pDados, sizeof(unsigned char), img->c * img->r, fp);
    fclose(fp);
}