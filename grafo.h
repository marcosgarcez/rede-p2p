#ifndef GRAFO_H
#define GRAFO_H

#define _XOPEN_SOURCE 600  /* habilita usleep / POSIX no C11 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX 100

/* ─────────────────────────────────────────────
   ESTRUTURAS DO GRAFO
───────────────────────────────────────────── */

typedef struct Aresta {
    int destino;
    int latencia;
    struct Aresta *proxima;
} Aresta;

typedef struct {
    int     id;
    Aresta *vizinhos;
} No;

typedef struct {
    No **nos;
    int  total;
} Grafo;

/* ─────────────────────────────────────────────
   MENSAGEM P2P
───────────────────────────────────────────── */

typedef struct {
    int   origem;
    char  tipo[20];
    float dados[3];
    int   timestamp;
} Mensagem;

/* ─────────────────────────────────────────────
   FILA MANUAL (BFS)
───────────────────────────────────────────── */

typedef struct {
    int dados[MAX];
    int inicio;
    int fim;
} Fila;

/* ─────────────────────────────────────────────
   PROTÓTIPOS — GRAFO
───────────────────────────────────────────── */

Grafo *criar_grafo(int n);
void   adicionar_aresta(Grafo *g, int a, int b, int lat);
void   remover_aresta(Grafo *g, int a, int b);
void   liberar_grafo(Grafo *g);
void   imprimir_grafo(Grafo *g);

/* ─────────────────────────────────────────────
   PROTÓTIPOS — BUSCA
───────────────────────────────────────────── */

void bfs(Grafo *g, int src, int pai[], int prof[], Mensagem *msg);
void dfs(Grafo *g, int id, int visitado[], int pai[], int prof[], int profAtual, Mensagem *msg);

/* ─────────────────────────────────────────────
   PROTÓTIPOS — ÁRVORE
───────────────────────────────────────────── */

void imprimir_predecessores(int src, int pai[], int prof[], int n);
void imprimir_arvore(Grafo *g, int src, int pai[], int prof[]);

/* ─────────────────────────────────────────────
   PROTÓTIPOS — UNION-FIND / COMPONENTES
───────────────────────────────────────────── */

void uf_inicializar(int *parent, int *rank, int n);
int  uf_find(int *parent, int x);
void uf_unite(int *parent, int *rank, int a, int b);
int  detectar_componentes(Grafo *g);

/* ─────────────────────────────────────────────
   PROTÓTIPOS — UTILITÁRIOS
───────────────────────────────────────────── */

void enfileirar(Fila *f, int id);
int  desenfileirar(Fila *f);
int  fila_vazia(Fila *f);

#endif /* GRAFO_H */
