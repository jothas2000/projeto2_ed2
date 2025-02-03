//NOME:Thales do Prado Menendez   RA: 2510170
//NOME: Vitor Pereira de Moraes   RA: 2510197

//alguns cometantarios sao meio compridos, entao aconselho ativar o word wrap alt+z no VS code, e nao uso pontuacao gramatical nos comentarios e nem letra maiuscula

//bibliotecas nativas
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//estruturacao da avl, heap, no da heap, nohash e tabelahash
typedef struct NoAVL {
    int codigo;
    int capacidade;
    int altura;
    struct NoAVL* esq;
    struct NoAVL* dir;
} NoAVL;

typedef struct NoHeap {
    int codigoSala;
    int prioridade;
} NoHeap;

typedef struct Heap {
    NoHeap* reservas;
    int tamanho;
    int capacidade;
} Heap;

typedef struct NoHash {
    int codigoSala;
    char estado[20];
    struct NoHash* proximo;
} NoHash;

typedef struct TabelaHash {
    NoHash** tabela;
    int tamanho;
} TabelaHash;

typedef struct SalaCapacidade{
    int codigo;
    int capacidade;
} SalaCapacidade;

//prototipos das funcoes
NoAVL* criarNoAVL(int codigo, int capacidade);
int alturaAVL(NoAVL* no);
int fatorBalanceamento(NoAVL* no);
NoAVL* rotacaoDireita(NoAVL* y);
NoAVL* rotacaoEsquerda(NoAVL* x);
NoAVL* inserirSala(NoAVL* no, int codigo, int capacidade);
Heap* criarHeap(int capacidade);
void inserirReserva(Heap* heap, int codigoSala, int prioridade);
NoHeap removerMax(Heap* heap);
void listarReservasPorPrioridade(Heap* heap, FILE* arquivo);
TabelaHash* criarTabelaHash(int tamanho);
int hash(int codigoSala, int tamanho);
void inserirEstado(TabelaHash* th, int codigoSala, const char* estado);
char* consultarEstado(TabelaHash* th, int codigoSala);
void listarSalasInOrder(NoAVL* raiz, FILE* arquivo);
void listarSalasPorEstado(TabelaHash* th, FILE* arquivo);
void listarSalasPorCapacidade(NoAVL* raiz, FILE* arquivo, int ordem);
void cancelarReserva(Heap* heap, int codigoSala);
void atualizarPrioridade(Heap* heap, int codigoSala, int novaPrioridade);
void lerArquivoEntrada(const char* nomeArquivo, NoAVL** raiz, Heap* heap, TabelaHash* th);
void gerarArquivoSaida(const char* nomeArquivo, NoAVL* raiz, Heap* heap, TabelaHash* th);
void desalocarTabelaHash(TabelaHash* th);
void desalocarHeap(Heap* heap);
void desalocarAVL(NoAVL* raiz);
int maximo(int a, int b);

//funcao main
int main() 
{
    //forma facil de fazer um log com todos as informacoes que seriam mostradas no terminal
    if (freopen("saida.txt", "w", stdout) == NULL) 
    {
            perror("Erro ao redirecionar a saída");
            return 1;
    }
    printf("_ARQUIVO DE LOG_\n");
    printf("================\n\n");

    //inicializa as estruturas de dados utilizadas no projeto, coloquei um tamanho que acho que seja o suficiente na heap e na hash
    NoAVL* raiz = NULL; //arvore AVL para gerenciar salas
    Heap* heap = criarHeap(100); //heap para gerenciar reservas
    TabelaHash* th = criarTabelaHash(10); //tabela Hash para gerenciar estados das salas

    //ler o arquivo de entrada e processar as operacoes
    printf("Processando arquivo de entrada!\n");
    lerArquivoEntrada("entrada4.txt", &raiz, heap, th);

    //desalocar memórias
    printf("Desalocando memória!\n");
    desalocarAVL(raiz); //desaloca a arvore AVL
    desalocarHeap(heap); //desaloca a Heap
    desalocarTabelaHash(th); //desaloca a Tabela Hash

    //mensagem de conclusao
    printf("Processamento concluído!\nMemória desalocada com sucesso!\n");
    return 0;
}

