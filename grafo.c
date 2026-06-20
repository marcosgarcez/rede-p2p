/* ================================================================
   grafo.c — Implementação das funções do grafo P2P
   Rede P2P com Grafos — Estruturas de Dados II
   ================================================================ */

#include "grafo.h"

/* ────────────────────────────────────────────────
   CÓDIGOS ANSI DE COR
──────────────────────────────────────────────── */
#define RESET   "\033[0m"
#define NEGRITO "\033[1m"
#define CIANO   "\033[36m"
#define AMARELO "\033[33m"
#define VERDE   "\033[32m"
#define MAGENTA "\033[35m"
#define VERMELHO "\033[31m"
#define AZUL    "\033[34m"
#define BRANCO  "\033[97m"
#define CINZA   "\033[90m"

/* ────────────────────────────────────────────────
   FILA MANUAL
──────────────────────────────────────────────── */

void enfileirar(Fila *f, int id) {
    f->dados[f->fim++] = id;
}

int desenfileirar(Fila *f) {
    return f->dados[f->inicio++];
}

int fila_vazia(Fila *f) {
    return f->inicio >= f->fim;
}

/* ────────────────────────────────────────────────
   CRIAÇÃO E DESTRUIÇÃO DO GRAFO
──────────────────────────────────────────────── */

Grafo *criar_grafo(int n) {
    Grafo *g = malloc(sizeof(Grafo));
    if (!g) { perror("malloc grafo"); exit(1); }

    g->total = n;
    g->nos   = malloc(n * sizeof(No *));
    if (!g->nos) { perror("malloc nos"); exit(1); }

    for (int i = 0; i < n; i++) {
        g->nos[i]           = malloc(sizeof(No));
        if (!g->nos[i]) { perror("malloc no"); exit(1); }
        g->nos[i]->id       = i;
        g->nos[i]->vizinhos = NULL;
    }
    return g;
}

/* Insere aresta no início da lista encadeada (direção a → b) */
static void inserir_aresta_dir(Grafo *g, int a, int b, int lat) {
    Aresta *nova    = malloc(sizeof(Aresta));
    if (!nova) { perror("malloc aresta"); exit(1); }
    nova->destino   = b;
    nova->latencia  = lat;
    nova->proxima   = g->nos[a]->vizinhos;
    g->nos[a]->vizinhos = nova;
}

void adicionar_aresta(Grafo *g, int a, int b, int lat) {
    inserir_aresta_dir(g, a, b, lat);
    inserir_aresta_dir(g, b, a, lat);
}

/* Remove a aresta a → b da lista encadeada do nó a */
static void remover_aresta_dir(Grafo *g, int a, int b) {
    Aresta **atual = &g->nos[a]->vizinhos;
    while (*atual) {
        if ((*atual)->destino == b) {
            Aresta *tmp = *atual;
            *atual = (*atual)->proxima;
            free(tmp);
            return;
        }
        atual = &(*atual)->proxima;
    }
}

void remover_aresta(Grafo *g, int a, int b) {
    remover_aresta_dir(g, a, b);
    remover_aresta_dir(g, b, a);
}

void liberar_grafo(Grafo *g) {
    for (int i = 0; i < g->total; i++) {
        Aresta *aresta = g->nos[i]->vizinhos;
        while (aresta) {
            Aresta *tmp = aresta;
            aresta = aresta->proxima;
            free(tmp);
        }
        free(g->nos[i]);
    }
    free(g->nos);
    free(g);
}

/* ────────────────────────────────────────────────
   EXIBIÇÃO DO GRAFO
──────────────────────────────────────────────── */

