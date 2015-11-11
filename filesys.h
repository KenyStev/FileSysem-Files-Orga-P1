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

//    ofstream *diskManager = NULL;
    QString mounted_disk = "";
    bool is_mounted_disk=false;
    SuperBlock Super_Block;
    vector<FileData*> file_data_array;
    char *bitmap;
    char *bitmap_inodes;
    int start_bitmap=0,start_bitmap_inodes=0,start_filetable=0,start_inodes=0,start_datablocks=0;
    vector<QString> disks;

    QString root_path = "/";
    QString disks_path = "VirtualDisks/";
    QStringList current_path;
    QString format = ".data";
    QString fdisk_commands = "command' list for fdisk:"
                             "\nfdisk D [name_disk] : show disk's detalls"
                             "\nfdisk n [name_disk] [size_disk] [size_per_block] : create a new disk"
                             "\nfdisk L : show all hard drives created"
                             "\nfdisk d [name_disk] : delete a disk";
    QString fdisk_commands_empty = "we expect: fdisk n [name_disk] [size_disk] [size_per_block]";
    QString last_command_line = "";

    /**
     * @brief exCommand
     * @param command_line recibe la linea de comando ingresada por el usuario para parcearla y ejecutar el comando correcto
     */
    void exCommand(QString command_line);

    /**
     * @brief mountDisk
     * @param disk_name el nombre del disco a montar
     */
    void mountDisk(QString disk_name);

    /**
     * @brief listDisks enlista los discos creados
     */
    void listDisks();

    /**
     * @brief existDisk evalua si el disco enviado de parametro existe o no
     * @param disk_name
     * @return index del disco si el disco existe sino retorna -1
     */
    int existDisk(QString disk_name);

    /**
     * @brief deleteDisk borra el enviad de parametro si existe
     * @param disk_name
     */
    void deleteDisk(QString disk_name);

    /**
     * @brief showInfoDisk muestra la informacion del disco enviado de parametro si existe
     * @param disk_name
     */
    void showInfoDisk(QString disk_name);

    /**
     * @brief mkfile crea un archivo con caracteres random
     * @param name nombre del archivo
     * @param size tamanio en MB
     */
    void mkfile(string name,int size);
};

#endif // FILESYS_H
