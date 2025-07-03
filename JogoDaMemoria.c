/*2 Trabalho de programação Jogo da memoria
Alunos: Erick Lafayete
*/

//bibliotecas usadas
#include <stdio.h>   //função para entrada e saída de dados
#include <stdlib.h>  //função para aloção da memoria
#include <time.h>    //usada para  o embaralhamento das cartas
#include <stdbool.h> //true ou false
#include <string.h>  //manipulação de strings



#define LINHAS 4         //definição de linhas do tabuleiro
#define COLUNAS 4        //definição de coluna di tabuleiro
#define TOTAL_PARES 8    //total de pares segundo no numero de cartas
#define MAX_JOGADORES 10 //capacidade maxima de ppessoas no ranking
#define NOME_TAM 50      //tamanho maximo do nome do jogador



typedef struct {
    char valor;
    bool virada;
    bool encontrada;
} Carta;

typedef struct {
    char nome[NOME_TAM]; // NOME_TAM = 50
    int pontuacao;  // quanto menor melhor (baseado em tentativas)
} Jogador;



Carta tabuleiro[LINHAS][COLUNAS];
Jogador ranking[MAX_JOGADORES]; //MAX_JOGADORES = 10
int numJogadores = 0;
int tentativas = 0;


// funções
void inicializarTabuleiro();               //preencher o tabuleiro com os pares
void embaralharCartas();                   //embalha as cartas
void exibirTabuleiro();                    //mostra o tabuleiro
void virarCarta(int, int);                 //vira uma carta especifica
bool verificarPar(int, int, int, int);     //verifica as cartas viradas se são par
void jogar(char*);                         //
void mostrarMenu();                        //exibe o menu principal
void carregarRanking();                    //carrega o ranking
void salvarRanking();                      //salva o ranking
void adicionarJogador(char*, int);        //adiciona um jogador ao ranking
void exibirRanking();                     //exibe o ranking


//função principal
int main() {
    srand(time(NULL));
    carregarRanking();
    mostrarMenu();
    return 0;
}

void carregarRanking() {
    FILE *arquivo = fopen("ranking.dat", "rb");
    if (arquivo) {
        fread(&numJogadores, sizeof(int), 1, arquivo);
        fread(ranking, sizeof(Jogador), numJogadores, arquivo);
        fclose(arquivo);
    }
}

void salvarRanking() {
    FILE *arquivo = fopen("ranking.dat", "wb");
    if (arquivo) {
        fwrite(&numJogadores, sizeof(int), 1, arquivo);
        fwrite(ranking, sizeof(Jogador), numJogadores, arquivo);
        fclose(arquivo);
    }
}

//verifica se o ranking esta cheio se sim tira o de pior score no ranking e adiona o novo
void adicionarJogador(char *nome, int pontuacao) {
    if (numJogadores < MAX_JOGADORES) {
        strncpy(ranking[numJogadores].nome, nome, NOME_TAM);
        ranking[numJogadores].pontuacao = pontuacao;
        numJogadores++;
    } else {
        //substitui o pior score se necessário
        int pior = 0;
        for (int i = 1; i < MAX_JOGADORES; i++) {
            if (ranking[i].pontuacao > ranking[pior].pontuacao) {
                pior = i;
            }
        }
        if (pontuacao < ranking[pior].pontuacao) {
            strncpy(ranking[pior].nome, nome, NOME_TAM);
            ranking[pior].pontuacao = pontuacao;
        }
    }
    
    //ordena o ranking por pontuação (menor preimeiro)
    for (int i = 0; i < numJogadores - 1; i++) {
        for (int j = i + 1; j < numJogadores; j++) {
            if (ranking[i].pontuacao > ranking[j].pontuacao) {
                Jogador temp = ranking[i]; 
                ranking[i] = ranking[j];
                ranking[j] = temp;
            }
        }
    }
    
    salvarRanking(); 
}

//mostra o ranking
void exibirRanking() {
    printf("\n=== MELHORES JOGADORES ===\n");
    printf("Pos. Nome %-30s Pontuacao\n", ""); 
    for (int i = 0; i < numJogadores; i++) {
        printf("%2d.  %-30s %5d\n", i+1, ranking[i].nome, ranking[i].pontuacao); //lista de jogadores
    }
}

