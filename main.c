/* ================================================================
   main.c — Ponto de entrada da simulação da rede P2P
   Rede P2P com Grafos — Estruturas de Dados II
   ================================================================ */

#define _XOPEN_SOURCE 600  /* habilita usleep no C11 */
#include "grafo.h"

/* Limpa o terminal de forma portável */
static void limpar_tela(void) {
    system("clear");
}

/* Pausa aguardando o usuário pressionar Enter */
static void aguardar_enter(const char *msg) {
    printf("\n\033[90m  [ %s — pressione ENTER para continuar ]\033[0m", msg);
    while (getchar() != '\n');
}

int main(void) {

    /* ── 1. Topologia inicial ────────────────────────────────── */
    limpar_tela();
    printf("\033[1m\033[34m"
           "\n  ██████╗ ██████╗ ██████╗     ██████╗ ██████╗ ██████╗\n"
           "  ██╔══██╗╚════██╗██╔══██╗    ██╔══██╗╚════██╗██╔══██╗\n"
           "  ██████╔╝ █████╔╝██████╔╝    ██████╔╝ █████╔╝██████╔╝\n"
           "  ██╔═══╝ ██╔═══╝ ██╔═══╝     ██╔═══╝  ╚═══██╗██╔═══╝\n"
           "  ██║     ███████╗██║         ██║      ██████╔╝██║\n"
           "  ╚═╝     ╚══════╝╚═╝         ╚═╝      ╚═════╝ ╚═╝\n"
           "\033[0m");
    printf("\033[90m  Simulação de Rede P2P com Grafos em C\n"
           "  Estruturas de Dados II — Seminário\n\033[0m\n");

    /* Cria grafo com 7 jogadores */
    Grafo *g = criar_grafo(7);

    adicionar_aresta(g, 0, 1, 12);
    adicionar_aresta(g, 0, 2, 45);
    adicionar_aresta(g, 1, 3,  8);
    adicionar_aresta(g, 1, 4, 30);
    adicionar_aresta(g, 2, 4, 20);
    adicionar_aresta(g, 3, 5, 15);
    adicionar_aresta(g, 4, 6, 22);
    adicionar_aresta(g, 5, 6, 10);

    imprimir_grafo(g);
    aguardar_enter("topologia carregada");

    /* ── 2. BFS a partir de P2 ───────────────────────────────── */
    limpar_tela();
    int pai_bfs[MAX], prof_bfs[MAX];

    Mensagem msg;
    msg.origem    = 2;
    msg.timestamp = 1001;
    strcpy(msg.tipo, "UPDATE_POS");
    msg.dados[0]  = 10.5f;
    msg.dados[1]  =  3.0f;
    msg.dados[2]  =  0.0f;

    bfs(g, msg.origem, pai_bfs, prof_bfs, &msg);

    imprimir_predecessores(msg.origem, pai_bfs, prof_bfs, g->total);
    imprimir_arvore(g, msg.origem, pai_bfs, prof_bfs);
    aguardar_enter("BFS concluído");

    /* ── 3. DFS a partir de P0 (comparação) ─────────────────── */
    limpar_tela();
    int pai_dfs[MAX], prof_dfs[MAX];
    int visitado[MAX] = {0};

    for (int i = 0; i < g->total; i++) { pai_dfs[i] = -1; prof_dfs[i] = -1; }
    pai_dfs[0]  = -1;
    prof_dfs[0] = 0;

    printf("\n\033[1m\033[35m"
           "╔══════════════════════════════════════════════╗\n"
           "║       DFS — BROADCAST EM PROFUNDIDADE        ║\n"
           "╚══════════════════════════════════════════════╝\n" "\033[0m");

    Mensagem msg2;
    msg2.origem    = 0;
    msg2.timestamp = 1002;
    strcpy(msg2.tipo, "SYNC_STATE");
    msg2.dados[0]  = 5.0f;
    msg2.dados[1]  = 1.0f;
    msg2.dados[2]  = 2.0f;

    printf("\033[1m\033[33m"
           "\n  [P%d] ★ DISPARA  →  %s  (x=%.1f, y=%.1f, z=%.1f)  · timestamp=%d\n\n"
           "\033[0m",
           msg2.origem, msg2.tipo,
           msg2.dados[0], msg2.dados[1], msg2.dados[2],
           msg2.timestamp);

    dfs(g, msg2.origem, visitado, pai_dfs, prof_dfs, 0, &msg2);

    imprimir_predecessores(msg2.origem, pai_dfs, prof_dfs, g->total);
    imprimir_arvore(g, msg2.origem, pai_dfs, prof_dfs);
    aguardar_enter("DFS concluído");

    /* ── 4. Rede íntegra — detectar componentes ─────────────── */
    limpar_tela();
    printf("\033[1m\033[36m\n  ▶ Verificando integridade da rede...\n\n\033[0m");
    int comps = detectar_componentes(g);
    if (comps > 1)
        printf("\033[31m\033[1m  ALERTA: rede fragmentada em %d grupos!\033[0m\n\n", comps);
    aguardar_enter("rede verificada");

    /* ── 5. Simular quedas de conexão ───────────────────────── */
    limpar_tela();
    printf("\033[1m\033[31m\n"
           "╔══════════════════════════════════════════════╗\n"
           "║       SIMULAÇÃO DE QUEDAS DE CONEXÃO         ║\n"
           "╚══════════════════════════════════════════════╝\n\n\033[0m");

    printf("\033[90m  Removendo conexão P1 ↔ P3...\033[0m\n");
    remover_aresta(g, 1, 3);
    usleep(400000);

    printf("\033[90m  Removendo conexão P4 ↔ P6...\033[0m\n");
    remover_aresta(g, 4, 6);
    usleep(400000);

    printf("\033[90m  Removendo conexão P3 ↔ P5...\033[0m\n");
    remover_aresta(g, 3, 5);
    usleep(400000);

    printf("\n");
    imprimir_grafo(g);
    aguardar_enter("quedas aplicadas");

    /* ── 6. Detectar fragmentação após quedas ───────────────── */
    limpar_tela();
    printf("\033[1m\033[33m\n  ▶ Analisando fragmentação da rede após quedas...\n\n\033[0m");
    comps = detectar_componentes(g);
    if (comps > 1)
        printf("\033[31m\033[1m  ALERTA: rede fragmentada em %d grupos!\033[0m\n\n", comps);
    aguardar_enter("análise concluída");

    /* ── 7. Encerramento ────────────────────────────────────── */
    limpar_tela();
    printf("\033[1m\033[32m"
           "\n  ✓ Simulação concluída!\n\n"
           "\033[0m\033[90m"
           "  Estruturas liberadas da memória.\n"
           "  Encerrando...\n\n\033[0m");

    liberar_grafo(g);
    return 0;
}