NoAVL* criarNoAVL(int codigo, int capacidade) 
{
    NoAVL* novoNo = (NoAVL*)malloc(sizeof(NoAVL));
    if (!novoNo) { //se malloc falhou
        printf("Erro ao alocar memória para a sala %d!\n", codigo);
        return NULL; //retorna NULL para indicar falha
    }

    //inicializa o no apenas se malloc deu certo
    novoNo->codigo = codigo;
    novoNo->capacidade = capacidade;
    novoNo->altura = 1;
    novoNo->esq = NULL;
    novoNo->dir = NULL;

    return novoNo; //retorna o ponteiro
}

//funcao para inserir uma sala na arvore AVL
NoAVL* inserirSala(NoAVL* no, int codigo, int capacidade) 
{
    //insercao normal na arvore binaria de busca
    if (no == NULL)
        return criarNoAVL(codigo, capacidade);

    if (codigo < no->codigo)
        no->esq = inserirSala(no->esq, codigo, capacidade);
    else if (codigo > no->codigo)
        no->dir = inserirSala(no->dir, codigo, capacidade);
    else //nao pode codigos iguais
        return no;

    //autaliza a altura do no atual
    no->altura = 1 + maximo(alturaAVL(no->esq), alturaAVL(no->dir));

    //calcula o fator de balanceamento
    int balanceamento = fatorBalanceamento(no);

    //verifica os casos e realiza a rotacao
    //caso Esquerda-Esquerda (LL)
    if (balanceamento > 1 && codigo < no->esq->codigo)
        return rotacaoDireita(no);

    //caso Direita-Direita (RR)
    if (balanceamento < -1 && codigo > no->dir->codigo)
        return rotacaoEsquerda(no);

    //caso Esquerda-Direita (LR)
    if (balanceamento > 1 && codigo > no->esq->codigo) 
    {
        no->esq = rotacaoEsquerda(no->esq);
        return rotacaoDireita(no);
    }

    //caso Direita-Esquerda (RL)
    if (balanceamento < -1 && codigo < no->dir->codigo) 
    {
        no->dir = rotacaoDireita(no->dir);
        return rotacaoEsquerda(no);
    }

    //retorna o ponteiro do nó sem mudanca caso nao caia em nenhum caso
    return no;
}

//funcao para calcular a altura de um no
int alturaAVL(NoAVL* no) 
{
    if (no == NULL)
        return 0;
    return no->altura;
}

//funcao para calcular o fator de balanceamento de um no
int fatorBalanceamento(NoAVL* no) 
{
    if (no == NULL)
        return 0;
    return alturaAVL(no->esq) - alturaAVL(no->dir);
}

//funcao para realizar uma rotacao a direita
NoAVL* rotacaoDireita(NoAVL* y) 
{
    NoAVL* x = y->esq;
    NoAVL* T2 = x->dir;

    //realiza a rotacao
    x->dir = y;
    y->esq = T2;

    //atualiza alturas
    y->altura = 1 + maximo(alturaAVL(y->esq), alturaAVL(y->dir));
    x->altura = 1 + maximo(alturaAVL(x->esq), alturaAVL(x->dir));

    //retorna a nova raiz
    return x;
}

//funcao para realizar uma rotacao a esquerda
NoAVL* rotacaoEsquerda(NoAVL* x) 
{
    NoAVL* y = x->dir;
    NoAVL* T2 = y->esq;

    //realiza a rotacao
    y->esq = x;
    x->dir = T2;

    //atualiza alturas
    x->altura = 1 + maximo(alturaAVL(x->esq), alturaAVL(x->dir));
    y->altura = 1 + maximo(alturaAVL(y->esq), alturaAVL(y->dir));

    //retorna a nova raiz
    return y;
}

//funcao para criar uma Heap
Heap* criarHeap(int capacidade) 
{
    Heap* heap = (Heap*)malloc(sizeof(Heap));
    heap->reservas = (NoHeap*)malloc(capacidade * sizeof(NoHeap));
    heap->tamanho = 0;
    heap->capacidade = capacidade;
    return heap;
}