//mostra op menu principal
void mostrarMenu() {
    int opcao;
    char nome[NOME_TAM];
    
    printf("Digite seu nome: ");
    fgets(nome, NOME_TAM, stdin);
    nome[strcspn(nome, "\n")] = '\0';  // Remove \n
    
    do {
        //opções do menu
        printf("\n=== JOGO DA MEMORIA ===\n");
        printf("Jogador: %s\n", nome);
        printf("1. Jogar\n");
        printf("2. Ver Ranking\n");
        printf("3. Regras\n");
        printf("4. Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao); //lê a opção escolhida
        getchar();  // Limpa o buffer do teclado
        
        switch(opcao) {
            case 1: jogar(nome); break; //inicia o jogo
            case 2: exibirRanking(); break; //mostra o ranking
            case 3:
                printf("\n=== REGRAS DO JOGO da MEMORIA ===\n");
                printf("Encontre todos os pares de cartas no tabuleiro 4x4.\n");
                printf("A cada jogada, selecione 2 cartas.\n");
                printf("Se formar par, ficam viradas. Caso contrario, sao escondidas.\n");
                printf("Pontuacao: Quanto menos tentativas, melhor!\n");
                break;
            case 4: printf("Ate logo!\n"); break; //sai do jogo
            default: printf("Opção invalida!\n"); 
        }
    } while(opcao != 4); //repete até o usuario escolher sair
}

void inicializarTabuleiro() {
    char valores[TOTAL_PARES * 2] = {'A','A','B','B','C','C','D','D',
                                    'E','E','F','F','G','G','H','H'}; //oq cada carta é 
    int index = 0;
    for(int i = 0; i < LINHAS; i++) {
        for(int j = 0; j < COLUNAS; j++) {
            tabuleiro[i][j].valor = valores[index++];
            tabuleiro[i][j].virada = false;
            tabuleiro[i][j].encontrada = false;
        }
    }
}

//embaralha as cartas no tabuleiro
void embaralharCartas() {
    for(int i = 0; i < LINHAS; i++) {
        for(int j = 0; j < COLUNAS; j++) {
            //gera posições aleatorias
            int r = rand() % LINHAS;
            int c = rand() % COLUNAS;
            //troca as cartas de posição
            Carta temp = tabuleiro[i][j];
            tabuleiro[i][j] = tabuleiro[r][c];
            tabuleiro[r][c] = temp;
        }
    }
}

// Exibe o tabuleiro na tela
void exibirTabuleiro() {
    printf("\n   "); // Espaço para alinhamento
    // Mostra números das colunas
    for(int j = 0; j < COLUNAS; j++) printf("%d ", j);
    printf("\n");
    
    // Mostra cada linha do tabuleiro
    for(int i = 0; i < LINHAS; i++) {
        printf("%d  ", i); //numero da linha
        for(int j = 0; j < COLUNAS; j++) {
            if(tabuleiro[i][j].encontrada) {
                printf("%c ", tabuleiro[i][j].valor); //mostra carta encontrada
            } else if(tabuleiro[i][j].virada) {
                printf("%c ", tabuleiro[i][j].valor); //mostra carta virada
            } else {
                printf("* "); //mostra carta não virada
            }
        }
        printf("\n");
    }
    printf("\nTentativas: %d\n", tentativas); //mostra contador de tentativas
}


//vira uma carta especifica escolhida
void virarCarta(int linha, int coluna) {
    //verifica se as coordenadas são válidas
    if(linha >= 0 && linha < LINHAS && coluna >= 0 && coluna < COLUNAS) {
        //só vira se n estiver virada a carta
        if(!tabuleiro[linha][coluna].virada && !tabuleiro[linha][coluna].encontrada) {
            tabuleiro[linha][coluna].virada = true;
        }
    }
}

//verifica se as duas cartas formam um par
bool verificarPar(int linha1, int coluna1, int linha2, int coluna2) {
    return tabuleiro[linha1][coluna1].valor == tabuleiro[linha2][coluna2].valor;
}

//jogo 
void jogar(char *nome) {
    tentativas = 0;
    inicializarTabuleiro(); //preenche o tabuleiro
    embaralharCartas();     //embaralha as cartas
    int pares = 0;

    while(pares < TOTAL_PARES) {
        exibirTabuleiro(); //mostra o tabuleiro
        
        int l1, c1, l2, c2;
        //pede a primeira carta
        printf("Primeira carta (linha coluna): ");
        scanf("%d %d", &l1, &c1);
        virarCarta(l1, c1); //vira a carta 
        exibirTabuleiro();  //mostra o tabuleiro com a carta virada
        
        printf("Segunda carta (linha coluna): ");
        scanf("%d %d", &l2, &c2);
        virarCarta(l2, c2); //vira a carta 
        exibirTabuleiro();  //mostra o tabuleiro com a carta virada
        
        //verifica se  formou um par
        if(verificarPar(l1, c1, l2, c2)) {
            tabuleiro[l1][c1].encontrada = true;
            tabuleiro[l2][c2].encontrada = true;
            pares++; //
            printf("\nPar encontrado!\n");
        } else {
            //esconde as cartas
            tabuleiro[l1][c1].virada = false;
            tabuleiro[l2][c2].virada = false;
            printf("\nNao e um par. Tente novamente!\n");
        }
        
        tentativas++; //adiciona tentativas
        printf("Pressione Enter para continuar...");
        getchar(); getchar(); //Enter para continuar
    }
    
    //vitótia 
    printf("\nParabens %s! Voce completou em %d tentativas!\n", nome, tentativas);
    adicionarJogador(nome, tentativas); //adiciona ao ranking
    exibirRanking(); //mostra o ranking atualizado
}