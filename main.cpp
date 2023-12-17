#include "wastemanager.h"
#include "strsafe.h"
#include "treestruct.h"
#include "linkedlist.h"

/*
    Salvar relatórios MTR, CDF e FDSR com o seguinte padrão:

    [TIPORELATÓRIO]_[IDEMPRESA]_[NUMERO ANO E MÊS] [NUMERO HORA E MINUTO].w
    Ex.: MTR_1_2023111810.w
    Ex.: CDF_2_2023121710.w

    O número no relatório nunca será o mesmo, portanto será único!
    Cada empresa poderá ter vários relatórios.

    O próximo procedimento é ler cada relatório do diretório e identificar o
    seu número (Ano, Mês, Hora e Minuto). Separar-los em cada variável e
    organizar no formato MM/YYYY HH:MM. Carregar estas variáveis na estrutura
    "Industry" em dateScheduled e timeScheduled.

    Obter o número completo do relatório sem formato e deste número, armazenar
    em uma variável inteira. Usar este número para salvar um novo nó da árvore.
    Desta forma, na exibição LNR, o sistema considerará a ordenação simétrica
    pela data e horário da geração do relatório para a caixa de listagem 7.

    O sistema deve perguntar se o usuário deseja salvar uma cópia do relatório com
    outro nome, se o usuário aceitar, então abrir o janela de diretórios para
    escrever o nome do relatório. Assim teremos um relatório seguindo o formato
    padrão para ser aberto no software e uma cópia pro usuário abrir em qualquer lugar.
    A cópia também poderá criar com a extensão .W, podendo assim ser aberta pelo software,
    no entanto, se o usuário optar por escolher a opção .TXT, o relatório não mais poderá
    ser aberto no software até que converta sua extensão para .W

    Durante as administrações de tratamento na tela de sessão, o software deve seguir a mesma
    etapa para formatação do nome de relatório, lendo a data e horário da geração dos mesmos
    e exibição em formato padrão na caixa de listagem.

    Para Relatórios MTR e CDF:

    * Criar um vetor de índices de Industry
    * Cada índice desse vetor será um índice de um item MTR ou CDF
    * No conteúdo de cada índice terá o índice da Industry
    * No conteúdo da Industry recuperada terá um ponteiro para lista
    * Em cada nó dessa lista terão valores de data/hora para recuperação de relatório
    * Em cada nó dessa lista terá o ID, sendo o índice do item MTR ou CDF
*/

typedef struct
{
    CHAR personName[100];
    CHAR companyName[100];
    CHAR timeScheduled[20];
    CHAR dateScheduled[20];
    CHAR dataCNPJ[50];
} Industry;

typedef struct
{
    CHAR companyName[100];
    CHAR financial[50];
    CHAR createdWaste[50];
    CHAR isDanger[5];
    CHAR ammountCreated[50];
    CHAR destScheduled[50];
    CHAR treatedScheduled[50];
} Manager;

Manager *manager;
Industry IndData[20];
int IndStatus[20];

Tree *dat;
Node *reports;
bool updatedata = false;
int lastindex = 0;
int companyid = 0;
int reportid = 0;

int itemPos = 0, itemIndex = 0;
bool isReport = false;

ULONG_PTR EnableVisualStyles(VOID)
{
    TCHAR dir[MAX_PATH];
    ULONG_PTR ulpActivationCookie = FALSE;
    ACTCTX actCtx =
    {
        sizeof(actCtx),
        ACTCTX_FLAG_RESOURCE_NAME_VALID
            | ACTCTX_FLAG_SET_PROCESS_DEFAULT
            | ACTCTX_FLAG_ASSEMBLY_DIRECTORY_VALID,
        TEXT("shell32.dll"), 0, 0, dir, (LPCTSTR)124
    };
    UINT cch = GetSystemDirectory(dir, sizeof(dir) / sizeof(*dir));
    if (cch >= sizeof(dir) / sizeof(*dir)) { return FALSE; /*shouldn't happen*/ }
    dir[cch] = TEXT('\0');
    ActivateActCtx(CreateActCtx(&actCtx), &ulpActivationCookie);
    return ulpActivationCookie;
}

bool GetDataField(char *dst, char *src){
    int i;
    for(i = 0; src[i] != ':'; i++);
    i += 2;
    int j;
    for(j = 0; src[i] != '\n'; i++, j++)
        dst[j] = src[i];
    dst[j] = 0;
    return true;
}

int GetLastId(char *file){
    char lastid[5] = {0};
    FILE *arqid = fopen(file, "r");
    fgets(lastid, 5, arqid);
    fclose(arqid);
    return atoi(lastid) - 1;
}

void AddOrdernedList(Tree *t1){
    if(!empty(t1)){
		AddOrdernedList(t1->sae);

        TCHAR buff[200];
        HWND currentList;

        switch(IndStatus[t1->index]){
            case 0:
                StringCbPrintf (buff, ARRAYSIZE(buff), TEXT("%s | %s"),
                    IndData[t1->index].companyName, IndData[t1->index].timeScheduled);

                if(!strcmp(IndData[t1->index].dateScheduled, "Segunda-feira"))
                    currentList = hwndList1;
                else if(!strcmp(IndData[t1->index].dateScheduled, "Terça-feira"))
                        currentList = hwndList2;
                else if(!strcmp(IndData[t1->index].dateScheduled, "Quarta-feira"))
                        currentList = hwndList3;
                else if(!strcmp(IndData[t1->index].dateScheduled, "Quinta-feira"))
                        currentList = hwndList4;
                else if(!strcmp(IndData[t1->index].dateScheduled, "Sexta-feira"))
                        currentList = hwndList5;
            break;
            case 1:
                StringCbPrintf (buff, ARRAYSIZE(buff), TEXT("%s | %s | Não há relatório"),
                    IndData[t1->index].companyName, IndData[t1->index].timeScheduled);
                currentList = hwndList6;
            break;
            case 2:
                StringCbPrintf (buff, ARRAYSIZE(buff), TEXT("%s | %s | Visualizar relatório"),
                    IndData[t1->index].companyName, IndData[t1->index].timeScheduled);
                currentList = hwndList6;
            break;
        }

        int pos = (int)SendMessage(currentList, LB_ADDSTRING, 0,(LPARAM) buff);
        SendMessage(currentList, LB_SETITEMDATA, pos, (LPARAM) t1->index);

		AddOrdernedList(t1->sad);
	}

    SetFocus(hwndList1);
}

bool SetDataFile(char archive[], char data[]){

}

bool GetFileStatus(char archive[], char data[]){
    FILE *arch = fopen(archive, "r");
    if(arch != NULL){
        fgets(data, sizeof(data), arch);
        fclose(arch);
    }
    return arch != NULL;
}

void LoadClass(WNDCLASSEX classType, char *ClassName, char *MenuName, HINSTANCE instance, int system){
    switch(system){
        case 0: classType.lpfnWndProc = LoginManager;
                break;
        case 1: classType.lpfnWndProc = SignManager;
                break;
        case 2: classType.lpfnWndProc = CostumerManager;
                break;
        case 3: classType.lpfnWndProc = WasteManager;
                break;
        case 4: classType.lpfnWndProc = DataManager;
                break;
        case 5: classType.lpfnWndProc = SessionManager;
                break;
        case 6: classType.lpfnWndProc = MTRManager;
                break;
        case 7: classType.lpfnWndProc = CDFManager;
                break;
        case 8: classType.lpfnWndProc = FDSRManager;
                break;
    }

    classType.lpszClassName  = ClassName;
    classType.lpszMenuName   = MenuName;
    classType.hInstance      = instance;
    classType.style    = CS_DBLCLKS;
    classType.hIcon    = LoadIcon (NULL, IDI_APPLICATION);
    classType.hIconSm  = LoadIcon (NULL, IDI_APPLICATION);
    classType.hCursor  = LoadCursor (NULL, IDC_ARROW);
    classType.cbSize   = sizeof (WNDCLASSEX);
    classType.hbrBackground  = (HBRUSH) COLOR_WINDOW+3;
    classType.cbClsExtra  = 0;
    classType.cbWndExtra  = 0;
    RegisterClassEx (&classType);
}

int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
    MSG messages;
    instance1 = hThisInstance;
    //instance2 = hPrevInstance;

    EnableVisualStyles();

    hFont = CreateFont(24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                     CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Times New Roman");

    LoadClass(loginClass, classNameLogin, NULL, instance1, 0);
    LoadClass(signClass, classNameSign, NULL, instance2, 1);
    LoadClass(costumerClass, classNameCost, NULL, instance3, 2);
    LoadClass(wasteClass, classNameWaste, NULL, instance4, 3);
    LoadClass(managerClass, classNameMngr, NULL, instance5, 4);
    LoadClass(sessionClass, classNameSess, classNameMenu, instance6, 5);
    LoadClass(MTRClass, classNameMTR, NULL, instance7, 6);
    LoadClass(CDFClass, classNameCDF, NULL, instance8, 7);
    LoadClass(FDSRClass, classNameFDSR, NULL, instance9, 8);

    LoginScrn = CreateWindowEx (WS_EX_CLIENTEDGE, classNameLogin, loginTitle, WS_OVERLAPPEDWINDOW | WS_BORDER, CW_USEDEFAULT, CW_USEDEFAULT, LOGINW_X, LOGINW_Y,
           HWND_DESKTOP, NULL, instance1, NULL);
    ShowWindow (LoginScrn, SW_SHOWNORMAL);

    while (GetMessage (&messages, NULL, 0, 0) || parentstate || statelogin)
    {
        if (
                !IsDialogMessage(LoginScrn,     &messages)  &&
                !IsDialogMessage(SignScrn,      &messages)  &&
                !IsDialogMessage(CostumerScrn,  &messages)  &&
                !IsDialogMessage(WasteScrn,     &messages)  &&
                !IsDialogMessage(ManagerScrn,   &messages)  &&
                !IsDialogMessage(MTRScrn,       &messages)  &&
                !IsDialogMessage(CDFScrn,       &messages)  &&
                !IsDialogMessage(FDSRScrn,      &messages)
            )
        {
            TranslateMessage(&messages);    // Translate virtual-key messages into character messages
            DispatchMessage(&messages);     // Send message to WindowProcedure
        }
    }

    return messages.wParam;
}


LRESULT CALLBACK LoginManager (HWND app, UINT message, WPARAM action1, LPARAM action2)
{
    switch (message)                  /* handle the messages */
    {
        case WM_CREATE: {
            parentstate = true;
            statelogin = true;
            lEdit1 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 50, 60, 270, 40,
			app, (HMENU) TEXT_USER, NULL, NULL);
			lEdit2 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_PASSWORD | WS_TABSTOP, 50, 110, 270, 40,
			app, (HMENU) TEXT_PASS, NULL, NULL);
			CreateWindow(TEXT("BUTTON"), TEXT("Cadastrar"), WS_VISIBLE | WS_CHILD | WS_BORDER, 50, 170, 125, 40 ,
			app, (HMENU) SIGN, NULL, NULL);

			CreateWindow(TEXT("BUTTON"), TEXT("Entrar"), WS_VISIBLE | WS_CHILD | WS_BORDER, 195, 170, 125, 40 ,
			app, (HMENU) LOGGIN, NULL, NULL);

			SendMessage(lEdit1, WM_SETFONT, (WPARAM)hFont, TRUE);
			SendMessage(lEdit1, EM_SETCUEBANNER, false, (LPARAM)L"Digite seu usuario ...");

            SendMessage(lEdit2, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(lEdit2, EM_SETCUEBANNER, false, (LPARAM)L"Digite sua senha ...");

            SYSTEMTIME lt;

            GetLocalTime(&lt);

            printf(" The local time is: %d/%d/%d %d %02d:%02d\n", lt.wDay, lt.wMonth, lt.wYear, lt.wDayOfWeek, lt.wHour, lt.wMinute);


        }
        break;
        /*
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(app, &ps);

                // All painting occurs here, between BeginPaint and EndPaint.

            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+2));
            EndPaint(app, &ps);
        }
        break;
        */
        case WM_COMMAND:
            switch(action1){
                case SIGN:
                    if(!childstate){
                        SignScrn = CreateWindow (classNameSign, signTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, SIGNW_X, SIGNW_Y,
                        app, NULL, NULL, NULL);
                        ShowWindow (SignScrn, SW_SHOWNORMAL);
                    }
                break;
                case LOGGIN: {
                    int size_user = GetWindowTextLength(lEdit1);
                    int size_pass = GetWindowTextLength(lEdit2);
                    if(!size_user){
                        MessageBox(LoginScrn, "O 1ª campo de 'usuário' não foi preenchido!", "", MB_OKCANCEL);
                        break;
                    }else{
                        if(!size_pass){
                            MessageBox(LoginScrn, "O 2ª campo de 'senha' não foi preenchido!", "", MB_OKCANCEL);
                            break;
                        }
                    }
                    CHAR *get_text_user = (CHAR*) malloc(size_user * sizeof(CHAR));
                    CHAR *get_text_pass = (CHAR*) malloc(size_pass * sizeof(CHAR));
                    GetWindowText(lEdit1, get_text_user, size_user+1);
                    GetWindowText(lEdit2, get_text_pass, size_pass+1);

                    FILE* filer = fopen(FILE_SIGN, "r");

                    char buffer[100];
                    char user[100];
                    char pass[100];
                    int countline = 0;
                    bool userfound = false;
                    bool passfound = false;

                    if (filer != NULL) {
                        while (fgets(buffer, sizeof(buffer), filer) != NULL) {

                            // ler ID 1 (usuario) e 3 (senha)
                            if(countline == 1){
                                int i;
                                for(i = 0; buffer[i] != ':'; i++);
                                i += 2;
                                int j;
                                for(j = 0; buffer[i] != '\n'; i++, j++)
                                    user[j] = buffer[i];

                                user[j] = 0;
                                userfound = !strcmp(user, get_text_user);
                                printf("USER: %s\n", user);
                            }

                            if(countline == 3){
                                int i;
                                for(i = 0; buffer[i] != ':'; i++);
                                i += 2;
                                int j;
                                for(j = 0; buffer[i] != '\n'; i++, j++)
                                    pass[j] = buffer[i];

                                pass[j] = 0;
                                size_t sizepass = strlen(get_text_pass);
                                char key[ ] = "chave_sistema";
                                char hashpass[100] = {0};
                                char newvalue[100] = {0};
                                int x = 0;

                                for(int i = 0, j = 0; i < sizepass; i++, j++){
                                    if(j == strlen(key)){
                                        j = 0;
                                    }else{
                                        int val = get_text_pass[i] ^ key[j];
                                        itoa(val, newvalue, 10);
                                        for(int y = 0; y < strlen(newvalue); y++, x++)
                                            hashpass[x] = newvalue[y];
                                    }
                                }
                                passfound = !strcmp(pass, hashpass);
                                if(userfound && passfound || userfound){
                                    printf("HASH_FIELD: %s\n", hashpass);
                                    printf("HASH_FILE: %s\n", pass);
                                    printf("REAL_PASS: %s\n", get_text_pass);
                                    break;
                                }

                            }

                            countline = (countline == 4) ? 0 : countline + 1;
                        }

                    }else{
                        MessageBox(LoginScrn, "Não há cadastro de funcionários!", "Error", MB_OKCANCEL);
                        break;
                    }

                    if(userfound){
                        if(!passfound){
                            MessageBox(LoginScrn, "A senha está incorreta!", "Error", MB_OKCANCEL);
                        }else{
                            SessionScrn = CreateWindowEx (0, classNameSess, sessionTitle, WS_OVERLAPPEDWINDOW | WS_BORDER | WS_VSCROLL | WS_HSCROLL, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                            HWND_DESKTOP, NULL, instance6, NULL);
                            ShowWindow (SessionScrn, SW_MAXIMIZE);

                            statelogin = true;

                            free(get_text_user);
                            free(get_text_pass);
                            DestroyWindow(LoginScrn);
                            break;
                        }
                    }else{
                        MessageBox(LoginScrn, "Usuário não encontrado!", "Error", MB_OKCANCEL);
                    }

                    free(get_text_user);
                    free(get_text_pass);
                    break;
                }
            }
        break;
        case WM_DESTROY:
            parentstate = false;
            PostQuitMessage (0);
            break;
        case WM_CLOSE:
            if (MessageBox(LoginScrn, "Deseja realmente sair?", "Login Exit", MB_OKCANCEL) == IDOK)
            {
                DestroyWindow(LoginScrn);
                statelogin = false;
            }
            // Else: User canceled. Do nothing.
            return 0;
        case WM_GETMINMAXINFO: {
            MINMAXINFO* info = (MINMAXINFO*)action2;
            info->ptMinTrackSize.x = LOGINW_X;
            info->ptMinTrackSize.y = LOGINW_Y;
            info->ptMaxTrackSize.x = LOGINW_X;
            info->ptMaxTrackSize.y = LOGINW_Y;
            break;
        }
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (app, message, action1, action2);
    }

    return 0;
}