void imprimir_grafo(Grafo *g) {
    printf("\n" NEGRITO AZUL
           "╔══════════════════════════════════════════════╗\n"
           "║         TOPOLOGIA DA REDE P2P                ║\n"
           "╚══════════════════════════════════════════════╝\n" RESET);

    for (int i = 0; i < g->total; i++) {
        printf(CIANO NEGRITO "  P%-2d" RESET, g->nos[i]->id);
        printf(CINZA " ──▶ " RESET);

        Aresta *a = g->nos[i]->vizinhos;
        if (!a) {
            printf(VERMELHO "(sem conexões)\n" RESET);
            continue;
        }
        while (a) {
            printf(VERDE "P%d" RESET
                   "(" AMARELO "%dms" RESET ")",
                   a->destino, a->latencia);
            if (a->proxima) printf(CINZA "  ──▶  " RESET);
            a = a->proxima;
        }
        printf("\n");
    }
    printf("\n");
}

/* ────────────────────────────────────────────────
   BFS
──────────────────────────────────────────────── */

/* Retorna a latência da aresta entre `a` e `b`, ou 0 se não existir */
static int latencia_entre(Grafo *g, int a, int b) {
    Aresta *ar = g->nos[a]->vizinhos;
    while (ar) {
        if (ar->destino == b) return ar->latencia;
        ar = ar->proxima;
    }
    return 0;
}

void bfs(Grafo *g, int src, int pai[], int prof[], Mensagem *msg) {
    int visitado[MAX] = {0};

    /* Inicializa arrays */
    for (int i = 0; i < g->total; i++) { pai[i] = -1; prof[i] = -1; }

    Fila f = { .inicio = 0, .fim = 0 };
    visitado[src] = 1;
    pai[src]      = -1;
    prof[src]     = 0;
    enfileirar(&f, src);

    printf("\n" NEGRITO MAGENTA
           "╔══════════════════════════════════════════════╗\n"
           "║       BFS — BROADCAST EM LARGURA             ║\n"
           "╚══════════════════════════════════════════════╝\n" RESET);

    if (msg) {
        printf(NEGRITO AMARELO
               "\n  [P%d] ★ DISPARA  →  %s  (x=%.1f, y=%.1f, z=%.1f)  · timestamp=%d\n\n" RESET,
               msg->origem, msg->tipo,
               msg->dados[0], msg->dados[1], msg->dados[2],
               msg->timestamp);
    }

    while (!fila_vazia(&f)) {
        int atual = desenfileirar(&f); //desenfileira o nó atual

        /* Exibe o nó atual sendo processado */
        if (atual == src) { //se o nó atual for a fonte
            printf(CIANO "  [P%d]" RESET VERDE " (fonte/raiz)\n" RESET, atual); //mostra que é a fonte
        } else if (msg) { //se tiver mensagem
            int lat = latencia_entre(g, pai[atual], atual);
            printf(AZUL "  [P%d]" RESET " RECEBE  ← P%d  · salto %d · latência " AMARELO "%dms\n" RESET,
                   atual, pai[atual], prof[atual], lat);
        } else {
            printf(AZUL "  [P%d]" RESET " ← P%d  · nível %d\n",
                   atual, pai[atual], prof[atual]);
        }

        /* Pequena pausa para efeito de animação */
        usleep(300000);

        /*
         * Percorre vizinhos. Para preferir menor latência, coletamos
         * os não-visitados e os inserimos na fila em ordem crescente
         * de latência (selection simples, lista pequena).
         */
        /* Monta lista temporária de candidatos */
        int cand[MAX], clat[MAX], nc = 0; //canditados, latencia dos canditados e numero de canditados
        for (Aresta *a = g->nos[atual]->vizinhos; a; a = a->proxima) {
            if (!visitado[a->destino]) {
                cand[nc]   = a->destino;
                clat[nc++] = a->latencia;
            }
        }
        /* Ordena por latência (insertion sort) */
        for (int i = 1; i < nc; i++) {
            int kd = cand[i], kl = clat[i], j = i - 1; /*key destino, key latencia e j (indice anterior)*/
            while (j >= 0 && clat[j] > kl) {
                cand[j+1] = cand[j]; 
                clat[j+1] = clat[j]; 
                j--;
            }
            cand[j+1] = kd; 
            clat[j+1] = kl;
        }
        /* Enfileira os vizinhos do nó atual em ordem crescente de latência */
        for (int i = 0; i < nc; i++) {
            visitado[cand[i]] = 1;
            pai[cand[i]]      = atual;
            prof[cand[i]]     = prof[atual] + 1;
            enfileirar(&f, cand[i]); //enfileira os vizinhos do nó atual
        }
    }
    printf("\n");
}