//funcao para inserir uma reserva na Heap
void inserirReserva(Heap* heap, int codigoSala, int prioridade) 
{
    if (heap->tamanho == heap->capacidade) {
        printf("Heap cheia! Não é possível inserir mais reservas.\n");
        return;
    }

    //adiciona a nova reserva no final da Heap
    heap->reservas[heap->tamanho].codigoSala = codigoSala;
    heap->reservas[heap->tamanho].prioridade = prioridade;
    heap->tamanho++;

    //ajusta a Heap para manter a propriedade de Heap de maximo
    int i = heap->tamanho - 1;
    while (i > 0 && heap->reservas[(i - 1) / 2].prioridade < heap->reservas[i].prioridade) 
    {
        //troca com o pai
        NoHeap temp = heap->reservas[i];
        heap->reservas[i] = heap->reservas[(i - 1) / 2];
        heap->reservas[(i - 1) / 2] = temp;

        //move para o pai
        i = (i - 1) / 2;
    }
}

//funcao para remover a reserva de maior prioridade
NoHeap removerMax(Heap* heap) 
{
    if (heap->tamanho == 0) 
    {
        printf("Heap vazia! Não há reservas para remover.\n");
        NoHeap vazio = {-1, -1}; //retorna uma reserva invalida
        return vazio;
    }

    //a reserva de maior prioridade esta no topo
    NoHeap maximo = heap->reservas[0];

    //move a última reserva para o topo
    heap->reservas[0] = heap->reservas[heap->tamanho - 1];
    heap->tamanho--;

    //ajusta a Heap para manter a propriedade de Heap de Maximo
    int i = 0;
    while (1) 
    {
        int maior = i;
        int esq = 2 * i + 1;
        int dir = 2 * i + 2;

        if (esq < heap->tamanho && heap->reservas[esq].prioridade > heap->reservas[maior].prioridade)
            maior = esq;

        if (dir < heap->tamanho && heap->reservas[dir].prioridade > heap->reservas[maior].prioridade)
            maior = dir;

        if (maior == i)
            break;

        //troca com o filho de maior prioridade
        NoHeap temp = heap->reservas[i];
        heap->reservas[i] = heap->reservas[maior];
        heap->reservas[maior] = temp;

        i = maior;
    }

    return maximo;
}

//funcao para listar todas as reservas
void listarReservas(Heap* heap) 
{
    printf("Reservas (codigo_sala, prioridade):\n");
    for (int i = 0; i < heap->tamanho; i++) 
    {
        printf("(%d, %d)\n", heap->reservas[i].codigoSala, heap->reservas[i].prioridade);
    }
}

//funcao para criar uma Tabela Hash
TabelaHash* criarTabelaHash(int tamanho) 
{
    TabelaHash* th = (TabelaHash*)malloc(sizeof(TabelaHash));
    th->tamanho = tamanho;
    th->tabela = (NoHash**)malloc(tamanho * sizeof(NoHash*));
    for (int i = 0; i < tamanho; i++)
        th->tabela[i] = NULL; //inicializa todas as posicoes como NULL
    return th;
}

//funcao de hash simples para evitar colisões
int hash(int codigoSala, int tamanho) 
{
    return codigoSala % tamanho;
}

//funcao para inserir ou atualizar o estado de uma sala
void inserirEstado(TabelaHash* th, int codigoSala, const char* estado) 
{
    if (strlen(estado) >= 20) {
        printf("Erro: Estado muito longo para a sala %d.\n", codigoSala);
        return;
    }
    int indice = hash(codigoSala, th->tamanho);

    //verifica se a sala ja existe na Tabela Hash
    NoHash* atual = th->tabela[indice];
    while (atual != NULL) 
    {
        if (atual->codigoSala == codigoSala) 
        {
            // Atualiza o estado da sala
            strcpy(atual->estado, estado);
            return;
        }
        atual = atual->proximo;
    }

    //se a sala nao existe, cria um novo no
    NoHash* novoNo = (NoHash*)malloc(sizeof(NoHash));
    novoNo->codigoSala = codigoSala;
    strcpy(novoNo->estado, estado);
    novoNo->proximo = th->tabela[indice];
    th->tabela[indice] = novoNo;
}