LRESULT CALLBACK SignManager (HWND app, UINT message, WPARAM action1, LPARAM action2)
{

    switch (message)                  /* handle the messages */
    {
        case WM_CREATE:
            //childstate = true;
            radiopressed = false;

            sEdit1 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 50, 55, 320, 40,
			app, (HMENU) NEW_USER, NULL, NULL);
			sEdit2 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 50, 105, 320, 40,
			app, (HMENU) EMAIL, NULL, NULL);
			sEdit3 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_PASSWORD | WS_TABSTOP, 50, 155, 320, 40,
			app, (HMENU) NEW_PASS, NULL, NULL);
			sEdit4 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_PASSWORD | WS_TABSTOP, 50, 205, 320, 40,
			app, (HMENU) PASS_CONFIRM, NULL, NULL);
			CreateWindow(TEXT("STATIC"), TEXT("Selecione sua ocupação"), WS_VISIBLE | WS_CHILD, 50, 255, 320, 20 ,
			app, (HMENU) NULL, NULL, NULL);
            CreateWindow(TEXT("BUTTON"), TEXT("Empresário"), WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 50, 276, 125, 20 ,
			app, (HMENU) RADIOB1, NULL, NULL);
			CreateWindow(TEXT("BUTTON"), TEXT("Gerente"), WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 50, 296, 125, 20 ,
			app, (HMENU) RADIOB2, NULL, NULL);
			CreateWindow(TEXT("BUTTON"), TEXT("Funcionário"), WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 50, 316, 125, 20 ,
			app, (HMENU) RADIOB3, NULL, NULL);
			CreateWindow(TEXT("BUTTON"), TEXT("Desenvolvedor"), WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 50, 336, 125, 20 ,
			app, (HMENU) RADIOB4, NULL, NULL);

			CreateWindow(TEXT("BUTTON"), TEXT("Cadastrar"), WS_VISIBLE | WS_CHILD | WS_BORDER, 245, 316, 125, 40 ,
			app, (HMENU) REGISTER, NULL, NULL);

            SendMessage(sEdit1, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(sEdit1, EM_SETCUEBANNER, false, (LPARAM)L"Digite seu usuario ...");

            SendMessage(sEdit2, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(sEdit2, EM_SETCUEBANNER, false, (LPARAM)L"Digite seu email ...");

            SendMessage(sEdit3, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(sEdit3, EM_SETCUEBANNER, false, (LPARAM)L"Crie sua senha ...");

            SendMessage(sEdit4, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(sEdit4, EM_SETCUEBANNER, false, (LPARAM)L"Confirme sua senha ...");

        break;
        case WM_COMMAND:
            switch(action1){
                case REGISTER: {
                    int size_new_user = GetWindowTextLength(sEdit1);
                    int size_email = GetWindowTextLength(sEdit2);
                    int size_new_pass = GetWindowTextLength(sEdit3);
                    int size_pass_confirm = GetWindowTextLength(sEdit4);
                    if(!size_new_user){
                        MessageBox(SignScrn, "O 1ª campo de 'usuário' não foi preenchido!", "", MB_OKCANCEL);
                        break;
                    }else
                        if(!size_email){
                            MessageBox(SignScrn, "O 2ª campo de 'email' não foi preenchido!", "", MB_OKCANCEL);
                            break;
                        }else if(!size_new_pass){
                            MessageBox(SignScrn, "O 3ª campo de 'senha' não foi preenchido!", "", MB_OKCANCEL);
                            break;
                        }else if(!size_pass_confirm){
                            MessageBox(SignScrn, "O 4ª campo de 'confirmação' não foi preenchido!", "", MB_OKCANCEL);
                            break;
                        }else if(!radiopressed){
                            MessageBox(SignScrn, "Nenhuma ocupação foi selecionada!", "", MB_OKCANCEL);
                            break;
                        }

                    CHAR *get_new_user = (CHAR*) malloc(size_new_user * sizeof(CHAR));
                    CHAR *get_email = (CHAR*) malloc(size_email * sizeof(CHAR));
                    CHAR *get_new_pass = (CHAR*) malloc(size_new_pass * sizeof(CHAR));
                    CHAR *get_pass_confirm = (CHAR*) malloc(size_pass_confirm * sizeof(CHAR));
                    GetWindowText(sEdit1, get_new_user, size_new_user+1);
                    GetWindowText(sEdit2, get_email, size_email+1);
                    GetWindowText(sEdit3, get_new_pass, size_new_pass+1);
                    GetWindowText(sEdit4, get_pass_confirm, size_pass_confirm+1);

                    if(strcmp(get_new_pass, get_pass_confirm)){
                        MessageBox(SignScrn, "As senhas não se conferem!", "", MB_OKCANCEL);
                        free(get_new_user);
                        free(get_email);
                        free(get_new_pass);
                        free(get_pass_confirm);
                        break;
                    }

                    int size_data = size_new_user + size_email + size_new_pass + size_pass_confirm + 120;
                    CHAR concat[size_data] = {0};

                    char num_id[10];
                    FILE *fileid = fopen(FILE_ID_SIGN, "r");
                    fgets(num_id, sizeof(num_id), fileid);
                    fclose(fileid);

                    size_t sizepass = strlen(get_new_pass);
                    char key[ ] = "chave_sistema";
                    char hashpass[100] = {0};
                    char newvalue[100] = {0};
                    int x = 0;

                    for(int i = 0, j = 0; i < sizepass; i++, j++){
                        if(j == strlen(key)){
                            j = 0;
                        }else{
                            int val = get_new_pass[i] ^ key[j];
                            itoa(val, newvalue, 10);
                            for(int y = 0; y < strlen(newvalue); y++, x++)
                                hashpass[x] = newvalue[y];
                        }
                    }


                    StringCchCat(concat, size_data, byteb);
                    StringCchCat(concat, size_data, "ID: ");
                    StringCchCat(concat, size_data, num_id);
                    StringCchCat(concat, size_data, "\n");

                    StringCchCat(concat, size_data, byteb);
                    StringCchCat(concat, size_data, "Usuário: ");
                    StringCchCat(concat, size_data, get_new_user);
                    StringCchCat(concat, size_data, "\n");
                    StringCchCat(concat, size_data, byteb);
                    StringCchCat(concat, size_data, "Email: ");
                    StringCchCat(concat, size_data, get_email);
                    StringCchCat(concat, size_data, "\n");
                    StringCchCat(concat, size_data, byteb);
                    StringCchCat(concat, size_data, "Senha: ");
                    StringCchCat(concat, size_data, hashpass);
                    StringCchCat(concat, size_data, "\n");
                    StringCchCat(concat, size_data, byteb);
                    StringCchCat(concat, size_data, "Ocupação: ");
                    StringCchCat(concat, size_data, radiotext);
                    StringCchCat(concat, size_data, "\n");

                    if(MessageBox(SignScrn, concat, "Confirme seus dados", MB_OKCANCEL) == IDOK){

                        FILE *filew = fopen(FILE_SIGN, "a");
                        fputs(concat, filew);
                        fclose(filew);

                        int id = atoi(num_id) + 1;
                        itoa(id, num_id, 10);

                        FILE *fileidw = fopen(FILE_ID_SIGN, "w");
                        fputs(num_id, fileidw);
                        fclose(fileidw);

                        MessageBox(SignScrn, "Dados cadastrados com sucesso!", "", MB_OKCANCEL);

                        free(get_new_user);
                        free(get_email);
                        free(get_new_pass);
                        free(get_pass_confirm);
                        DestroyWindow(SignScrn);
                        break;
                    }

                    free(get_new_user);
                    free(get_email);
                    free(get_new_pass);
                    free(get_pass_confirm);
                    break;
                }
                case RADIOB1:
                    radiopressed = true;
                    radiotext = "Empresário";
                break;
                case RADIOB2:
                    radiopressed = true;
                    radiotext = "Gerente";
                break;
                case RADIOB3:
                    radiopressed = true;
                    radiotext = "Funcionário";
                break;
                case RADIOB4:
                    radiopressed = true;
                    radiotext = "Desenvolvedor";
                break;
            }

        break;
        case WM_DESTROY:
            //childstate = false;
            PostQuitMessage (0);
            break;
        case WM_CLOSE:
            DestroyWindow(SignScrn);
            parentstate = true;
            return 0;
        case WM_GETMINMAXINFO: {
            MINMAXINFO* info = (MINMAXINFO*)action2;
            info->ptMinTrackSize.x = SIGNW_X;
            info->ptMinTrackSize.y = SIGNW_Y;
            info->ptMaxTrackSize.x = SIGNW_X;
            info->ptMaxTrackSize.y = SIGNW_Y;
            break;
        }
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (app, message, action1, action2);
    }

    return 0;
}


LRESULT CALLBACK CostumerManager (HWND app, UINT message, WPARAM action1, LPARAM action2)
{
    switch (message)                  /* handle the messages */
    {
        case WM_CREATE:
           // parentstate = true;
           CreateWindow(TEXT("BUTTON"), TEXT("Dados Pessoais:"), WS_VISIBLE | WS_CHILD | WS_BORDER | BS_GROUPBOX, 5, 5, 350, 200,
           app, NULL, NULL, NULL);
           CreateWindow(TEXT("BUTTON"), TEXT("Dados Empresariais:"), WS_VISIBLE | WS_CHILD | WS_BORDER | BS_GROUPBOX, 5, 210, 350, 285,
           app, NULL, NULL, NULL);
           CreateWindow(TEXT("BUTTON"), TEXT("Informações de Resíduos:"), WS_VISIBLE | WS_CHILD | WS_BORDER | BS_GROUPBOX, 360, 5, 280, 260,
           app, NULL, NULL, NULL);
           CreateWindow(TEXT("BUTTON"), TEXT("Agendamento e Cargas:"), WS_VISIBLE | WS_CHILD | WS_BORDER | BS_GROUPBOX, 360, 270, 280, 200,
           app, NULL, NULL, NULL);

            cEdit[0] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 10, 25, 340, 40,
			app, (HMENU) COSTUMER_NAME, NULL, NULL);
			cEdit[1] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 10, 68, 340, 40,
			app, (HMENU) COSTUMER_EMAIL, NULL, NULL);
			cEdit[2] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP | ES_NUMBER | ES_SYSTEM_REQUIRED, 10, 111, 168, 40,
			app, (HMENU) PERSONAL_PHONE, NULL, NULL);
			cEdit[3] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP | ES_NUMBER | ES_SYSTEM_REQUIRED, 182, 111, 168, 40,
			app, (HMENU) WORK_PHONE, NULL, NULL);
			cEdit[4] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 10, 154, 340, 40,
			app, (HMENU) PERSONAL_ADDR, NULL, NULL);

            cEdit[5] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 10, 230, 340, 40,
			app, (HMENU) COMPANY_NAME, NULL, NULL);
			cEdit[6] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 10, 273, 340, 40,
			app, (HMENU) FANTASY_NAME, NULL, NULL);
			cEdit[7] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 10, 316, 340, 40,
			app, (HMENU) COMPANY_ADDR, NULL, NULL);
			cEdit[8] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 10, 359, 188, 40,
			app, (HMENU) CNPJ_NUMBER, NULL, NULL);
			cEdit[9] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 200, 359, 150, 40,
			app, (HMENU) OPENING_DATE, NULL, NULL);
			cEdit[10] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 10, 402, 340, 40,
			app, (HMENU) COMPANY_ATIVITY, NULL, NULL);
			cEdit[11] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 10, 445, 340, 40,
			app, (HMENU) ACTION_FORMS, NULL, NULL);

            cEdit[12] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 365, 25, 270, 40,
			app, (HMENU) WASTE_CLASS, NULL, NULL);
			cEdit[13] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 365, 68, 270, 40,
			app, (HMENU) WASTE_COUNT, NULL, NULL);
			cEdit[14] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 365, 111, 270, 40,
			app, (HMENU) WASTE_DANGER, NULL, NULL);
			CreateWindow(TEXT("STATIC"), TEXT("Considerações Básicas:"), WS_VISIBLE | WS_CHILD, 365, 154, 270, 20, app, (HMENU) NULL, NULL, NULL);
			cEdit[15] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER |  ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL | WS_TABSTOP | ES_WANTRETURN, 365, 177, 270, 80,
			app, (HMENU) WASTE_DESCR, NULL, NULL);

            cEdit[16] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("BUTTON"), TEXT("Escolha o dia"), WS_VISIBLE | WS_CHILD | WS_BORDER | BS_SPLITBUTTON | WS_TABSTOP, 365, 290, 132, 40,
			app, (HMENU) COLLECT_DATE, NULL, NULL);
			cEdit[17] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 500, 290, 134, 40,
			app, (HMENU) COLLECT_HOUR, NULL, NULL);
			cEdit[18] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 365, 333, 270, 40,
			app, (HMENU) TRUCK_MODEL, NULL, NULL);
			cEdit[19] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP | ES_NUMBER | ES_SYSTEM_REQUIRED, 365, 376, 152, 40,
			app, (HMENU) TRUCK_SIZE, NULL, NULL);
			CreateWindow(TEXT("STATIC"), TEXT("quilos"), WS_VISIBLE | WS_CHILD, 520, 385, 40, 20, app, (HMENU) NULL, NULL, NULL);
			cEdit[20] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP | ES_NUMBER | ES_SYSTEM_REQUIRED, 365, 419, 82, 40,
			app, (HMENU) TRAVEL_TIME, NULL, NULL);
			CreateWindow(TEXT("STATIC"), TEXT("mins"), WS_VISIBLE | WS_CHILD, 450, 429, 40, 20, app, (HMENU) NULL, NULL, NULL);
            cEdit[21] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP | ES_NUMBER | ES_SYSTEM_REQUIRED, 495, 419, 67, 40,
			app, (HMENU) TRUCK_COUNTS, NULL, NULL);
			CreateWindow(TEXT("STATIC"), TEXT("caminhões"), WS_VISIBLE | WS_CHILD, 567, 429, 70, 20, app, (HMENU) NULL, NULL, NULL);

			for(int i = 0; i < 22; i++){
                if(i != 16) SendMessage(cEdit[i], WM_SETFONT, (WPARAM)hFont, TRUE);
                SendMessage(cEdit[i], EM_SETCUEBANNER, false, (LPARAM)TEXT(cBanner_cEdit[i]));
			}

            CreateWindow(TEXT("BUTTON"), TEXT("Salvar"), WS_VISIBLE | WS_CHILD | WS_BORDER, 515, 472, 125, 30 ,
			app, (HMENU) COSTUMER_SAVE, NULL, NULL);

        break;
        case WM_COMMAND:
            if(action1 == SELECT_MONTH){
                GetMenuStringA(hSplitMenu7, SEG_OP, textdate, 20, NULL);
                SetWindowText(cEdit[16], textdate);
                dateselected = true;
                break;
            }else if(action1 >= SEG_OP && action1 <= SEX_OP){
                GetMenuStringA(hSplitMenu7, action1, textdate, 20, NULL);
                SetWindowText(cEdit[16], textdate);
                dateselected = true;
                break;
            }
            switch(action1){
                case COSTUMER_SAVE: {

                    isupdate = true;
                    int costumer_data_size[22];
                    int total_size;
                    CHAR *get_costumer_data[22];
                    CHAR *concat = (CHAR*) malloc(500 * sizeof(CHAR));
                    concat[0] = {0};
                    CHAR concat1[50] = {0};
                    int current_index = 0;
                    bool errordata = false;


                    for(int i = 0; i < 22; i++){
                        costumer_data_size[i] = GetWindowTextLength(cEdit[i]);

                        if(!costumer_data_size[i]){
                            StringCchCat(concat1, 50, "O campo '");
                            StringCchCat(concat1, 50, cBanner_cEdit1[i]);
                            StringCchCat(concat1, 50, "' ");
                            StringCchCat(concat1, 50, " está vazio!");
                            MessageBox(CostumerScrn, concat1, "Erro de cadastro", MB_OKCANCEL);
                            errordata = true;
                            current_index = i;
                            break;
                        }
                        if(!dateselected){
                            MessageBox(CostumerScrn, "O campo de semana não foi selecionado!", "Erro de cadastro", MB_OKCANCEL);
                            errordata = true;
                            current_index = i;
                            break;
                        }

                            get_costumer_data[i] = (CHAR*) malloc(costumer_data_size[i] * sizeof(CHAR));
                            GetWindowText(cEdit[i], get_costumer_data[i], costumer_data_size[i]+1);
                            total_size += costumer_data_size[i];
                    }

                    if(errordata){
                        for(int j = 0; j < current_index; j++)
                            free(get_costumer_data[j]);

                        free(concat);
                        break;
                    }
                    concat = (CHAR*) realloc(concat, total_size);

                    char num_id[10];
                    FILE *fileid = fopen(FILE_ID_COSTUMER, "r");
                    fgets(num_id, sizeof(num_id), fileid);
                    fclose(fileid);

                    StringCchCat(concat, total_size, byteb);
                    StringCchCat(concat, total_size, "ID: ");
                    StringCchCat(concat, total_size, num_id);
                    StringCchCat(concat, total_size, "\n");

                    for(int i = 0; i < 22; i++){
                        StringCchCat(concat, total_size, byteb);
                        StringCchCat(concat, total_size, cBanner_cEdit1[i]);
                        StringCchCat(concat, total_size, " : ");
                        StringCchCat(concat, total_size, get_costumer_data[i]);
                        StringCchCat(concat, total_size, "\n");
                    }

                    if(MessageBox(CostumerScrn, concat, "Confirmação de dados", MB_OKCANCEL) == IDOK){

                        FILE *filew = fopen(FILE_COSTUMER, "a");
                        fputs(concat, filew);
                        fclose(filew);

                        int id = atoi(num_id) + 1;
                        itoa(id, num_id, 10);

                        FILE *fileidw = fopen(FILE_ID_COSTUMER, "w");
                        fputs(num_id, fileidw);
                        fclose(fileidw);

                        CHAR sysconfirm[50] = {0};
                        StringCchCat(sysconfirm, 50, FILE_STAT_COMPANY);
                        StringCchCat(sysconfirm, 50, num_id);
                        StringCchCat(sysconfirm, 50, EXTENSION);

                        FILE *filesys = fopen(sysconfirm, "w");
                        fputs("0", filesys);
                        fclose(filesys);


                        MessageBox(CostumerScrn, "Cadastro de clientes realizado com sucesso!", "Sucesso", MB_OKCANCEL);
                        for(int j = 0; j < 22; j++)
                            free(get_costumer_data[j]);
                    }

                    break;
                }
            }
        break;
        case WM_NOTIFY:
            switch (((LPNMHDR)action2)->code) {
                case BCN_DROPDOWN:
                        NMBCHOTITEM* pDropDown = (NMBCHOTITEM*)action2;
                        if (pDropDown->hdr.hwndFrom == GetDlgItem(app, COLLECT_DATE))
                        {

                            hSplitMenu7 = CreatePopupMenu();

                            for(int i = 0; i < 5; i++)
                                AppendMenu(hSplitMenu7, MF_BYPOSITION, SEG_OP+i, days[i]);

                            RECT Rect;
                            GetWindowRect(cEdit[16], &Rect);

                            TrackPopupMenu(hSplitMenu7, TPM_LEFTALIGN | TPM_TOPALIGN, Rect.left, Rect.top+30, 0, app, NULL);

                            return TRUE;
                        }
                        break;
                    }
                return FALSE;
        case WM_DESTROY:
            //parentstate = false;
            PostQuitMessage (0);
            break;
        case WM_CLOSE:
            DestroyWindow(CostumerScrn);
            return 0;
        case WM_GETMINMAXINFO: {
            MINMAXINFO* info = (MINMAXINFO*)action2;
            info->ptMinTrackSize.x = COSTW_X;
            info->ptMinTrackSize.y = COSTW_Y;
            info->ptMaxTrackSize.x = COSTW_X;
            info->ptMaxTrackSize.y = COSTW_Y;
            break;
        }
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (app, message, action1, action2);
    }

    return 0;
}


