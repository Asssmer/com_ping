#include "framework.h"
#include "com_ping.h"
#include "resource.h"
using namespace std;

// 控件标识符
#define ID_TOPMOST_CHECK__ 100

const bool ADD_console = true;
//-->全局变量<--
HINSTANCE hInst;
HWND hWnd___________ = 0;
HWND HWND_TOPMOST_CHECK__ = 0;
HWND hButton____ = 0;

// 线程全局变量
HANDLE g_com_read_hThread = NULL;

std::vector<HWND> serialPorts_HWND;
std::vector<HWND> serialPorts_checked_HWND;

std::atomic<bool> g_com_read_hThread_running{true};  // 原子操作保证线程安全
std::atomic<bool> g_com_write_hThread_running{true}; // 原子操作保证线程安全
// 文件句柄

//-->函数声明<--
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
//-->入口<--
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // 构建一个控制台-----------------------------------------
    if (ADD_console)
    {
        CreateConsole();
    }
    //-------------------------------------------------------
    hInst = hInstance;
    WNDCLASSEXW wcex = {0};

    wcex.style = CS_HREDRAW | CS_VREDRAW;                         // 类样式
    wcex.hbrBackground = (HBRUSH)(COLOR_MENU + 1);                // 类背景画笔的句柄
    wcex.lpszClassName = TEXT("main_window_class");               // 如果 lpszClassName 是字符串，则指定窗口类名
    wcex.cbSize = sizeof(WNDCLASSEX);                             // 此结构的大小（以字节为单位）
    wcex.lpfnWndProc = WndProc;                                   // 窗口过程的指针
    wcex.hInstance = hInst;                                       // 实例的句柄
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);                   // 类游标的句柄
    wcex.cbClsExtra = 0;                                          // 根据窗口类结构分配的额外字节数
    wcex.cbWndExtra = 0;                                          // 窗口实例之后分配的额外字节数
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)); // 类图标的句柄
    wcex.lpszMenuName = NULL;                                     // 类菜单的资源名称
    wcex.hIconSm = NULL;
    RegisterClassExW(&wcex);

    hWnd___________ = CreateWindowEx(
        NULL,                                                        // 扩展窗口样式
        TEXT("main_window_class"),                                   // RegisterClassEx 注册的任何名称
        TEXT("串口列表"),                                            // 窗口名称,控件的文本
        WS_OVERLAPPEDWINDOW & (~WS_MAXIMIZEBOX) & (~WS_MINIMIZEBOX), // 正在创建的窗口的样式
        // GetSystemMetrics(SM_CXSCREEN) - 200,                         // 窗口的初始水平位置
        GetSystemMetrics(SM_CXSCREEN) - 1200, // 窗口的初始水平位置
        0,                                    // 窗口的初始垂直位置
        // 140,                                                         // 窗口的宽度（以设备单位为单位）
        800,     // 窗口的宽度（以设备单位为单位）
        600,     // 窗口的高度（以设备单位为单位）
        nullptr, // 所有者窗口的句柄
        nullptr, // 子窗口标识符
        hInst,   // 与窗口关联的模块实例的句柄。
        nullptr);

    HWND_TOPMOST_CHECK__ = CreateWindowW(L"BUTTON", L"置顶", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 10, 0, 100, 20, hWnd___________, (HMENU)ID_TOPMOST_CHECK__, NULL, NULL);

    init_com_port();

    ShowWindow(hWnd___________, nCmdShow);
    UpdateWindow(hWnd___________);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
    case WM_CREATE:
    case WM_NOTIFY:
        break;
    case WM_COMMAND:
        //! 消息源	wParam（高字）	wParam（低字）	       lParam
        //! 菜单	0	           菜单标识符 (IDM_*)	   0
        //! 加速器	1	           加速器标识符 (IDM_*)	   0
        //! 控件	通知代码	   控制标识符	            控制窗口的句柄
        if (LOWORD(wParam) == ID_TOPMOST_CHECK__)
        {
            if (HIWORD(wParam) == BN_CLICKED)
            {
                wcout << L"Button Clicked" << endl;
                On_HWND_TOPMOST_CHECK___clicked(hWnd, wParam, lParam);
            }
        }
        break;
    case WM_TIMER:
        break;
    case WM_SIZE:
        On_Size(hWnd, wParam, lParam);
        break;
    case WM_LBUTTONDOWN:
        break;
    case WM_PAINT:
        On_paint(hWnd, wParam, lParam);
        break;
    case WM_DESTROY:
        if (ADD_console)
        {
            FreeConsole();
        }
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
        return 0;
    }
}
