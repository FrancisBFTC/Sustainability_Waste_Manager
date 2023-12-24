#if !defined(_WASTEMANAGER_H_)
    #define _WASTEMANAGER_H_

#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <winuser.h>
#include <windowsx.h>
#include <commdlg.h>


#pragma comment(lib, "comctl32.lib")
#include <commctrl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sysinfoapi.h>

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' \
version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define LOGINW_X    390
#define LOGINW_Y    330
#define SIGNW_X     440
#define SIGNW_Y     410
#define COSTW_X     665
#define COSTW_Y     550
#define WASTEW_X    440
#define WASTEW_Y    310
#define MNGRW_X     440
#define MNGRW_Y     410
#define SESSW_X     1300
#define SESSW_Y     500

#define FILE_COSTUMER   "data/registers/costumer.data"
#define FILE_WASTE      "data/registers/waste.data"
#define FILE_SIGN       "data/registers/sign.data"
#define FILE_MANAGER    "data/registers/manager.data"
#define FILE_REPORT     "data/registers/report.data"
#define FILE_CDF        "data/registers/cdf.data"
#define FILE_FDSR       "data/registers/fdsr.data"

#define FILE_ID_COSTUMER    "data/ids/costumer.id"
#define FILE_ID_WASTE       "data/ids/waste.id"
#define FILE_ID_SIGN        "data/ids/sign.id"
#define FILE_ID_MANAGER     "data/ids/manager.id"
#define FILE_ID_REPORT      "data/ids/report.id"
#define FILE_ID_FDSR        "data/ids/fdsr.id"

#define FILE_STAT_COMPANY   "data/status/company"
#define EXTENSION           ".sts"
#define FILE_INSP_COMPANY   "data/reports/inspection"
#define W_EXT               ".w"
#define FILE_MTR_COMPANY    "data/reports/MTR"
#define FILE_CDF_COMPANY    "data/reports/CDF"
#define FILE_FDS_COMPANY    "data/reports/FDS"

#define IDI_WASTE       178
#define IDI_LOGIN       179
#define IDI_REPORT      180

