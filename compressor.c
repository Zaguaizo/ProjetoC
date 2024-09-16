
#include <stdio.h>
#include <stdlib.h>

// Estrutura da imagem PGM
struct pgm {
    int tipo;      // Tipo da imagem (P2 ou P5)
    int c;         // Largura (colunas)
    int r;         // Altura (linhas)
    int mv;        // Valor máximo de intensidade
    unsigned char *pData;  // Dados dos pixels
};

// Funções para leitura, exibição e escrita de imagens PGM
void lerImagemPGM(struct pgm *, char *);
void exibirImagemPGM(struct pgm *);
void salvarImagemPGM(struct pgm *, char *);

// Funções de compressão
int ehHomogeneo(struct pgm *img, int x, int y, int tamanho, int limiar);
void comprimir(struct pgm *img, int x, int y, int tamanho, int limiar, FILE *bitstream);

//Função principal
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso correto: %s <imagemEntrada.pgm>\n", argv[0]);
        return 1;
    }

    // Estrutura da imagem PGM
    struct pgm img;

    // 1. Ler a imagem PGM de entrada
    lerImagemPGM(&img, argv[1]);

    // 2. Abrir o arquivo para salvar o bitstream em binário
    FILE *bitstream = fopen("bitstream.bin", "wb");
    if (!bitstream) {
        perror("Erro ao criar o arquivo bitstream");
        exit(1);
    }

    // 3. Compressão com Quadtree
    int limiar = 150; // Critério de homogeneidade (ajustável)

    comprimir(&img, 0, 0, img.c, limiar, bitstream);

    // 4. Fechar o bitstream
    fclose(bitstream);

    // Liberação da memória da imagem
    free(img.pData);

    return 0;
}

// Função para verificar se um bloco da imagem é homogêneo usando a diferença máxima
int ehHomogeneo(struct pgm *img, int x, int y, int tamanho, int limiar) {
    unsigned char min_val = 255, max_val = 0;

    // Encontra o valor mínimo e máximo no bloco
    for (int i = x; i < x + tamanho && i < img->r; i++) {
        for (int j = y; j < y + tamanho && j < img->c; j++) {
            unsigned char pixel = img->pData[i * img->c + j];
            if (pixel < min_val) min_val = pixel;
            if (pixel > max_val) max_val = pixel;
        }
    }

    // Verifica se a diferença entre o valor máximo e o mínimo é menor que o limiar
    return (max_val - min_val) <= limiar;
}

// Função para realizar a compressão utilizando Quadtree
void comprimir(struct pgm *img, int x, int y, int tamanho, int limiar, FILE *bitstream) {
    if (tamanho <= 1) {
        // Caso base: último pixel
        unsigned char flag = 1;
        fwrite(&flag, sizeof(unsigned char), 1, bitstream); // Escrever a flag (1 para homogêneo)
        fwrite(&img->pData[x * img->c + y], sizeof(unsigned char), 1, bitstream); // Escrever o valor do pixel
        return;
    }

    if (ehHomogeneo(img, x, y, tamanho, limiar)) {
        // Homogêneo: gravar a flag e a média no bitstream
        unsigned char flag = 1;
        fwrite(&flag, sizeof(unsigned char), 1, bitstream); // Escrever a flag (1 para homogêneo)

        int soma = 0, contagem = 0;
        for (int i = x; i < x + tamanho && i < img->r; i++) {
            for (int j = y; j < y + tamanho && j < img->c; j++) {
                soma += img->pData[i * img->c + j];
                contagem++;
            }
        }
        unsigned char media = soma / contagem;
        fwrite(&media, sizeof(unsigned char), 1, bitstream); // Escrever a média do bloco homogêneo
    } else {
        // Não homogêneo: gravar a flag e subdividir em 4 quadrantes
        unsigned char flag = 0;
        fwrite(&flag, sizeof(unsigned char), 1, bitstream); // Escrever a flag (0 para não homogêneo)

        int metadeTamanho = tamanho / 2;
        comprimir(img, x, y, metadeTamanho, limiar, bitstream);                // Quadrante superior esquerdo
        comprimir(img, x, y + metadeTamanho, metadeTamanho, limiar, bitstream); // Quadrante superior direito
        comprimir(img, x + metadeTamanho, y, metadeTamanho, limiar, bitstream); // Quadrante inferior esquerdo
        comprimir(img, x + metadeTamanho, y + metadeTamanho, metadeTamanho, limiar, bitstream); // Quadrante inferior direito
    }
}

// Função para ler imagens PGM e ignorar bytes extras
void lerImagemPGM(struct pgm *img, char *nomeArquivo) {
    FILE *fp;
    char ch;

    if (!(fp = fopen(nomeArquivo, "r"))) {
        perror("Erro ao abrir o arquivo.");
        exit(1);
    }

    // Verifica se o arquivo está no formato PGM
    if ((ch = getc(fp)) != 'P') {
        puts("A imagem fornecida não está no formato PGM");
        exit(2);
    }

    img->tipo = getc(fp) - 48;  // Lê o tipo (P2 ou P5)
    
    //Ignora caracteres de espaço após o tipo
    fseek(fp, 1, SEEK_CUR);

    // Pular os comentários
    while ((ch = getc(fp)) == '#') {
        while ((ch = getc(fp)) != '\n');  // Pula até o final da linha (comentário)
    }
    
    fseek(fp,-2, SEEK_CUR); // Volta ao primeiro caractere da linha das dimensões;
   
    fscanf(fp,"%d %d",&img->c,&img->r); // Ler dimensões da imagem

    if (ferror(fp)){ 
		perror(NULL);
		exit(3);
	}	// Saída de erro;

    fscanf(fp, "%d", &img->mv); // Ler valor maximo de cinza da imagem

    fseek(fp,1, SEEK_CUR); // Pula o ponteiro pra primeira linha da stream.

    printf("Dimensoes: %d x %d\n", img->c, img->r);
    printf("Valor maximo de cinza: %d\n", img->mv);

    // Alocar memória para os dados da imagem
    img->pData = (unsigned char *)malloc(img->r * img->c * sizeof(unsigned char));
    if (img->pData == NULL) {
        perror("Erro ao alocar memoria para a stream");
        exit(5);
    }

    // Ler os dados da imagem dependendo do tipo (P2 ou P5)
    switch (img->tipo) {
        case 2:
			puts("Lendo imagem PGM P2(Texto)");
			for (int k=0; k < (img->r * img->c); k++){
				fscanf(fp, "%hhu", img->pData+k);
			}
		break;	
        case 5:
            puts("Lendo imagem PGM P5(Binario)");
			fread(img->pData,sizeof(unsigned char),img->r * img->c, fp);
            break;
        default:
            puts("Tipo PGM nao suportado");
    }

    fclose(fp);
}

// Função para salvar a imagem PGM no formato binário
void salvarImagemPGM(struct pgm *img, char *nomeArquivo) {
    FILE *fp;

    if (!(fp = fopen(nomeArquivo, "wb"))) {
        perror("Erro ao criar o arquivo.");
        exit(1);
    }

    fprintf(fp, "P5\n");
    fprintf(fp, "%d %d\n", img->c, img->r);
    fprintf(fp, "%d\n", img->mv);

    fwrite(img->pData, sizeof(unsigned char), img->c * img->r, fp);
    fclose(fp);
}
