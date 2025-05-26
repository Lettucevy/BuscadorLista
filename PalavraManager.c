#include "PalavraManager.h"
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

// Estrutura de dados lista
struct PalavraManager {
    wchar_t** palavras;
	int count; // Quantidade de palavras na lista
	int capacity; // Capacidade
	int ordenado; // 0 se não ordenado, 1 se ordenado
};

// Cria uma nova lista foda lek
PalavraManager* CriarLista() {
    PalavraManager* pm = (PalavraManager*)malloc(sizeof(PalavraManager));
    pm->count = 0;
    pm->capacity = 10;
    pm->ordenado = 0;
    pm->palavras = (wchar_t**)malloc(sizeof(wchar_t*) * pm->capacity);
    return pm;
}

// Explode a lista e libera a memória
void DestruirLista(PalavraManager* pm) {
    if (!pm) return;
    for (int i = 0; i < pm->count; i++) {
        free(pm->palavras[i]);
    }
    free(pm->palavras);
    free(pm);
}

// Adiciona uma palavra na lista se a lista estiver cheia dobra a capacidade coisa boa
void AdicionarNaLista(PalavraManager* pm, const wchar_t* palavra) {
    if (pm->count >= pm->capacity) {
        pm->capacity *= 2;
        pm->palavras = (wchar_t**)realloc(pm->palavras, sizeof(wchar_t*) * pm->capacity);
    }

    pm->palavras[pm->count] = (wchar_t*)malloc(sizeof(wchar_t) * (wcslen(palavra) + 1));
    wcscpy_s(pm->palavras[pm->count], wcslen(palavra) + 1, palavra);
    pm->count++;
    pm->ordenado = 0;
}

// Preciso comentar isso?
int ListaTamanho(PalavraManager* pm) {
    return pm->count;
}

// Retorna a palavra no índice Tipo um palavra = Lista[i];
const wchar_t* ListaGet(PalavraManager* pm, int index) {
    if (index < 0 || index >= pm->count) return NULL;
    return pm->palavras[index];
}

// Função de comparação para qsort 
int Compare(const void* a, const void* b) {
    const wchar_t* pa = *(const wchar_t**)a;
    const wchar_t* pb = *(const wchar_t**)b;
    return wcscmp(pa, pb);
}

// Ordena as palavras usando qsort
void ListaSort(PalavraManager* pm) {
    qsort(pm->palavras, pm->count, sizeof(wchar_t*), Compare);
    pm->ordenado = 1;
}

// Busca linear percorre a lista e compara palavra por palavra dependendo do tamanho da lista é lento pa caralho
int ListaBuscaLinear(PalavraManager* pm, const wchar_t* palavra) {
    for (int i = 0; i < pm->count; i++) {
        if (wcscmp(pm->palavras[i], palavra) == 0) {
            return i;
        }
    }
    return -1;
}

// Busca binária, assume que a lista está ordenada e divide no meio da lista para encontrar a palavra
int ListaBuscaBinaria(PalavraManager* pm, const wchar_t* palavra) {
    if (!pm->ordenado) return -2;

    int left = 0;
    int right = pm->count - 1;

    while (left <= right) {
        int mid = (left + right) / 2;
        int cmp = wcscmp(palavra, pm->palavras[mid]);

        if (cmp == 0) return mid;
        if (cmp < 0) right = mid - 1;
        else left = mid + 1;
    }

    return -1;
}

// Preciso?
void LimparLista(PalavraManager* pm) {
    for (int i = 0; i < pm->count; i++) {
        free(pm->palavras[i]);
    }
    pm->count = 0;
    pm->ordenado = 0;
}
