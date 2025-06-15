#include "PalavraManager.h"
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <stdio.h>
#include <commdlg.h> 

// Estrutura de dados PalavraManager (lista de palavras)
struct PalavraManager {
	wchar_t** palavras; // Array de palavras
    int count; // Quantidade de palavras na lista
    int capacity; // Capacidade
    int ordenado; // 0 se não ordenado, 1 se ordenado
};

// Funções para criar a lista
PalavraManager* CriarLista() {
    PalavraManager* pm = (PalavraManager*)malloc(sizeof(PalavraManager)); // criação do malloc para lista
	if (!pm) return NULL; // retorna se não conseguiu alocar memória
    pm->count = 0; // iniciar a Quantidade pra 0
	pm->capacity = 10; // capacidade inicial de 10 palavras
	pm->ordenado = 0; // não ordenado inicialmente
	pm->palavras = (wchar_t**)malloc(sizeof(wchar_t*) * pm->capacity); // aloca memória para as palavras passando a capacidade da lista
    if (!pm->palavras) {
        free(pm);
        return NULL;
    }
    return pm;
}

// Explode a lista booooooommm aosfsksdf
void DestruirLista(PalavraManager* pm) {
    if (!pm) return;
    for (int i = 0; i < pm->count; i++) {
        free(pm->palavras[i]);
    }
    free(pm->palavras);
    free(pm);
}

// Preciso comentar essa funções ou ta dificil?
void AdicionarNaLista(PalavraManager* pm, const wchar_t* palavra) {
	if (pm->count >= pm->capacity) { // verifica se a lista está cheia
		pm->capacity *= 2; // dobra a capacidade da lista se estiver cheia
		pm->palavras = (wchar_t**)realloc(pm->palavras, sizeof(wchar_t*) * pm->capacity); // realoca a memória para aumentar a capacidade
        if (!pm->palavras) return;
    }

	pm->palavras[pm->count] = (wchar_t*)malloc(sizeof(wchar_t) * (wcslen(palavra) + 1)); // aloca memória para a nova palavra
    if (!pm->palavras[pm->count]) return;
    wcscpy_s(pm->palavras[pm->count], wcslen(palavra) + 1, palavra);
	pm->count++; // incrementa a contagem de palavras
	pm->ordenado = 0; // marca como não ordenado
}

int ListaTamanho(PalavraManager* pm) {
    return pm->count;
}

// Retorna a palavra na posição index da lista, ou NULL se o índice for inválido
const wchar_t* ListaGet(PalavraManager* pm, int index) {
    if (index < 0 || index >= pm->count) return NULL;
	return pm->palavras[index]; // retorna a palavra na posição index
}

// Função de comparação para qsort
int Compare(const void* a, const void* b) {
    const wchar_t* pa = *(const wchar_t**)a;
    const wchar_t* pb = *(const wchar_t**)b;
    return wcscmp(pa, pb); 
}

// Ordena a lista de palavras usando qsort
void ListaSort(PalavraManager* pm) {
    qsort(pm->palavras, pm->count, sizeof(wchar_t*), Compare);
    pm->ordenado = 1;
}

// Busca linear na lista de palavras
int ListaBuscaLinear(PalavraManager* pm, const wchar_t* palavra) {
    for (int i = 0; i < pm->count; i++) {
        if (wcscmp(pm->palavras[i], palavra) == 0) {
            return i; 
        }
    }
    return -1;
}

// Busca binária na lista de palavras (assumindo que a lista está ordenada)
int ListaBuscaBinaria(PalavraManager* pm, const wchar_t* palavra) {
    if (!pm->ordenado) return -2;

    int left = 0;
    int right = pm->count - 1;

    while (left <= right) {
        int mid = (left + right) / 2;
		int cmp = wcscmp(palavra, pm->palavras[mid]); // compara a palavra com a do meio

        if (cmp == 0) return mid;
        if (cmp < 0) right = mid - 1;
        else left = mid + 1;
    }

    return -1;
}

// Limpa a lista de palavras, liberando memória e resetando o contador
void LimparLista(PalavraManager* pm) {
    for (int i = 0; i < pm->count; i++) {
        free(pm->palavras[i]);
    }
    pm->count = 0;
    pm->ordenado = 0;
}

// Remove uma palavra da lista pelo índice, liberando a memória alocada
void RemoverPalavra(PalavraManager* pm, int index) {
    if (index < 0 || index >= pm->count) return;

    free(pm->palavras[index]);

	for (int i = index; i < pm->count - 1; i++) { // move as palavras para preencher o espaço vazio
		pm->palavras[i] = pm->palavras[i + 1]; // copia a palavra seguinte para a posição atual
    }
	pm->count--; // decrementa a quantidade de palavras
	pm->ordenado = 0; // marca como não ordenado após remoção
}

// Funções para ListaManager (gerenciador de listas abertas) cria o gerenciador de listas abertas
ListaManager* CriarListaManager() {
	ListaManager* lm = (ListaManager*)malloc(sizeof(ListaManager)); // Aloca memoria para o gerenciador de listas abertas
	if (!lm) return NULL;   // verifica se a alocação foi bem-sucedida
	lm->count = 0; // Inicializa a contagem de listas abertas
	lm->currentListIndex = -1; // Nenhuma lista aberta inicialmente
    return lm;
}