//funcao para consultar o estado de uma sala
char* consultarEstado(TabelaHash* th, int codigoSala) 
{
    int indice = hash(codigoSala, th->tamanho);

    //procura a sala na Tabela Hash
    NoHash* atual = th->tabela[indice];
    while (atual != NULL) 
    {
        if (atual->codigoSala == codigoSala)
            return atual->estado;
        atual = atual->proximo;
    }

    return "Nao_Encontrado"; //geralmente a funcao nao chega ate aqui, mas aq vai retornar um estado padrao em char caso a sala nao seja encontrada
}

//funcao para ler o arquivo de entrada e fazer as operacoess
//passo o stdout em algumas funcoes aqui para imprimir as informacoes no terminal e/ou no arquivo
//funcao sscanf verifica se as operacoes foram lidas corretamente, cada parametro do sscanf lido corretamente soma +1 no retorno da funcao, ou seja se ele ler 2 variaveis corretamente, o retorno da funcao e 2.
void lerArquivoEntrada(const char* nomeArquivo, NoAVL** raiz, Heap* heap, TabelaHash* th) 
{
    FILE* arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) 
    {
        printf("Erro ao abrir o arquivo de entrada!\n");
        return;
    }

    char linha[100]; //buffer para ler cada linha do arquivo
    while (fgets(linha, sizeof(linha), arquivo) != NULL) 
    {
        char operacao[50];
        if (sscanf(linha, "%s", operacao) != 1) 
        {
            printf("\nErro ao ler operação.\n");
            continue;
        }

        if (strcmp(operacao, "CRIAR_SALA") == 0) 
        {
            int codigo, capacidade;
            if (sscanf(linha, "%*s %d %d", &codigo, &capacidade) != 2) 
            {
                printf("\nErro ao ler dados da operação CRIAR_SALA.\n");
                continue;
            }
            *raiz = inserirSala(*raiz, codigo, capacidade);
            inserirEstado(th, codigo, "disponivel");
            printf("\nSala %d criada com capacidade %d.\n", codigo, capacidade);
        }
        else if (strcmp(operacao, "RESERVAR_SALA") == 0) 
        {
            int codigo, prioridade;
            if (sscanf(linha, "%*s %d %d", &codigo, &prioridade) != 2) 
            {
                printf("\nErro ao ler dados da operação RESERVAR_SALA.\n");
                continue;
            }
            inserirReserva(heap, codigo, prioridade);
            printf("\nReserva para a sala %d com prioridade %d adicionada.\n", codigo, prioridade);
        }
        else if (strcmp(operacao, "BLOQUEAR_SALA") == 0) 
        {
            int codigo;
            if (sscanf(linha, "%*s %d", &codigo) != 1) 
            {
                printf("\nErro ao ler código da operação BLOQUEAR_SALA.\n");
                continue;
            }
            inserirEstado(th, codigo, "bloqueada");
            printf("\nSala %d bloqueada.\n", codigo);
        }
        else if (strcmp(operacao, "DESBLOQUEAR_SALA") == 0) 
        {
            int codigo;
            if (sscanf(linha, "%*s %d", &codigo) != 1) 
            {
                printf("\nErro ao ler código da operação DESBLOQUEAR_SALA.\n");
                continue;
            }
            inserirEstado(th, codigo, "disponivel");
            printf("\nSala %d desbloqueada.\n", codigo);
        }
        else if (strcmp(operacao, "CANCELAR_RESERVA") == 0) 
        {
            int codigo;
            if (sscanf(linha, "%*s %d", &codigo) != 1) 
            {
                printf("\nErro ao ler código da operação CANCELAR_RESERVA.\n");
                continue;
            }
            cancelarReserva(heap, codigo);
        }
        else if (strcmp(operacao, "CONSULTAR_SALA") == 0) 
        {
            int codigo;
            if (sscanf(linha, "%*s %d", &codigo) != 1) 
            {
                printf("\nErro ao ler código da operação CONSULTAR_SALA.\n");
                continue;
            }
            char* estado = consultarEstado(th, codigo);
            printf("\nEstado da sala %d: %s\n", codigo, estado);
        }
        else if (strcmp(operacao, "LISTAR_SALAS") == 0) 
        {
            printf("\nListando todas as salas:\n");
            listarSalasInOrder(*raiz, stdout);
        }
        else if (strcmp(operacao, "ATUALIZAR_PRIORIDADE") == 0) 
        {
            int codigo, prioridade;
            if (sscanf(linha, "%*s %d %d", &codigo, &prioridade) != 2) 
            {
                printf("\nErro ao ler dados da operação ATUALIZAR_PRIORIDADE.\n");
                continue;
            }
            atualizarPrioridade(heap, codigo, prioridade);
        }
        else if (strcmp(operacao, "LISTAR_RESERVAS_PRIORIDADE") == 0) 
        {
            printf("\nListando reservas por prioridade:\n");
            listarReservasPorPrioridade(heap, stdout);
        }
        else if (strcmp(operacao, "LISTAR_SALAS_ESTADO") == 0) 
        {
            printf("\nListando salas por estado:\n");
            listarSalasPorEstado(th, stdout);
        }
        else if (strcmp(operacao, "LISTAR_SALAS_CAPACIDADE") == 0) 
        {
            int ordem;
            if (sscanf(linha, "%*s %d", &ordem) != 1) 
            {
                printf("\nErro ao ler ordem da operação LISTAR_SALAS_CAPACIDADE.\n");
                continue;
            }
            printf("\nListando salas por capacidade (%s):\n", ordem ? "crescente" : "decrescente");
            listarSalasPorCapacidade(*raiz, stdout, ordem);
        }
        else {
            printf("\nOperação desconhecida: %s\n", operacao);
        }
    }

    fclose(arquivo);
}

