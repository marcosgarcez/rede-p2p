# 🌐 Simulador de rede P2P (Peer-to-Peer)

Bem-vindo ao **Simulador de rede P2P**, um projeto desenvolvido em **C** que utiliza a estrutura de dados de **grafos** para visualizar e simular o roteamento de pacotes e broadcast em uma rede de computadores descentralizada! 🚀

## 🎯 Sobre o projeto

Este projeto tem o objetivo de demonstrar como mensagens navegam através de uma rede P2P simulada. Ele constrói uma topologia de rede através de um **grafo direcionado**, onde cada "nó" representa uma máquina (P0, P1, etc.) e cada "aresta" representa uma conexão com uma determinada **latência** (em milissegundos).

A principal estrela do show é o algoritmo de **Busca em Largura (BFS)** modificado para simular a realidade das redes: mensagens sempre trafegam primeiro pelos links mais rápidos (menor latência)! ⚡

## ✨ Funcionalidades principais

- 📍 **Topologia realista:** Definição manual de nós e vizinhos, com latências personalizadas para cada conexão.
- 📡 **Simulação de broadcast:** Um nó dispara uma mensagem (ex: arquivo, alerta) e ela se propaga em cascata por toda a rede.
- ⏱️ **Roteamento por baixa latência:** O tráfego de mensagens prioriza os cabos mais rápidos, usando uma otimização com *Insertion Sort* na exploração dos nós da BFS.
- 🌳 **Árvore de predecessores:** Rastreamento completo de como a mensagem viajou (quem enviou para quem) e quantos saltos a mensagem precisou dar.
- 🎨 **Terminal animado e colorido:** As atualizações acontecem em tempo real no terminal, utilizando cores ANSI e pequenos "delays" (`usleep`) para criar uma sensação visual de fluxo de rede.
- 🔀 **Broadcast via DFS:** Exploração profunda da rede simulando sincronização de estado.
- ✂️ **Simulação de falhas:** Desconexão dinâmica de computadores (queda de link) para testar a resiliência da topologia.
- 🧩 **Verificação de integridade:** Detecção de componentes conexos isolados utilizando o algoritmo *Union-Find*.

## 🛠️ Tecnologias utilizadas

- **Linguagem:** C (Padrão C11, com suporte POSIX para animações)
- **Estruturas de dados:** 
  - Grafos (Lista de Adjacência)
  - Filas (Implementação por array para a BFS)
- **Algoritmos:**
  - BFS (Busca em Largura) customizada
  - DFS (Busca em Profundidade)
  - Insertion Sort (Ordenação de prioridade)
  - Union-Find (Detecção de componentes conexos)

## 🚀 Como executar

### Pré-requisitos
- Compilador GCC instalado no sistema (Linux/macOS ou MinGW no Windows).
- Ferramenta `make` instalada.

### Passo a passo

1. **Clone o repositório:**
   ```bash
   git clone <url-do-seu-repositorio>
   cd rede-p2p
   ```

2. **Compile o projeto usando o Makefile:**
   ```bash
   make
   ```

3. **Rode a simulação:**
   ```bash
   ./rede_p2p
   ```

4. *(Opcional)* Para limpar os arquivos compilados:
   ```bash
   make clean
   ```

## 📂 Estrutura de arquivos

* `grafo.h`: Cabeçalho do projeto. Contém as definições das estruturas de `Grafo`, `Aresta`, `Fila`, `Mensagem` e as assinaturas das funções.
* `grafo.c`: O coração lógico! Implementação da criação do grafo, impressão colorida, e dos algoritmos de busca (BFS e DFS).
* `main.c`: Ponto de entrada do sistema. Monta a topologia da rede, inicializa os dados e aciona as simulações.
* `Makefile`: Automação da compilação do código C.
* `docs/index.html`: Documentação web interativa detalhando estruturas e algoritmos do projeto.

---
*Feito com 💻 e muita lógica de Grafos!*