LRESULT CALLBACK WasteManager (HWND app, UINT message, WPARAM action1, LPARAM action2)
{
    switch (message)                  /* handle the messages */
    {
        case WM_CREATE:
            compselected = false;
            monthselected = false;

            spltBtn1 = CreateWindow(TEXT("BUTTON"), TEXT("Escolha o mês"), WS_VISIBLE | WS_CHILD | WS_BORDER | BS_SPLITBUTTON, 5, 5, 130, 30 ,
			app, (HMENU) SELECT_MONTH, (HINSTANCE) action2, NULL);
			spltBtn2 = CreateWindow(TEXT("BUTTON"), TEXT("Escolha a empresa"), WS_VISIBLE | WS_CHILD | WS_BORDER | BS_SPLITBUTTON, 140, 5, 270, 30 ,
			app, (HMENU) SELECT_COMPANY, (HINSTANCE) action2, NULL);

			wEdit[0] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 5, 40, 405, 40,
			app, (HMENU) TREATED_WASTE, NULL, NULL);
			wEdit[1] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 5, 85, 202, 40,
			app, (HMENU) TREATED_DATE, NULL, NULL);
			wEdit[2] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 207, 85, 202, 40,
			app, (HMENU) TREATED_HOUR, NULL, NULL);
			wEdit[3] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 5, 130, 202, 40,
			app, (HMENU) WASTE_AMMOUNT, NULL, NULL);
			wEdit[4] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 207, 130, 202, 40,
			app, (HMENU) USED_TECNO, NULL, NULL);
			wEdit[5] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 5, 175, 405, 40,
			app, (HMENU) TREATED_COST, NULL, NULL);

			CreateWindow(TEXT("BUTTON"), TEXT("Registrar"), WS_VISIBLE | WS_CHILD | WS_BORDER, 270, 230, 125, 30 ,
			app, (HMENU) WASTE_SAVE, NULL, NULL);

			for(int i = 0; i < 6; i++){
                SendMessage(wEdit[i], WM_SETFONT, (WPARAM)hFont, TRUE);
                SendMessage(wEdit[i], EM_SETCUEBANNER, false, (LPARAM)TEXT(cBanner_wEdit[i]));
			}

        break;
        case WM_COMMAND:
            if(action1 == SELECT_MONTH){
                GetMenuStringA(hSplitMenu, JAN_OP, textmonth, 10, NULL);
                SetWindowText(spltBtn1, textmonth);
                monthselected = true;
            }else if(action1 >= 39 && action1 <= 49){
                GetMenuStringA(hSplitMenu, action1, textmonth, 10, NULL);
                SetWindowText(spltBtn1, textmonth);
                monthselected = true;
            }if(action1 >= COSTUMER_LIST && action1 <= COSTUMER_LIST+GetLastId(FILE_ID_COSTUMER)){
                GetMenuStringA(hSplitMenu4, action1, companyname1, 30, NULL);
                SetWindowText(spltBtn2, companyname1);
                compselected = true;
            }else{
                switch(action1){
                    case WASTE_SAVE:
                        int waste_data_size[6];
                        int total_size;
                        CHAR *get_waste_data[6];
                        CHAR *concat = (CHAR*) malloc(500 * sizeof(CHAR));
                        concat[0] = {0};
                        int current_index = 0;
                        bool errordata = false;

                        for(int i = 0; i < 6; i++){
                            waste_data_size[i] = GetWindowTextLength(wEdit[i]);
                            CHAR concat1[50] = {0};
                            if(!waste_data_size[i]){
                                StringCchCat(concat, 50, "O campo '");
                                StringCchCat(concat, 50, cBanner_wEdit1[i]);
                                StringCchCat(concat, 50, "' ");
                                StringCchCat(concat, 50, " está vazio!");
                                MessageBox(WasteScrn, concat1, "Erro de cadastro", MB_OKCANCEL);
                                errordata = true;
                                current_index = i;
                                break;
                            }
                            if(!compselected || !monthselected){
                                MessageBox(WasteScrn, "Algum dos campos de seleção não foram marcados!", "Erro de cadastro", MB_OKCANCEL);
                                errordata = true;
                                current_index = i;
                                break;
                            }

                            get_waste_data[i] = (CHAR*) malloc(waste_data_size[i] * sizeof(CHAR));
                            GetWindowText(wEdit[i], get_waste_data[i], waste_data_size[i]+1);
                            total_size += waste_data_size[i];

                        }

                        if(errordata){
                            for(int j = 0; j < current_index; j++)
                                free(get_waste_data[j]);

                            free(concat);
                            break;
                        }
                        concat = (CHAR*) realloc(concat, total_size);

                        char num_id[10];
                        FILE *fileid = fopen(FILE_ID_WASTE, "r");
                        fgets(num_id, sizeof(num_id), fileid);
                        fclose(fileid);

                        StringCchCat(concat, total_size, byteb);
                        StringCchCat(concat, total_size, "ID: ");
                        StringCchCat(concat, total_size, num_id);
                        StringCchCat(concat, total_size, "\n");

                        StringCchCat(concat, total_size, byteb);
                        StringCchCat(concat, total_size, "Mês selecionado");
                        StringCchCat(concat, total_size, " : ");
                        StringCchCat(concat, total_size, textmonth);
                        StringCchCat(concat, total_size, "\n");
                        StringCchCat(concat, total_size, byteb);
                        StringCchCat(concat, total_size, "Nome da empresa:");
                        StringCchCat(concat, total_size, " : ");
                        StringCchCat(concat, total_size, companyname1);
                        StringCchCat(concat, total_size, "\n");

                        for(int i = 0; i < 6; i++){
                            StringCchCat(concat, total_size, byteb);
                            StringCchCat(concat, total_size, cBanner_wEdit1[i]);
                            StringCchCat(concat, total_size, " : ");
                            StringCchCat(concat, total_size, get_waste_data[i]);
                            StringCchCat(concat, total_size, "\n");
                        }

                        if(MessageBox(WasteScrn, concat, "Confirmação de dados", MB_OKCANCEL) == IDOK){

                            FILE *filew = fopen(FILE_WASTE, "a");
                            fputs(concat, filew);
                            fclose(filew);

                            int id = atoi(num_id) + 1;
                            itoa(id, num_id, 10);

                            FILE *fileidw = fopen(FILE_ID_WASTE, "w");
                            fputs(num_id, fileidw);
                            fclose(fileidw);

                            MessageBox(WasteScrn, "Cadastro de resíduos efetuados!", "Sucesso", MB_OKCANCEL);
                            for(int j = 0; j < 6; j++)
                                free(get_waste_data[j]);

                            free(concat);
                            DestroyWindow(WasteScrn);
                        }

                    break;
                }
            }
        break;
        case WM_NOTIFY:
            switch (((LPNMHDR)action2)->code) {
                case BCN_DROPDOWN:
                        NMBCHOTITEM* pDropDown = (NMBCHOTITEM*)action2;
                        if (pDropDown->hdr.hwndFrom == GetDlgItem(app, SELECT_MONTH))
                        {

                            hSplitMenu = CreatePopupMenu();

                            for(int i = 0; i < 12; i++)
                                AppendMenu(hSplitMenu, MF_BYPOSITION, JAN_OP+i, months[i]);

                            RECT Rect;
                            GetWindowRect(spltBtn1, &Rect);

                            TrackPopupMenu(hSplitMenu, TPM_LEFTALIGN | TPM_TOPALIGN, Rect.left, Rect.top+30, 0, app, NULL);

                            return TRUE;
                        }else{
                            if (pDropDown->hdr.hwndFrom == GetDlgItem(app, SELECT_COMPANY))
                            {
                                FILE* filer = fopen(FILE_COSTUMER, "r");

                                char buffer[100];
                                char company[100];
                                int countline = 0;
                                int index = 0;

                                hSplitMenu4 = CreatePopupMenu();

                                if (filer != NULL) {
                                    // Ler cada linha do arquivo
                                    while (fgets(buffer, sizeof(buffer), filer) != NULL) {

                                        if(countline == 6){
                                            int i;
                                            for(i = 0; buffer[i] != ':'; i++);
                                            i += 2;
                                            int j;
                                            for(j = 0; buffer[i] != '\n'; i++, j++)
                                                company[j] = buffer[i];

                                            company[j] = 0;
                                            AppendMenu(hSplitMenu4, MF_BYPOSITION, COSTUMER_LIST+index, company);
                                            index++;
                                        }

                                        countline = (countline == 22) ? 0 : countline + 1;
                                    }
                                }else{
                                    MessageBox(FDSRScrn, "Por favor, cadastre um cliente!", "Error", MB_OKCANCEL);
                                    break;
                                }

                                fclose(filer);

                                RECT Rect;
                                GetWindowRect(spltBtn2, &Rect);
                                TrackPopupMenu(hSplitMenu4, TPM_LEFTALIGN | TPM_TOPALIGN, Rect.left, Rect.top+30, 0, app, NULL);
                            }
                        }
                        break;
                    }
                return FALSE;
        case WM_DESTROY:
            PostQuitMessage (0);
            break;
        case WM_CLOSE:
            DestroyWindow(WasteScrn);
            return 0;
        case WM_GETMINMAXINFO: {
            MINMAXINFO* info = (MINMAXINFO*)action2;
            info->ptMinTrackSize.x = WASTEW_X;
            info->ptMinTrackSize.y = WASTEW_Y;
            info->ptMaxTrackSize.x = WASTEW_X;
            info->ptMaxTrackSize.y = WASTEW_Y;
            break;
        }
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (app, message, action1, action2);
    }

    return 0;
}