//funcao para cancelar uma reserva na Heap
void cancelarReserva(Heap* heap, int codigoSala) 
{
    // Procura a reserva na Heap
    int indice = -1;
    for (int i = 0; i < heap->tamanho; i++) 
    {
        if (heap->reservas[i].codigoSala == codigoSala) 
        {
            indice = i;
            break;
        }
    }

    if (indice == -1) 
    {
        printf("Reserva para a sala %d não encontrada.\n", codigoSala);
        return;
    }

    //move a ultima reserva para a posicao da reserva cancelada
    heap->reservas[indice] = heap->reservas[heap->tamanho - 1];
    heap->tamanho--;

    //reorganiza a Heap para manter a propriedade de Heap de Maximo
    int i = indice;
    while (1) 
    {
        int maior = i;
        int esq = 2 * i + 1;
        int dir = 2 * i + 2;

        if (esq < heap->tamanho && heap->reservas[esq].prioridade > heap->reservas[maior].prioridade)
            maior = esq;

        if (dir < heap->tamanho && heap->reservas[dir].prioridade > heap->reservas[maior].prioridade)
            maior = dir;

        if (maior == i)
            break;

        //troca com o filho de maior prioridade
        NoHeap temp = heap->reservas[i];
        heap->reservas[i] = heap->reservas[maior];
        heap->reservas[maior] = temp;

        i = maior;
    }

    printf("\nReserva para a sala %d cancelada.\n", codigoSala);
}

//funcao para listar todas as salas em ordem crescente (percurso in-order)
void listarSalasInOrder(NoAVL* raiz, FILE* arquivo) 
{
    if (raiz != NULL) 
    {
        listarSalasInOrder(raiz->esq, arquivo);
        fprintf(arquivo, "Sala %d, Capacidade %d\n", raiz->codigo, raiz->capacidade);
        listarSalasInOrder(raiz->dir, arquivo);
    }
}

