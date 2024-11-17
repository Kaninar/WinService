#include <windows.h>
#include <iostream>
#include "res.h"
using namespace std;
// #include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include "srvctrl.cpp"

int main(int argc, char *argv[])
{
    static struct option long_options[] =
        {
            {"install", required_argument, NULL, 'i'},
            {"remove", required_argument, NULL, 'd'},
            {"start", required_argument, NULL, 's'},
            {"stop", required_argument, NULL, 'p'},
            {NULL, 0, NULL, 0}};
    SC_HANDLE hServiceControlManager, hService;

    // * TODO add try-catch-finally blocks

    if (argc > 2)
    {
        cout << "Too many arguments" << endl;
        return -1;
    }

    int opt = getopt_long(argc, argv, "irsp", long_options, NULL);

    switch (opt)
    {
    case 'i':
    {
        return InstallService();
        break;
    }
    case 'r':
    {
        return RemoveService();
        break;
    }
    case 's':
    {
        return StartService();
        break;
    }
    case 'p':
    {
        break;
    }
    default:
        fprintf(stderr, "Usage: %s [-ilw] [file...] [file...] [-option]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // // связываемся с менеджером сервисов
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
    //     cin.get();

    //     throw 5;
    // }
    // cout << "Service control manager is opened." << endl
    //      << "Press any key to continue." << endl;
    // cin.get();

    // // устанавливаем новый сервис
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
    //     cin.get();

    //     // закрываем дескриптор менеджера сервисов
    //     CloseServiceHandle(hServiceControlManager);

    //     return 0;
    // }

    // cout << "Service is installed." << endl
    //      << "Press any key to exit." << endl;
    // cin.get();

    // // закрываем дескрипторы
    // CloseServiceHandle(hService);
    // CloseServiceHandle(hServiceControlManager);

    return 0;
}