/* ────────────────────────────────────────────────
   DFS
──────────────────────────────────────────────── */

void dfs(Grafo *g, int id, int visitado[], int pai[], int prof[],
         int profAtual, Mensagem *msg) {

    visitado[id] = 1;

    /* Indentação proporcional à profundidade */
    for (int i = 0; i < profAtual; i++) printf("   ");

    if (profAtual == 0) {
        printf(CIANO NEGRITO "[P%d]" RESET VERDE " ★ (raiz DFS)\n" RESET, id);
    } else {
        int lat = latencia_entre(g, pai[id], id);
        if (msg) {
            printf(AZUL "[P%d]" RESET " ← P%d · salto %d · " AMARELO "%dms\n" RESET,
                   id, pai[id], profAtual, lat);
        } else {
            printf(AZUL "[P%d]" RESET " ← P%d · nível %d · " AMARELO "%dms\n" RESET,
                   id, pai[id], profAtual, lat);
        }
    }

    usleep(300000);

    /* Percorre os vizinhos do nó atual */
    for (Aresta *a = g->nos[id]->vizinhos; a; a = a->proxima) {
        if (!visitado[a->destino]) { //se o vizinho não foi visitado
            pai[a->destino]  = id; //marca o pai do vizinho
            prof[a->destino] = profAtual + 1; //marca a profundidade do vizinho
            dfs(g, a->destino, visitado, pai, prof, profAtual + 1, msg); //chama a dfs para o vizinho
        }
    }
}

/* ────────────────────────────────────────────────
   ÁRVORE DE PREDECESSORES
──────────────────────────────────────────────── */

void imprimir_predecessores(int src, int pai[], int prof[], int n) {
    (void)src;  /* parâmetro reservado para possível uso futuro */
    printf("\n" NEGRITO VERDE
           "╔══════════════════════════════════════════════╗\n"
           "║         ÁRVORE DE PREDECESSORES              ║\n"
           "╚══════════════════════════════════════════════╝\n" RESET);

    printf(NEGRITO "  %-6s  %-12s  %s\n" RESET, "Nó", "Predecessor", "Salto");
    printf(CINZA "  ──────────────────────────────────\n" RESET);

    for (int i = 0; i < n; i++) {
        if (prof[i] < 0) continue; /* nó não alcançado */
        printf("  " CIANO "P%-4d" RESET, i);
        if (pai[i] == -1)
            printf(CINZA "%-14s" RESET, "--  (fonte)");
        else
            printf(VERDE "P%-13d" RESET, pai[i]);
        printf(AMARELO "%d\n" RESET, prof[i]);
    }
    printf("\n");
}

/* ────────────────────────────────────────────────
   ÁRVORE GERADORA (VISUAL COM ├── / └──)
──────────────────────────────────────────────── */

/* Imprime recursivamente a subárvore enraizada em `raiz` */
static void imprimir_sub(Grafo *g, int raiz, int pai[], int prof[],
                          int n, char *prefixo, int eh_ultimo) {
    (void)eh_ultimo;  /* mantido na assinatura para legibilidade */

    /* Coleta filhos do nó raiz */
    int filhos[MAX], nf = 0;
    for (int i = 0; i < n; i++)
        if (pai[i] == raiz && i != raiz && prof[i] == prof[raiz] + 1)
            filhos[nf++] = i;

    for (int f = 0; f < nf; f++) {
        int filho = filhos[f];
        int lat   = latencia_entre(g, raiz, filho);
        int ultimo = (f == nf - 1);

        printf("%s", prefixo);
        printf(CINZA "%s" RESET, ultimo ? "└── " : "├── ");
        printf(AZUL NEGRITO "P%d" RESET
               CINZA "  (salto %d, " RESET
               AMARELO "%dms" RESET CINZA ")" RESET "\n",
               filho, prof[filho], lat);

        /* Novo prefixo para os filhos deste filho */
        char novo_pref[256];
        snprintf(novo_pref, sizeof(novo_pref), "%s%s",
                 prefixo, ultimo ? "    " : "│   ");

        imprimir_sub(g, filho, pai, prof, n, novo_pref, ultimo);
    }
}