LRESULT CALLBACK DataManager (HWND app, UINT message, WPARAM action1, LPARAM action2)
{

    switch (message)                  /* handle the messages */
    {
        case WM_CREATE:
            compselected2 = false;
            yesstate = false;

            mspltBtn1 = CreateWindow(TEXT("BUTTON"), TEXT("ID da empresa"), WS_VISIBLE | WS_CHILD | WS_BORDER | BS_SPLITBUTTON, 5, 5, 130, 30 ,
			app, (HMENU) ID_COMPANY, (HINSTANCE) action2, NULL);

			mEdit[0] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP | ES_SYSTEM_REQUIRED, 140, 5, 270, 30,
			app, (HMENU) FINANCIAL, NULL, NULL);
			mEdit[1] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 5, 40, 405, 30,
			app, (HMENU) CREATED_WASTE, NULL, NULL);
			CreateWindow(TEXT("STATIC"), TEXT("É perigoso?"), WS_VISIBLE | WS_CHILD, 5, 80, 100, 20, app, (HMENU) NULL, NULL, NULL);
			mEdit[2] = CreateWindow(TEXT("BUTTON"), TEXT("S/N"), WS_VISIBLE | WS_CHILD | WS_BORDER | BS_SPLITBUTTON, 110, 75, 50, 30 ,
			app, (HMENU) IS_DANGER, (HINSTANCE) action2, NULL);
			mEdit[3] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP | ES_NUMBER | ES_SYSTEM_REQUIRED, 165, 75, 245, 30,
			app, (HMENU) AMMOUNT_CREATED, NULL, NULL);
			mEdit[4] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 5, 110, 202, 30,
			app, (HMENU) DEST_SCHED, NULL, NULL);
			mEdit[5] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 207, 110, 202, 30,
			app, (HMENU) TREATED_SCHED, NULL, NULL);
			CreateWindow(TEXT("STATIC"), TEXT("Resultado de inspeccao (detalhes tecnicos):"), WS_VISIBLE | WS_CHILD, 5, 145, 405, 20, app, (HMENU) NULL, NULL, NULL);
			mEdit[6] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER |  ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL | WS_TABSTOP | ES_WANTRETURN, 5, 170, 405, 150,
			app, (HMENU) INSPECTION, NULL, NULL);

			CreateWindow(TEXT("BUTTON"), TEXT("Registrar"), WS_VISIBLE | WS_CHILD | WS_BORDER, 285, 330, 125, 30 ,
			app, (HMENU) MNG_BTN_REGIST, NULL, NULL);
			CreateWindow(TEXT("BUTTON"), TEXT("Atualizar"), WS_VISIBLE | WS_CHILD | WS_BORDER, 155, 330, 125, 30 ,
			app, (HMENU) MNG_BTN_UPDATE, NULL, NULL);
			CreateWindow(TEXT("BUTTON"), TEXT("Gerar relatório"), WS_VISIBLE | WS_CHILD | WS_BORDER, 25, 330, 125, 30 ,
			app, (HMENU) MNG_BTN_REPORT, NULL, NULL);


			for(int i = 0; i < 7; i++){
                if(i != 2){
                    SendMessage(mEdit[i], WM_SETFONT, (WPARAM)hFont, TRUE);
                    SendMessage(mEdit[i], EM_SETCUEBANNER, false, (LPARAM)TEXT(cBanner_mEdit[i]));
                }
			}

        break;
        case WM_COMMAND:
            if(action1 == IS_DANGER){
                GetMenuStringA(hSplitMenu2, YES_OP, yesorno, 5, NULL);
                SetWindowText(mEdit[2], yesorno);
                yesstate = true;
            }else if(action1 == 68 || action1 == 69){
                GetMenuStringA(hSplitMenu2, action1, yesorno, 5, NULL);
                SetWindowText(mEdit[2], yesorno);
                yesstate = true;
            }else if(action1 >= COSTUMER_LIST && action1 <= COSTUMER_LIST+GetLastId(FILE_ID_COSTUMER)){
                GetMenuStringA(hSplitMenu3, action1, companyname2, 30, NULL);
                SetWindowText(mspltBtn1, companyname2);
                compselected2 = true;
                if(updatedata){
                    int lastid = GetLastId(FILE_ID_MANAGER) + 1;
                    for(int i = 0; i < lastid; i++){
                        printf("STRUCT: %s\nSELECTED: %s\n", manager[i].companyName, companyname2);
                        if(!strcmp(companyname2, manager[i].companyName)){
                            SetWindowText(mEdit[0], manager[i].financial);
                            SetWindowText(mEdit[1], manager[i].createdWaste);
                            SetWindowText(mEdit[2], manager[i].isDanger);
                            SetWindowText(mEdit[3], manager[i].ammountCreated);
                            SetWindowText(mEdit[4], manager[i].destScheduled);
                            SetWindowText(mEdit[5], manager[i].treatedScheduled);
                            lastindex = i;
                            break;
                        }
                    }
                }
                companyid = action1 - COSTUMER_LIST;
                printf("ID COMPANY: %d\n", companyid);
            }
                switch(action1){

                    case MNG_BTN_REGIST: {
                        if(!updatedata){
                            int manager_data_size[7];
                            int total_size;
                            CHAR *get_manager_data[7];

                            int current_index;
                            bool errordata = false;

                            for(int i = 0; i < 6; i++){
                                if(i != 2){
                                    manager_data_size[i] = GetWindowTextLength(mEdit[i]);
                                    CHAR concat1[50] = {0};
                                    if(!manager_data_size[i]){
                                        //itoa(i+1, current_index, 10);
                                        StringCchCat(concat1, 50, "O campo '");
                                        StringCchCat(concat1, 50, cBanner_mEdit1[i]);
                                        StringCchCat(concat1, 50, "' ");
                                        StringCchCat(concat1, 50, " está vazio!");
                                        MessageBox(ManagerScrn, concat1, "Erro de cadastro", MB_OKCANCEL);
                                        current_index = i;
                                        errordata = true;
                                        break;
                                    }
                                    if(!compselected2 || !yesstate){
                                        MessageBox(ManagerScrn, "Algum dos campos de seleção não foram marcados!", "Erro de cadastro", MB_OKCANCEL);
                                        current_index = i;
                                        errordata = true;
                                        break;
                                    }

                                    get_manager_data[i] = (CHAR*) malloc(manager_data_size[i] * sizeof(CHAR));
                                    GetWindowText(mEdit[i], get_manager_data[i], manager_data_size[i]+1);
                                    total_size += manager_data_size[i];

                                }
                            }

                            if(errordata){
                                for(int j = 0; j < current_index; j++)
                                    if(j != 2)
                                        free(get_manager_data[j]);

                                break;
                            }

                            total_size += 500;
                            CHAR *concat = (CHAR*) malloc(total_size);
                            concat[0] = {0};

                            char num_id[10];
                            FILE *fileid = fopen(FILE_ID_MANAGER, "r");
                            fgets(num_id, sizeof(num_id), fileid);
                            fclose(fileid);

                            StringCchCat(concat, total_size, byteb);
                            StringCchCat(concat, total_size, "ID: ");
                            StringCchCat(concat, total_size, num_id);
                            StringCchCat(concat, total_size, "\n");

                            for(int i = 0; i < 6; i++){
                                if(i == 2){
                                    StringCchCat(concat, total_size, byteb);
                                    StringCchCat(concat, total_size, "É perigoso?: ");
                                    StringCchCat(concat, total_size, yesorno);
                                    StringCchCat(concat, total_size, "\n");
                                }else{
                                    if(i == 0){
                                        StringCchCat(concat, total_size, byteb);
                                        StringCchCat(concat, total_size, "Nome da empresa: ");
                                        StringCchCat(concat, total_size, companyname2);
                                        StringCchCat(concat, total_size, "\n");
                                    }
                                    StringCchCat(concat, total_size, byteb);
                                    StringCchCat(concat, total_size, cBanner_mEdit1[i]);
                                    StringCchCat(concat, total_size, " : ");
                                    StringCchCat(concat, total_size, get_manager_data[i]);
                                    StringCchCat(concat, total_size, "\n");
                                }
                            }

                            if(MessageBox(ManagerScrn, concat, "Confirmação de dados", MB_OKCANCEL) == IDOK){

                                FILE *filew = fopen(FILE_MANAGER, "a");
                                fputs(concat, filew);
                                fclose(filew);

                                int id = atoi(num_id) + 1;
                                itoa(id, num_id, 10);

                                FILE *fileidw = fopen(FILE_ID_MANAGER, "w");
                                fputs(num_id, fileidw);
                                fclose(fileidw);

                                MessageBox(ManagerScrn, "Cadastro das informações realizadas!", "Sucesso", MB_OKCANCEL);
                                for(int j = 0; j < 6; j++)
                                    if(j != 2)
                                        free(get_manager_data[j]);

                                free(concat);
                                //DestroyWindow(ManagerScrn);

                            }
                        }else{
                            FILE *filemng;
                            filemng = fopen(FILE_MANAGER, "w");
                            fputs("", filemng);
                            fclose(filemng);
                            char num_id[10] = {0};

                            GetWindowText(mspltBtn1, manager[lastindex].companyName, GetWindowTextLength(mspltBtn1)+1);
                            GetWindowText(mEdit[0], manager[lastindex].financial, GetWindowTextLength(mEdit[0])+1);
                            GetWindowText(mEdit[1], manager[lastindex].createdWaste, GetWindowTextLength(mEdit[1])+1);
                            GetWindowText(mEdit[2], manager[lastindex].isDanger, GetWindowTextLength(mEdit[2])+1);
                            GetWindowText(mEdit[3], manager[lastindex].ammountCreated, GetWindowTextLength(mEdit[3])+1);
                            GetWindowText(mEdit[4], manager[lastindex].destScheduled, GetWindowTextLength(mEdit[4])+1);
                            GetWindowText(mEdit[5], manager[lastindex].treatedScheduled, GetWindowTextLength(mEdit[5])+1);

                            filemng = fopen(FILE_MANAGER, "a");

                            int lastid = GetLastId(FILE_ID_MANAGER) + 1;

                            //int totalsize = (sizeof(Manager) * lastid) + (strlen(cBanner_mEdit1) * lastid)+50;
                            //CHAR *structs = (CHAR*) malloc(totalsize);

                            for(int i = 0; i < lastid; i++){

                                int totalsize = sizeof(Manager) + 200;
                                CHAR *structs = (CHAR*) malloc(totalsize);
                                structs[0] = {0};
                                //printf("SIZEOF: %d\n", strlen((char*)cBanner_mEdit1));

                                itoa(i, num_id, 10);
                                StringCchCat(structs, totalsize, byteb);
                                StringCchCat(structs, totalsize, "ID: ");
                                StringCchCat(structs, totalsize, num_id);
                                StringCchCat(structs, totalsize, "\n");

                                StringCchCat(structs, totalsize, byteb);
                                StringCchCat(structs, totalsize, "Nome da empresa: ");
                                StringCchCat(structs, totalsize, manager[i].companyName);
                                StringCchCat(structs, totalsize, "\n");

                                StringCchCat(structs, totalsize, byteb);
                                StringCchCat(structs, totalsize, cBanner_mEdit1[0]);
                                StringCchCat(structs, totalsize, " : ");
                                StringCchCat(structs, totalsize, manager[i].financial);
                                StringCchCat(structs, totalsize, "\n");

                                StringCchCat(structs, totalsize, byteb);
                                StringCchCat(structs, totalsize, cBanner_mEdit1[1]);
                                StringCchCat(structs, totalsize, " : ");
                                StringCchCat(structs, totalsize, manager[i].createdWaste);
                                StringCchCat(structs, totalsize, "\n");

                                StringCchCat(structs, totalsize, byteb);
                                StringCchCat(structs, totalsize, cBanner_mEdit1[2]);
                                StringCchCat(structs, totalsize, " : ");
                                StringCchCat(structs, totalsize, manager[i].isDanger);
                                StringCchCat(structs, totalsize, "\n");

                                StringCchCat(structs, totalsize, byteb);
                                StringCchCat(structs, totalsize, cBanner_mEdit1[3]);
                                StringCchCat(structs, totalsize, " : ");
                                StringCchCat(structs, totalsize, manager[i].ammountCreated);
                                StringCchCat(structs, totalsize, "\n");

                                StringCchCat(structs, totalsize, byteb);
                                StringCchCat(structs, totalsize, cBanner_mEdit1[4]);
                                StringCchCat(structs, totalsize, " : ");
                                StringCchCat(structs, totalsize, manager[i].destScheduled);
                                StringCchCat(structs, totalsize, "\n");

                                StringCchCat(structs, totalsize, byteb);
                                StringCchCat(structs, totalsize, cBanner_mEdit1[5]);
                                StringCchCat(structs, totalsize, " : ");
                                StringCchCat(structs, totalsize, manager[i].treatedScheduled);
                                StringCchCat(structs, totalsize, "\n");

                                fputs(structs, filemng);
                                free(structs);

                            }

                            fclose(filemng);

                            MessageBox(app, "Os dados foram atualizados com sucesso!", "Atualização da gestão", MB_OK | MB_ICONINFORMATION);
                        }
                        break;
                    }
                    case MNG_BTN_UPDATE: {

                        FILE *filedata = fopen(FILE_MANAGER, "r");

                        if(filedata == NULL){
                            MessageBox(ManagerScrn, "Não há nenhuma informação cadastrada!", "Error", MB_OKCANCEL | MB_ICONERROR);
                        }else{
                            if(compselected2){
                                if(!updatedata){
                                    char buffer[100];
                                    int line = 0;
                                    int index = 0;

                                    int lastid = GetLastId(FILE_ID_MANAGER) + 1;
                                    manager = (Manager*) malloc(sizeof(Manager) * lastid);

                                    while (fgets(buffer, sizeof(buffer), filedata) != NULL) {
                                        switch(line){
                                            case 1:
                                                GetDataField(manager[index].companyName, buffer);
                                            break;
                                            case 2:
                                                GetDataField(manager[index].financial, buffer);
                                            break;
                                            case 3:
                                                GetDataField(manager[index].createdWaste, buffer);
                                            break;
                                            case 4:
                                                GetDataField(manager[index].isDanger, buffer);
                                            break;
                                            case 5:
                                                GetDataField(manager[index].ammountCreated, buffer);
                                            break;
                                            case 6:
                                                GetDataField(manager[index].destScheduled, buffer);
                                            break;
                                            case 7:
                                                GetDataField(manager[index].treatedScheduled, buffer);
                                            break;
                                        }

                                        if(line == 7){
                                            index++;
                                            line = 0;
                                        }else{
                                            line++;
                                        }

                                    }

                                    MessageBox(ManagerScrn, "A atualização de dados foi ativada!\n" \
                                               "Basta selecionar a empresa e os dados serão carregados.\n" \
                                               "Clique em registrar após as mudanças.\n" \
                                               "Para desativar, clique novamente no botão 'Atualizar Dados'.", "Warning", MB_OK | MB_ICONWARNING);


                                    for(int i = 0; i < lastid; i++){
                                        if(!strcmp(companyname2, manager[i].companyName)){
                                            SetWindowText(mEdit[0], manager[i].financial);
                                            SetWindowText(mEdit[1], manager[i].createdWaste);
                                            SetWindowText(mEdit[2], manager[i].isDanger);
                                            SetWindowText(mEdit[3], manager[i].ammountCreated);
                                            SetWindowText(mEdit[4], manager[i].destScheduled);
                                            SetWindowText(mEdit[5], manager[i].treatedScheduled);
                                            lastindex = i;
                                            break;
                                        }
                                    }

                                    updatedata = true;
                                }else{
                                    MessageBox(ManagerScrn, "A atualização de dados foi desativada!\n" \
                                               "Para ativar, clique novamente no botão 'Atualizar Dados'.", "Warning", MB_OK | MB_ICONWARNING);
                                    updatedata = false;
                                    free(manager);
                                }

                                fclose(filedata);

                            }else{
                                MessageBox(ManagerScrn, "Campo de empresa não selecionado!", "Error", MB_OK | MB_ICONERROR);
                            }

                        }
                        break;
                    }
                    case MNG_BTN_REPORT: {
                        if(!GetWindowTextLength(mEdit[6])){
                            MessageBox(app, "Preencha o relatório de inspeção!", "Error", MB_OK | MB_ICONERROR);
                        }else{
                            CHAR namefile[50] = {0};
                            CHAR num_id[10] = {0};

                            if(updatedata){
                                int totalsize = sizeof(Manager) + GetWindowTextLength(mEdit[6]) + 500;
                                CHAR *structs = (CHAR*) malloc(totalsize);
                                structs[0] = {0};

                                // Será que itemIndex assume o mesmo valor??
                                // talvez alterar itemIndex quando isReport for false
                                // sendo itemIndex = action1 = COSTUMER_LIST = id_company;
                                if(!isReport)
                                    itemIndex = companyid;
                                itoa(itemIndex, num_id, 10);
                                StringCchCat(namefile, 50, FILE_INSP_COMPANY);
                                StringCchCat(namefile, 50, num_id);
                                StringCchCat(namefile, 50, W_EXT);

                                FILE *filerep = fopen(namefile, "w");

                                StringCchCat(structs, totalsize, "RELATÓRIO DE INSPEÇÃO\n\n");

                                StringCchCat(structs, totalsize, byteb);
                                StringCchCat(structs, totalsize, "Nome da empresa: ");
                                StringCchCat(structs, totalsize, manager[lastindex].companyName);
                                StringCchCat(structs, totalsize, "\n");

                                StringCchCat(structs, totalsize, byteb);
                                StringCchCat(structs, totalsize, cBanner_mEdit1[0]);
                                StringCchCat(structs, totalsize, " : ");
                                StringCchCat(structs, totalsize, manager[lastindex].financial);
                                StringCchCat(structs, totalsize, "\n");

                                StringCchCat(structs, totalsize, byteb);
                                StringCchCat(structs, totalsize, cBanner_mEdit1[1]);
                                StringCchCat(structs, totalsize, " : ");
                                StringCchCat(structs, totalsize, manager[lastindex].createdWaste);
                                StringCchCat(structs, totalsize, "\n");

                                StringCchCat(structs, totalsize, byteb);
                                StringCchCat(structs, totalsize, cBanner_mEdit1[2]);
                                StringCchCat(structs, totalsize, " : ");
                                StringCchCat(structs, totalsize, manager[lastindex].isDanger);
                                StringCchCat(structs, totalsize, "\n");

                                StringCchCat(structs, totalsize, byteb);
                                StringCchCat(structs, totalsize, cBanner_mEdit1[3]);
                                StringCchCat(structs, totalsize, " : ");
                                StringCchCat(structs, totalsize, manager[lastindex].ammountCreated);
                                StringCchCat(structs, totalsize, "\n");

                                StringCchCat(structs, totalsize, byteb);
                                StringCchCat(structs, totalsize, cBanner_mEdit1[4]);
                                StringCchCat(structs, totalsize, " : ");
                                StringCchCat(structs, totalsize, manager[lastindex].destScheduled);
                                StringCchCat(structs, totalsize, "\n");

                                StringCchCat(structs, totalsize, byteb);
                                StringCchCat(structs, totalsize, cBanner_mEdit1[5]);
                                StringCchCat(structs, totalsize, " : ");
                                StringCchCat(structs, totalsize, manager[lastindex].treatedScheduled);
                                StringCchCat(structs, totalsize, "\n\n");


                                int sizeReport = GetWindowTextLength(mEdit[6]);
                                CHAR reportText[sizeReport];
                                GetWindowText(mEdit[6], reportText, sizeReport+1);
                                StringCchCat(structs, totalsize, reportText);

                                fputs(structs, filerep);
                                fclose(filerep);
                                free(structs);

                                MessageBox(app, "Relatório gerado com sucesso!", "Geração de Relatório", MB_OK | MB_ICONINFORMATION);
                            }else{
                                MessageBox(app, "Clique no botão atualizar dados!", "Error", MB_OK | MB_ICONERROR);
                            }

                            if(isReport){

                                CHAR buff[MAX_PATH];
                                StringCbPrintf (buff, ARRAYSIZE(buff), TEXT("%s | %s | Visualizar relatório"),
                                IndData[itemIndex].companyName, IndData[itemIndex].timeScheduled);

                                ListBox_DeleteString(hwndList6, itemPos);
                                ListBox_InsertString(hwndList6, itemPos, buff);
                                ListBox_SetCurSel(hwndList6, itemPos);
                                ListBox_SetItemData(hwndList6, itemPos, itemIndex);

                                if(IndStatus[itemIndex]){
                                   CHAR sysconfirm[50] = {0};
                                    CHAR num_stat[10] = {0};
                                    itoa(++IndStatus[itemIndex], num_stat, 10);
                                    StringCchCat(sysconfirm, 50, FILE_STAT_COMPANY);
                                    StringCchCat(sysconfirm, 50, num_id);
                                    StringCchCat(sysconfirm, 50, EXTENSION);

                                    FILE *filesys = fopen(sysconfirm, "w");
                                    fputs(num_stat, filesys);
                                    fclose(filesys);
                                }

                                isReport = false;
                            }
                        }
                        break;
                    }
                }
        break;
        case WM_NOTIFY:
            switch (((LPNMHDR)action2)->code) {
                case BCN_DROPDOWN:
                        NMBCHOTITEM* pDropDown = (NMBCHOTITEM*)action2;
                        if (pDropDown->hdr.hwndFrom == GetDlgItem(app, IS_DANGER))
                        {

                            hSplitMenu2 = CreatePopupMenu();
                            AppendMenu(hSplitMenu2, MF_BYPOSITION, YES_OP, "Sim");
                            AppendMenu(hSplitMenu2, MF_BYPOSITION, NO_OP, "Não");

                            RECT Rect;
                            GetWindowRect(mEdit[2], &Rect);

                            TrackPopupMenu(hSplitMenu2, TPM_LEFTALIGN | TPM_TOPALIGN, Rect.left, Rect.top+30, 0, app, NULL);

                            return TRUE;
                        }else{
                            if (pDropDown->hdr.hwndFrom == GetDlgItem(app, ID_COMPANY))
                            {

                                FILE* filer = fopen(FILE_COSTUMER, "r");

                                char buffer[100];
                                char company[100];
                                int countline = 0;
                                int index = 0;

                                hSplitMenu3 = CreatePopupMenu();

                                if (filer != NULL) {
                                    // Ler cada linha do arquivo
                                    while (fgets(buffer, sizeof(buffer), filer) != NULL) {

                                        if(countline == 6){
                                            int i;
                                            for(i = 0; buffer[i] != ':'; i++);
                                            i += 2;
                                            int j;
                                            for(j = 0; buffer[i] != '\n'; i++, j++)
                                                company[j] = buffer[i];

                                            company[j] = 0;
                                            AppendMenu(hSplitMenu3, MF_BYPOSITION, COSTUMER_LIST+index, company);
                                            index++;
                                        }

                                        countline = (countline == 22) ? 0 : countline + 1;
                                    }
                                }else{
                                    MessageBox(ManagerScrn, "Por favor, cadastre um cliente!", "Error", MB_OKCANCEL);
                                    break;
                                }

                                fclose(filer);

                                RECT Rect;
                                GetWindowRect(mspltBtn1, &Rect);
                                TrackPopupMenu(hSplitMenu3, TPM_LEFTALIGN | TPM_TOPALIGN, Rect.left, Rect.top+30, 0, app, NULL);
                            }
                        }
                        break;
                    }
                return FALSE;
        case WM_DESTROY:
            PostQuitMessage (0);
            break;
        case WM_CLOSE:
            DestroyWindow(ManagerScrn);
            return 0;
        case WM_GETMINMAXINFO: {
            MINMAXINFO* info = (MINMAXINFO*)action2;
            info->ptMinTrackSize.x = MNGRW_X;
            info->ptMinTrackSize.y = MNGRW_Y;
            info->ptMaxTrackSize.x = MNGRW_X;
            info->ptMaxTrackSize.y = MNGRW_Y;
            break;
        }
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (app, message, action1, action2);
    }

    return 0;
}


