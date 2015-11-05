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

    bool mounted_disk=false;
    QString root_path = "/";
    QString current_path = "";
    QString fdisk_commands = "command' list for fdisk:"
                             "\nfdisk D [name_disk] : show disk's detalls"
                             "\nfdisk n [name_disk] [size_disk] [size_per_block] : create a new disk"
                             "\nfdisk L : show all hard drives created"
                             "\nfdisk d [name_disk] : delete a disk";
    QString fdisk_commands_empty = "we expect: fdisk n [name_disk] [size_disk] [size_per_block]";
    QString last_command_line = "";

    void exCommand(QString);
};

#endif // FILESYS_H
