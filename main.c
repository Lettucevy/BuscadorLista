#pragma execution_character_set("utf-8") 
#include <windows.h>
#include <wchar.h> // Por favor, não mexer nesta linha ou na linha 1 a não ser que queiram o programa todo em chinês
#include <commdlg.h>
#include <shlwapi.h>
#include <stdio.h>
#include <fcntl.h>
#include "PalavraManager.h"
#include "Resource.h" // Aquivos PalavraManager.h e Resouce.h são cabeçalhos como requesitado pelo professor


// Cores do css
#define COLOR_BACKGROUND RGB(48, 138, 255)          // #308aff
#define COLOR_H3_TEXT RGB(255, 255, 255)            // #ffffff
#define COLOR_INPUT_BG RGB(255, 255, 255)           // #ffffff
#define COLOR_INPUT_TEXT RGB(0, 0, 0)               // #000000
#define COLOR_INPUT_BORDER RGB(159, 203, 255)       // #9fcbff
#define COLOR_BUTTON_DEFAULT_BG RGB(255, 255, 255)  // #ffffff
#define COLOR_BUTTON_DEFAULT_TEXT RGB(68, 133, 218) // #4485da
#define COLOR_BUTTON_ADD_BG RGB(38, 109, 205)       // #266dcd
#define COLOR_BUTTON_ADD_TEXT RGB(255, 255, 255)    // #ffffff
#define COLOR_BUTTON_BORDER RGB(159, 203, 255)      // #9fcbff

// Dimensões e espaçamentos estilo HTML/CSS
#define BODY_PADDING 20
#define CONTAINER_GAP 20
#define LEFT_PANEL_GAP 20
#define RIGHT_PANEL_GAP 8

#define EDIT_WIDTH 285
#define EDIT_HEIGHT 35
#define LISTBOX_WIDTH 300
#define LISTBOX_HEIGHT 200
#define BUTTON_WIDTH 150
#define BUTTON_HEIGHT 35
#define BUTTON_BORDER_RADIUS 8

#define CLIENT_WIDTH 800
#define CLIENT_HEIGHT 600

#define EDIT_WIDTH 285
#define EDIT_HEIGHT 35

#define BUTTON_WIDTH 150
#define BUTTON_HEIGHT 35
#define BUTTON_BORDER_RADIUS 8

#define CONSOLE_BAR_HEIGHT 80
#define GAP_ABOVE_CONSOLE 10

// Instancia do gerenciador de palavras
ListaManager* global_lm;

// Fontes
HFONT hFontTitle;
HFONT hFontControls;
HFONT hFontButtons;
HFONT hFontListBox;
HFONT hFontPromptBox;

// Pinceis
HBRUSH hBrushAppBackground;
HBRUSH hBrushInputBackground;
HBRUSH hBrushConsole;

// Console
HWND hConsoleEdit;
LARGE_INTEGER timerFrequency;

// Controles da janela principal e de gerenciamento de listas
HWND hStaticTitle, hEdit, hList;
HWND hBtnAdd, hBtnSort, hBtnLinear, hBtnBinary, hBtnClear;
HWND hBtnCopy, hBtnRemove;
HWND hBtnNewList, hBtnOpenList, hBtnSaveList, hBtnExportList, hBtnCloseList;
HWND hListOfLists;
HWND hStaticListTitle;

INT_PTR CALLBACK NewListDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

// Adiciona no Console
void AppendToConsoleBar(const wchar_t* newText) {
    if (!hConsoleEdit) return;

    int len = GetWindowTextLengthW(hConsoleEdit);
    SendMessageW(hConsoleEdit, EM_SETSEL, (WPARAM)len, (LPARAM)len);
    SendMessageW(hConsoleEdit, EM_REPLACESEL, FALSE, (LPARAM)newText);
    SendMessageW(hConsoleEdit, EM_REPLACESEL, FALSE, (LPARAM)L"\r\n");

    SendMessageW(hConsoleEdit, WM_VSCROLL, SB_BOTTOM, (LPARAM)NULL);
}

// Atualiza o ListBox com as palavras da lista atual
void AtualizarListBoxPalavras(HWND hList) {
    SendMessageW(hList, LB_RESETCONTENT, 0, 0);
    if (global_lm->currentListIndex != -1) {
        PalavraManager* current_pm = global_lm->lists[global_lm->currentListIndex].pm;
        if (current_pm) { // Verifica se a lista existe (não é NULL)
            for (int i = 0; i < ListaTamanho(current_pm); i++) {
                SendMessageW(hList, LB_ADDSTRING, 0, (LPARAM)ListaGet(current_pm, i));
            }
        }
    }
}

// Atualiza o ListBox de listas abertas
void AtualizarListBoxListasAbertas(HWND hListOfLists) {
    SendMessageW(hListOfLists, LB_RESETCONTENT, 0, 0);
    for (int i = 0; i < global_lm->count; i++) {
        SendMessageW(hListOfLists, LB_ADDSTRING, 0, (LPARAM)global_lm->lists[i].name);
    }
}

// Função para definir o título da lista atual
void SetCurrentListTitle(HWND hStaticTitle, int currentListIndex) {
    wchar_t titleBuffer[256];
    if (currentListIndex != -1 && currentListIndex < global_lm->count) {
        swprintf_s(titleBuffer, sizeof(titleBuffer) / sizeof(wchar_t), L"Lista de Palavras 3000 - %s", global_lm->lists[currentListIndex].name);
    }
    else {
        wcscpy_s(titleBuffer, sizeof(titleBuffer) / sizeof(wchar_t), L"Lista de Palavras 3000 - Nenhuma Lista Aberta");
    }
    SetWindowTextW(hStaticTitle, titleBuffer);
}