//funcao semelhante a heapify tradicional mostrada em aula, so muda que ela faz heapify up e heapify down, subindo ou descendo na heap dependendo da nova prioridade :)
void atualizarPrioridade(Heap* heap, int codigoSala, int novaPrioridade) 
{
    //procura a reserva na Heap
    int indice = -1;
    for (int i = 0; i < heap->tamanho; i++) 
    {
        if (heap->reservas[i].codigoSala == codigoSala) 
        {
            indice = i;
            break;
        }
    }

    if (indice == -1) 
    {
        printf("Reserva para a sala %d não encontrada.\n", codigoSala);
        return;
    }

    // atualiza a prioridade
    int prioridadeAntiga = heap->reservas[indice].prioridade;
    heap->reservas[indice].prioridade = novaPrioridade;

    //reorganiza a Heap
    if (novaPrioridade > prioridadeAntiga) 
    {
        //se a nova prioridade e maior, sobe na Heap
        int i = indice;
        while (i > 0 && heap->reservas[(i - 1) / 2].prioridade < heap->reservas[i].prioridade) 
        {
            //troca com o pai
            NoHeap temp = heap->reservas[i];
            heap->reservas[i] = heap->reservas[(i - 1) / 2];
            heap->reservas[(i - 1) / 2] = temp;

            //move para o pai
            i = (i - 1) / 2;
        }
    } else if (novaPrioridade < prioridadeAntiga) 
    {
        //se a nova prioridade e menor, desce na Heap
        int i = indice;
        while (1) 
        {
            int maior = i;
            int esq = 2 * i + 1;
            int dir = 2 * i + 2;

            if (esq < heap->tamanho && heap->reservas[esq].prioridade > heap->reservas[maior].prioridade)
                maior = esq;

            if (dir < heap->tamanho && heap->reservas[dir].prioridade > heap->reservas[maior].prioridade)
                maior = dir;

            if (maior == i)
                break;

            // Troca com o filho de maior prioridade
            NoHeap temp = heap->reservas[i];
            heap->reservas[i] = heap->reservas[maior];
            heap->reservas[maior] = temp;

            i = maior;
        }
    }

    printf("\nPrioridade da reserva para a sala %d atualizada para %d.\n", codigoSala, novaPrioridade);
}

//funcao para listar as reservas por prioridade
void listarReservasPorPrioridade(Heap* heap, FILE* arquivo) 
{
    if (heap->tamanho == 0) 
    {
        fprintf(arquivo, "Nenhuma reserva encontrada.\n");
        return;
    }

    fprintf(arquivo, "\nReservas por prioridade:\n");
    for (int i = 0; i < heap->tamanho; i++) 
    {
        fprintf(arquivo, "Sala %d, Prioridade %d\n", heap->reservas[i].codigoSala, heap->reservas[i].prioridade);
    }
}

//funcao para listar salas por estado
void listarSalasPorEstado(TabelaHash* th, FILE* arquivo) 
{
    int capacidade = 100; // Tamanho inicial dos arrays
    int* salasDisponivel = (int*)malloc(capacidade * sizeof(int));
    int* salasBloqueada = (int*)malloc(capacidade * sizeof(int));
    int contadorDisponivel = 0, contadorBloqueada = 0;

    //percorre a Tabela Hash e agrupa as salas por estado
    for (int i = 0; i < th->tamanho; i++) 
    {
        NoHash* atual = th->tabela[i];
        while (atual != NULL) 
        {
            if (strcmp(atual->estado, "disponivel") == 0) 
            {
                if (contadorDisponivel == capacidade) 
                {
                    capacidade *= 2; //dobra a capacidade
                    salasDisponivel = (int*)realloc(salasDisponivel, capacidade * sizeof(int));
                    salasBloqueada = (int*)realloc(salasBloqueada, capacidade * sizeof(int));
                }
                salasDisponivel[contadorDisponivel++] = atual->codigoSala;
            } else if (strcmp(atual->estado, "bloqueada") == 0) {
                if (contadorBloqueada == capacidade) 
                {
                    capacidade *= 2; // Dobra a capacidade
                    salasDisponivel = (int*)realloc(salasDisponivel, capacidade * sizeof(int));
                    salasBloqueada = (int*)realloc(salasBloqueada, capacidade * sizeof(int));
                }
                salasBloqueada[contadorBloqueada++] = atual->codigoSala;
            }
            atual = atual->proximo;
        }
    }

    //exibe as salas disponíveis
    fprintf(arquivo, "Disponíveis:\n");
    for (int i = 0; i < contadorDisponivel; i++) 
    {
        fprintf(arquivo, "Sala %d\n", salasDisponivel[i]);
    }

    //exibe as salas bloqueadas
    fprintf(arquivo, "Bloqueadas:\n");
    for (int i = 0; i < contadorBloqueada; i++) 
    {
        fprintf(arquivo, "Sala %d\n", salasBloqueada[i]);
    }

    //libera a memória alocada
    free(salasDisponivel);
    free(salasBloqueada);
}

