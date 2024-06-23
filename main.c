#include <stdio.h>
#include <string.h>
#include <stdlib.h> // For itoa in some environments
#include <windows.h>
#include <commctrl.h>
#include <stdbool.h>
#include <math.h>
#include "resource.h"
#include "menu_operations.c"

BOOL outstandingChanges = false;
char szFileName[MAX_PATH] = "";
OPENFILENAME ofn;

WNDPROC oldChildWndProc;

// Forward declaration of functions
LRESULT CALLBACK ParentWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildWndProc(HWND, UINT, WPARAM, LPARAM);
void WindowResizeTriggered(HWND);
int Max(int, int);

// The Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_CREATE:
        {
            // Initialize common controls (required for using the status bar)
            InitCommonControls();

            HWND hStatus; // Status bar extension window
            HFONT hfDefault = GetStockObject(DEFAULT_GUI_FONT);

            // Creating the 'StatusBox' footer on the notepad
            hStatus = CreateWindowEx(0, STATUSCLASSNAME, NULL, WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0, hwnd, (HMENU)IDC_STATUS_WINDOW, GetModuleHandle(NULL), NULL);
            if(hStatus == NULL)
            {
                MessageBox(hwnd, "Could not create status box.", "Error", MB_OK | MB_ICONERROR);
            }
            SendMessage(hStatus, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
            // Adding messages to a dialog box
            int statwidths[] = {100, -1};
            SendMessage(hStatus, SB_SETPARTS, sizeof(statwidths)/sizeof(int), (LPARAM)statwidths);
            SendMessage(hStatus, SB_SETTEXT, 0, (LPARAM)"Line 1, Column 1");
            SendMessage(hStatus, SB_SETTEXT, 1, (LPARAM)"0 Characters");

            WindowResizeTriggered(hwnd);
        }

        case WM_SIZE:
        {
            WindowResizeTriggered(hwnd);
        }
        break;

        case WM_KEYUP:
        case WM_KEYDOWN:
        {
            outstandingChanges = true;
            
            HWND hStatus = GetDlgItem(hwnd, IDC_STATUS_WINDOW);
            HWND hEdit = GetDlgItem(hwnd, IDC_EDIT_WINDOW);

            int characterCount = GetWindowTextLength(hEdit);
            int nDigits = floor(log10(abs(Max(characterCount, 1)))) + 1;
            char* outputString = (char *)malloc((11 + nDigits) * sizeof(char));

            sprintf(outputString, "%d Characters", characterCount);
            SendMessage(hStatus, SB_SETTEXT, 1, (LPARAM)outputString);
            free(outputString);
            break;
        }

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {

                case ID_FILE_EXIT:
                    DestroyWindow(hwnd);
                    break;

                case ID_FILE_SAVE:
                    // Some APIs require unused members to be null
                    ZeroMemory(&ofn, sizeof(ofn));
                    // Initialise the OpenFileName struct
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = hwnd;
                    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
                    ofn.lpstrFile = szFileName;
                    ofn.nMaxFile = MAX_PATH;
                    ofn.lpstrDefExt = "txt";
                    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
                    if (*ofn.lpstrFile != '\0')
                    {
                        MessageBox(hwnd, "Save", ID_APP_NAME, MB_OK | MB_ICONINFORMATION);   
                        outstandingChanges = false;
                    }
                    else
                    {
                        if(GetSaveFileName(&ofn))
                        {
                            SaveTextFileFromEdit(GetDlgItem(hwnd, IDC_EDIT_WINDOW), ofn.lpstrFile);
                            MessageBox(hwnd, "File has been saved", ID_APP_NAME, MB_OK | MB_ICONINFORMATION);   
                            outstandingChanges = false;
                        }
                    }
                    break;

                case ID_FILE_SAVE_AS:
                    // Some APIs require unused members to be null
                    ZeroMemory(&ofn, sizeof(ofn));
                    // Initialise the OpenFileName struct
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = hwnd;
                    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
                    ofn.lpstrFile = szFileName;
                    ofn.nMaxFile = MAX_PATH;
                    ofn.lpstrDefExt = "txt";
                    ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT; 
                    if(GetSaveFileName(&ofn))
                    {
                        SaveTextFileFromEdit(GetDlgItem(hwnd, IDC_EDIT_WINDOW), ofn.lpstrFile);
                        MessageBox(hwnd, "File has been saved", ID_APP_NAME, MB_OK | MB_ICONINFORMATION);   
                        outstandingChanges = false;
                    }
                    break;

                case ID_FILE_OPEN:
                    // Some APIs require unused members to be null
                    ZeroMemory(&ofn, sizeof(ofn));
                    // Initialise the OpenFileName struct
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = hwnd;
                    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
                    ofn.lpstrFile = szFileName;
                    ofn.nMaxFile = MAX_PATH;
                    ofn.lpstrDefExt = "txt";
                    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
                    //HWND hEdit = GetDlgItem(hwnd, IDC_EDIT_WINDOW);
                    if(GetOpenFileName(&ofn))
                    {
                        LoadTextFileToEdit(GetDlgItem(hwnd, IDC_EDIT_WINDOW), ofn.lpstrFile);
                    }
                    //int textLength = GetWindowTextLength(hEdit);
                    //char* textBuffer = (char*)malloc(textLength*(sizeof(char)+1)); // Need to account for null terminator when copying text
                    //GetWindowText(hEdit, textBuffer, textLength);
                    //MessageBox(hwnd, textBuffer, ID_APP_NAME, MB_OK | MB_ICONINFORMATION);
                    //free(textBuffer);
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
            if (outstandingChanges && MessageBox(hwnd, "There are outstanding changes. Are you sure you want to quit?", ID_APP_NAME, MB_YESNO | MB_ICONINFORMATION | MB_DEFBUTTON2) == IDYES){
                DestroyWindow(hwnd);
            }
            else if (!outstandingChanges){
                DestroyWindow(hwnd);
            }
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
    HWND hwnd, hEdit;
    MSG Msg;
    HFONT hfDefault = GetStockObject(DEFAULT_GUI_FONT);
    HACCEL hAccel;

    // Register the Window Class
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
    wc.lpszClassName = ID_APP_CLASS;

    hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR));

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Create the Window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        ID_APP_CLASS,
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

    // Register child window class
    wc.lpfnWndProc = DefWindowProc;
    wc.lpszClassName = "ChildWindowClass";
    RegisterClass(&wc);

    // Create child window
    hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL, 0, 0, 100, 100, hwnd, (HMENU)IDC_EDIT_WINDOW, GetModuleHandle(NULL), NULL);
    if(hEdit == NULL)
    {
        MessageBox(hwnd, "Could not create edit box.", "Error", MB_OK | MB_ICONERROR);
    }
    SendMessage(hEdit, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

    // Subclass the child window
    oldChildWndProc = (WNDPROC)SetWindowLongPtr(hEdit, GWLP_WNDPROC, (LONG_PTR)ChildWndProc);

    // Engage The Message Loop
    while(GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        if(!TranslateAccelerator(hwnd, hAccel, &Msg)){
            TranslateMessage(&Msg);
            DispatchMessage(&Msg);
        }
    }
    return Msg.wParam;
}

