#include "handle.h"
#include "../tool/tool.h"
#include <windows.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

//! MessageBox(
//!     hWnd,
//!     (LPCWSTR)L"内容?", // 内容
//!     (LPCWSTR)L"标题",                                   // 标题
//!    MB_OKCANCEL|MB_ICONINFORMATION);

//! 全局变量
// static HANDLE g_file_com = INVALID_HANDLE_VALUE;

//! 封装
void QuerySerialPorts(std::vector<std::wstring> &ports)
{
    HKEY hKey;
    // 打开注册表键
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        TCHAR valueName[100];
        WCHAR portName[100]; // 定义为 WCHAR 类型
        DWORD valueNameSize, portNameSize, type;
        DWORD i = 0;

        // 枚举所有值
        while (true)
        {
            valueNameSize = sizeof(valueName) / sizeof(TCHAR);
            portNameSize = sizeof(portName); // 注意单位是字节
            if (RegEnumValue(hKey, i, valueName, &valueNameSize, NULL, &type, (LPBYTE)portName, &portNameSize) == ERROR_SUCCESS)
            {
                if (type == REG_SZ) // 确保是字符串类型
                {
                    ports.push_back(portName); // 添加到向量
                }
                i++;
            }
            else
            {
                break; // 退出循环
            }
        }

        RegCloseKey(hKey);
    }
    else
    {
        std::wcout << L"Failed to open registry key" << std::endl;
    }
}
void init_com_port(void)
{
    extern std::vector<HWND> serialPorts_HWND;
    extern HWND hWnd___________; // 父窗口句柄
    extern HINSTANCE hInst;      // 应用程序实例句柄
    // 查询当前可用的串口
    std::vector<std::wstring> serialPorts;
    QuerySerialPorts(serialPorts);
    // 销毁旧控件并清理属性
    for (HWND hwnd : serialPorts_HWND)
    {
        RemoveProp(hwnd, L"PortName");
        DestroyWindow(hwnd);
    }
    serialPorts_HWND.clear();

    // 创建新控件并设置属性
    int yPos = 30;
    int buttonHeight = 20;
    int buttonWidth = 100;
    int verticalSpacing = 25;
    for (const auto &portName : serialPorts)
    {
        HWND hCom = CreateWindowW(L"BUTTON", portName.c_str(), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                                  10, yPos, buttonWidth, buttonHeight, hWnd___________, NULL, hInst, NULL);
        serialPorts_HWND.push_back(hCom);
        SetProp(hCom, L"PortName", (HANDLE) new wstring(portName)); // 设置属性
        yPos += verticalSpacing;                                    // 更新下一个按钮的垂直位置
    }
}
void UpdateComPorts(const std::wstring &portName, bool added)
{
    extern std::vector<HWND> serialPorts_HWND;
    extern HWND hWnd___________; // 父窗口句柄
    extern HINSTANCE hInst;      // 应用程序实例句柄
    if (added)
    {
        // 添加新的控件
        int yPos = 30 + serialPorts_HWND.size() * 25;
        HWND hCom = CreateWindowW(L"BUTTON", portName.c_str(), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                                  10, yPos, 100, 20, hWnd___________, NULL, hInst, NULL);
        serialPorts_HWND.push_back(hCom);
        SetProp(hCom, L"PortName", (HANDLE) new std::wstring(portName)); // 设置属性
    }
    else
    {
        // 移除指定的控件
        auto it = std::find_if(serialPorts_HWND.begin(), serialPorts_HWND.end(), [&portName](HWND hCom)
                               {
            wchar_t className[256];
            GetWindowText(hCom, className, 256);
            return std::wstring(className) == portName; });
        if (it != serialPorts_HWND.end())
        {
            DestroyWindow(*it);
            serialPorts_HWND.erase(it);
            RelayoutComPorts();
        }
        //! 移除选定的串口
    }
}
void RelayoutComPorts(void)
{
    extern std::vector<HWND> serialPorts_HWND;
    int yPos = 30;
    int verticalSpacing = 25;
    for (HWND hCom : serialPorts_HWND)
    {
        SetWindowPos(hCom, NULL, 10, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
        yPos += verticalSpacing;
    }
}

// handle
void On_Size(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    int width = LOWORD(lParam);
    int height = HIWORD(lParam);
}

void On_paint(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    // TODO: 在此处添加使用 hdc 的任何绘图代码...
    EndPaint(hWnd, &ps);
}

void On_device_change(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    cout << "On_device_change\n";
    PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)lParam;
    if (wParam == DBT_DEVICEARRIVAL && pHdr->dbch_devicetype == DBT_DEVTYP_PORT)
    {
        PDEV_BROADCAST_PORT pPort = (PDEV_BROADCAST_PORT)pHdr;
        // wcout << pPort->dbcp_name << "串口增加\n";
        UpdateComPorts(pPort->dbcp_name, true); // 添加新的串口控件
    }
    else if (wParam == DBT_DEVICEREMOVECOMPLETE && pHdr->dbch_devicetype == DBT_DEVTYP_PORT)
    {
        PDEV_BROADCAST_PORT pPort = (PDEV_BROADCAST_PORT)pHdr;
        // wcout << pPort->dbcp_name << "串口拔出\n";
        UpdateComPorts(pPort->dbcp_name, false); // 移除串口控件
    }
}

void On_HWND_TOPMOST_CHECK___clicked(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    HWND HWND__ = (HWND)lParam;
    LRESULT checkState = SendMessage(HWND__, BM_GETCHECK, 0, 0);
    if (checkState == BST_CHECKED)
    {
        cout << "BST_CHECKED\n";
        SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }
    else if (checkState == BST_UNCHECKED)
    {
        SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        cout << "BST_UNCHECKED\n";
    }
}

//! 线程定义
DWORD WINAPI Serial_Read_Thread(LPVOID lpParam)
{
    char buffer;
    extern std::atomic<bool> g_com_read_hThread_running;

    HANDLE file_com = *(HANDLE *)lpParam;
    DWORD bytesRead;
    while (g_com_read_hThread_running && ReadFile(file_com, &buffer, 1, &bytesRead, NULL))
    {
        if (bytesRead > 0)
        {
            // 输出读取到的数据
            cout << buffer;
        }
    }
    return 0;
}
DWORD WINAPI Serial_Write_Thread(LPVOID lpParam)
{
    const char *dataToSend = "Test data"; // 要发送的数据
    extern std::atomic<bool> g_com_write_hThread_running;

    HANDLE file_com = *(HANDLE *)lpParam;
    DWORD bytesWritten;
    while (g_com_write_hThread_running)
    {
        WriteFile(file_com, dataToSend, strlen(dataToSend), &bytesWritten, NULL);
    }
    return 0;
}