//funcao para armazenar salas em um array durante o percurso in-order
void armazenarSalasInOrder(NoAVL* raiz, SalaCapacidade* salas, int* indice) 
{
    if (raiz != NULL) {
        armazenarSalasInOrder(raiz->esq, salas, indice);
        salas[*indice].codigo = raiz->codigo;
        salas[*indice].capacidade = raiz->capacidade;
        (*indice)++;
        armazenarSalasInOrder(raiz->dir, salas, indice);
    }
}

//funcao de comparacao para ordenar salas por capacidade (crescente)
int compararCapacidadeCrescente(const void* a, const void* b) 
{
    SalaCapacidade* salaA = (SalaCapacidade*)a;
    SalaCapacidade* salaB = (SalaCapacidade*)b;
    return salaA->capacidade - salaB->capacidade;
}

//funcao para listar salas por capacidade (crescente ou decrescente)
void listarSalasPorCapacidade(NoAVL* raiz, FILE* arquivo, int ordem) 
{
    if (raiz == NULL) 
    {
        fprintf(arquivo, "Nenhuma sala cadastrada.\n");
        return;
    }

    //armazena todas as salas em um array
    SalaCapacidade salas[100]; //estabelecendo um limite de 100 salas
    int indice = 0;
    armazenarSalasInOrder(raiz, salas, &indice);

    //ordena o array por capacidade
    qsort(salas, indice, sizeof(SalaCapacidade), compararCapacidadeCrescente);

    //exibe as salas em ordem crescente ou decrescente
    fprintf(arquivo, "\nSalas por capacidade (%s):\n", ordem == 1 ? "crescente" : "decrescente");
    if (ordem == 1) 
    {
        for (int i = 0; i < indice; i++) 
        {
            fprintf(arquivo, "Sala %d, Capacidade %d\n", salas[i].codigo, salas[i].capacidade);
        }
    } else 
    {
        for (int i = indice - 1; i >= 0; i--) 
        {
            fprintf(arquivo, "Sala %d, Capacidade %d\n", salas[i].codigo, salas[i].capacidade);
        }
    }
}

//funcao para desalocar a arvore AVL
void desalocarAVL(NoAVL* raiz) 
{
    if (raiz != NULL) 
    {
        desalocarAVL(raiz->esq); // Desaloca a subarvore esquerda
        desalocarAVL(raiz->dir); // Desaloca a subarvore direita
        free(raiz); //libera o no atual
    }
}

//funcao para desalocar a Heap
void desalocarHeap(Heap* heap) 
{
    if (heap != NULL) 
    {
        free(heap->reservas); // Libera o array de reservas
        free(heap); // Libera a estrutura da Heap
    }
}

//funcao para desalocar a Tabela Hash
void desalocarTabelaHash(TabelaHash* th) 
{
    if (th != NULL) 
    {
        for (int i = 0; i < th->tamanho; i++) 
        {
            NoHash* atual = th->tabela[i];
            while (atual != NULL) 
            {
                NoHash* proximo = atual->proximo;
                free(atual); //libera o no atual
                atual = proximo;
            }
        }
        free(th->tabela); //libera o array da tabela
        free(th); //libera a estrutura da Tabela Hash
    }
}

//funcao para retornar o maior valor entre dois numeros utilizando 
int maximo(int a, int b) 
{
    if(a>b)
        return a;
    else
        return b;
}