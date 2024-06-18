#pragma once
#include "../framework.h"
#include <windows.h>
#include <iostream>
#include <vector>
#define log_error(msg) log_error_impl(msg, __LINE__)

void log_error_impl(const char* customMessage, int lineNumber);
BOOL CreateConsole(void);