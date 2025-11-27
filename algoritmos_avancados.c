#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//
// =============================================================
//  DETECTIVE QUEST - NÍVEL MESTRE
//  Mansão (Árvore Binária) + Pistas (BST) + Suspeitos (Hash)
// =============================================================
//

// -------------------------------------------------------------
//                      ÁRVORE BINÁRIA (Salas)
// -------------------------------------------------------------

typedef struct Sala {
    char nome[50];
    struct Sala *esq;
    struct Sala *dir;
    char pista[50]; // pista encontrada nesta sala (se houver)
    char suspeitoLigado[50]; // suspeito ligado a esta pista (para hash)
} Sala;

Sala* criarSala(char nome[], char pista[], char suspeito[]) {
    Sala *nova = malloc(sizeof(Sala));
    strcpy(nova->nome, nome);

    if (pista != NULL)
        strcpy(nova->pista, pista);
    else
        strcpy(nova->pista, "");

    if (suspeito != NULL)
        strcpy(nova->suspeitoLigado, suspeito);
    else
        strcpy(nova->suspeitoLigado, "");

    nova->esq = NULL;
    nova->dir = NULL;
    return nova;
}

// -------------------------------------------------------------
//               ÁRVORE DE BUSCA BINÁRIA (Pistas)
// -------------------------------------------------------------

typedef struct PistaNode {
    char texto[50];
    struct PistaNode *esq;
    struct PistaNode *dir;
} PistaNode;

PistaNode* inserirPista(PistaNode* raiz, char texto[]) {
    if (!raiz) {
        PistaNode* novo = malloc(sizeof(PistaNode));
        strcpy(novo->texto, texto);
        novo->esq = novo->dir = NULL;
        return novo;
    }

    if (strcmp(texto, raiz->texto) < 0)
        raiz->esq = inserirPista(raiz->esq, texto);
    else if (strcmp(texto, raiz->texto) > 0)
        raiz->dir = inserirPista(raiz->dir, texto);

    return raiz;
}

void listarPistas(PistaNode *raiz) {
    if (!raiz) return;
    listarPistas(raiz->esq);
    printf(" - %s\n", raiz->texto);
    listarPistas(raiz->dir);
}

// -------------------------------------------------------------
//               TABELA HASH (Pista → Suspeito)
// -------------------------------------------------------------

#define TAM_HASH 26

typedef struct Relacao {
    char pista[50];
    char suspeito[50];
    int contador;
    struct Relacao *prox;
} Relacao;

Relacao* tabelaHash[TAM_HASH];

int hashFunc(char *suspeito) {
    char c = toupper(suspeito[0]);
    return (c - 'A') % TAM_HASH;
}

void inserirHash(char pista[], char suspeito[]) {
    int h = hashFunc(suspeito);

    Relacao *aux = tabelaHash[h];
    while (aux) {
        if (strcmp(aux->suspeito, suspeito) == 0) {
            aux->contador++;
            return;
        }
        aux = aux->prox;
    }

    Relacao *nova = malloc(sizeof(Relacao));
    strcpy(nova->pista, pista);
    strcpy(nova->suspeito, suspeito);
    nova->contador = 1;
    nova->prox = tabelaHash[h];
    tabelaHash[h] = nova;
}

void listarAssociacoes() {
    printf("\n===== Associações de Pistas e Suspeitos =====\n");
    for (int i = 0; i < TAM_HASH; i++) {
        Relacao *aux = tabelaHash[i];
        while (aux) {
            printf("Pista: %-20s → Suspeito: %-15s (citado %d vezes)\n",
                   aux->pista, aux->suspeito, aux->contador);
            aux = aux->prox;
        }
    }
}

char* encontrarMaisSuspeito() {
    Relacao *mais = NULL;

    for (int i = 0; i < TAM_HASH; i++) {
        Relacao *aux = tabelaHash[i];
        while (aux) {
            if (!mais || aux->contador > mais->contador)
                mais = aux;
            aux = aux->prox;
        }
    }

    return mais ? mais->suspeito : "Nenhum";
}

// -------------------------------------------------------------
//                      EXPLORAÇÃO DAS SALAS
// -------------------------------------------------------------

void explorarSalas(Sala *atual, PistaNode **raizPistas) {
    char opcao;

    while (1) {
        printf("\nVocê está em: %s\n", atual->nome);

        if (strlen(atual->pista) > 0) {
            printf("🔎 Você encontrou uma pista: %s\n", atual->pista);
            *raizPistas = inserirPista(*raizPistas, atual->pista);

            inserirHash(atual->pista, atual->suspeitoLigado);
            printf("Esta pista está associada ao suspeito: %s\n",
                   atual->suspeitoLigado);
        }

        printf("Escolha:\n");
        printf("  (e) Ir para a esquerda\n");
        printf("  (d) Ir para a direita\n");
        printf("  (p) Ver pistas coletadas\n");
        printf("  (s) Sair da exploração\n> ");
        scanf(" %c", &opcao);

        if (opcao == 's') break;

        if (opcao == 'p') {
            printf("\n==== Pistas Coletadas ====\n");
            listarPistas(*raizPistas);
            continue;
        }

        if (opcao == 'e') {
            if (atual->esq) atual = atual->esq;
            else printf("Não há sala à esquerda!\n");
        }

        else if (opcao == 'd') {
            if (atual->dir) atual = atual->dir;
            else printf("Não há sala à direita!\n");
        }
    }
}

// -------------------------------------------------------------
//                            MAIN
// -------------------------------------------------------------

int main() {

    // inicializa hash
    for (int i = 0; i < TAM_HASH; i++)
        tabelaHash[i] = NULL;

    // monta a mansão (fixa)
    Sala *hall = criarSala("Hall de Entrada", "", "");

    hall->esq = criarSala("Biblioteca", "Marca de sapato", "Sr. Victor");
    hall->dir = criarSala("Cozinha", "", "");

    hall->esq->esq = criarSala("Arquivo Antigo", "Documento rasgado", "Sra. Helena");
    hall->esq->dir = criarSala("Sala de Leitura", "", "");

    hall->dir->dir = criarSala("Sótão", "Chave enferrujada", "Jardineiro");
    hall->dir->esq = criarSala("Despensa", "", "");

    PistaNode *raizPistas = NULL;

    printf("======= Detective Quest - Nível Mestre =======\n");
    explorarSalas(hall, &raizPistas);

    printf("\n=================================================\n");
    listarAssociacoes();

    printf("\n>>> Suspeito mais provável: %s\n",
           encontrarMaisSuspeito());

    printf("======== Fim da Investigação =========\n");

    return 0;
}
