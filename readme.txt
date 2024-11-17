В листингах при ошибке 5 (ERROR_ACCESS_DENIED; (0x5); Access is denied.)
.exe файлы из папки bin нужно запустить из cmd от имени Администратора

sc create "aaaService" binPath= "aaaService.exe"
    or
пункт выше
sc delete "aaaService"