LRESULT CALLBACK SessionManager (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_CREATE: {

            // Get the handle to the client area's device context.
            CreateWindow(TEXT("BUTTON"), TEXT("Coletas da semana"), WS_VISIBLE | WS_CHILD | WS_BORDER | BS_GROUPBOX, 0, 0, 1340, 300,
            hwnd, NULL, NULL, NULL);


            CreateWindow(TEXT("BUTTON"), TEXT("Segunda"), WS_VISIBLE | WS_CHILD | WS_BORDER | BS_GROUPBOX, 10, 30, 270, 250,
            hwnd, NULL, NULL, NULL);
            CreateWindow(TEXT("BUTTON"), TEXT("Terça"), WS_VISIBLE | WS_CHILD | WS_BORDER | BS_GROUPBOX, 280, 30, 270, 250,
            hwnd, (HMENU) NULL, NULL, NULL);

            CreateWindow(TEXT("BUTTON"), TEXT("Quarta"), WS_VISIBLE | WS_CHILD | WS_BORDER | BS_GROUPBOX, 550, 30, 270, 250,
            hwnd, NULL, NULL, NULL);
            CreateWindow(TEXT("BUTTON"), TEXT("Quinta"), WS_VISIBLE | WS_CHILD | WS_BORDER | BS_GROUPBOX, 820, 30, 270, 250,
            hwnd, NULL, NULL, NULL);
            CreateWindow(TEXT("BUTTON"), TEXT("Sexta"), WS_VISIBLE | WS_CHILD | WS_BORDER | BS_GROUPBOX, 1090, 30, 250, 250,
            hwnd, NULL, NULL, NULL);

            CreateWindow(TEXT("BUTTON"), TEXT("Empresas com Resíduos Armazenados"), WS_VISIBLE | WS_CHILD | WS_BORDER | BS_GROUPBOX, 0, 310, 643, 300,
            hwnd, NULL, NULL, NULL);

            CreateWindow(TEXT("BUTTON"), TEXT("Relatórios Gerados de Empresas Tratadas"), WS_VISIBLE | WS_CHILD | WS_BORDER | BS_GROUPBOX, 688, 310, 653, 300,
            hwnd, NULL, NULL, NULL);

            hwndList1 = CreateWindow(TEXT("LISTBOX"), TEXT(""), WS_VISIBLE | WS_CHILD | LBS_NOTIFY | WS_VSCROLL | WS_BORDER | LBS_USETABSTOPS, 20, 50, 250, 230,
            hwnd, (HMENU) IDC_LISTBOX1, NULL, NULL);
            hwndList2 = CreateWindow(TEXT("LISTBOX"), TEXT(""), WS_VISIBLE | WS_CHILD | LBS_NOTIFY | WS_VSCROLL | WS_BORDER | LBS_USETABSTOPS, 290, 50, 250, 230,
            hwnd, (HMENU) IDC_LISTBOX2, NULL, NULL);
            hwndList3 = CreateWindow(TEXT("LISTBOX"), TEXT(""), WS_VISIBLE | WS_CHILD | LBS_NOTIFY | WS_VSCROLL | WS_BORDER | LBS_USETABSTOPS, 560, 50, 250, 230,
            hwnd, (HMENU) IDC_LISTBOX3, NULL, NULL);
            hwndList4 = CreateWindow(TEXT("LISTBOX"), TEXT(""), WS_VISIBLE | WS_CHILD | LBS_NOTIFY | WS_VSCROLL | WS_BORDER | LBS_USETABSTOPS, 830, 50, 250, 230,
            hwnd, (HMENU) IDC_LISTBOX4, NULL, NULL);
            hwndList5 = CreateWindow(TEXT("LISTBOX"), TEXT(""), WS_VISIBLE | WS_CHILD | LBS_NOTIFY | WS_VSCROLL | WS_BORDER | LBS_USETABSTOPS, 1100, 50, 240, 230,
            hwnd, (HMENU) IDC_LISTBOX5, NULL, NULL);

            hwndList6 = CreateWindow(TEXT("LISTBOX"), TEXT(""), WS_VISIBLE | WS_CHILD | LBS_NOTIFY | WS_VSCROLL | WS_BORDER | LBS_USETABSTOPS, 10, 330, 623, 280,
            hwnd, (HMENU) IDC_LISTBOX6, NULL, NULL);
            hwndList7 = CreateWindow(TEXT("LISTBOX"), TEXT(""), WS_VISIBLE | WS_CHILD | LBS_NOTIFY | WS_VSCROLL | WS_BORDER | LBS_USETABSTOPS, 698, 330, 633, 280,
            hwnd, (HMENU) IDC_LISTBOX7, NULL, NULL);


            FILE* filer = fopen(FILE_COSTUMER, "r");
            Tree* t1;

            char buffer[100];
            int line = 0;
            int index = 0;
            bool first = false;
            int readstat = 0;

            if (filer != NULL) {
                while (fgets(buffer, sizeof(buffer), filer) != NULL) {

                    if(!line){
                        CHAR id_char[10] = {0};
                        CHAR stat[2] = {0};
                        CHAR sysconfirm[50] = {0};

                        itoa(index, id_char, 10);
                        StringCchCat(sysconfirm, 50, FILE_STAT_COMPANY);
                        StringCchCat(sysconfirm, 50, id_char);
                        StringCchCat(sysconfirm, 50, EXTENSION);

                        if(GetFileStatus(sysconfirm, stat)){
                            readstat = atoi(stat);
                            IndStatus[index] = readstat;
                        }else{
                            MessageBox(hwnd, "Erro: Arquivo de status de confirmação faltando!", "Error", MB_OK | MB_ICONERROR);
                        }
                    }


                    switch(line){
                        case 1:
                            GetDataField(IndData[index].personName, buffer);
                        break;
                        case 6:
                            GetDataField(IndData[index].companyName, buffer);
                        break;
                        case 9:
                            GetDataField(IndData[index].dataCNPJ, buffer);
                        break;
                        case 17:
                            GetDataField(IndData[index].dateScheduled, buffer);
                        break;
                        case 18:
                            GetDataField(IndData[index].timeScheduled, buffer);
                        break;
                    }

                    if(line == 22){
                        index++;
                        line = 0;
                        readstat = true;
                    }else{
                        line++;
                    }
                }

                for(int i = 0; i < index; i++){

                    char time[5] = {0};
                    for(int j = 0, y = 0; j < 5; j++)
                        if(IndData[i].timeScheduled[j] != ':')
                            time[y++] = IndData[i].timeScheduled[j];

                    int hour = atoi(time);

                    if(hour){
                        if(!first){
                            dat = create(hour, i, initialize(), initialize());
                            first = true;
                        }else{
                            insert(dat, hour, i);
                        }
                    }
                }

                AddOrdernedList(dat);
            }

            fclose(filer);

            FILE* repfile = fopen(FILE_REPORT, "r");
            reports = begin();
            line = 0;
            index = 0;

            int report_id;
            char company_id[100];
            char type_report[5];
            char name_file[100];
            char date_format[50];

            if (repfile != NULL) {
                    while (fgets(buffer, sizeof(buffer), repfile) != NULL) {

                        switch(line){
                            case 0:
                                GetDataField(company_id, buffer);
                            break;
                            case 1:
                                GetDataField(type_report, buffer);
                            break;
                            case 2:
                                GetDataField(name_file, buffer);
                            break;
                            case 3:
                                GetDataField(date_format, buffer);
                            break;
                        }

                        if(line++ == 3){
                            line = 0;
                            reports = insertNode(reports, report_id, atoi(company_id), type_report, name_file, date_format);
                            report_id++;
                        }

                    }
                }

                for(Node *li = reports; li != NULL; li = li->next){
                    CHAR buff0[MAX_PATH];
                    StringCbPrintf (buff0, ARRAYSIZE(buff0), TEXT("%s | %s | Relatório %s"),
                    IndData[li->companyId].companyName, li->dateFormat, li->typeReport);

                    int iPos = ListBox_AddString(hwndList7, buff0);
                    ListBox_SetItemData(hwndList7, iPos, li->reportId);
                }

            break;
        }
        case WM_COMMAND:

            if(LOWORD(wParam) >= IDC_LISTBOX1 && LOWORD(wParam) <= IDC_LISTBOX7){

                HWND hwndList = GetDlgItem(hwnd, LOWORD(wParam));
                int lbItem = (int)SendMessage(hwndList, LB_GETCURSEL, 0, 0);
                int i = (int)SendMessage(hwndList, LB_GETITEMDATA, lbItem, 0);
                TCHAR buff[MAX_PATH];
                int sizem = MAX_PATH;
                CHAR *data = (CHAR*) malloc(sizem);
                isReport = false;

                if(LOWORD(wParam) == IDC_LISTBOX7 && (HIWORD(wParam) == LBN_DBLCLK && i > -1)){
                    CHAR buffer[sizem];
                    data[0] = {0};

                    Node *item = search(reports, i);
                    FILE *report = fopen(item->nameFile, "r");

                    while (fgets(buffer, sizeof(buffer), report) != NULL) {
                        StringCchCat(data, sizem, buffer);
                        sizem+=MAX_PATH;
                        data = (CHAR*) realloc(data, sizem);
                    }

                    StringCchCat(data, sizem, "\n\nDeseja gerar uma cópia desse relatório?");
                    if(MessageBox(hwnd, data, "Cliente de Resíduos", MB_YESNO | MB_ICONWARNING) == IDYES)
                        MessageBox(hwnd, "Fazer cópia de relatório!", "Cliente de Resíduos", MB_OK);

                    break;
                }

                switch(IndStatus[i]){
                    case 0:
                        StringCbPrintf (data, ARRAYSIZE(buff),
                        TEXT("Empresa: %s\nHoráro: %s %s\n\n Deseja confirmar o recebimento dos resíduos?"), IndData[i].companyName,
                                        IndData[i].dateScheduled, IndData[i].timeScheduled);

                        break;
                    case 1:
                        StringCbPrintf (data, ARRAYSIZE(buff),
                        TEXT("Empresa: %s\nHoráro: %s %s\n\n Deseja gerar o relatório de inspeção?"), IndData[i].companyName,
                                        IndData[i].dateScheduled, IndData[i].timeScheduled);
                        break;
                    case 2: {
                        CHAR buffer[sizem];
                        CHAR namefile[50] = {0};
                        CHAR num_id[10] = {0};
                        data[0] = {0};

                        itoa(i, num_id, 10);
                        StringCchCat(namefile, 50, FILE_INSP_COMPANY);
                        StringCchCat(namefile, 50, num_id);
                        StringCchCat(namefile, 50, W_EXT);

                        FILE *report = fopen(namefile, "r");

                        while (fgets(buffer, sizeof(buffer), report) != NULL) {
                            StringCchCat(data, sizem, buffer);
                            sizem+=MAX_PATH;
                            data = (CHAR*) realloc(data, sizem);
                        }
                        StringCbPrintf (buff, ARRAYSIZE(buff), TEXT("\nArmazenado em: %s \n\nDeseja confirmar o tratamento e gerar os relatórios MTR/CDF?"),
                                        IndData[i].dateScheduled);
                        StringCchCat(data, sizem, buff);
                        break;
                    }
                }


                if(HIWORD(wParam) == LBN_DBLCLK && i > -1){
                    if(MessageBox(hwnd, data, "Cliente de Resíduos", MB_YESNO | MB_ICONWARNING) == IDYES){
                        if(IndStatus[i] == 0){
                            CHAR sysconfirm[50] = {0};
                            CHAR num_id[10] = {0};
                            CHAR num_stat[10] = {0};
                            itoa(i, num_id, 10);
                            itoa(++IndStatus[i], num_stat, 10);
                            StringCchCat(sysconfirm, 50, FILE_STAT_COMPANY);
                            StringCchCat(sysconfirm, 50, num_id);
                            StringCchCat(sysconfirm, 50, EXTENSION);

                            FILE *filesys = fopen(sysconfirm, "w");
                            fputs(num_stat, filesys);
                            fclose(filesys);

                            char timebuff[5] = {0};
                            for(int y = 0, j = 0; y < 5; y++)
                                if(IndData[i].timeScheduled[y] != ':')
                                    timebuff[j++] = IndData[i].timeScheduled[y];
                            int time = atoi(timebuff);
                            printf("\nDeletando hora: %d\n", time);

                            if(remove(dat, time))
                                ListBox_DeleteString(hwndList, lbItem);

                            StringCbPrintf (buff, ARRAYSIZE(buff), TEXT("%s | %s | Não há relatório"),
                                IndData[i].companyName, IndData[i].timeScheduled);

                            int pos = ListBox_AddString(hwndList6, buff);
                            ListBox_SetItemData(hwndList6, pos, i);

                            if(MessageBox(hwnd, "Deseja gerar o relatório de inspeção?", "Cliente de Resíduos", MB_YESNO | MB_ICONWARNING) == IDYES){
                                isReport = true;
                                itemIndex = i;
                                itemPos = pos;

                                ManagerScrn = CreateWindowEx (WS_EX_CLIENTEDGE, classNameMngr, managerTitle, WS_OVERLAPPEDWINDOW | WS_BORDER, CW_USEDEFAULT, CW_USEDEFAULT, MNGRW_X, MNGRW_Y,
                                       SessionScrn, NULL, instance5, NULL);
                                ShowWindow (ManagerScrn, SW_SHOWNORMAL);
                            }
                        }else{
                            if(IndStatus[i] == 1){
                                isReport = true;
                                itemIndex = i;
                                itemPos = lbItem;

                                ManagerScrn = CreateWindowEx (WS_EX_CLIENTEDGE, classNameMngr, managerTitle, WS_OVERLAPPEDWINDOW | WS_BORDER, CW_USEDEFAULT, CW_USEDEFAULT, MNGRW_X, MNGRW_Y,
                                       SessionScrn, NULL, instance5, NULL);
                                ShowWindow (ManagerScrn, SW_SHOWNORMAL);
                            }else{
                                if(IndStatus[i] == 2){
                                    isReport = true;
                                    itemIndex = i;
                                    itemPos = lbItem;

                                    MTRScrn = CreateWindowEx (0, classNameMTR, MTRTitle, WS_OVERLAPPEDWINDOW | WS_BORDER, CW_USEDEFAULT, CW_USEDEFAULT, 450, 430,
                                            SessionScrn, NULL, instance7, NULL);
                                    ShowWindow (MTRScrn, SW_SHOWNORMAL);

                                    SetWindowText(mtrEdit[3], IndData[i].dataCNPJ);
                                    SetWindowText(mtrEdit[4], IndData[i].personName);
                                    SetWindowText(mtrEdit[8], IndData[i].dateScheduled);

                                    CDFScrn = CreateWindowEx (0, classNameCDF, CDFTitle, WS_OVERLAPPEDWINDOW | WS_BORDER, CW_USEDEFAULT, CW_USEDEFAULT, 450, 500,
                                            SessionScrn, NULL, instance8, NULL);
                                    ShowWindow (CDFScrn, SW_SHOWNORMAL);

                                    SetWindowText(cdfEdit[0], IndData[i].companyName);
                                    compselected3 = true;
                                }
                            }
                        }

                    }
                    free(data);
                }


                    break;

            }

            switch(LOWORD(wParam)){
                case MENU_MTR:
                    isReport = false;
                    MTRScrn = CreateWindowEx (0, classNameMTR, MTRTitle, WS_OVERLAPPEDWINDOW | WS_BORDER, CW_USEDEFAULT, CW_USEDEFAULT, 450, 430,
                           SessionScrn, NULL, instance7, NULL);
                    ShowWindow (MTRScrn, SW_SHOWNORMAL);
                break;
                case MENU_CDF:
                    isReport = false;
                    CDFScrn = CreateWindowEx (0, classNameCDF, CDFTitle, WS_OVERLAPPEDWINDOW | WS_BORDER, CW_USEDEFAULT, CW_USEDEFAULT, 450, 500,
                           SessionScrn, NULL, instance8, NULL);
                    ShowWindow (CDFScrn, SW_SHOWNORMAL);
                break;
                case MENU_FDSR:
                    isReport = false;
                    FDSRScrn = CreateWindowEx (0, classNameFDSR, FDSRTitle, WS_OVERLAPPEDWINDOW | WS_BORDER, CW_USEDEFAULT, CW_USEDEFAULT, 450, 500,
                           SessionScrn, NULL, instance9, NULL);
                    ShowWindow (FDSRScrn, SW_SHOWNORMAL);
                break;
                case MENU_INSPECT:
                    ManagerScrn = CreateWindowEx (WS_EX_CLIENTEDGE, classNameMngr, managerTitle, WS_OVERLAPPEDWINDOW | WS_BORDER, CW_USEDEFAULT, CW_USEDEFAULT, MNGRW_X, MNGRW_Y,
                           SessionScrn, NULL, instance5, NULL);
                    ShowWindow (ManagerScrn, SW_SHOWNORMAL);
                break;
                case MENU_SEMMI_MANUAL: {
                    FILE* filer = fopen(FILE_WASTE, "r");

                    char buffer[100];
                    char wastecount[100];
                    char wastespending[100];
                    char wastedate[100];
                    char concat[1000];
                    int line = 0;
                    int index = 0;
                    int totalresiduo = 0;
                    int totalgasto = 0;
                    char totalresiduoChar[100];
                    char totalgastoChar[100];

                    if (filer != NULL) {
                        while (fgets(buffer, sizeof(buffer), filer) != NULL) {

                            if(index == 6){
                                StringCchCat(concat, sizeof(concat), "RELATÓRIO SEMESTRAL DE INSUMOS E GASTOS\n\n");
                                StringCchCat(concat, sizeof(concat), "De acordo com os 6 meses de janeiro à junho, foram \n tratados ");
                                itoa(totalresiduo, totalresiduoChar, 10);
                                StringCchCat(concat, sizeof(concat), totalresiduoChar);
                                StringCchCat(concat, sizeof(concat), " resíduos, onde a empresa receptora atribuiu um gasto total de \n");
                                itoa(totalgasto, totalgastoChar, 10);
                                StringCchCat(concat, sizeof(concat), totalgastoChar);
                                StringCchCat(concat, sizeof(concat), " reais. Levando em consideração os diversos gastos de transporte, \n");
                                StringCchCat(concat, sizeof(concat), " tratamento, destinação e outros fatores. \n\n");
                                StringCchCat(concat, sizeof(concat), " Deseja armazenar o relatório em arquivo? (OK/CANCELAR)");

                                int reportcount = 0;
                                char reportchar[20];
                                char namefile[100];

                                itoa(reportcount, reportchar, 10);

                                StringCchCat(namefile, sizeof(namefile), "relatorio_semestral_de_insumos_e_gastos");
                                StringCchCat(namefile, sizeof(namefile), reportchar);
                                StringCchCat(namefile, sizeof(namefile), ".");
                                StringCchCat(namefile, sizeof(namefile), "txt");

                                if(MessageBox(SessionScrn, concat, "Relatório de insumos tratados", MB_OKCANCEL) == IDOK){
                                    FILE *filew = fopen(namefile, "w");
                                    fputs(concat, filew);
                                    fclose(filew);
                                }

                                totalgasto = 0;
                                totalresiduo = 0;
                                index = 0;
                                reportcount++;
                            }

                            if(line == 6){
                                GetDataField(wastecount, buffer);
                                totalresiduo += atoi(wastecount);
                            }else if(line == 8){
                                GetDataField(wastespending, buffer);
                                totalgasto += atoi(wastespending);
                                index++;
                            }

                            line = (line == 8) ? 0 : line + 1;
                        }
                    }else{
                        MessageBox(SessionScrn, "Não há dados suficiente pra gerar este relatório!", "Erro de cadastro", MB_OKCANCEL);
                        break;
                    }

                    break;
                }
                case MENU_PROFITS:
                break;
                case MENU_PRODUC:
                break;
                case OPEN:
                break;
                case LOGGOFF:
                    DestroyWindow(SessionScrn);
                    parentstate = false;
                    statelogin = false;
                break;
                case MENU_COSTUMER_REG:
                    CostumerScrn = CreateWindowEx (WS_EX_CLIENTEDGE, classNameCost, costTitle, WS_OVERLAPPEDWINDOW | WS_BORDER, CW_USEDEFAULT, CW_USEDEFAULT, COSTW_X, COSTW_Y,
                           SessionScrn, NULL, instance3, NULL);
                    ShowWindow (CostumerScrn, SW_SHOWNORMAL);
                break;
                case MENU_WORKER_REG:
                    SignScrn = CreateWindow (classNameSign, signTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, SIGNW_X, SIGNW_Y,
                        SessionScrn, NULL, NULL, NULL);
                    ShowWindow (SignScrn, SW_SHOWNORMAL);
                break;
                case MENU_COSTUMER_UPD:
                break;
                case MENU_WASTE_UPD:
                    WasteScrn = CreateWindowEx (WS_EX_CLIENTEDGE, classNameWaste, wasteTitle, WS_OVERLAPPEDWINDOW | WS_BORDER, CW_USEDEFAULT, CW_USEDEFAULT, WASTEW_X, WASTEW_Y,
                           SessionScrn, NULL, instance4, NULL);
                    ShowWindow (WasteScrn, SW_SHOWNORMAL);
                break;
                case MENU_ABOUT_SOFT:
                break;
                case MENU_TUTORIAL:
                break;
            }
        break;
        case WM_DESTROY:
            PostQuitMessage (0);
            break;
        case WM_CLOSE:
                DestroyWindow(SessionScrn);
            return 0;
        case WM_GETMINMAXINFO: {
            //MINMAXINFO* info = (MINMAXINFO*)action2;
            //info->ptMinTrackSize.x = info->ptMinTrackSize.x;
            //info->ptMinTrackSize.y = info->ptMinTrackSize.y;
            //info->ptMaxTrackSize.x = info->ptMaxTrackSize.x;
            //info->ptMaxTrackSize.y = info->ptMaxTrackSize.y;
            break;
        }
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}


