#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#define RESET_PAR for (j = 0; j < n; j++) queue.parents[j] = -1

typedef int edge; // uchovává hrany grafu

typedef struct {
    int indexAc;
    int indexFu;
    unsigned *stackAc;
    unsigned *stackFu;
    int *parents;
} buffer_t; // buffer se všemi proměnými potřebnými pro hledání v grafu

int makeSteps(edge **graph, buffer_t *queue, int max, int*, int);
int solveStep(edge **graph, buffer_t *queue, int index, int, int*, int);
void addEdge(edge *vertex, int to);
void initGraph(edge **graph, int d);
int solve(edge **graph, buffer_t *queue, int max, int*, int);
void bts(edge **graph, int n, int maxCount, int*, int);
int isIn(int centr, int* attacked, int from, int to);

void addEdge(edge *vertex, int to) {
    vertex[0]++;
    vertex[vertex[0]] = to;
}

void initGraph(edge **graph, int d) {
    int i;

    *graph = malloc((d + 1) * sizeof(edge));
    (*graph)[0] = 0;
    for (i = 1; i <= d ; i++) {
        (*graph)[i] = -1;
    }
}

int solve(edge **graph, buffer_t *queue, int max, int *attacked, int k) {
    int back = 1;
    while (back == 1) {
        back = makeSteps(graph, queue, max, attacked, k); // volá hledání pro 1 sadu vrcholů
    }
    return back;
}

void bts(edge **graph, int n, int maxCount, int *attacked, int k) { // provádí hledání v grafu
    int i, j;
    int back; // návratová hodnota solveStep
    buffer_t queue = {0, -1, NULL, NULL, NULL}; // indexFu je -1, před 1. použitím se zvětší

    queue.stackAc = malloc(n * sizeof(unsigned));
    queue.stackFu = malloc(n * sizeof(unsigned));
    queue.parents = malloc(n * sizeof(int));
    if (queue.stackAc == NULL || queue.stackFu == NULL) {
        puts("Málo paměti");
        exit(1);
    }

    for (i = 1;; i++) {
        RESET_PAR;              // inicializuje queue před hledáním
        queue.stackAc[0] = 0;
        queue.indexAc = 0;
        queue.indexFu = -1;
        queue.parents[0] = 0;
        back = solve(graph, &queue, i, attacked, k); // hledá dokud nedosáhne konce, nebo jí nedojdou volné vrcholy
        if (back == 0) { // jsi u cíle, vypiš cestu
            printf("%d\n", i-1); // vypiš maximální počet hlídek
            break;
        }
        if (i == maxCount && back == 2) { // nic jsi nenašel, vypiš nesmysly
            printf("-1\n");
            break;
        }
    }
    free(queue.stackAc);
    free(queue.stackFu);
    free(queue.parents);
}

int makeSteps(edge **graph, buffer_t *queue, int max, int* attacked, int k) {
    unsigned int i, *swapS, test = 0;
    int back; // návratová hodnota solveStep

    for (i = 0; i <= queue->indexAc; i++) {
        back = solveStep(graph, queue, i, max, attacked, k); // hledá všechny možné cesty pro 1 vrchol grafu
        if (back == 0) { // jsi u cíle, dej vědět
            return 0;
        }
        if (back != 2) {
            test = 1;
        }
    }
    if (test == 1) { // když se nějaké cesty našly prohodí body, takže příště se bude hledat to co se našlo
        swapS = queue->stackAc;
        queue->stackAc = queue->stackFu;
        queue->stackFu = swapS;
        queue->indexAc = queue->indexFu;
        queue->indexFu = -1;
        return 1;
    }
    return 2; // když ne, dá vědět
}

int isIn(int centr, int* attacked, int from, int to) {
    int hub = (from + to)/2;
    if (attacked[hub] == centr) {
        return 0;
    }
    else if (from != hub) {
        if (attacked[hub] > centr) {
            return isIn(centr, attacked, from, hub);
        }
        else {
            return isIn(centr, attacked, hub, to);
        }
    }
    else {
        return 1;
    }
}

int solveStep(edge **graph, buffer_t *queue, int index, int max, int *attacked, int k) {
    int i, test = 0;
    for (i = 1; i <= max; i++) { // projde všechny hrany z daného vrcholu
        if (i > graph[queue->stackAc[index]][0]) continue;
        if (!isIn(graph[queue->stackAc[index]][i], attacked, 0, k)) { // jsi u cíle, dej vědět
            return 0;
        }
        if (queue->parents[graph[queue->stackAc[index]][i]] == -1) { // Nebyl jsem tu?
            test = 1;
            queue->indexFu++; // zvyš index příštích bodů o 1
            queue->stackFu[queue->indexFu] = graph[queue->stackAc[index]][i]; // ulož tam aktuální bod
            queue->parents[graph[queue->stackAc[index]][i]] = queue->stackAc[queue->indexAc]; // ulož odkud jsi vyšel, nutné pro rekonstrukci cesty
        }
    }
    return test?1:2; // ne dej vědět zda jsi něco našel(1), nebo ne(2)
}

int cmp(const void *a, const void *b) {
    int* x = (int*)a;
    int* y = (int*)b;
    return *x - *y;
}

int main() {
    int t, n, k, d, to;
    int maxCount = 0;
    edge **graph;
    int i, j;

    scanf("%d",&t);
    for (; t > 0; t--) {
        scanf("%d%d", &n, &k);

        int attacked[k];
        graph = malloc(n * sizeof(edge*));

        for (i = 0; i < k; i++) {
            scanf("%d", &attacked[i]);
        }
        for (i = 0; i < n; i++) { // načte graf
            scanf("%d", &d);
            initGraph(&graph[i], d);
            for (j = 1; j <= d; j++) {
                scanf("%d", &to);
                addEdge(graph[i], to);
                if (j > maxCount) {
                    maxCount = j;
                }
            }
        }
        qsort(attacked, k, sizeof(int), cmp);
        bts(graph, n, maxCount, attacked, k);

        for (n--; n >= 0; n--) {
            free(graph[n]);
        }
        free(graph);
    }
    return 0;
}