// Habilita e desabilita os bagui
void EnableWordListControls(BOOL enable) {
    EnableWindow(hEdit, enable);
    EnableWindow(hList, enable);
    EnableWindow(hBtnAdd, enable);
    EnableWindow(hBtnSort, enable);
    EnableWindow(hBtnLinear, enable);
    EnableWindow(hBtnBinary, enable);
    EnableWindow(hBtnClear, enable);
    EnableWindow(hBtnCopy, enable);
    EnableWindow(hBtnRemove, enable);
    EnableWindow(hBtnSaveList, enable);
    EnableWindow(hBtnExportList, enable);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
		// Inicializar o TimerFrequency para medir o tempo de execução
        QueryPerformanceFrequency(&timerFrequency);

        // Criar pinceis de fundo principal essas para ai..
        hBrushAppBackground = CreateSolidBrush(COLOR_BACKGROUND);
        hBrushInputBackground = CreateSolidBrush(COLOR_INPUT_BG);
        hBrushConsole = NULL;

        // Criar Fontes
        hFontTitle = CreateFontW(
            25, 0, 0, 0, FW_NORMAL,
            FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH, L"Arial"
        );

        hFontControls = CreateFontW(
            16, 0, 0, 0, FW_NORMAL,
            FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH, L"Arial"
        );

        hFontListBox = CreateFontW(
            16, 0, 0, 0, FW_NORMAL,
            FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH, L"Consolas"
        );

        hFontButtons = CreateFontW(
            15, 0, 0, 0, FW_BOLD,
            FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH, L"Arial"
        );
        hFontPromptBox = CreateFontW(
            20, 0, 0, 0, FW_NORMAL,
            FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH, L"Arial"
        );

        // Criar gerenciador de listas e carregar listas salvas
        global_lm = CriarListaManager();

		// Criar janela sopa de letrinha do krl so para declar elementos slc não compensa
        hStaticTitle = CreateWindowW(
            L"STATIC", L"Lista de Palavras 3000",
            WS_CHILD | WS_VISIBLE,
            BODY_PADDING, BODY_PADDING, 400, 30,
            hwnd, (HMENU)IDC_STATIC_TITLE, NULL, NULL
        );
        SendMessageW(hStaticTitle, WM_SETFONT, (WPARAM)hFontTitle, TRUE);

        int current_y = BODY_PADDING + 30 + 5;
        int current_y_left = BODY_PADDING + 30 + 5;

        hEdit = CreateWindowW(
            L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            BODY_PADDING, current_y, EDIT_WIDTH, EDIT_HEIGHT,
            hwnd, (HMENU)ID_EDIT, NULL, NULL
        );
        SendMessageW(hEdit, WM_SETFONT, (WPARAM)hFontPromptBox, TRUE);

        current_y += EDIT_HEIGHT + LEFT_PANEL_GAP;

        int statusBarYPos = CLIENT_HEIGHT - BODY_PADDING - CONSOLE_BAR_HEIGHT;
        int listboxHeight = statusBarYPos - GAP_ABOVE_CONSOLE - current_y_left;
        if (listboxHeight < 50) listboxHeight = 50;

        hList = CreateWindowW(L"LISTBOX", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY | WS_VSCROLL,
            BODY_PADDING, current_y_left + 50, LISTBOX_WIDTH, listboxHeight,
            hwnd, (HMENU)ID_LISTBOX, NULL, NULL
        );
        SendMessageW(hList, WM_SETFONT, (WPARAM)hFontListBox, TRUE);

        int right_panel_x = BODY_PADDING + LISTBOX_WIDTH + CONTAINER_GAP;
        int current_y_right = BODY_PADDING + 30 + 5;

        hBtnAdd = CreateWindowW(
            L"BUTTON", L"ADICIONAR",
            WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            right_panel_x, current_y - 55, BUTTON_WIDTH, BUTTON_HEIGHT,
            hwnd, (HMENU)ID_ADD, NULL, NULL
        );
        SendMessageW(hBtnAdd, WM_SETFONT, (WPARAM)hFontButtons, TRUE);
        current_y += BUTTON_HEIGHT + RIGHT_PANEL_GAP;

        hBtnSort = CreateWindowW(
            L"BUTTON", L"ORDENAR",
            WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            right_panel_x, current_y, BUTTON_WIDTH, BUTTON_HEIGHT,
            hwnd, (HMENU)ID_SORT, NULL, NULL
        );
        SendMessageW(hBtnSort, WM_SETFONT, (WPARAM)hFontButtons, TRUE);
        current_y += BUTTON_HEIGHT + RIGHT_PANEL_GAP;

        hBtnLinear = CreateWindowW(
            L"BUTTON", L"BUSCA LINEAR",
            WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            right_panel_x, current_y, BUTTON_WIDTH, BUTTON_HEIGHT,
            hwnd, (HMENU)ID_SEARCH_LINEAR, NULL, NULL
        );
        SendMessageW(hBtnLinear, WM_SETFONT, (WPARAM)hFontButtons, TRUE);
        current_y += BUTTON_HEIGHT + RIGHT_PANEL_GAP;

        hBtnBinary = CreateWindowW(
            L"BUTTON", L"BUSCA BINÁRIA",
            WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            right_panel_x, current_y, BUTTON_WIDTH, BUTTON_HEIGHT,
            hwnd, (HMENU)ID_SEARCH_BINARY, NULL, NULL
        );
        SendMessageW(hBtnBinary, WM_SETFONT, (WPARAM)hFontButtons, TRUE);
        current_y += BUTTON_HEIGHT + RIGHT_PANEL_GAP;

        hBtnClear = CreateWindowW(
            L"BUTTON", L"LIMPAR",
            WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            right_panel_x, current_y, BUTTON_WIDTH, BUTTON_HEIGHT,
            hwnd, (HMENU)ID_CLEAR, NULL, NULL
        );
        SendMessageW(hBtnClear, WM_SETFONT, (WPARAM)hFontButtons, TRUE);

        current_y_right += BUTTON_HEIGHT + RIGHT_PANEL_GAP + 15;

        hBtnCopy = CreateWindowW(
            L"BUTTON", L"COPIAR",
            WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            right_panel_x, current_y_right - 6, BUTTON_WIDTH, BUTTON_HEIGHT,
            hwnd, (HMENU)ID_COPY, NULL, NULL
        );
        SendMessageW(hBtnCopy, WM_SETFONT, (WPARAM)hFontButtons, TRUE);
        current_y_right += BUTTON_HEIGHT + RIGHT_PANEL_GAP;

        hBtnRemove = CreateWindowW(
            L"BUTTON", L"REMOVER",
            WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            right_panel_x, current_y_right + 170, BUTTON_WIDTH, BUTTON_HEIGHT,
            hwnd, (HMENU)ID_REMOVE, NULL, NULL
        );
        SendMessageW(hBtnRemove, WM_SETFONT, (WPARAM)hFontButtons, TRUE);
        current_y_right += BUTTON_HEIGHT + RIGHT_PANEL_GAP + 15;

        int list_manager_x = right_panel_x + BUTTON_WIDTH + CONTAINER_GAP;
        int list_manager_y = BODY_PADDING + 30 + 5;

        hStaticListTitle = CreateWindowW(
            L"STATIC", L"Listas Abertas",
            WS_CHILD | WS_VISIBLE,
            list_manager_x, list_manager_y, 200, 20,
            hwnd, (HMENU)IDC_STATIC_TITLE, NULL, NULL
        );
        SendMessageW(hStaticListTitle, WM_SETFONT, (WPARAM)hFontControls, TRUE);
        list_manager_y += 25;

        hListOfLists = CreateWindowW(L"LISTBOX", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY | WS_VSCROLL | LBS_WANTKEYBOARDINPUT,
            list_manager_x, list_manager_y, BUTTON_WIDTH + 50, LISTBOX_HEIGHT - 50,
            hwnd, (HMENU)ID_LIST_OF_LISTS, NULL, NULL
        );
        SendMessageW(hListOfLists, WM_SETFONT, (WPARAM)hFontListBox, TRUE);
        list_manager_y += LISTBOX_HEIGHT - 50 + RIGHT_PANEL_GAP;

        hBtnNewList = CreateWindowW(
            L"BUTTON", L"NOVA LISTA",
            WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            list_manager_x, list_manager_y, BUTTON_WIDTH + 50, BUTTON_HEIGHT,
            hwnd, (HMENU)ID_NEW_LIST, NULL, NULL
        );
        SendMessageW(hBtnNewList, WM_SETFONT, (WPARAM)hFontButtons, TRUE);
        list_manager_y += BUTTON_HEIGHT + RIGHT_PANEL_GAP;

        hBtnOpenList = CreateWindowW(
            L"BUTTON", L"ABRIR LISTA",
            WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            list_manager_x, list_manager_y, BUTTON_WIDTH + 50, BUTTON_HEIGHT,
            hwnd, (HMENU)ID_OPEN_LIST, NULL, NULL
        );
        SendMessageW(hBtnOpenList, WM_SETFONT, (WPARAM)hFontButtons, TRUE);
        list_manager_y += BUTTON_HEIGHT + RIGHT_PANEL_GAP;

        hBtnCloseList = CreateWindowW(
            L"BUTTON", L"FECHAR LISTA",
            WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            list_manager_x, list_manager_y, BUTTON_WIDTH + 50, BUTTON_HEIGHT,
            hwnd, (HMENU)ID_CLOSE_LIST, NULL, NULL
        );
        SendMessageW(hBtnCloseList, WM_SETFONT, (WPARAM)hFontButtons, TRUE);
        list_manager_y += BUTTON_HEIGHT + RIGHT_PANEL_GAP;

        hBtnSaveList = CreateWindowW(
            L"BUTTON", L"SALVAR LISTA",
            WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            list_manager_x, list_manager_y, BUTTON_WIDTH + 50, BUTTON_HEIGHT,
            hwnd, (HMENU)ID_SAVE_LIST, NULL, NULL
        );
        SendMessageW(hBtnSaveList, WM_SETFONT, (WPARAM)hFontButtons, TRUE);
        list_manager_y += BUTTON_HEIGHT + RIGHT_PANEL_GAP;

        hBtnExportList = CreateWindowW(
            L"BUTTON", L"EXPORTAR TXT",
            WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            list_manager_x, list_manager_y, BUTTON_WIDTH + 50, BUTTON_HEIGHT,
            hwnd, (HMENU)ID_EXPORT_LIST, NULL, NULL
        );
        SendMessageW(hBtnExportList, WM_SETFONT, (WPARAM)hFontButtons, TRUE);

        hConsoleEdit = CreateWindowExW(
            0, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | WS_VSCROLL,
            BODY_PADDING,
            statusBarYPos + 57,
            CLIENT_WIDTH - (2 * BODY_PADDING),
            CONSOLE_BAR_HEIGHT + 23,
            hwnd, (HMENU)IDC_CONSOLE_EDIT, NULL, NULL
        );
        SendMessageW(hConsoleEdit, WM_SETFONT, (WPARAM)hFontControls, TRUE);
        AppendToConsoleBar(L"Programa iniciado. Bem-vindo!");

        global_lm->currentListIndex = -1;
        SetCurrentListTitle(hStaticTitle, -1);
        AtualizarListBoxPalavras(hList);
        EnableWordListControls(FALSE); // Desabilita controles até uma lista ser aberta/criada

        break;
    }
	// Criação de elementos visuais, tudo no switch essa bosta de win32 tem bater no cara que acho que isso era uma boa ideia
    case WM_CTLCOLORSTATIC: {
        HDC hdcStatic = (HDC)wParam;
        HWND hwndStatic = (HWND)lParam;
        if (hwndStatic == hStaticTitle || hwndStatic == hStaticListTitle) {
            SetTextColor(hdcStatic, COLOR_H3_TEXT);
            SetBkMode(hdcStatic, TRANSPARENT);
            return (INT_PTR)hBrushAppBackground;
        }
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
    case WM_CTLCOLOREDIT: {
        HDC hdcEditCtrl = (HDC)wParam;
        HWND hwndCtrl = (HWND)lParam;
        if (hwndCtrl == hEdit) {
            SetTextColor(hdcEditCtrl, COLOR_INPUT_TEXT);
            SetBkColor(hdcEditCtrl, COLOR_INPUT_BG);
            return (INT_PTR)hBrushInputBackground;
        }
        else if (hwndCtrl == hConsoleEdit) {
            SetTextColor(hdcEditCtrl, RGB(170, 255, 170));
            SetBkColor(hdcEditCtrl, RGB(30, 30, 30));
            if (hBrushConsole == NULL) {
                hBrushConsole = CreateSolidBrush(RGB(30, 30, 30));
            }
            return (INT_PTR)hBrushConsole;
        }
        break;
    }
    case WM_CTLCOLORLISTBOX: {
        HDC hdcListBox = (HDC)wParam;
        SetTextColor(hdcListBox, COLOR_INPUT_TEXT);
        SetBkColor(hdcListBox, COLOR_INPUT_BG);
        return (INT_PTR)hBrushInputBackground;
    }
    case WM_DRAWITEM: {
        LPDRAWITEMSTRUCT pdis = (LPDRAWITEMSTRUCT)lParam;
    
        if (pdis->CtlType == ODT_BUTTON) {
            HDC hdc = pdis->hDC;
            RECT rc = pdis->rcItem;
            UINT itemState = pdis->itemState;
            wchar_t buttonText[256];
    
            GetWindowTextW(pdis->hwndItem, buttonText, sizeof(buttonText) / sizeof(wchar_t));
    
            COLORREF bgColor, textColor, borderColor;
            borderColor = COLOR_BUTTON_BORDER;
    
            if (pdis->CtlID == ID_ADD) {
                bgColor = (itemState & ODS_SELECTED) ? RGB(25, 80, 150) : COLOR_BUTTON_ADD_BG;
                textColor = COLOR_BUTTON_ADD_TEXT;
            }
            else {
                bgColor = (itemState & ODS_SELECTED) ? RGB(220, 220, 220) : COLOR_BUTTON_DEFAULT_BG;
                textColor = COLOR_BUTTON_DEFAULT_TEXT;
            }
            if (itemState & ODS_DISABLED) {
                bgColor = RGB(200, 200, 200);
                textColor = RGB(120, 120, 120);
            }
    
            HBRUSH hBgBrush = CreateSolidBrush(bgColor);
            HPEN hBorderPen = CreatePen(PS_SOLID, 2, borderColor);
    
            HGDIOBJ hOldBrush = SelectObject(hdc, hBgBrush);
            HGDIOBJ hOldPen = SelectObject(hdc, hBorderPen);
    
            RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, BUTTON_BORDER_RADIUS, BUTTON_BORDER_RADIUS);
    
            SelectObject(hdc, hOldPen);
            SelectObject(hdc, hOldBrush);
            DeleteObject(hBorderPen);
            DeleteObject(hBgBrush);
    
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, textColor);
            DrawTextW(hdc, buttonText, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    
            if (itemState & ODS_FOCUS && !(itemState & ODS_NOFOCUSRECT)) {
                HPEN hFocusPen = CreatePen(PS_DOT, 1, RGB(0, 0, 0));
                SelectObject(hdc, GetStockObject(NULL_BRUSH));
                SelectObject(hdc, hFocusPen);
                RoundRect(hdc, rc.left + 3, rc.top + 3, rc.right - 3, rc.bottom - 3, BUTTON_BORDER_RADIUS - 2, BUTTON_BORDER_RADIUS - 2);
                DeleteObject(hFocusPen);
            }
            return TRUE;
        }
        break;
    }
    case WM_COMMAND: {
        wchar_t buffer[MAX_WORD_LEN];
        wchar_t consoleMsg[256];
        GetWindowTextW(hEdit, buffer, MAX_WORD_LEN);

        LARGE_INTEGER startTime, endTime;
        double timeElapsed;

        PalavraManager* current_pm = NULL;
        if (global_lm->currentListIndex != -1 && global_lm->currentListIndex < global_lm->count) {
            current_pm = global_lm->lists[global_lm->currentListIndex].pm;
        }

        switch (LOWORD(wParam)) {
        case ID_ADD:
            if (current_pm) {
                if (wcslen(buffer) > 0) {
                    AdicionarNaLista(current_pm, buffer);
                    AtualizarListBoxPalavras(hList);
                    swprintf_s(consoleMsg, sizeof(consoleMsg) / sizeof(wchar_t), L"Palavra \"%s\" adicionada à lista '%s'.", buffer, global_lm->lists[global_lm->currentListIndex].name);
                    AppendToConsoleBar(consoleMsg);
                    SetWindowTextW(hEdit, L"");
                }
                else {
                    AppendToConsoleBar(L"Nenhuma palavra digitada para adicionar.");
                }
            }
            else {
                AppendToConsoleBar(L"Nenhuma lista aberta para adicionar palavras. Crie ou abra uma lista.");
            }
            break;

        case ID_SORT:
            if (current_pm) {
                if (ListaTamanho(current_pm) > 0) {
                    QueryPerformanceCounter(&startTime);
                    ListaSort(current_pm);
                    QueryPerformanceCounter(&endTime);
                    timeElapsed = (double)(endTime.QuadPart - startTime.QuadPart) * 1000.0 / timerFrequency.QuadPart;
                    AtualizarListBoxPalavras(hList);
                    swprintf_s(consoleMsg, sizeof(consoleMsg) / sizeof(wchar_t), L"Lista '%s' ordenada em %.4f ms.", global_lm->lists[global_lm->currentListIndex].name, timeElapsed);
                    AppendToConsoleBar(consoleMsg);
                }
                else {
                    AppendToConsoleBar(L"Lista vazia. Nada para ordenar.");
                }
            }
            else {
                AppendToConsoleBar(L"Nenhuma lista aberta para ordenar.");
            }
            break;

        case ID_SEARCH_LINEAR: {
            if (!current_pm) {
                AppendToConsoleBar(L"Nenhuma lista aberta para busca.");
                break;
            }
            if (wcslen(buffer) == 0) {
                AppendToConsoleBar(L"Digite uma palavra para buscar (Linear).");
                break;
            }
            QueryPerformanceCounter(&startTime);
            int idx = ListaBuscaLinear(current_pm, buffer);
            QueryPerformanceCounter(&endTime);
            timeElapsed = (double)(endTime.QuadPart - startTime.QuadPart) * 1000.0 / timerFrequency.QuadPart;

            if (idx >= 0) {
                SendMessageW(hList, LB_SETCURSEL, idx, 0);
                swprintf_s(consoleMsg, sizeof(consoleMsg) / sizeof(wchar_t),
                    L"Busca Linear na lista '%s': '%s' encontrada no índice %d. Tempo: %.4f ms.", global_lm->lists[global_lm->currentListIndex].name, buffer, idx, timeElapsed);
            }
            else {
                swprintf_s(consoleMsg, sizeof(consoleMsg) / sizeof(wchar_t),
                    L"Busca Linear na lista '%s': '%s' não encontrada. Tempo: %.4f ms.", global_lm->lists[global_lm->currentListIndex].name, buffer, timeElapsed);
            }
            AppendToConsoleBar(consoleMsg);
            break;
        }

        case ID_SEARCH_BINARY: {
            if (!current_pm) {
                AppendToConsoleBar(L"Nenhuma lista aberta para busca.");
                break;
            }
            if (wcslen(buffer) == 0) {
                AppendToConsoleBar(L"Digite uma palavra para buscar (Binária).");
                break;
            }
            QueryPerformanceCounter(&startTime);
            int idx = ListaBuscaBinaria(current_pm, buffer);
            QueryPerformanceCounter(&endTime);
            timeElapsed = (double)(endTime.QuadPart - startTime.QuadPart) * 1000.0 / timerFrequency.QuadPart;

            if (idx == -2) {
                MessageBoxW(hwnd, L"A lista não está ordenada!", L"Erro", MB_OK | MB_ICONERROR);
                swprintf_s(consoleMsg, sizeof(consoleMsg) / sizeof(wchar_t),
                    L"Erro Busca Binária na lista '%s': A lista precisa ser ordenada primeiro. Tempo: %.4f ms.", global_lm->lists[global_lm->currentListIndex].name, timeElapsed);
            }
            else if (idx >= 0) {
                SendMessageW(hList, LB_SETCURSEL, idx, 0);
                swprintf_s(consoleMsg, sizeof(consoleMsg) / sizeof(wchar_t),
                    L"Busca Binária na lista '%s': '%s' encontrada no índice %d. Tempo: %.4f ms.", global_lm->lists[global_lm->currentListIndex].name, buffer, idx, timeElapsed);
            }
            else {
                swprintf_s(consoleMsg, sizeof(consoleMsg) / sizeof(wchar_t),
                    L"Busca Binária na lista '%s': '%s' não encontrada. Tempo: %.4f ms.", global_lm->lists[global_lm->currentListIndex].name, buffer, timeElapsed);
            }
            AppendToConsoleBar(consoleMsg);
            break;
        }

        case ID_CLEAR:
            if (current_pm) {
                LimparLista(current_pm);
                AtualizarListBoxPalavras(hList);
                swprintf_s(consoleMsg, sizeof(consoleMsg) / sizeof(wchar_t), L"Lista '%s' limpa.", global_lm->lists[global_lm->currentListIndex].name);
                AppendToConsoleBar(consoleMsg);
            }
            else {
                AppendToConsoleBar(L"Nenhuma lista aberta para limpar.");
            }
            break;

        case ID_COPY: {
            int sel = SendMessageW(hList, LB_GETCURSEL, 0, 0);
            if (sel != LB_ERR) {
                int len = SendMessageW(hList, LB_GETTEXTLEN, sel, 0);
                if (len > 0) {
                    wchar_t* wordToCopy = (wchar_t*)malloc(sizeof(wchar_t) * (len + 1));
                    if (wordToCopy) {
                        SendMessageW(hList, LB_GETTEXT, sel, (LPARAM)wordToCopy);
                        if (OpenClipboard(hwnd)) {
                            EmptyClipboard();
                            HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, (len + 1) * sizeof(wchar_t));
                            if (hGlobal) {
                                wchar_t* pGlobal = (wchar_t*)GlobalLock(hGlobal);
                                if (pGlobal) {
                                    wcscpy_s(pGlobal, len + 1, wordToCopy);
                                    GlobalUnlock(hGlobal);
                                    SetClipboardData(CF_UNICODETEXT, hGlobal);
                                    swprintf_s(consoleMsg, sizeof(consoleMsg) / sizeof(wchar_t), L"Palavra \"%s\" copiada para a área de transferência.", wordToCopy);
                                    AppendToConsoleBar(consoleMsg);
                                }
                                else {
                                    AppendToConsoleBar(L"Erro ao copiar palavra (GlobalLock).");
                                }
                            }
                            else {
                                AppendToConsoleBar(L"Erro ao copiar palavra (GlobalAlloc).");
                            }
                            CloseClipboard();
                        }
                        else {
                            AppendToConsoleBar(L"Erro ao abrir a área de transferência.");
                        }
                        free(wordToCopy);
                    }
                }
            }
            else {
                AppendToConsoleBar(L"Nenhuma palavra selecionada para copiar.");
            }
            break;
        }

        case ID_REMOVE: {
            int sel = SendMessageW(hList, LB_GETCURSEL, 0, 0);
            if (current_pm && sel != LB_ERR) {
                wchar_t removedWord[MAX_WORD_LEN];
                SendMessageW(hList, LB_GETTEXT, sel, (LPARAM)removedWord);
                RemoverPalavra(current_pm, sel);
                AtualizarListBoxPalavras(hList);
                swprintf_s(consoleMsg, sizeof(consoleMsg) / sizeof(wchar_t), L"Palavra \"%s\" removida da lista '%s'.", removedWord, global_lm->lists[global_lm->currentListIndex].name);
                AppendToConsoleBar(consoleMsg);
            }
            else {
                AppendToConsoleBar(L"Nenhuma palavra selecionada ou lista não aberta para remover.");
            }
            break;
        }

        case ID_NEW_LIST: {
            wchar_t newListName[MAX_LIST_NAME_LEN];
            if (IDOK == DialogBoxParamW(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_NEW_LIST_DIALOG), hwnd, (DLGPROC)NewListDlgProc, (LPARAM)newListName)) {
                if (wcslen(newListName) > 0) {
                    if (global_lm->count >= MAX_OPEN_LISTS) {
                        AppendToConsoleBar(L"Limite máximo de listas abertas atingido.");
                        break;
                    }

                    PalavraManager* new_pm = CriarLista();
                    if (!new_pm) {
                        AppendToConsoleBar(L"Erro: Não foi possível criar uma nova lista.");
                        break;
                    }
                    int newIdx = AdicionarListaAberta(global_lm, newListName, L"", new_pm);
                    if (newIdx != -1) {
                        global_lm->currentListIndex = newIdx;
                        SetCurrentListTitle(hStaticTitle, global_lm->currentListIndex);
                        AtualizarListBoxListasAbertas(hListOfLists);
                        AtualizarListBoxPalavras(hList);
                        EnableWordListControls(TRUE);
                        swprintf_s(consoleMsg, sizeof(consoleMsg) / sizeof(wchar_t), L"Nova lista '%s' criada e aberta.", newListName);
                        AppendToConsoleBar(consoleMsg);
                    }
                    else {
                        AppendToConsoleBar(L"Erro ao adicionar nova lista aberta.");
                        DestruirLista(new_pm);
                    }
                }
                else {
                    AppendToConsoleBar(L"Nome da nova lista não pode ser vazio.");
                }
            }
            break;
        }

        case ID_OPEN_LIST: {
            OPENFILENAMEW ofn;
            wchar_t szFile[MAX_PATH] = L"";

            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
            ofn.lpstrFilter = L"Arquivos de Lista de Palavras (*.bin)\0*.bin\0Todos os Arquivos (*.*)\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
            ofn.lpstrDefExt = L"bin";

            if (GetOpenFileNameW(&ofn) == TRUE) {
                if (global_lm->count >= MAX_OPEN_LISTS) {
                    AppendToConsoleBar(L"Limite máximo de listas abertas atingido. Feche uma lista antes de abrir outra.");
                    break;
                }
                for (int i = 0; i < global_lm->count; ++i) {
                    if (wcscmp(global_lm->lists[i].filePath, ofn.lpstrFile) == 0) {
                        AppendToConsoleBar(L"A lista selecionada já está aberta.");
                        global_lm->currentListIndex = i;
                        SetCurrentListTitle(hStaticTitle, global_lm->currentListIndex);
                        AtualizarListBoxPalavras(hList);
                        SendMessageW(hListOfLists, LB_SETCURSEL, i, 0);
                        return 0;
                    }
                }

                PalavraManager* opened_pm = NULL;
                if (CarregarListaDeArquivo(&opened_pm, ofn.lpstrFile)) {
                    wchar_t listName[MAX_LIST_NAME_LEN];
                    wchar_t* fileName = PathFindFileNameW(ofn.lpstrFile);
                    if (fileName) {
                        wcsncpy_s(listName, MAX_LIST_NAME_LEN, fileName, _TRUNCATE);
                        wchar_t* dot = wcsrchr(listName, L'.');
                        if (dot && wcscmp(dot, L".bin") == 0) {
                            *dot = L'\0';
                        }
                    }
                    else {
                        wcscpy_s(listName, MAX_LIST_NAME_LEN, L"Lista Carregada");
                    }

                    int newIdx = AdicionarListaAberta(global_lm, listName, ofn.lpstrFile, opened_pm);
                    if (newIdx != -1) {
                        global_lm->currentListIndex = newIdx;
                        SetCurrentListTitle(hStaticTitle, global_lm->currentListIndex);
                        AtualizarListBoxListasAbertas(hListOfLists);
                        AtualizarListBoxPalavras(hList);
                        EnableWordListControls(TRUE);
                        swprintf_s(consoleMsg, sizeof(consoleMsg) / sizeof(wchar_t), L"Lista '%s' aberta do arquivo '%s'.", listName, ofn.lpstrFile);
                        AppendToConsoleBar(consoleMsg);
                        SendMessageW(hListOfLists, LB_SETCURSEL, newIdx, 0);
                    }
                    else {
                        AppendToConsoleBar(L"Erro ao adicionar lista carregada ao gerenciador de abertas.");
                        DestruirLista(opened_pm);
                    }
                }
                else {
                    AppendToConsoleBar(L"Erro ao carregar lista do arquivo. Arquivo inválido ou corrompido.");
                }
            }
            break;
        }

        case ID_CLOSE_LIST: {
            int sel = SendMessageW(hListOfLists, LB_GETCURSEL, 0, 0);
            if (sel != LB_ERR && sel < global_lm->count) {
                wchar_t closedListName[MAX_LIST_NAME_LEN];
                wcscpy_s(closedListName, MAX_LIST_NAME_LEN, global_lm->lists[sel].name);

                RemoverListaAberta(global_lm, sel);

                AtualizarListBoxListasAbertas(hListOfLists);

                if (global_lm->count > 0) {
                    if (global_lm->currentListIndex == -1 || global_lm->currentListIndex >= global_lm->count) {
                        global_lm->currentListIndex = 0;
                    }
                    SetCurrentListTitle(hStaticTitle, global_lm->currentListIndex);
                    AtualizarListBoxPalavras(hList);
                    SendMessageW(hListOfLists, LB_SETCURSEL, global_lm->currentListIndex, 0);
                    swprintf_s(consoleMsg, sizeof(consoleMsg) / sizeof(wchar_t), L"Lista '%s' fechada. Agora '%s' está aberta.", closedListName, global_lm->lists[global_lm->currentListIndex].name);
                }
                else {
                    global_lm->currentListIndex = -1;
                    SetCurrentListTitle(hStaticTitle, -1);
                    AtualizarListBoxPalavras(hList);
                    EnableWordListControls(FALSE);
                    swprintf_s(consoleMsg, sizeof(consoleMsg) / sizeof(wchar_t), L"Lista '%s' fechada. Nenhuma lista aberta.", closedListName);
                }
                AppendToConsoleBar(consoleMsg);
            }
            else {
                AppendToConsoleBar(L"Nenhuma lista selecionada para fechar.");
            }
            break;
        }

        case ID_LIST_OF_LISTS:
            if (HIWORD(wParam) == LBN_DBLCLK) {
                int selectedIndex = SendMessageW(hListOfLists, LB_GETCURSEL, 0, 0);
                if (selectedIndex != LB_ERR) {
                    if (selectedIndex != global_lm->currentListIndex) {
                        global_lm->currentListIndex = selectedIndex;
                        SetCurrentListTitle(hStaticTitle, global_lm->currentListIndex);
                        AtualizarListBoxPalavras(hList);
                        EnableWordListControls(TRUE);
                        swprintf_s(consoleMsg, sizeof(consoleMsg) / sizeof(wchar_t), L"Lista '%s' agora está aberta.", global_lm->lists[global_lm->currentListIndex].name);
                        AppendToConsoleBar(consoleMsg);
                    }
                    else {
                        AppendToConsoleBar(L"A lista selecionada já está aberta.");
                    }
                }
            }
            break;

        case ID_SAVE_LIST: {
            if (current_pm && global_lm->currentListIndex != -1) {
                wchar_t filePath[MAX_PATH];
                if (wcslen(global_lm->lists[global_lm->currentListIndex].filePath) > 0) {
                    wcscpy_s(filePath, MAX_PATH, global_lm->lists[global_lm->currentListIndex].filePath);
                    if (SalvarListaEmArquivo(current_pm, filePath)) {
                        swprintf_s(consoleMsg, sizeof(consoleMsg) / sizeof(wchar_t), L"Lista '%s' salva em '%s'.", global_lm->lists[global_lm->currentListIndex].name, filePath);
                        AppendToConsoleBar(consoleMsg);
                    }
                    else {
                        AppendToConsoleBar(L"Erro ao salvar a lista.");
                    }
                }
                else {
                    OPENFILENAMEW ofn;
                    wchar_t szFile[MAX_PATH] = L"";
                    wcsncpy_s(szFile, MAX_PATH, global_lm->lists[global_lm->currentListIndex].name, _TRUNCATE);
                    wcscat_s(szFile, MAX_PATH, L".bin");

                    ZeroMemory(&ofn, sizeof(ofn));
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = hwnd;
                    ofn.lpstrFile = szFile;
                    ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
                    ofn.lpstrFilter = L"Arquivos de Lista de Palavras (*.bin)\0*.bin\0Todos os Arquivos (*.*)\0*.*\0";
                    ofn.nFilterIndex = 1;
                    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
                    ofn.lpstrDefExt = L"bin";

                    if (GetSaveFileNameW(&ofn) == TRUE) {
                        if (SalvarListaEmArquivo(current_pm, ofn.lpstrFile)) {
                            wcscpy_s(global_lm->lists[global_lm->currentListIndex].filePath, MAX_PATH, ofn.lpstrFile);
                            swprintf_s(consoleMsg, sizeof(consoleMsg) / sizeof(wchar_t), L"Lista '%s' salva como '%s'.", global_lm->lists[global_lm->currentListIndex].name, ofn.lpstrFile);
                            AppendToConsoleBar(consoleMsg);
                        }
                        else {
                            AppendToConsoleBar(L"Erro ao salvar a lista no novo arquivo.");
                        }
                    }
                    else {
                        AppendToConsoleBar(L"Salvamento da lista cancelado.");
                    }
                }
            }
            else {
                AppendToConsoleBar(L"Nenhuma lista aberta para salvar.");
            }
            break;
        }

        case ID_EXPORT_LIST:
            if (current_pm) {
                ExportarListaParaTXT(current_pm, hwnd);
            }
            else {
                AppendToConsoleBar(L"Nenhuma lista aberta para exportar.");
            }
            break;

        case LBN_DBLCLK:
            if ((HWND)lParam == hListOfLists) {
                int sel = SendMessageW(hListOfLists, LB_GETCURSEL, 0, 0);
                if (sel != LB_ERR && sel < global_lm->count) {
                    global_lm->currentListIndex = sel;
                    SetCurrentListTitle(hStaticTitle, global_lm->currentListIndex);
                    AtualizarListBoxPalavras(hList);
                    EnableWordListControls(TRUE);
                    swprintf_s(consoleMsg, sizeof(consoleMsg) / sizeof(wchar_t), L"Lista '%s' selecionada como a lista atual.", global_lm->lists[global_lm->currentListIndex].name);
                    AppendToConsoleBar(consoleMsg);
                }
            }
            break;
        }
        break;
    }
    case WM_DESTROY:
        DestruirListaManager(global_lm);
        DeleteObject(hFontTitle);
        DeleteObject(hFontControls);
        DeleteObject(hFontButtons);
        DeleteObject(hFontListBox);
        DeleteObject(hBrushAppBackground);
        DeleteObject(hBrushInputBackground);
        if (hBrushConsole != NULL) {
            DeleteObject(hBrushConsole);
        }
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

