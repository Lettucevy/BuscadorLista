#ifndef PALAVRAMANAGER_H
#define PALAVRAMANAGER_H

#include <wchar.h>
#include <windows.h>

#define MAX_WORD_LEN 100
#define MAX_LIST_NAME_LEN 50
#define MAX_PATH_LEN MAX_PATH
#define MAX_OPEN_LISTS 10

typedef struct PalavraManager PalavraManager;

typedef struct OpenList {
    wchar_t name[MAX_LIST_NAME_LEN];
    wchar_t filePath[MAX_PATH_LEN];
    PalavraManager* pm;
} OpenList;

typedef struct ListaManager {
    OpenList lists[MAX_OPEN_LISTS];
    int count;
    int currentListIndex;
} ListaManager;


PalavraManager* CriarLista();
void DestruirLista(PalavraManager* pm);
void AdicionarNaLista(PalavraManager* pm, const wchar_t* palavra);
int ListaTamanho(PalavraManager* pm);
const wchar_t* ListaGet(PalavraManager* pm, int index);
void ListaSort(PalavraManager* pm);
int ListaBuscaLinear(PalavraManager* pm, const wchar_t* palavra);
int ListaBuscaBinaria(PalavraManager* pm, const wchar_t* palavra);
void LimparLista(PalavraManager* pm);
void RemoverPalavra(PalavraManager* pm, int index);

// Funções para ListaManager (gerenciador de listas abertas)
ListaManager* CriarListaManager();
void DestruirListaManager(ListaManager* lm);
int AdicionarListaAberta(ListaManager* lm, const wchar_t* name, const wchar_t* filePath, PalavraManager* pm);
void RemoverListaAberta(ListaManager* lm, int index);

// Funções de E/S de arquivo para UMA lista (PalavraManager)
int CarregarListaDeArquivo(PalavraManager** pm, const wchar_t* filePath);
int SalvarListaEmArquivo(PalavraManager* pm, const wchar_t* filePath);
void ExportarListaParaTXT(PalavraManager* pm, HWND hwnd); 

#endif