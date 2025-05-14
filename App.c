#include <windows.h>
#include <stdlib.h>
#include <string.h>

#define _CRT_SECURE_NO_WARNINGS
#define ID_EDITBOX 1001
#define ID_BUTTON  1002
#define ID_LISTBOX 1003
#define MAX_WORD_LENGTH 100

typedef struct Palavra {
    char texto[MAX_WORD_LENGTH];
    struct Palavra* proxima;
} Palavra;

Palavra* lista = NULL;

void adicionarPalavra(const char* texto) {
    Palavra* nova = (Palavra*)malloc(sizeof(Palavra));
    strncpy_s(nova->texto, MAX_WORD_LENGTH, texto, _TRUNCATE);
    nova->texto[MAX_WORD_LENGTH - 1] = '\0';
    nova->proxima = NULL;

    if (lista == NULL) {
        lista = nova;
    }
    else {
        Palavra* atual = lista;
        while (atual->proxima != NULL)
            atual = atual->proxima;
        atual->proxima = nova;
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"WordListWindowClass";

    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"Sistema de Lista de Palavras",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
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

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND hEdit, hButton, hListBox;

    switch (uMsg) {
    case WM_CREATE:
        hEdit = CreateWindowExW(0, L"EDIT", NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            10, 10, 250, 25,
            hwnd, (HMENU)ID_EDITBOX, NULL, NULL);

        hButton = CreateWindowExW(0, L"BUTTON", L"Inserir",
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            270, 10, 80, 25,
            hwnd, (HMENU)ID_BUTTON, NULL, NULL);

        hListBox = CreateWindowExW(0, L"LISTBOX", NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY,
            10, 50, 340, 200,
            hwnd, (HMENU)ID_LISTBOX, NULL, NULL);
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == ID_BUTTON) {
            wchar_t wBuffer[MAX_WORD_LENGTH];
            GetWindowTextW(hEdit, wBuffer, MAX_WORD_LENGTH);

            if (wcslen(wBuffer) > 0) {
                // Adiciona à ListBox
                SendMessageW(hListBox, LB_ADDSTRING, 0, (LPARAM)wBuffer);

                // Converte wchar_t para char
                char palavraAnsi[MAX_WORD_LENGTH];
                size_t convertedChars = 0;
                wcstombs_s(&convertedChars, palavraAnsi, MAX_WORD_LENGTH, wBuffer, _TRUNCATE);

                // Adiciona à lista em memória
                adicionarPalavra(palavraAnsi);

                // Limpa a caixa de texto
                SetWindowTextW(hEdit, L"");
            }
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}
