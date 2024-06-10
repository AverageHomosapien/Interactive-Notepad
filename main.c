#include <windows.h>
#include "resource.h"
#include "menu_operations.c"

const char g_szClassName[] = "myWindowClass";

// Step 4: the Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    //HWND hEdit; // Struggling with scope of these dialog boxes
    OPENFILENAME ofn;
    char szFileName[MAX_PATH] = "";
    ZeroMemory(&ofn, sizeof(ofn));

    switch(msg)
    {
        case WM_CREATE:
        {
            HFONT hfDefault;
            HWND hEdit;

            hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", 
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL, 
            0, 0, 100, 100, hwnd, (HMENU)IDC_EDIT_WINDOW, GetModuleHandle(NULL), NULL);
            if(hEdit == NULL)
            {
                MessageBox(hwnd, "Could not create edit box.", "Error", MB_OK | MB_ICONERROR);
            }
            hfDefault = GetStockObject(DEFAULT_GUI_FONT);
            SendMessage(hEdit, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
        }
        case WM_SIZE:
        {
            HWND hEdit;
            RECT rcClient;

            GetClientRect(hwnd, &rcClient);

            hEdit = GetDlgItem(hwnd, IDC_EDIT_WINDOW);
            SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);
        }
        break;
        case WM_LBUTTONDOWN:
        {
	    char szFileName[MAX_PATH];
            HINSTANCE hInstance = GetModuleHandle(NULL);

            GetModuleFileName(hInstance, szFileName, MAX_PATH);
            MessageBox(hwnd, szFileName, ID_APP_NAME, MB_OK | MB_ICONINFORMATION);   
        }
            break;
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case ID_FILE_EXIT:
                    DestroyWindow(hwnd);
                    break;

                case ID_FILE_SAVE:
                    MessageBox(hwnd, "Save", ID_APP_NAME, MB_OK | MB_ICONINFORMATION);   
                    break;

                case ID_FILE_SAVE_AS:
                    ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
                    ofn.hwndOwner = hwnd;
                    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
                    ofn.lpstrFile = szFileName;
                    ofn.nMaxFile = MAX_PATH;
                    ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT; 
                    ofn.lpstrDefExt = "txt";

                    if(GetSaveFileName(&ofn))
                    {
                        SaveTextFileFromEdit(GetDlgItem(hwnd, IDC_EDIT_WINDOW), ofn.lpstrFile);
                    }
                    MessageBox(hwnd, "Save As", ID_APP_NAME, MB_OK | MB_ICONINFORMATION);   
                    break;
                    
                case ID_FILE_OPEN:
                    ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
                    ofn.hwndOwner = hwnd;
                    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
                    ofn.lpstrFile = szFileName;
                    ofn.nMaxFile = MAX_PATH;
                    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
                    ofn.lpstrDefExt = "txt";

                    if(GetOpenFileName(&ofn))
                    {
                        LoadTextFileToEdit(GetDlgItem(hwnd, IDC_EDIT_WINDOW), ofn.lpstrFile);
                    }
                    break;

                case ID_FILE_UNDO:
                    MessageBox(hwnd, "Undo", ID_APP_NAME, MB_OK | MB_ICONINFORMATION);   
                    break;

                case ID_FILE_REDO:
                    MessageBox(hwnd, "Redo", ID_APP_NAME, MB_OK | MB_ICONINFORMATION);   
                    break;
            }
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    //Step 1: Registering the Window Class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_NOTEPADICON));
    wc.hIconSm       = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_NOTEPADICON), IMAGE_ICON, 16, 16, 0);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = MAKEINTRESOURCE(IDR_NOTEPADMENU);
    wc.lpszClassName = g_szClassName;

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Step 2: Creating the Window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        "Interactive Notepad",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
        NULL, NULL, hInstance, NULL);

    if(hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Step 3: The Message Loop
    while(GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}
