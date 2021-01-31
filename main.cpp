#include "mainwindow.h"
#include <QApplication>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

int startApplication(int argc, char* argv[], pid_t& pid)
{
    pid = fork();

    if(pid < 0)
    {
        qDebug() << "fork error. errno:" << errno << " error: " << strerror(errno) << "\n";
        return -1;
    }
    if(pid > 0)
        return 0;

    QApplication a(argc, argv);
    QString config = "./usbcam.ini";
    if(argc != 2)
        printf("Usage: \n\t%s configure\n", argv[0]);

    if(argc >= 2)
        config = argv[1];

    MainWindow w(config);
    w.setFixedSize(SCREENWIDTH, SCREENHEIGHT);
    w.show();

    return a.exec();
}

int main(int argc, char *argv[])
{
    pid_t pid;

    startApplication(argc, argv, pid);

    while(1)
    {
        sleep(10);
        if(waitpid(pid, nullptr, WNOHANG) != 0)
        {
            qDebug() << "restart application...\n";
            startApplication(argc, argv, pid);
        }
    }

    return 0;
}
