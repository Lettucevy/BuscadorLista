#pragma execution_character_set("utf-8")
#include <windows.h>
#include <wchar.h> // Por favor, não mexer nesta linha ou na linha 1 a não ser que queiram o programa todo em chinês
#include "PalavraManager.h" // Aquivos PalavraManager.h e Resouce.h são cabeçalhos como requesitado pelo professor
#include "Resource.h"

PalavraManager* pm; // Instancia do gerenciador de palavras

// Atualiza o ListBox com as palavras do gerenciador
void AtualizarListBox(HWND hList) {
    SendMessageW(hList, LB_RESETCONTENT, 0, 0);
    for (int i = 0; i < ListaTamanho(pm); i++) {
        SendMessageW(hList, LB_ADDSTRING, 0, (LPARAM)ListaGet(pm, i));
    }
}

// Implementação do win32 API para a janela principal só sei que funfa
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND hEdit, hList;

    switch (uMsg) {
    case WM_CREATE:
		//Criar lista de palavras
        pm = CriarLista();

		// Botões e controles
        hEdit = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER,
            20, 20, 200, 25, hwnd, (HMENU)ID_EDIT, NULL, NULL);

        hList = CreateWindowW(L"LISTBOX", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY,
            20, 60, 300, 200, hwnd, (HMENU)ID_LISTBOX, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Adicionar", WS_CHILD | WS_VISIBLE,
            230, 20, 90, 25, hwnd, (HMENU)ID_ADD, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Ordenar", WS_CHILD | WS_VISIBLE,
            330, 20, 90, 25, hwnd, (HMENU)ID_SORT, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Busca Linear", WS_CHILD | WS_VISIBLE,
            330, 60, 90, 25, hwnd, (HMENU)ID_SEARCH_LINEAR, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Busca Binária", WS_CHILD | WS_VISIBLE,
            330, 100, 90, 25, hwnd, (HMENU)ID_SEARCH_BINARY, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Limpar", WS_CHILD | WS_VISIBLE,
            330, 140, 90, 25, hwnd, (HMENU)ID_CLEAR, NULL, NULL);
        break;

    case WM_COMMAND: {
        wchar_t buffer[MAX_WORD_LEN];
        GetWindowTextW(hEdit, buffer, MAX_WORD_LEN);

        switch (LOWORD(wParam)) {
        case ID_ADD:
            if (wcslen(buffer) > 0) {
                AdicionarNaLista(pm, buffer);
                AtualizarListBox(hList);
                SetWindowTextW(hEdit, L"");
            }
            break;

        case ID_SORT:
            ListaSort(pm);
            AtualizarListBox(hList);
            break;

        case ID_SEARCH_LINEAR: {
            int idx = ListaBuscaLinear(pm, buffer);
            if (idx >= 0)
                SendMessageW(hList, LB_SETCURSEL, idx, 0);
            else
                MessageBoxW(hwnd, L"Palavra não encontrada (Linear)", L"Busca", MB_OK);
            break;
        }

        case ID_SEARCH_BINARY: {
            int idx = ListaBuscaBinaria(pm, buffer);
            if (idx == -2) {
                MessageBoxW(hwnd, L"A lista não está ordenada!", L"Erro", MB_OK | MB_ICONERROR);
            }
            else if (idx >= 0) {
                SendMessageW(hList, LB_SETCURSEL, idx, 0);
            }
            else {
                MessageBoxW(hwnd, L"Palavra não encontrada (Binária)", L"Busca", MB_OK);
            }
            break;
        }

        case ID_CLEAR:
            LimparLista(pm);
            AtualizarListBox(hList);
            break;
        }
        break;
    }

    case WM_DESTROY:
        DestruirLista(pm);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

// Função principal do aplicativo, cria a janela e inicia
// o loop de mensagens Aka While loop menu feito no console tradicionalmente
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"ListaPalavras";

    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0, CLASS_NAME, L"Lista de Palavras 3000",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 450, 320,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) return 0;

    ShowWindow(hwnd, nCmdShow);

    MSG msg = { 0 };
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}