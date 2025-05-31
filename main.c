#pragma execution_character_set("utf-8") 
#include <windows.h>
#include <wchar.h> // Por favor, não mexer nesta linha ou na linha 1 a não ser que queiram o programa todo em chinês
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

// Dimensões e espaçamentos do HTML
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

// Instancia do gerenciador de palavras
PalavraManager* pm;

// Fontes
HFONT hFontTitle;
HFONT hFontControls;
HFONT hFontButtons;
HFONT hFontListBox;

// Pinceis
HBRUSH hBrushAppBackground;
HBRUSH hBrushInputBackground;

// Atualiza o ListBox com as palavras do gerenciador
void AtualizarListBox(HWND hList) {
    SendMessageW(hList, LB_RESETCONTENT, 0, 0);
    for (int i = 0; i < ListaTamanho(pm); i++) {
        SendMessageW(hList, LB_ADDSTRING, 0, (LPARAM)ListaGet(pm, i));
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND hStaticTitle, hEdit, hList;
    static HWND hBtnAdd, hBtnSort, hBtnLinear, hBtnBinary, hBtnClear;

    switch (uMsg) {
    case WM_CREATE: {

        // Criar pincel de fundo principal
        hBrushAppBackground = CreateSolidBrush(COLOR_BACKGROUND);
        hBrushInputBackground = CreateSolidBrush(COLOR_INPUT_BG);

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
		// Criar janela sopa de letrinha do krl so para declar elementos slc não compensa
        hStaticTitle = CreateWindowW(
            L"STATIC", L"Lista de Palavras 3000",
            WS_CHILD | WS_VISIBLE,
            BODY_PADDING, BODY_PADDING, 400, 30,
            hwnd, (HMENU)IDC_STATIC_TITLE, NULL, NULL
        );
        SendMessageW(hStaticTitle, WM_SETFONT, (WPARAM)hFontTitle, TRUE);

        int current_y = BODY_PADDING + 30 + 5;

        hEdit = CreateWindowW(
            L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            BODY_PADDING, current_y, EDIT_WIDTH, EDIT_HEIGHT,
            hwnd, (HMENU)ID_EDIT, NULL, NULL
        );
        SendMessageW(hEdit, WM_SETFONT, (WPARAM)hFontControls, TRUE);

        current_y += EDIT_HEIGHT + LEFT_PANEL_GAP;

        hList = CreateWindowW(
            L"LISTBOX", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY | WS_VSCROLL,
            BODY_PADDING, current_y, LISTBOX_WIDTH, LISTBOX_HEIGHT,
            hwnd, (HMENU)ID_LISTBOX, NULL, NULL
        );
        SendMessageW(hList, WM_SETFONT, (WPARAM)hFontListBox, TRUE);

        int right_panel_x = BODY_PADDING + LISTBOX_WIDTH + CONTAINER_GAP;
        current_y = BODY_PADDING + 30 + 5;

        hBtnAdd = CreateWindowW(
            L"BUTTON", L"ADICIONAR",
            WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            right_panel_x, current_y, BUTTON_WIDTH, BUTTON_HEIGHT,
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

        // Criar lista de palavras
        pm = CriarLista();
        break;
    }
	// Criação de elementos visuais, tudo no switch essa bosta de win32 tem bater no cara que acho que isso era uma boa ideia
    case WM_CTLCOLORSTATIC: {
        HDC hdcStatic = (HDC)wParam;
        HWND hwndStatic = (HWND)lParam;
        if (hwndStatic == hStaticTitle) {
            SetTextColor(hdcStatic, COLOR_H3_TEXT);
            SetBkMode(hdcStatic, TRANSPARENT);
            return (INT_PTR)hBrushAppBackground;
        }
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
    case WM_CTLCOLOREDIT: {
        HDC hdcEdit = (HDC)wParam;
        HWND hwndEditCtrl = (HWND)lParam;
        if (hwndEditCtrl == hEdit) {
            SetTextColor(hdcEdit, COLOR_INPUT_TEXT);
            SetBkColor(hdcEdit, COLOR_INPUT_BG);
            return (INT_PTR)hBrushInputBackground;
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
                SelectObject(hdc, hOldPen);
                DeleteObject(hFocusPen);
            }
            return TRUE;
        }
        break;
    }
    // Adição de funcionalidade dos botões
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
        DeleteObject(hFontTitle);
        DeleteObject(hFontControls);
        DeleteObject(hFontButtons);
        DeleteObject(hFontListBox);
        DeleteObject(hBrushAppBackground);
        DeleteObject(hBrushInputBackground);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}
// Criação da janela Titulo e janela principal do aplicativo icon etc...
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
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

    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"Lista de Palavras 3000",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        816, 639,
        NULL,
        NULL,
        hInstance,
        NULL                            
    );

    if (hwnd == NULL) {
        DeleteObject(wc.hbrBackground);
        MessageBoxW(NULL, L"Falha ao criar janela!", L"Erro", MB_ICONERROR | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = { 0 };
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    DeleteObject(wc.hbrBackground);
    return (int)msg.wParam;
}