INT_PTR CALLBACK NewListDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static wchar_t* listNameBuffer;

    switch (message) {
    case WM_INITDIALOG:
        listNameBuffer = (wchar_t*)lParam;
        listNameBuffer[0] = L'\0';
        SetFocus(GetDlgItem(hDlg, IDC_NEW_LIST_NAME_EDIT));
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
            GetWindowTextW(GetDlgItem(hDlg, IDC_NEW_LIST_NAME_EDIT), listNameBuffer, MAX_LIST_NAME_LEN);
            EndDialog(hDlg, IDOK);
            return TRUE;
        case IDCANCEL:
            listNameBuffer[0] = L'\0';
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        break;
    }
    return FALSE;
}
// Criação da janela Titulo e janela principal do aplicativo icon etc...
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    if (!QueryPerformanceFrequency(&timerFrequency)) {
        MessageBoxW(NULL, L"Contador de performance de alta precisão não suportado!", L"Erro de Timing", MB_OK | MB_ICONERROR);
    }

    const wchar_t CLASS_NAME[] = L"ListaPalavrasApp";

    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = CreateSolidBrush(COLOR_BACKGROUND);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BUSCADORLISTA));

    if (!RegisterClassW(&wc)) {
        MessageBoxW(NULL, L"Falha ao registrar classe da janela!", L"Erro", MB_ICONERROR | MB_OK);
        return 0;
    }

    WNDCLASSW wcDlg = { 0 };
    if (!GetClassInfoW(hInstance, L"NewListDialogClass", &wcDlg)) {
        wcDlg.lpfnWndProc = NewListDlgProc;
        wcDlg.hInstance = hInstance;
        wcDlg.lpszClassName = L"NewListDialogClass";
        wcDlg.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcDlg.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcDlg.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        if (!RegisterClassW(&wcDlg)) {
            MessageBoxW(NULL, L"Falha ao registrar classe da dialog box!", L"Erro", MB_ICONERROR | MB_OK);
            return 0;
        }
    }

    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"Lista de Palavras 3000",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        816, 720,
        NULL,
        NULL,
        hInstance,
        NULL                            
    );

    if (hwnd == NULL) {
        if (wc.hbrBackground) DeleteObject(wc.hbrBackground);
        MessageBoxW(NULL, L"Falha ao criar janela!", L"Erro", MB_ICONERROR | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = { 0 };
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        if (msg.hwnd == hListOfLists && msg.message == WM_LBUTTONDBLCLK) {
            SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(ID_LIST_OF_LISTS, LBN_DBLCLK), (LPARAM)msg.hwnd);
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (wc.hbrBackground) DeleteObject(wc.hbrBackground);
    return (int)msg.wParam;
}