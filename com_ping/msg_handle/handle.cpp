#include "handle.h"
#include "../tool/tool.h"
#include <windows.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

//! MessageBox(
//!     hWnd,
//!     (LPCWSTR)L"����?", // ����
//!     (LPCWSTR)L"����",                                   // ����
//!    MB_OKCANCEL|MB_ICONINFORMATION);

//! ȫ�ֱ���
// static HANDLE g_file_com = INVALID_HANDLE_VALUE;

//! ��װ
void QuerySerialPorts(std::vector<std::wstring> &ports)
{
    HKEY hKey;
    // ��ע����
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        TCHAR valueName[100];
        WCHAR portName[100]; // ����Ϊ WCHAR ����
        DWORD valueNameSize, portNameSize, type;
        DWORD i = 0;

        // ö������ֵ
        while (true)
        {
            valueNameSize = sizeof(valueName) / sizeof(TCHAR);
            portNameSize = sizeof(portName); // ע�ⵥλ���ֽ�
            if (RegEnumValue(hKey, i, valueName, &valueNameSize, NULL, &type, (LPBYTE)portName, &portNameSize) == ERROR_SUCCESS)
            {
                if (type == REG_SZ) // ȷ�����ַ�������
                {
                    ports.push_back(portName); // ��ӵ�����
                }
                i++;
            }
            else
            {
                break; // �˳�ѭ��
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
    extern HWND hWnd___________; // �����ھ��
    extern HINSTANCE hInst;      // Ӧ�ó���ʵ�����
    // ��ѯ��ǰ���õĴ���
    std::vector<std::wstring> serialPorts;
    QuerySerialPorts(serialPorts);
    // ���پɿؼ�����������
    for (HWND hwnd : serialPorts_HWND)
    {
        RemoveProp(hwnd, L"PortName");
        DestroyWindow(hwnd);
    }
    serialPorts_HWND.clear();

    // �����¿ؼ�����������
    int yPos = 30;
    int buttonHeight = 20;
    int buttonWidth = 100;
    int verticalSpacing = 25;
    for (const auto &portName : serialPorts)
    {
        HWND hCom = CreateWindowW(L"BUTTON", portName.c_str(), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                                  10, yPos, buttonWidth, buttonHeight, hWnd___________, NULL, hInst, NULL);
        serialPorts_HWND.push_back(hCom);
        SetProp(hCom, L"PortName", (HANDLE) new wstring(portName)); // ��������
        yPos += verticalSpacing;                                    // ������һ����ť�Ĵ�ֱλ��
    }
}
void UpdateComPorts(const std::wstring &portName, bool added)
{
    extern std::vector<HWND> serialPorts_HWND;
    extern HWND hWnd___________; // �����ھ��
    extern HINSTANCE hInst;      // Ӧ�ó���ʵ�����
    if (added)
    {
        // ����µĿؼ�
        int yPos = 30 + serialPorts_HWND.size() * 25;
        HWND hCom = CreateWindowW(L"BUTTON", portName.c_str(), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                                  10, yPos, 100, 20, hWnd___________, NULL, hInst, NULL);
        serialPorts_HWND.push_back(hCom);
        SetProp(hCom, L"PortName", (HANDLE) new std::wstring(portName)); // ��������
    }
    else
    {
        // �Ƴ�ָ���Ŀؼ�
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
        //! �Ƴ�ѡ���Ĵ���
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
    // TODO: �ڴ˴����ʹ�� hdc ���κλ�ͼ����...
    EndPaint(hWnd, &ps);
}

void On_device_change(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    cout << "On_device_change\n";
    PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)lParam;
    if (wParam == DBT_DEVICEARRIVAL && pHdr->dbch_devicetype == DBT_DEVTYP_PORT)
    {
        PDEV_BROADCAST_PORT pPort = (PDEV_BROADCAST_PORT)pHdr;
        // wcout << pPort->dbcp_name << "��������\n";
        UpdateComPorts(pPort->dbcp_name, true); // ����µĴ��ڿؼ�
    }
    else if (wParam == DBT_DEVICEREMOVECOMPLETE && pHdr->dbch_devicetype == DBT_DEVTYP_PORT)
    {
        PDEV_BROADCAST_PORT pPort = (PDEV_BROADCAST_PORT)pHdr;
        // wcout << pPort->dbcp_name << "���ڰγ�\n";
        UpdateComPorts(pPort->dbcp_name, false); // �Ƴ����ڿؼ�
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

//! �̶߳���
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
            // �����ȡ��������
            cout << buffer;
        }
    }
    return 0;
}
DWORD WINAPI Serial_Write_Thread(LPVOID lpParam)
{
    const char *dataToSend = "Test data"; // Ҫ���͵�����
    extern std::atomic<bool> g_com_write_hThread_running;

    HANDLE file_com = *(HANDLE *)lpParam;
    DWORD bytesWritten;
    while (g_com_write_hThread_running)
    {
        WriteFile(file_com, dataToSend, strlen(dataToSend), &bytesWritten, NULL);
    }
    return 0;
}
