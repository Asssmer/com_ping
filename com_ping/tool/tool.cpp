#include "tool.h"



void log_error_impl(const char *customMessage, int lineNumber)
{
    DWORD dwError = GetLastError();
    std::cout << customMessage << " at line " << lineNumber << "\n";
    std::cout << "Error No: " << dwError << std::endl;
    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
    std::cout << "Error Message: " << messageBuffer << std::endl;
    LocalFree(messageBuffer);
}
BOOL CreateConsole(void)
{
    AllocConsole();

    FILE *fp;

    fflush(stdin);
    fflush(stdout);
    fflush(stderr);

    // 重定向 STDOUT
    freopen_s(&fp, "CONOUT$", "w", stdout);
    if (!fp)
        return FALSE;
    //  setvbuf(stdout, NULL, _IONBF, 0); // 禁用stdout的缓冲
    // 重定向 STDIN
    freopen_s(&fp, "CONIN$", "r", stdin);
    if (!fp)
        return FALSE;

    // 重定向 STDERR
    freopen_s(&fp, "CONOUT$", "w", stderr);
    if (!fp)
        return FALSE;
    return TRUE;
    // FreeConsole();
}
