#pragma once
#include "../framework.h"
#include <windowsx.h>
#include <dbt.h>

#include <iostream>
#include <vector>

void QuerySerialPorts(std::vector<std::wstring> &ports);
void init_com_port(void);
void UpdateComPorts(const std::wstring &portName, bool added);
void RelayoutComPorts(void);

void On_Size(HWND hWnd, WPARAM wParam, LPARAM lParam);
void On_paint(HWND hWnd, WPARAM wParam, LPARAM lParam);
void On_device_change(HWND hWnd, WPARAM wParam, LPARAM lParam);
void On_HWND_TOPMOST_CHECK___clicked(HWND hWnd, WPARAM wParam, LPARAM lParam);

// 串口读取线程的函数声明
DWORD WINAPI Serial_Read_Thread(LPVOID lpParam);
DWORD WINAPI Serial_Write_Thread(LPVOID lpParam);
