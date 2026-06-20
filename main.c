/* ================================================================
   main.c — Ponto de entrada da simulação da rede P2P
   Rede P2P com Grafos — Estruturas de Dados II
   ================================================================ */

#define _XOPEN_SOURCE 600
#include "grafo.h"

/* Limpa o terminal de forma portável */
static void limpar_tela(void) {
    system("clear");
}

/* Pausa aguardando o usuário pressionar Enter */
static void aguardar_enter(const char *msg) {
    printf("\n\033[90m  [ %s — pressione ENTER para continuar ]\033[0m", msg);
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Lê um número inteiro de forma segura do terminal */
static int ler_inteiro(const char *prompt) {
    int valor;
    char buffer[256];
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            if (sscanf(buffer, "%d", &valor) == 1) {
                return valor;
            }
        }
        printf("\033[31m  Entrada inválida. Digite um número inteiro.\033[0m\n");
    }
}

int main(void) {
    /* Cria grafo com 7 jogadores para iniciar a topologia padrão */
    Grafo *g = criar_grafo(7);

    adicionar_aresta(g, 0, 1, 12);
    adicionar_aresta(g, 0, 2, 45);
    adicionar_aresta(g, 1, 3,  8);
    adicionar_aresta(g, 1, 4, 30);
    adicionar_aresta(g, 2, 4, 20);
    adicionar_aresta(g, 3, 5, 15);
    adicionar_aresta(g, 4, 6, 22);
    adicionar_aresta(g, 5, 6, 10);

    /* Loop do Menu Interativo */
    while (1) {
        limpar_tela();
        printf("\033[1m\033[34m"
               "  _____ ___  _____     _____ _____ __  __ \n"
               " |  __ \\__ \\|  __ \\   / ____|_   _|  \\/  |\n"
               " | |__) | ) | |__) | | (___   | | | \\  / |\n"
               " |  ___/ / /|  ___/   \\___ \\  | | | |\\/| |\n"
               " | |    / /_| |       ____) |_| |_| |  | |\n"
               " |_|   |____|_|      |_____/|_____|_|  |_|\n"
               "\033[0m");
        printf("\033[90m  Simulação de Rede P2P com Grafos em C\n"
               "  Estruturas de Dados II — Seminário\n\033[0m\n");
               
        printf("  \033[1m1.\033[0m Visualizar topologia da rede\n");
        printf("  \033[1m2.\033[0m Disparar Mensagem (Broadcast BFS)\n");
        printf("  \033[1m3.\033[0m Disparar Mensagem (Broadcast DFS)\n");
        printf("  \033[1m4.\033[0m Desconectar computadores (Simular queda)\n");
        printf("  \033[1m5.\033[0m Verificar integridade (Componentes Conexos isolados)\n");
        printf("  \033[1m0.\033[0m Sair\n\n");

        int opcao = ler_inteiro("  \033[33mEscolha uma opção:\033[0m ");

        if (opcao == 0) {
            break;
        } else if (opcao == 1) {
            /* 1. Visualizar rede */
            limpar_tela();
            imprimir_grafo(g);
            aguardar_enter("visualização concluída");
            
        } else if (opcao == 2) {
            /* 2. Disparar Mensagem BFS */
            limpar_tela();
            int src = ler_inteiro("  \033[36mDigite o ID do nó de origem (ex: 0):\033[0m ");
            if (src < 0 || src >= g->total) {
                printf("\033[31m\n  Nó inválido. O grafo possui nós de 0 a %d.\033[0m\n", g->total - 1);
                aguardar_enter("voltar");
                continue;
            }
            
            /* Pré-requisito: mostrar os grafos desconexos antes do disparo */
            limpar_tela();
            printf("\033[1m\033[36m\n  ▶ Verificando conectividade da rede antes do disparo...\n\033[0m");
            detectar_componentes(g);
            aguardar_enter("iniciar broadcast (BFS)");
            
            limpar_tela();
            int pai_bfs[MAX], prof_bfs[MAX];
            Mensagem msg;
            msg.origem    = src;
            msg.timestamp = 2001;
            strcpy(msg.tipo, "UPDATE_POS");
            msg.dados[0]  = 10.5f;
            msg.dados[1]  =  3.0f;
            msg.dados[2]  =  0.0f;

            bfs(g, src, pai_bfs, prof_bfs, &msg);
            imprimir_predecessores(src, pai_bfs, prof_bfs, g->total);
            imprimir_arvore(g, src, pai_bfs, prof_bfs);
            aguardar_enter("BFS concluída");

        } else if (opcao == 3) {
            /* 3. Disparar Mensagem DFS */
            limpar_tela();
            int src = ler_inteiro("  \033[36mDigite o ID do nó de origem (ex: 0):\033[0m ");
            if (src < 0 || src >= g->total) {
                printf("\033[31m\n  Nó inválido. O grafo possui nós de 0 a %d.\033[0m\n", g->total - 1);
                aguardar_enter("voltar");
                continue;
            }
            
            /* Pré-requisito: mostrar os grafos desconexos antes do disparo */
            limpar_tela();
            printf("\033[1m\033[36m\n  ▶ Verificando conectividade da rede antes do disparo...\n\033[0m");
            detectar_componentes(g);
            aguardar_enter("iniciar broadcast (DFS)");

            limpar_tela();
            int pai_dfs[MAX], prof_dfs[MAX], visitado[MAX] = {0};
            for (int i = 0; i < g->total; i++) { pai_dfs[i] = -1; prof_dfs[i] = -1; }
            pai_dfs[src]  = -1;
            prof_dfs[src] = 0;

            printf("\n\033[1m\033[35m"
                   "╔══════════════════════════════════════════════╗\n"
                   "║       DFS — BROADCAST EM PROFUNDIDADE        ║\n"
                   "╚══════════════════════════════════════════════╝\n" "\033[0m");

            Mensagem msg2;
            msg2.origem    = src;
            msg2.timestamp = 2002;
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

            dfs(g, src, visitado, pai_dfs, prof_dfs, 0, &msg2);
            imprimir_predecessores(src, pai_dfs, prof_dfs, g->total);
            imprimir_arvore(g, src, pai_dfs, prof_dfs);
            aguardar_enter("DFS concluída");

        } else if (opcao == 4) {
            /* 4. Desconectar computadores */
            limpar_tela();
            printf("\033[1m\033[31m\n  Desconectar Computadores (Simular Queda de Link)\n\n\033[0m");
            int a = ler_inteiro("  Digite o ID do primeiro nó: ");
            int b = ler_inteiro("  Digite o ID do segundo nó: ");
            if (a >= 0 && a < g->total && b >= 0 && b < g->total) {
                remover_aresta(g, a, b);
                printf("\033[32m\n  Conexão entre P%d e P%d removida com sucesso!\033[0m\n", a, b);
            } else {
                printf("\033[31m\n  IDs inválidos!\033[0m\n");
            }
            aguardar_enter("retornar ao menu");

        } else if (opcao == 5) {
            /* 5. Verificar Integridade */
            limpar_tela();
            printf("\033[1m\033[36m\n  ▶ Verificando componentes conexos isolados...\n\033[0m");
            detectar_componentes(g);
            aguardar_enter("retornar ao menu");
            
        } else {
            printf("\033[31m\n  Opção inválida!\033[0m\n");
            aguardar_enter("tentar novamente");
        }
    }

    /* ── Encerramento ────────────────────────────────────── */
    limpar_tela();
    printf("\033[1m\033[32m"
           "\n  ✓ Simulação encerrada com sucesso!\n\n"
           "\033[0m\033[90m"
           "  Estruturas liberadas da memória.\n"
           "  Até logo!\n\n\033[0m");

    liberar_grafo(g);
    return 0;
}