// Destrói o gerenciador de listas abertas, liberando memória alocada para cada PalavraManager
void DestruirListaManager(ListaManager* lm) {
    if (!lm) return;
    for (int i = 0; i < lm->count; i++) {
        DestruirLista(lm->lists[i].pm);
    }
    free(lm);
}

// Adiciona uma lista que já foi criada/carregada ao gerenciador de listas abertas
int AdicionarListaAberta(ListaManager* lm, const wchar_t* name, const wchar_t* filePath, PalavraManager* pm) {
    if (lm->count >= MAX_OPEN_LISTS) return -1;

    wcscpy_s(lm->lists[lm->count].name, MAX_LIST_NAME_LEN, name);
    wcscpy_s(lm->lists[lm->count].filePath, MAX_PATH_LEN, filePath);
    lm->lists[lm->count].pm = pm;
    lm->count++;
    return lm->count - 1;
}

// Remove uma lista do gerenciador de listas abertas (não destrói o PalavraManager, apenas o remove do array)
void RemoverListaAberta(ListaManager* lm, int index) {
    if (index < 0 || index >= lm->count) return;

    DestruirLista(lm->lists[index].pm);

    for (int i = index; i < lm->count - 1; ++i) {
        lm->lists[i] = lm->lists[i + 1];
    }
    lm->count--;
    if (lm->currentListIndex == index) {
        lm->currentListIndex = -1;
    }
    else if (lm->currentListIndex > index) {
        lm->currentListIndex--;
    }
}

// Funções de E/S de arquivo para UMA lista (PalavraManager)

// Carrega uma lista de um arquivo binário para um novo PalavraManager
int CarregarListaDeArquivo(PalavraManager** pm, const wchar_t* filePath) {
    FILE* file;
    _wfopen_s(&file, filePath, L"rb");
    if (!file) {
        *pm = NULL;
        return 0; 
    }

    PalavraManager* loaded_pm = CriarLista();
    if (!loaded_pm) {
        fclose(file);
        return 0;
    }

    fread(&(loaded_pm->count), sizeof(int), 1, file);
    fread(&(loaded_pm->capacity), sizeof(int), 1, file);
    fread(&(loaded_pm->ordenado), sizeof(int), 1, file);

    loaded_pm->palavras = (wchar_t**)realloc(loaded_pm->palavras, sizeof(wchar_t*) * loaded_pm->capacity);
    if (!loaded_pm->palavras) {
        DestruirLista(loaded_pm);
        fclose(file);
        return 0;
    }

    for (int j = 0; j < loaded_pm->count; j++) {
        int wordLen;
        fread(&wordLen, sizeof(int), 1, file);
        loaded_pm->palavras[j] = (wchar_t*)malloc(sizeof(wchar_t) * (wordLen + 1));
        if (!loaded_pm->palavras[j]) {
            DestruirLista(loaded_pm);
            fclose(file);
            return 0;
        }
        fread(loaded_pm->palavras[j], sizeof(wchar_t), wordLen, file);
        loaded_pm->palavras[j][wordLen] = L'\0';
    }

    fclose(file);
    *pm = loaded_pm;
    return 1;
}

// Salva uma lista em um arquivo binário
int SalvarListaEmArquivo(PalavraManager* pm, const wchar_t* filePath) {
    FILE* file;
    _wfopen_s(&file, filePath, L"wb");
    if (!file) return 0;

    fwrite(&(pm->count), sizeof(int), 1, file);
    fwrite(&(pm->capacity), sizeof(int), 1, file);
    fwrite(&(pm->ordenado), sizeof(int), 1, file);

    for (int i = 0; i < pm->count; i++) {
        int wordLen = wcslen(pm->palavras[i]);
        fwrite(&wordLen, sizeof(int), 1, file);
        fwrite(pm->palavras[i], sizeof(wchar_t), wordLen, file);
    }
    fclose(file);
    return 1;
}

// Exporta a lista atual para um arquivo TXT
void ExportarListaParaTXT(PalavraManager* pm, HWND hwnd) {
    OPENFILENAMEW ofn;
    wchar_t szFile[MAX_PATH] = L"";

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
    ofn.lpstrFilter = L"Arquivos de Texto (*.txt)\0*.txt\0Todos os Arquivos (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
    ofn.lpstrDefExt = L"txt";

    if (GetSaveFileNameW(&ofn) == TRUE) {
        FILE* file;
        _wfopen_s(&file, ofn.lpstrFile, L"wb");
        if (file) {
            unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
            fwrite(bom, sizeof(unsigned char), 3, file);

            for (int i = 0; i < pm->count; i++) {
                int utf8Len = WideCharToMultiByte(CP_UTF8, 0, pm->palavras[i], -1, NULL, 0, NULL, NULL);
                if (utf8Len > 0) {
                    char* utf8Word = (char*)malloc(utf8Len);
                    if (utf8Word) {
                        WideCharToMultiByte(CP_UTF8, 0, pm->palavras[i], -1, utf8Word, utf8Len, NULL, NULL);
                        fwrite(utf8Word, sizeof(char), utf8Len - 1, file);
                        fwrite("\r\n", sizeof(char), 2, file);
                        free(utf8Word);
                    }
                }
            }
            fclose(file);
            MessageBoxW(hwnd, L"Lista exportada com sucesso!", L"Exportar", MB_OK | MB_ICONINFORMATION);
        }
        else {
            MessageBoxW(hwnd, L"Erro ao abrir arquivo para exportação.", L"Erro", MB_OK | MB_ICONERROR);
        }
    }
}