// Child window procedure
LRESULT CALLBACK ChildWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    // Redirect input messages to the parent window
    HWND hwndParent = GetParent(hwnd);

    switch (msg) {
    case WM_KEYDOWN:
    case WM_KEYUP:
        //MessageBox(hwnd, "Callback!", ID_APP_NAME, MB_OK | MB_ICONINFORMATION);
        SendMessage(hwndParent, msg, wParam, lParam);
        return 0;
    }

    // Call the original window procedure for default handling
    return CallWindowProc(oldChildWndProc, hwnd, msg, wParam, lParam);
}

void WindowResizeTriggered(HWND hwnd){
    HWND hStatus;
    RECT rcStatus;
    int iStatusHeight;

    HWND hEdit;
    int iEditHeight;
    RECT rcClient;

    // Size status bar and get height
    hStatus = GetDlgItem(hwnd, IDC_STATUS_WINDOW);
    SendMessage(hStatus, WM_SIZE, 0, 0);
    GetWindowRect(hStatus, &rcStatus);
    iStatusHeight = rcStatus.bottom - rcStatus.top;

    // Calculate remaining height and size edit
    GetClientRect(hwnd, &rcClient);
    iEditHeight = rcClient.bottom - iStatusHeight;
    hEdit = GetDlgItem(hwnd, IDC_EDIT_WINDOW);
    SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, iEditHeight, SWP_NOZORDER); 
}

int Max(int num1, int num2){
    if (num1 > num2){
        return num1;
    }
    return num2;
}
