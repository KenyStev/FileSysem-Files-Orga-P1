#ifndef FILESYS_H
#define FILESYS_H

#include <QMainWindow>
#include "disk.h"

namespace Ui {
class FileSys;
}

class FileSys : public QMainWindow
{
    Q_OBJECT

public:
    explicit FileSys(QWidget *parent = 0);
    ~FileSys();

private slots:
    void on_txtcommandLine_returnPressed();

private:
    Ui::FileSys *ui;

    ofstream *diskManager = NULL;
    bool mounted_disk=false;
    vector<QString> disks;

    QString root_path = "/";
    QString disks_path = "VirtualDisks/";
    QString current_path = "";
    QString format = ".data";
    QString fdisk_commands = "command' list for fdisk:"
                             "\nfdisk D [name_disk] : show disk's detalls"
                             "\nfdisk n [name_disk] [size_disk] [size_per_block] : create a new disk"
                             "\nfdisk L : show all hard drives created"
                             "\nfdisk d [name_disk] : delete a disk";
    QString fdisk_commands_empty = "we expect: fdisk n [name_disk] [size_disk] [size_per_block]";
    QString last_command_line = "";

    void exCommand(QString);
    void mountDisk(QString);
    void listDisks();
    int existDisk(QString);
    void deleteDisk(QString);
};

#endif // FILESYS_H
