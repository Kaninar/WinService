#include <iostream>
#include <windows.h>
#include "res.h"

using namespace std;

SC_HANDLE hService, hServiceControlManager;
SERVICE_STATUS g_ServiceStatus = {0};
SERVICE_STATUS_HANDLE g_StatusHandle = NULL;
HANDLE g_ServiceStopEvent = INVALID_HANDLE_VALUE;

VOID WINAPI ServiceMain(DWORD argc, LPTSTR *argv);
VOID WINAPI ServiceCtrlHandler(DWORD);
DWORD WINAPI ServiceWorkerThread(LPVOID lpParam);

VOID WINAPI ServiceMain(DWORD argc, LPTSTR *argv)
{
    DWORD Status = E_FAIL;

    // Register our service control handler with the SCM
    //!
    // ЭТА ЗАЛУПЕНЬ НЕ РАБОТАЕТ НИХУЯ ТУТ
    // hServiceControlManager = OpenSCManager(
    //     NULL,                     // локальная машина
    //     NULL,                     // активная база данных сервисов
    //     SC_MANAGER_CREATE_SERVICE // возможно создание сервиса
    // );
    // if (hServiceControlManager == NULL)
    // {
    //     cout << "Open service control manager failed." << endl
    //          << "The last error code: " << GetLastError() << endl
    //          << "Press any key to exit." << endl;
    //     // cin.get();
    // }
    // hService = CreateService(
    //     hServiceControlManager, // дескриптор менеджера сервисов
    //     SERVICE_NAME,           // внутреннее имя сервиса, используемое SCM
    //     SERVICE_NAME,           // внешнее имя сервиса в панели управления
    //     SERVICE_ALL_ACCESS,     // полный контроль над сервисом

    //     SERVICE_WIN32_OWN_PROCESS, // сервис является процессом
    //     SERVICE_DEMAND_START,      // запускается сервис по требованию
    //     SERVICE_ERROR_NORMAL,      // обработка ошибок нормальная
    //     SERVICE_PATH,              // путь к сервису
    //     NULL,                      // сервис не принадлежит к группе
    //     NULL,                      // тег группы не изменяется
    //     NULL,                      // сервис не зависит от других сервисов
    //     NULL,                      // имя совпадает с текущим именем учетной записи
    //     NULL                       // пароля нет
    // );
    // if (hService == NULL)
    // {
    //     cout << "Create service failed." << endl
    //          << "The last error code: " << GetLastError() << endl
    //          << "Press any key to exit." << endl;
    //     // cin.get();

    //     // закрываем дескриптор менеджера сервисов
    //     CloseServiceHandle(hServiceControlManager);

    //     return;
    // }

    // CloseServiceHandle(hService);
    // CloseServiceHandle(hServiceControlManager);
    //!
    g_StatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceCtrlHandler);

    if (g_StatusHandle == NULL)
    {
        return;
    }

    // Tell the service controller we are starting
    ZeroMemory(&g_ServiceStatus, sizeof(g_ServiceStatus));
    g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_ServiceStatus.dwControlsAccepted = 0;
    g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    g_ServiceStatus.dwWin32ExitCode = 0;
    g_ServiceStatus.dwServiceSpecificExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 0;

    if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
    {
        OutputDebugString(TEXT(
            "My Sample Service: ServiceMain: SetServiceStatus returned error"));
    }

    /*
     * Perform tasks necessary to start the service here
     */

    // Create a service stop event to wait on later
    g_ServiceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (g_ServiceStopEvent == NULL)
    {
        // Error creating event
        // Tell service controller we are stopped and exit
        g_ServiceStatus.dwControlsAccepted = 0;
        g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        g_ServiceStatus.dwWin32ExitCode = GetLastError();
        g_ServiceStatus.dwCheckPoint = 1;

        if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
        {
            OutputDebugString(TEXT(
                "My Sample Service: ServiceMain: SetServiceStatus returned error"));
        }
        return;
    }

    // Tell the service controller we are started
    g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    g_ServiceStatus.dwWin32ExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 0;

    if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
    {
        OutputDebugString(TEXT(
            "My Sample Service: ServiceMain: SetServiceStatus returned error"));
    }

    // Start a thread that will perform the main task of the service
    HANDLE hThread = CreateThread(NULL, 0, ServiceWorkerThread, NULL, 0, NULL);

    // Wait until our worker thread exits signaling that the service needs to stop
    WaitForSingleObject(hThread, INFINITE);

    /*
     * Perform any cleanup tasks
     */

    CloseHandle(g_ServiceStopEvent);

    // Tell the service controller we are stopped
    g_ServiceStatus.dwControlsAccepted = 0;
    g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    g_ServiceStatus.dwWin32ExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 3;

    if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
    {
        OutputDebugString(TEXT(
            "My Sample Service: ServiceMain: SetServiceStatus returned error"));
    }
}

// this is what u are doing when service status changes
VOID WINAPI ServiceCtrlHandler(DWORD CtrlCode)
{
    switch (CtrlCode)
    {
    case SERVICE_CONTROL_STOP:

        if (g_ServiceStatus.dwCurrentState != SERVICE_RUNNING)
            break;

        /*
         * Perform tasks necessary to stop the service here
         */
        // no fucking need to add anything

        g_ServiceStatus.dwControlsAccepted = 0;
        g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
        g_ServiceStatus.dwWin32ExitCode = 0;
        g_ServiceStatus.dwCheckPoint = 4;

        if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
        {
            OutputDebugString(TEXT(
                "My Sample Service: ServiceCtrlHandler: SetServiceStatus returned error"));
        }

        // This will signal the worker thread to start shutting down
        SetEvent(g_ServiceStopEvent);

        break;

    default:
        break;
    }
}

DWORD WINAPI ServiceWorkerThread(LPVOID lpParam)
{
    //  Periodically check if the service has been requested to stop
    while (WaitForSingleObject(g_ServiceStopEvent, 0) != WAIT_OBJECT_0)
    {
        /*
         * Perform main service function here
         */

        //  Simulate some work by sleeping
        Sleep(3000);
    }

    return ERROR_SUCCESS;
}

int main(int argc, TCHAR *argv[])
{
    SERVICE_TABLE_ENTRY ServiceTable[] =
        {
            {SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain},
            {NULL, NULL}};

    if (StartServiceCtrlDispatcher(ServiceTable) == FALSE)
    {
        return -1;
    }
    return 0;
}