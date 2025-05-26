#ifndef PALAVRAMANAGER_H
#define PALAVRAMANAGER_H

#include <wchar.h>

#define MAX_WORD_LEN 100

typedef struct PalavraManager PalavraManager;

PalavraManager* CriarLista();

void DestruirLista(PalavraManager* pm);

void AdicionarNaLista(PalavraManager* pm, const wchar_t* palavra);

int ListaTamanho(PalavraManager* pm);

const wchar_t* ListaGet(PalavraManager* pm, int index);

void ListaSort(PalavraManager* pm);

int ListaBuscaLinear(PalavraManager* pm, const wchar_t* palavra);

int ListaBuscaBinaria(PalavraManager* pm, const wchar_t* palavra);

void LimparLista(PalavraManager* pm);
#endif