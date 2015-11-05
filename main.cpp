#include "filesys.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FileSys w;
    w.show();

    return a.exec();
}
