#include <windows.h>
#include <iostream>
#include "res.h"
using namespace std;

int InstallService()
{

    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

    if (!hSCManager)
    {
        cout << "Error: Can't open Service Control Manager";

        return -1;
    }

    SC_HANDLE hService = CreateService(
        hSCManager,
        SERVICE_NAME,
        SERVICE_NAME,
        SERVICE_ALL_ACCESS,
        SERVICE_WIN32_OWN_PROCESS,
        SERVICE_DEMAND_START,
        SERVICE_ERROR_NORMAL,
        SERVICE_PATH,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL);

    if (!hService)
    {
        int err = GetLastError();

        switch (err)
        {
        case ERROR_ACCESS_DENIED:

            cout << ("Error: ERROR_ACCESS_DENIED");
            break;

        case ERROR_CIRCULAR_DEPENDENCY:

            cout << ("Error: ERROR_CIRCULAR_DEPENDENCY");
            break;

        case ERROR_DUPLICATE_SERVICE_NAME:

            cout << ("Error: ERROR_DUPLICATE_SERVICE_NAME");
            break;

        case ERROR_INVALID_HANDLE:

            cout << ("Error: ERROR_INVALID_HANDLE");
            break;

        case ERROR_INVALID_NAME:

            cout << ("Error: ERROR_INVALID_NAME");
            break;

        case ERROR_INVALID_PARAMETER:

            cout << ("Error: ERROR_INVALID_PARAMETER");
            break;

        case ERROR_INVALID_SERVICE_ACCOUNT:

            cout << ("Error: ERROR_INVALID_SERVICE_ACCOUNT");
            break;

        case ERROR_SERVICE_EXISTS:

            cout << ("Error: ERROR_SERVICE_EXISTS");
            break;

        default:
            cout << ("Error: Undefined");
        }

        CloseServiceHandle(hSCManager);

        return err;
    }

    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);

    cout << "Success install service!";

    return 0;
}

int RemoveService()
{
    SERVICE_STATUS service_status;
    SC_HANDLE hSCManager = OpenSCManager(NULL,
                                         NULL,
                                         SC_MANAGER_CONNECT);

    if (!hSCManager)
    {

        cout << ("Error: Can't open Service Control Manager");

        return -1;
    }

    SC_HANDLE hService = OpenService(hSCManager,
                                     SERVICE_NAME,
                                     SERVICE_ALL_ACCESS | DELETE);

    if (!hService)
    {

        cout << ("Error: Can't remove service");

        CloseServiceHandle(hSCManager);

        return -1;
    }

    if (!QueryServiceStatus(hService, &service_status))
    {
        cout << "Query service status failed." << endl
             << "The last error code: " << GetLastError() << endl
             << "Press any key to exit." << endl;

        // закрываем дескрипторы
        CloseServiceHandle(hSCManager);
        CloseServiceHandle(hService);

        return 0;
    }

    // если сервис работает, то останавливаем его
    if (service_status.dwCurrentState != SERVICE_STOPPED)
    {
        cout << "Service is working. It will be stoped" << endl;
        if (!ControlService(hService, SERVICE_CONTROL_STOP, &service_status))
        {
            cout << "Control service failed." << endl
                 << "The last error code: " << GetLastError() << endl
                 << "Press any key to exit." << endl;

            // закрываем дескрипторы
            CloseServiceHandle(hSCManager);
            CloseServiceHandle(hService);

            return 0;
        }

        // ждем, пока сервис остановится
        Sleep(500);
    }

    DeleteService(hService);

    CloseServiceHandle(hService);

    CloseServiceHandle(hSCManager);

    return 0;
}

int StartService()
{
    SC_HANDLE hServiceControlManager, hService;
    char *param[] = {"p1", "p2", "p3"};
    // связываемся с менеджером сервисов
    hServiceControlManager = OpenSCManager(
        NULL,              // локальная машина
        NULL,              // активная база данных сервисов
        SC_MANAGER_CONNECT // соединение с менеджером сервисов
    );
    if (hServiceControlManager == NULL)
    {
        cout << "Open service control manager failed." << endl
             << "The last error code: " << GetLastError() << endl
             << "Press any key to continue." << endl;

        return 0;
    }

    // открываем сервис
    hService = OpenService(
        hServiceControlManager, // дескриптор менеджера сервисов
        SERVICE_NAME,           // имя сервиса
        SERVICE_ALL_ACCESS      // полный доступ к сервису
    );
    if (hService == NULL)
    {
        cout << "Open service failed." << endl
             << "The last error code: " << GetLastError() << endl
             << "Press any key to exit." << endl;

        // закрываем дескриптор менеджера сервисов
        CloseServiceHandle(hServiceControlManager);

        return -1;
    }

    // стартуем сервис
    if (!StartService(
            hService,            // дескриптор сервиса
            3,                   // три параметра
            (const char **)param // указатель на массив параметров
            ))

    {
        cout << "Start service failed." << endl
             << "The last error code: " << GetLastError() << endl
             << "Press any key to exit." << endl;

        // закрываем дескрипторы
        CloseServiceHandle(hServiceControlManager);
        CloseServiceHandle(hService);

        return -1;
    }

    // закрываем дескрипторы
    CloseServiceHandle(hServiceControlManager);
    CloseServiceHandle(hService);

    return 0;
}