void imprimir_arvore(Grafo *g, int src, int pai[], int prof[]) {
    printf("\n" NEGRITO AMARELO
           "╔══════════════════════════════════════════════╗\n"
           "║           ÁRVORE GERADORA                    ║\n"
           "╚══════════════════════════════════════════════╝\n\n" RESET);

    printf(CIANO NEGRITO "P%d" RESET VERDE "  ★ fonte\n" RESET, src);

    char prefixo[256] = "";
    imprimir_sub(g, src, pai, prof, g->total, prefixo, 1);
    printf("\n");
}

/* ────────────────────────────────────────────────
   UNION-FIND
──────────────────────────────────────────────── */

void uf_inicializar(int *parent, int *rank, int n) {
    for (int i = 0; i < n; i++) { parent[i] = i; rank[i] = 0; }
}

int uf_find(int *parent, int x) {
    if (parent[x] != x)
        parent[x] = uf_find(parent, parent[x]); /* compressão de caminho */
    return parent[x];
}

void uf_unite(int *parent, int *rank, int a, int b) {
    int ra = uf_find(parent, a);
    int rb = uf_find(parent, b);
    if (ra == rb) return;
    if (rank[ra] < rank[rb])      parent[ra] = rb;
    else if (rank[ra] > rank[rb]) parent[rb] = ra;
    else { parent[rb] = ra; rank[ra]++; }
}

/* ────────────────────────────────────────────────
   DETECTAR COMPONENTES CONEXOS
──────────────────────────────────────────────── */

int detectar_componentes(Grafo *g) {
    int n      = g->total;
    int *parent = malloc(n * sizeof(int));
    int *rank   = malloc(n * sizeof(int));
    if (!parent || !rank) { perror("malloc uf"); exit(1); }

    uf_inicializar(parent, rank, n);

    /* Une todos os pares conectados */
    for (int i = 0; i < n; i++) {
        for (Aresta *a = g->nos[i]->vizinhos; a; a = a->proxima)
            uf_unite(parent, rank, i, a->destino);
    }

    /* Conta raízes distintas */
    int num_comp = 0;
    int raiz_comp[MAX];
    for (int i = 0; i < n; i++) {
        if (uf_find(parent, i) == i)
            raiz_comp[num_comp++] = i;
    }

    /* Cabeçalho em destaque */
    printf("\n" NEGRITO VERMELHO
           "╔══════════════════════════════════════════════╗\n");
    printf("║  COMPONENTES CONEXOS ISOLADOS: %-13d║\n", num_comp);
    printf("╚══════════════════════════════════════════════╝\n\n" RESET);

    /* Lista cada componente */
    for (int c = 0; c < num_comp; c++) {
        int membros[MAX], nm = 0;
        for (int i = 0; i < n; i++)
            if (uf_find(parent, i) == raiz_comp[c])
                membros[nm++] = i;

        if (nm == 1)
            printf(AMARELO "  Componente %d " RESET CINZA "(1 jogador)  : " RESET, c + 1);
        else
            printf(AZUL   "  Componente %d " RESET CINZA "(%d jogadores): " RESET, c + 1, nm);

        for (int i = 0; i < nm; i++) {
            printf(CIANO "P%d" RESET, membros[i]);
            if (i < nm - 1) printf("  ");
        }
        if (nm == 1) printf(VERMELHO "  ← isolado" RESET);
        printf("\n");
    }

    if (num_comp > 1)
        printf(VERMELHO NEGRITO
               "\n  Rede fragmentada. Jogadores isolados não recebem atualizações.\n" RESET);
    else
        printf(VERDE NEGRITO
               "\n  Rede totalmente conectada. Todos os jogadores se comunicam.\n" RESET);

    printf("\n");

    free(parent);
    free(rank);
    return num_comp;
}