LRESULT CALLBACK MTRManager (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_CREATE: {
            radiopressed2 = false;

            CreateWindow(TEXT("BUTTON"), TEXT("Via do MTR"), WS_VISIBLE | WS_CHILD | WS_BORDER | BS_GROUPBOX, 0, 0, 430, 220,
            hwnd, NULL, NULL, NULL);
            mtrEdit[0] = CreateWindow(TEXT("BUTTON"), TEXT("Gerador"), WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 10, 20, 125, 20 ,
			hwnd, (HMENU) RADIOB5, NULL, NULL);
            mtrEdit[1] = CreateWindow(TEXT("BUTTON"), TEXT("Transportador"), WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 145, 20, 125, 20 ,
			hwnd, (HMENU) RADIOB6, NULL, NULL);
            mtrEdit[2] = CreateWindow(TEXT("BUTTON"), TEXT("Receptor"), WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 280, 20, 125, 20 ,
			hwnd, (HMENU) RADIOB7, NULL, NULL);
			mtrEdit[3] = CreateWindow(TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 10, 45, 410, 20 ,
			hwnd, (HMENU) MTR_ID+0, NULL, NULL);
			mtrEdit[4] = CreateWindow(TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 10, 70, 410, 20 ,
			hwnd, (HMENU) MTR_ID+1, NULL, NULL);
			mtrEdit[5] = CreateWindow(TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 10, 95, 410, 20 ,
			hwnd, (HMENU) MTR_ID+2, NULL, NULL);
			mtrEdit[6] = CreateWindow(TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 10, 120, 410, 20 ,
			hwnd, (HMENU) MTR_ID+3, NULL, NULL);
			mtrEdit[7] = CreateWindow(TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 10, 145, 410, 20 ,
			hwnd, (HMENU) MTR_ID+4, NULL, NULL);
			mtrEdit[8] = CreateWindow(TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 10, 170, 410, 20 ,
			hwnd, (HMENU) MTR_ID+5, NULL, NULL);


            CreateWindow(TEXT("BUTTON"), TEXT("Informações dos Resíduos"), WS_VISIBLE | WS_CHILD | WS_BORDER | BS_GROUPBOX, 0, 230, 430, 150,
            hwnd, NULL, NULL, NULL);
            mtrEdit[9] = CreateWindow(TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 10, 255, 410, 20 ,
			hwnd, (HMENU) MTR_ID+6, NULL, NULL);
			mtrEdit[10] = CreateWindow(TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 10, 280, 410, 20 ,
			hwnd, (HMENU) MTR_ID+7, NULL, NULL);
			mtrEdit[11] = CreateWindow(TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 10, 305, 410, 20 ,
			hwnd, (HMENU) MTR_ID+8, NULL, NULL);

			CreateWindow(TEXT("BUTTON"), TEXT("Gerar"), WS_VISIBLE | WS_CHILD | WS_BORDER, 295, 330, 125, 30,
            hwnd, (HMENU) MTR_GENERATE, NULL, NULL);

            for(int i = 0; i < 12; i++){
                //SendMessage(mtrEdit[i], WM_SETFONT, (WPARAM)hFont, TRUE);
                SendMessage(mtrEdit[i], EM_SETCUEBANNER, false, (LPARAM)TEXT(cBanner_mtrEdit[i]));
			}

            break;
        }
        case WM_COMMAND:
            switch(wParam){
                case MTR_GENERATE: {

                    int mtr_data_size[12];
                    int total_size;
                    CHAR *get_mtr_data[12];
                    CHAR *concat = (CHAR*) malloc(500 * sizeof(CHAR));
                    concat[0] = {0};
                    int current_index;
                    bool errordata = false;

                    for(int i = 3; i < 12; i++){
                        mtr_data_size[i] = GetWindowTextLength(mtrEdit[i]);
                        CHAR concat1[100] = {0};
                        if(!mtr_data_size[i]){
                            StringCchCat(concat1, 100, "O campo '");
                            StringCchCat(concat1, 100, cBanner_mtrEdit1[i]);
                            StringCchCat(concat1, 100, "' ");
                            StringCchCat(concat1, 100, " está vazio!");
                            MessageBox(MTRScrn, concat1, "Erro de cadastro", MB_OKCANCEL);
                            current_index = i;
                            errordata = true;
                            break;
                        }
                        if(!radiopressed2){
                            MessageBox(MTRScrn, "Algum dos botões não foram marcados!", "Erro de cadastro", MB_OKCANCEL);
                            current_index = i;
                            errordata = true;
                            break;
                        }

                        get_mtr_data[i] = (CHAR*) malloc(mtr_data_size[i] * sizeof(CHAR));
                        GetWindowText(mtrEdit[i], get_mtr_data[i], mtr_data_size[i]+1);
                        total_size += mtr_data_size[i];
                    }

                    if(errordata){
                        for(int j = 3; j < current_index; j++)
                            free(get_mtr_data[j]);

                        free(concat);
                        break;
                    }
                    concat = (CHAR*) realloc(concat, total_size);

                    StringCchCat(concat, total_size, byteb);
                    StringCchCat(concat, total_size, "Via do MTR");
                    StringCchCat(concat, total_size, " : ");
                    StringCchCat(concat, total_size, radiotext2);
                    StringCchCat(concat, total_size, "\n");

                    for(int i = 3; i < 12; i++){
                        StringCchCat(concat, total_size, byteb);
                        StringCchCat(concat, total_size, cBanner_mtrEdit1[i]);
                        StringCchCat(concat, total_size, " : ");
                        StringCchCat(concat, total_size, get_mtr_data[i]);
                        StringCchCat(concat, total_size, "\n");
                    }

                    int lastid = GetLastId(FILE_ID_COSTUMER);
                    bool cnpjFound = false;
                    bool cnpjFound2 = false;
                    for(int i = 0; i < (lastid+1); i++){
                        if(!strcmp(get_mtr_data[3], IndData[i].dataCNPJ)){
                            cnpjFound = true;
                            current_index = i;
                            break;
                        }
                    }

                    cnpjFound2 = cnpjFound;
                    if(!cnpjFound)
                        cnpjFound = MessageBox(MTRScrn, "Este CNPJ não consta na lista de clientes!\n" \
                                               "Deseja gerar o relatório mesmo assim?", "Confirmação de CNPJ", MB_YESNO | MB_ICONWARNING) == IDYES;

                    if(cnpjFound){
                        if(MessageBox(MTRScrn, concat, "Confirmação de dados", MB_OKCANCEL) == IDOK){

                            char typereport[5] = "MTR";
                            char namefile[50] = {0};
                            char dateformat[20] = {0};
                            CHAR sYear[10], sMonth[10], sDay[10], sHour[10], sMin[10];

                            SYSTEMTIME lt;
                            GetLocalTime(&lt);
                            itoa(lt.wYear, sYear, 10);
                            itoa(lt.wMonth, sMonth, 10);
                            itoa(lt.wDay, sDay, 10);
                            itoa(lt.wHour, sHour, 10);
                            itoa(lt.wMinute, sMin, 10);

                            StringCchCat(namefile, 50, FILE_MTR_COMPANY);
                            StringCchCat(namefile, 50, sYear);
                            if(strlen(sMonth) == 1)
                               StringCchCat(namefile, 50, "0");
                            StringCchCat(namefile, 50, sMonth);
                            if(strlen(sDay) == 1)
                               StringCchCat(namefile, 50, "0");
                            StringCchCat(namefile, 50, sDay);
                            if(strlen(sHour) == 1)
                                StringCchCat(namefile, 50, "0");
                            StringCchCat(namefile, 50, sHour);
                            if(strlen(sMin) == 1)
                               StringCchCat(namefile, 50, "0");
                            StringCchCat(namefile, 50, sMin);
                            StringCchCat(namefile, 50, W_EXT);

                            if(strlen(sDay) == 1)
                               StringCchCat(dateformat, 20, "0");
                            StringCchCat(dateformat, 20, sDay);
                            StringCchCat(dateformat, 20, "/");
                            if(strlen(sMonth) == 1)
                               StringCchCat(dateformat, 20, "0");
                            StringCchCat(dateformat, 20, sMonth);
                            StringCchCat(dateformat, 20, "/");
                            StringCchCat(dateformat, 20, sYear);
                            StringCchCat(dateformat, 20, " ");
                            if(strlen(sHour) == 1)
                                StringCchCat(dateformat, 20, "0");
                            StringCchCat(dateformat, 20, sHour);
                            StringCchCat(dateformat, 20, ":");
                            if(strlen(sMin) == 1)
                               StringCchCat(dateformat, 20, "0");
                            StringCchCat(dateformat, 20, sMin);

                            FILE *filew = fopen(namefile, "w");
                            fputs(concat, filew);
                            fclose(filew);

                            if(cnpjFound2){
                                if(!isReport)
                                    itemIndex = current_index;

                                reports = insertNode(reports, reportid, itemIndex, typereport, namefile, dateformat);

                                showList(reports);

                                CHAR buff0[MAX_PATH];
                                StringCbPrintf(buff0, ARRAYSIZE(buff0), TEXT("COMPANY_ID : %d\n"  \
                                               "TYPE_REPORT : %s\n" \
                                               "NAME_FILE : %s\n"   \
                                               "DATE_FORMAT : %s\n"),
                                                reports->companyId, reports->typeReport, reports->nameFile, reports->dateFormat);

                                FILE *repw = fopen(FILE_REPORT, "a");
                                fputs(buff0, repw);
                                fclose(repw);

                                CHAR buff[MAX_PATH];
                                StringCbPrintf (buff, ARRAYSIZE(buff), TEXT("%s | %s | Relatório %s"),
                                IndData[itemIndex].companyName, dateformat, typereport);

                                if(isReport && IndStatus[itemIndex] == 2)
                                    ListBox_DeleteString(hwndList6, itemPos);

                                ListBox_InsertString(hwndList7, 0, buff);
                                ListBox_SetCurSel(hwndList7, 0);
                                ListBox_SetItemData(hwndList7, 0, reportid);

                                char report_id[10];
                                reportid = GetLastId(FILE_ID_REPORT) + 1;
                                itoa(++reportid, report_id, 10);

                                FILE *fileidw = fopen(FILE_ID_REPORT, "w");
                                fputs(report_id, fileidw);
                                fclose(fileidw);

                                if(isReport){
                                    CHAR sysconfirm[50] = {0};
                                    CHAR num_stat[10] = {0};
                                    CHAR num_id[10] = {0};
                                    itoa(itemIndex, num_id, 10);
                                    itoa(++IndStatus[itemIndex], num_stat, 10);
                                    StringCchCat(sysconfirm, 50, FILE_STAT_COMPANY);
                                    StringCchCat(sysconfirm, 50, num_id);
                                    StringCchCat(sysconfirm, 50, EXTENSION);

                                    FILE *filesys = fopen(sysconfirm, "w");
                                    fputs(num_stat, filesys);
                                    fclose(filesys);
                                }
                            }

                            for(int j = 3; j < 12; j++)
                                free(get_mtr_data[j]);

                            free(concat);

                            MessageBox(MTRScrn, "Cadastro das informações realizadas!", "Sucesso", MB_OKCANCEL);
                            DestroyWindow(MTRScrn);
                        }
                    }

                    break;
                }
                case RADIOB5:
                    radiopressed2 = true;
                    radiotext2 = "Gerador";
                break;
                case RADIOB6:
                    radiopressed2 = true;
                    radiotext2 = "Transportador";
                break;
                case RADIOB7:
                    radiopressed2 = true;
                    radiotext2 = "Receptor";
                break;
            }
        break;
        case WM_DESTROY:
            PostQuitMessage (0);
            break;
        case WM_CLOSE:
                DestroyWindow(MTRScrn);
            return 0;
        case WM_GETMINMAXINFO: {
            MINMAXINFO* info = (MINMAXINFO*)lParam;
            info->ptMinTrackSize.x = 450;
            info->ptMinTrackSize.y = 430;
            info->ptMaxTrackSize.x = 450;
            info->ptMaxTrackSize.y = 430;
            break;
        }
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

LRESULT CALLBACK CDFManager (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_CREATE: {
            compselected3 = false;

            CreateWindow(TEXT("BUTTON"), TEXT("Informações de Cabeçalho"), WS_VISIBLE | WS_CHILD | WS_BORDER | BS_GROUPBOX, 0, 0, 430, 220,
            hwnd, NULL, NULL, NULL);
            cdfEdit[0] = CreateWindow(TEXT("BUTTON"), TEXT("Empresa geradora"), WS_VISIBLE | WS_CHILD | WS_BORDER | BS_SPLITBUTTON, 10, 25, 410, 20,
			hwnd, (HMENU) CDF_ID+0, (HINSTANCE) lParam, NULL);
            cdfEdit[1] = CreateWindow(TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 10, 50, 410, 20 ,
			hwnd, (HMENU) CDF_ID+1, NULL, NULL);
            cdfEdit[2] = CreateWindow(TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 10, 75, 410, 20 ,
			hwnd, (HMENU) CDF_ID+2, NULL, NULL);
			cdfEdit[3] = CreateWindow(TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 10, 100, 410, 20 ,
			hwnd, (HMENU) CDF_ID+3, NULL, NULL);
			cdfEdit[4] = CreateWindow(TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 10, 125, 410, 20 ,
			hwnd, (HMENU) CDF_ID+4, NULL, NULL);
			cdfEdit[5] = CreateWindow(TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 10, 150, 410, 20 ,
			hwnd, (HMENU) CDF_ID+5, NULL, NULL);
			cdfEdit[6] = CreateWindow(TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 10, 175, 410, 20 ,
			hwnd, (HMENU) CDF_ID+6, NULL, NULL);

            CreateWindow(TEXT("BUTTON"), TEXT("Declaração de Recebimento"), WS_VISIBLE | WS_CHILD | WS_BORDER | BS_GROUPBOX, 0, 230, 430, 220,
            hwnd, NULL, NULL, NULL);
            cdfEdit[7] = CreateWindow(TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER |  ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL | WS_TABSTOP | ES_WANTRETURN, 10, 255, 410, 140 ,
			hwnd, (HMENU) CDF_ID+7, NULL, NULL);

			CreateWindow(TEXT("BUTTON"), TEXT("Gerar"), WS_VISIBLE | WS_CHILD | WS_BORDER, 295, 400, 125, 30,
            hwnd, (HMENU) CDF_GENERATE, NULL, NULL);

            for(int i = 1; i < 8; i++)
                SendMessage(cdfEdit[i], EM_SETCUEBANNER, false, (LPARAM)TEXT(cBanner_cdfEdit[i]));

            break;
        }
        case WM_COMMAND: {

            int lastid = GetLastId(FILE_ID_COSTUMER);
            if(wParam >= COSTUMER_LIST && wParam <= COSTUMER_LIST+lastid){
                GetMenuStringA(hSplitMenu5, wParam, companyname3, 30, NULL);
                SetWindowText(cdfEdit[0], companyname3);
                compselected3 = true;
                companyid = wParam - COSTUMER_LIST;
            }

            switch(wParam){
                case CDF_GENERATE: {

                    int cdf_data_size[8];
                    int total_size;
                    CHAR *get_cdf_data[8];
                    CHAR *concat = (CHAR*) malloc(800 * sizeof(CHAR));
                    concat[0] = {0};
                    int current_index = 0;
                    bool errordata = false;

                    for(int i = 0; i < 8; i++){
                            cdf_data_size[i] = GetWindowTextLength(cdfEdit[i]);
                            CHAR concat1[50] = {0};
                            if(!cdf_data_size[i]){
                                StringCchCat(concat1, 50, "O campo '");
                                StringCchCat(concat1, 50, cBanner_cdfEdit1[i]);
                                StringCchCat(concat1, 50, "' ");
                                StringCchCat(concat1, 50, " está vazio!");
                                MessageBox(CDFScrn, concat1, "Erro de cadastro", MB_OKCANCEL);
                                current_index = i;
                                errordata = true;
                                break;
                            }

                            if(!compselected3){
                                MessageBox(CDFScrn, "Algum dos botões não foram marcados!", "Erro de cadastro", MB_OKCANCEL);
                                current_index = i;
                                errordata = true;
                                break;
                            }

                            get_cdf_data[i] = (CHAR*) malloc(cdf_data_size[i] * sizeof(CHAR));
                            GetWindowText(cdfEdit[i], get_cdf_data[i], cdf_data_size[i]+1);
                            total_size += cdf_data_size[i];
                    }

                    if(errordata){
                        for(int j = 0; j < current_index; j++)
                            free(get_cdf_data[j]);

                        free(concat);
                        break;
                    }

                    concat = (CHAR*) realloc(concat, total_size);

                    /*
                    char num_id[10];
                    FILE *fileid = fopen(FILE_ID_REPORT, "r");
                    fgets(num_id, sizeof(num_id), fileid);
                    fclose(fileid);
                    */

                    for(int i = 0; i < 8; i++){
                        StringCchCat(concat, total_size, byteb);
                        StringCchCat(concat, total_size, cBanner_cdfEdit1[i]);
                        StringCchCat(concat, total_size, " : ");
                        StringCchCat(concat, total_size, get_cdf_data[i]);
                        StringCchCat(concat, total_size, "\n");
                    }

                    if(MessageBox(CDFScrn, concat, "Confirmação de dados", MB_OKCANCEL) == IDOK){

                        char typereport[5] = "CDF";
                        CHAR namefile[50] = {0};
                        CHAR dateformat[20] = {0};
                        CHAR sYear[10], sMonth[10], sDay[10], sHour[10], sMin[10];

                        SYSTEMTIME lt;
                        GetLocalTime(&lt);
                        itoa(lt.wYear, sYear, 10);
                        itoa(lt.wMonth, sMonth, 10);
                        itoa(lt.wDay, sDay, 10);
                        itoa(lt.wHour, sHour, 10);
                        itoa(lt.wMinute, sMin, 10);

                        StringCchCat(namefile, 50, FILE_CDF_COMPANY);
                        StringCchCat(namefile, 50, sYear);
                        if(strlen(sMonth) == 1)
                           StringCchCat(namefile, 50, "0");
                        StringCchCat(namefile, 50, sMonth);
                        if(strlen(sDay) == 1)
                           StringCchCat(namefile, 50, "0");
                        StringCchCat(namefile, 50, sDay);
                        if(strlen(sHour) == 1)
                            StringCchCat(namefile, 50, "0");
                        StringCchCat(namefile, 50, sHour);
                        if(strlen(sMin) == 1)
                           StringCchCat(namefile, 50, "0");
                        StringCchCat(namefile, 50, sMin);
                        StringCchCat(namefile, 50, W_EXT);

                        if(strlen(sDay) == 1)
                           StringCchCat(dateformat, 20, "0");
                        StringCchCat(dateformat, 20, sDay);
                        StringCchCat(dateformat, 20, "/");
                        if(strlen(sMonth) == 1)
                           StringCchCat(dateformat, 20, "0");
                        StringCchCat(dateformat, 20, sMonth);
                        StringCchCat(dateformat, 20, "/");
                        StringCchCat(dateformat, 20, sYear);
                        StringCchCat(dateformat, 20, " ");
                        if(strlen(sHour) == 1)
                            StringCchCat(dateformat, 20, "0");
                        StringCchCat(dateformat, 20, sHour);
                        StringCchCat(dateformat, 20, ":");
                        if(strlen(sMin) == 1)
                           StringCchCat(dateformat, 20, "0");
                        StringCchCat(dateformat, 20, sMin);

                        FILE *filew = fopen(namefile, "w");
                        fputs(concat, filew);
                        fclose(filew);

                        if(!isReport)
                            itemIndex = companyid;

                        reports = insertNode(reports, reportid, itemIndex, typereport, namefile, dateformat);

                        showList(reports);

                        CHAR buff0[MAX_PATH];
                        StringCbPrintf(buff0, ARRAYSIZE(buff0), TEXT("COMPANY_ID : %d\n"  \
                                        "TYPE_REPORT : %s\n" \
                                        "NAME_FILE : %s\n"   \
                                        "DATE_FORMAT : %s\n"),
                                reports->companyId, reports->typeReport, reports->nameFile, reports->dateFormat);

                        FILE *repw = fopen(FILE_REPORT, "a");
                        fputs(buff0, repw);
                        fclose(repw);

                        CHAR buff[MAX_PATH];
                        StringCbPrintf (buff, ARRAYSIZE(buff), TEXT("%s | %s | Relatório %s"),
                        IndData[itemIndex].companyName, dateformat, typereport);

                        if(isReport && IndStatus[itemIndex] == 2)
                            ListBox_DeleteString(hwndList6, itemPos);

                        ListBox_InsertString(hwndList7, 0, buff);
                        ListBox_SetCurSel(hwndList7, 0);
                        ListBox_SetItemData(hwndList7, 0, reportid);

                        char report_id[10];
                        reportid = GetLastId(FILE_ID_REPORT) + 1;
                        itoa(++reportid, report_id, 10);

                        FILE *fileidw = fopen(FILE_ID_REPORT, "w");
                        fputs(report_id, fileidw);
                        fclose(fileidw);

                        if(isReport){
                            CHAR sysconfirm[50] = {0};
                            CHAR num_stat[10] = {0};
                            CHAR num_id[10] = {0};
                            itoa(itemIndex, num_id, 10);
                            itoa(++IndStatus[itemIndex], num_stat, 10);
                            StringCchCat(sysconfirm, 50, FILE_STAT_COMPANY);
                            StringCchCat(sysconfirm, 50, num_id);
                            StringCchCat(sysconfirm, 50, EXTENSION);

                            FILE *filesys = fopen(sysconfirm, "w");
                            fputs(num_stat, filesys);
                            fclose(filesys);
                        }

                        for(int j = 1; j < 8; j++)
                                free(get_cdf_data[j]);

                        free(concat);

                        MessageBox(CDFScrn, "Cadastro das informações realizadas!", "Sucesso", MB_OKCANCEL);

                        DestroyWindow(CDFScrn);
                    }

                    break;
                }
            }
            break;
        }
        case WM_NOTIFY:
            switch (((LPNMHDR)lParam)->code) {
                case BCN_DROPDOWN:
                        NMBCHOTITEM* pDropDown = (NMBCHOTITEM*)lParam;
                        if (pDropDown->hdr.hwndFrom == GetDlgItem(hwnd, CDF_ID+0))
                        {
                            FILE* filer = fopen(FILE_COSTUMER, "r");

                            char buffer[100], company[100];
                            int line = 0, index = 0;

                            hSplitMenu5 = CreatePopupMenu();

                            if (filer != NULL) {
                                while (fgets(buffer, sizeof(buffer), filer) != NULL) {

                                    if(line == 6){
                                        GetDataField(company, buffer);
                                        AppendMenu(hSplitMenu5, MF_BYPOSITION, COSTUMER_LIST+(index++), company);
                                    }
                                    line = (line == 22) ? 0 : line + 1;
                                }
                            }else{
                                MessageBox(CDFScrn, "Por favor, cadastre um cliente!", "Error", MB_OKCANCEL);
                                break;
                            }

                            fclose(filer);

                            RECT Rect;
                            GetWindowRect(cdfEdit[0], &Rect);
                            TrackPopupMenu(hSplitMenu5, TPM_LEFTALIGN | TPM_TOPALIGN, Rect.left, Rect.top+30, 0, hwnd, NULL);

                            return TRUE;
                        }
                        break;
                    }
                return FALSE;
        case WM_DESTROY:
            PostQuitMessage (0);
            break;
        case WM_CLOSE:
                DestroyWindow(CDFScrn);
            return 0;
        case WM_GETMINMAXINFO: {
            MINMAXINFO* info = (MINMAXINFO*)lParam;
            info->ptMinTrackSize.x = 450;
            info->ptMinTrackSize.y = 500;
            info->ptMaxTrackSize.x = 450;
            info->ptMaxTrackSize.y = 500;
            break;
        }
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

LRESULT CALLBACK FDSRManager (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_CREATE: {
            compselected4 = false;

            CreateWindow(TEXT("BUTTON"), TEXT("Informações Iniciais"), WS_VISIBLE | WS_CHILD | WS_BORDER | BS_GROUPBOX, 0, 0, 430, 220,
            hwnd, NULL, NULL, NULL);
            fdsrEdit[0] = CreateWindow(TEXT("BUTTON"), TEXT("Empresa geradora"), WS_VISIBLE | WS_CHILD | WS_BORDER | BS_SPLITBUTTON, 10, 25, 410, 20 ,
			hwnd, (HMENU) FDSR_ID+0, (HINSTANCE) NULL, NULL);
            fdsrEdit[1] = CreateWindow(TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 10, 50, 410, 20 ,
			hwnd, (HMENU) FDSR_ID+1, NULL, NULL);
            fdsrEdit[2] = CreateWindow(TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 10, 75, 410, 20 ,
			hwnd, (HMENU) FDSR_ID+2, NULL, NULL);

            CreateWindow(TEXT("BUTTON"), TEXT("Medidas e Controles"), WS_VISIBLE | WS_CHILD | WS_BORDER | BS_GROUPBOX, 0, 100, 430, 350,
            hwnd, NULL, NULL, NULL);
            CreateWindow(TEXT("STATIC"), TEXT("Perigos inerentes do resíduo:"), WS_VISIBLE | WS_CHILD, 5, 125, 410, 20, hwnd, (HMENU) NULL, NULL, NULL);
            fdsrEdit[3] = CreateWindow(TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER |  ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL | WS_TABSTOP | ES_WANTRETURN, 10, 150, 410, 100,
			hwnd, (HMENU) FDSR_ID+3, NULL, NULL);
			CreateWindow(TEXT("STATIC"), TEXT("Medidas de primeiros socorros:"), WS_VISIBLE | WS_CHILD, 5, 255, 410, 20, hwnd, (HMENU) NULL, NULL, NULL);
			fdsrEdit[4] = CreateWindow(TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER |  ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL | WS_TABSTOP | ES_WANTRETURN, 10, 280, 410, 100,
			hwnd, (HMENU) FDSR_ID+4, NULL, NULL);

			CreateWindow(TEXT("BUTTON"), TEXT("Gerar"), WS_VISIBLE | WS_CHILD | WS_BORDER, 270, 400, 125, 30, hwnd, (HMENU) FDSR_GENERATE, NULL, NULL);

            SendMessage(fdsrEdit[1], EM_SETCUEBANNER, false, (LPARAM)TEXT(cBanner_fdsrEdit[1]));
            SendMessage(fdsrEdit[2], EM_SETCUEBANNER, false, (LPARAM)TEXT(cBanner_fdsrEdit[2]));

            break;
        }
        case WM_COMMAND: {

            int lastid = GetLastId(FILE_ID_COSTUMER);
            printf("ID: %d\n", lastid);
            if(wParam >= COSTUMER_LIST && wParam <= COSTUMER_LIST+lastid){
                GetMenuStringA(hSplitMenu6, wParam, companyname4, 30, NULL);
                SetWindowText(fdsrEdit[0], companyname4);
                compselected4 = true;
                companyid = wParam - COSTUMER_LIST;
            }

            switch(wParam){
                case FDSR_GENERATE: {
                    int fdsr_data_size[5];
                    int total_size;
                    CHAR *get_fdsr_data[5];
                    CHAR *concat = (CHAR*) malloc(2000 * sizeof(CHAR));
                    concat[0] = {0};
                    int current_index;
                    bool errordata = false;

                    for(int i = 1; i < 5; i++){
                            fdsr_data_size[i] = GetWindowTextLength(fdsrEdit[i]);
                            CHAR concat1[200] = {0};
                            if(!fdsr_data_size[i]){
                                StringCchCat(concat1, 200, "O campo '");
                                StringCchCat(concat1, 200, cBanner_fdsrEdit1[i]);
                                StringCchCat(concat1, 200, "' ");
                                StringCchCat(concat1, 200, " está vazio!");
                                MessageBox(FDSRScrn, concat1, "Erro de cadastro", MB_OKCANCEL);
                                current_index = i;
                                errordata = true;
                                break;
                            }

                            if(!compselected4){
                                MessageBox(FDSRScrn, "Algum dos botões não foram marcados!", "Erro de cadastro", MB_OKCANCEL);
                                current_index = i;
                                errordata = true;
                                break;
                            }

                            get_fdsr_data[i] = (CHAR*) malloc(fdsr_data_size[i] * sizeof(CHAR));
                            GetWindowText(fdsrEdit[i], get_fdsr_data[i], fdsr_data_size[i]+1);
                            total_size += fdsr_data_size[i];
                    }

                    if(errordata){
                        for(int j = 1; j < current_index; j++)
                            free(get_fdsr_data[j]);

                        free(concat);
                        break;
                    }

                    //concat = (CHAR*) realloc(concat, total_size);


                    char num_id[10];
                    FILE *fileid = fopen(FILE_ID_FDSR, "r");
                    fgets(num_id, sizeof(num_id), fileid);
                    fclose(fileid);

                    StringCchCat(concat, 2000, byteb);
                    StringCchCat(concat, 2000, "ID: ");
                    StringCchCat(concat, 2000, num_id);
                    StringCchCat(concat, 2000, "\n");

                    StringCchCat(concat, 2000, byteb);
                    StringCchCat(concat, 2000, "Empresa geradora");
                    StringCchCat(concat, 2000, " : ");
                    StringCchCat(concat, 2000, companyname4);
                    StringCchCat(concat, 2000, "\n");

                    for(int i = 1; i < 5; i++){
                        //strcat(concat, byteb);
                        StringCchCat(concat, 2000, byteb);
                        StringCchCat(concat, 2000, cBanner_fdsrEdit1[1]);
                        StringCchCat(concat, 2000, " : ");
                        StringCchCat(concat, 2000, get_fdsr_data[1]);
                        StringCchCat(concat, 2000, "\n");
                    }


                    if(MessageBox(FDSRScrn, concat, "Confirmação de dados", MB_OKCANCEL) == IDOK){

                        FILE *filew = fopen(FILE_FDSR, "a");
                        fputs(concat, filew);
                        fclose(filew);

                        /*
                        int id = atoi(num_id) + 1;
                        itoa(id, num_id, 10);

                        FILE *fileidw = fopen(FILE_ID_FDSR, "w");
                        fputs(num_id, fileidw);
                        fclose(fileidw);
                        */




                        for(int j = 1; j < 5; j++)
                            free(get_fdsr_data[j]);

                        free(concat);

                        MessageBox(FDSRScrn, "Cadastro das informações realizadas!", "Sucesso", MB_OKCANCEL);
                        DestroyWindow(FDSRScrn);
                    }

                    break;
                }
            }
            break;
        }
        case WM_NOTIFY:
            switch (((LPNMHDR)lParam)->code) {
                case BCN_DROPDOWN:
                        NMBCHOTITEM* pDropDown = (NMBCHOTITEM*)lParam;
                        if (pDropDown->hdr.hwndFrom == GetDlgItem(hwnd, FDSR_ID+0))
                        {
                            FILE* filer = fopen(FILE_COSTUMER, "r");

                            char buffer[100];
                            char company[100];
                            int line = 0;
                            int index = 0;

                            hSplitMenu6 = CreatePopupMenu();

                            if (filer != NULL) {
                                while (fgets(buffer, sizeof(buffer), filer) != NULL) {

                                    if(line == 6){
                                        GetDataField(company, buffer);
                                        AppendMenu(hSplitMenu6, MF_BYPOSITION, COSTUMER_LIST+(index++), company);
                                    }

                                    line = (line == 22) ? 0 : line + 1;
                                }
                            }else{
                                MessageBox(FDSRScrn, "Por favor, cadastre um cliente!", "Error", MB_OKCANCEL);
                                break;
                            }

                            fclose(filer);

                            RECT Rect;
                            GetWindowRect(fdsrEdit[0], &Rect);
                            TrackPopupMenu(hSplitMenu6, TPM_LEFTALIGN | TPM_TOPALIGN, Rect.left, Rect.top+30, 0, hwnd, NULL);

                            return TRUE;
                        }
                        break;
                    }
                return FALSE;
        case WM_DESTROY:
            PostQuitMessage (0);
            break;
        case WM_CLOSE:
                DestroyWindow(FDSRScrn);
            return 0;
        case WM_GETMINMAXINFO: {
            MINMAXINFO* info = (MINMAXINFO*)lParam;
            info->ptMinTrackSize.x = 450;
            info->ptMinTrackSize.y = 500;
            info->ptMaxTrackSize.x = 450;
            info->ptMaxTrackSize.y = 500;
            break;
        }
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

/*
Macro para deletar String:
    1ª parametro : Caixa de listagem
    2ª parametro : indíce do item
    ListBox_DeleteString(hwndList5, 0);

*/