LRESULT CALLBACK LoginManager (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK SignManager (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK CostumerManager (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WasteManager (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DataManager (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK SessionManager (HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK MTRManager (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK CDFManager (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK FDSRManager (HWND, UINT, WPARAM, LPARAM);

WNDCLASSEX loginClass, signClass, costumerClass,
           wasteClass, managerClass, sessionClass,
           MTRClass, CDFClass, FDSRClass;
HWND LoginScrn, SignScrn, CostumerScrn,
     WasteScrn, ManagerScrn, SessionScrn,
     MTRScrn, CDFScrn, FDSRScrn;
HINSTANCE instance1, instance2, instance3,
          instance4, instance5, instance6,
          instance7, instance8, instance9;

LPSTR fileArgv = NULL;
CHAR *data;

HFONT hFont;
HWND hwndList1, hwndList2, hwndList3, hwndList4, hwndList5, hwndList6, hwndList7;

CHAR classNameLogin[ ]  = "LoginScrn";
CHAR classNameSign[ ]   = "SignScrn";
CHAR classNameCost[ ]   = "CostumerScrn";
CHAR classNameWaste[ ]  = "WasteScrn";
CHAR classNameMngr[ ]   = "ManagerScrn";
CHAR classNameSess[ ]   = "SessionScrn";
CHAR classNameMTR[ ]    = "MTRScrn";
CHAR classNameCDF[ ]    = "CDFScrn";
CHAR classNameFDSR[ ]   = "FDSRScrn";

CHAR loginTitle[ ]      = "System - Login do Usuário";
CHAR signTitle[ ]       = "System - Cadastro do Usuário";
CHAR costTitle[ ]       = "System - Cadastro de Clientes";
CHAR wasteTitle[ ]      = "System - Cadastro de Resíduos";
CHAR managerTitle[ ]    = "System - Cadastro de Gestão";
CHAR sessionTitle[ ]    = "System - Tela de Administração";
CHAR MTRTitle[ ]        = "System - Relatório MTR";
CHAR CDFTitle[ ]        = "System - Relatório CDF";
CHAR FDSRTitle[ ]       = "System - Relatório FDSR";

CHAR classNameMenu[ ]   = "WasteMenu";

HWND lEdit1, lEdit2;
HWND sEdit1, sEdit2, sEdit3, sEdit4;

HWND cEdit[23];
HWND wEdit[6];
HWND spltBtn1, spltBtn2;
HWND mEdit[7];
HWND mspltBtn1;

HWND mtrEdit[12];
HWND cdfEdit[8];
HWND fdsrEdit[5];


HWND weekCollection;
char byteb[ ] = " ";

HMENU hSplitMenu, hSplitMenu2, hSplitMenu3, hSplitMenu4, hSplitMenu5, hSplitMenu6, hSplitMenu7, hSplitMenu8;
CHAR *months[ ] =   {
                        "Janeiro", "Fevereiro", "Março", "Abril", "Maio",
                        "Junho", "Agosto", "Setembro", "Outubro", "Novembro",
                        "Dezembro"
                    };
CHAR *days[ ] =   {
                        "Segunda-feira", "Terça-feira", "Quarta-feira", "Quinta-feira",
                        "Sexta-feira"
                  };

const wchar_t *cBanner_wEdit[6] = {L"Residuo tratado...", L"Data do tratamento...",
                                L"Hora do tratamento...", L"Quant. residuos...",
                                L"Tecnologia usada...", L"Custo de tratamento..."};
const char *cBanner_wEdit1[6] = {"Residuo tratado", "Data do tratamento",
                                "Hora do tratamento", "Quant. residuos",
                                "Tecnologia usada", "Custo de tratamento"};

const wchar_t *cBanner_cEdit[22] = {L"Nome do empresario ...", L"Endereco de email ...", L"Telefone 1 ...",
                                    L"Telefone 2 ...", L"Endereco residencial ...", L"Nome da empresa ...",
                                    L"Nome fantasia ...", L"Localizacao da empresa ...", L"CNPJ da empresa...",
                                    L"Data de Abertura ...", L"Atividade da empresa ...", L"Formas de atuacao ...",
                                    L"Selecione a Classe ...", L"Quant. Media Total ...", L"Periculosidade da Classe ...",
                                    L"Descricoes Gerais ...", L"Data de Coleta ...", L"Horario ...", L"Modelo do Transporte ...",
                                    L"Limite de Carga ...", L"Viagem ...", L"Quant. ..."};
const char *cBanner_cEdit1[22] = {"Nome do empresario", "Endereco de email", "Telefone 1",
                                    "Telefone 2", "Endereco residencial", "Nome da empresa",
                                    "Nome fantasia", "Localizacao da empresa", "CNPJ da empresa",
                                    "Data de Abertura", "Atividade da empresa", "Formas de atuacao",
                                    "Selecione a Classe", "Quant. Media Total", "Periculosidade da Classe",
                                    "Descricoes Gerais", "Data de Coleta", "Horario", "Modelo do Transporte",
                                    "Limite de Carga", "Viagem", "Quant."};

const wchar_t *cBanner_mEdit[7] = {L"Investimento mensal ...", L"Residuos gerados ...",
                                   L" ", L"Quant. gerada ...", L"Data de destinacao ...",
                                   L"Data de tratamento ...", L"Data de tratamento..."};
const char *cBanner_mEdit1[7] = {"Investimento mensal", "Residuos gerados",
                                   "É perigoso?", "Quant. gerada", "Data de destinacao",
                                   "Data de tratamento", "Data de tratamento"};

const wchar_t *cBanner_mtrEdit[12] = {L"", L"", L"", L"CNPJ da via", L"Nome completo do gerador",
                                    L"Cargo do gerador", L"Nome do motorista da transportadora", L"Placa do veiculo",
                                    L"Data agendada para coleta", L"Identificacao do residuo", L"Quantidade em metros cubicos (m3)",
                                    L"Peso em Quilogramas (Kg)"};
const char *cBanner_mtrEdit1[12] = {"", "", "", "CNPJ da via", "Nome completo do gerador",
                                    "Cargo do gerador", "Nome do motorista da transportadora", "Placa do veiculo",
                                    "Data agendada para coleta", "Identificacao do residuo", "Quantidade em metros cubicos (m3)",
                                    "Peso em Quilogramas (Kg)"};


const wchar_t *cBanner_cdfEdit[8] = {L"Empresa geradora", L"CNPJ do tratador", L"Localizacao do tratador",
                                      L"Nome do tratador", L"LAO do Destinador (Licenca Ambiental de Operacao)", L"Prazo de Validade da LAO",
                                      L"Codigo da Atividade do Destinador", L""};
const char *cBanner_cdfEdit1[8] = {"Empresa geradora", "CNPJ do tratador", "Localizacao do tratador",
                                      "Nome do tratador", "LAO do Destinador (Licenca Ambiental de Operacao)", "Prazo de Validade da LAO",
                                      "Codigo da Atividade do Destinador", "Declaração de recebimento"};

const wchar_t *cBanner_fdsrEdit[5] = {L"", L"Identificacao do residuo quimico", L"Componentes quimicos do residuo", L"Perigos inerentes do residuo",
                                      L"Medidas de primeiros socorros"};
const char *cBanner_fdsrEdit1[5] =   {"Empresa Geradora", "Identificação do resíduo químico", "Componentes químicos do resíduo", "Perigos inerentes do resíduo",
                                      "Medidas de primeiros socorros"};


CHAR textmonth[10];
CHAR textdate[20];
CHAR yesorno[5];
CHAR companyname1[30];
CHAR companyname2[30];
CHAR companyname3[30];
CHAR companyname4[30];
CHAR *radiotext, *radiotext2;

bool isupdate = false;
bool parentstate = false;
bool statelogin = false;
bool childstate = false;
bool radiopressed = false;
bool radiopressed2 = false;
bool monthselected = false;
bool compselected = false;
bool compselected2 = false;
bool compselected3 = false;
bool compselected4 = false;
bool compselected5 = false;
bool dateselected = false;
bool yesstate = false;


#define TEXT_USER       1
#define TEXT_PASS       2

#define NEW_USER        3
#define EMAIL           4
#define NEW_PASS        5
#define PASS_CONFIRM    6
#define SIGN            7

#define LOGGIN          8
#define REGISTER        9

#define RADIOB1         10
#define RADIOB2         11
#define RADIOB3         12
#define RADIOB4         13
#define COSTUMER_NAME   14
#define COSTUMER_EMAIL  15
#define PERSONAL_PHONE  16
#define WORK_PHONE      17
#define PERSONAL_ADDR   18
#define COMPANY_NAME    19
#define FANTASY_NAME    20
#define COMPANY_ADDR    21
#define CNPJ_NUMBER     22
#define OPENING_DATE    23
#define COMPANY_ATIVITY 24
#define ACTION_FORMS    25
#define WASTE_CLASS     26
#define WASTE_COUNT     27
#define WASTE_DANGER    28
#define WASTE_DESCR     29
#define COLLECT_DATE    30
#define COLLECT_HOUR    31
#define TRUCK_SIZE      32
#define TRUCK_MODEL     33
#define TRAVEL_TIME     34
#define TRUCK_COUNTS    35

#define COSTUMER_SAVE   36

#define SELECT_MONTH    37
#define SELECT_COMPANY  38

#define JAN_OP 39
#define FEB_OP 40
#define MAR_OP 41
#define ABR_OP 42
#define MAI_OP 43
#define JUN_OP 44
#define AUG_OP 45
#define SET_OP 46
#define OUT_OP 47
#define NOV_OP 48
#define DEZ_OP 49

#define TREATED_WASTE   50
#define TREATED_DATE    51
#define TREATED_HOUR    52
#define WASTE_AMMOUNT   53
#define USED_TECNO      54
#define TREATED_COST    55

#define WASTE_SAVE      56

#define ID_COMPANY      57
#define FINANCIAL       58
#define CREATED_WASTE   59
#define IS_DANGER       60
#define AMMOUNT_CREATED 61
#define DEST_SCHED      62
#define TREATED_SCHED   63
#define INSPECTION      64

#define MNG_BTN_REGIST  65
#define MNG_BTN_UPDATE  66
#define MNG_BTN_REPORT  67

#define YES_OP      68
#define NO_OP       69

#define RADIOB5     70
#define RADIOB6     71
#define RADIOB7     72
#define MTR_ID      73
#define MTR_GENERATE    82
#define CDF_GENERATE    83
#define FDSR_GENERATE   84
#define CDF_ID          85
#define FDSR_ID         93

#define MENU_MTR        150
#define MENU_CDF        151
#define MENU_FDSR       152
#define MENU_INSPECT    153

#define MENU_SEMMI_MANUAL   154
#define MENU_PROFITS        155
#define MENU_PRODUC         156
#define OPEN                157
#define LOGGOFF             158

#define MENU_COSTUMER_REG   159
#define MENU_WORKER_REG     160
#define MENU_COSTUMER_UPD   161
#define MENU_WASTE_UPD      162

#define MENU_ABOUT_SOFT     163
#define MENU_TUTORIAL       164

#define IDC_LISTBOX1  165
#define IDC_LISTBOX2  166
#define IDC_LISTBOX3  167
#define IDC_LISTBOX4  168
#define IDC_LISTBOX5  169
#define IDC_LISTBOX6  170
#define IDC_LISTBOX7  171

#define SEG_OP 172
#define TER_OP 173
#define QUA_OP 174
#define QUI_OP 175
#define SEX_OP 176

#define COSTUMER_UPD    177
#define COSTUMER_LIST   200

#define VK_ENTER 0x0D

#endif

