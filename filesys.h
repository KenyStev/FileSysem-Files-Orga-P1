#ifndef FILESYS_H
#define FILESYS_H

#include <QMainWindow>
#include <QtConcurrent/QtConcurrent>
#include <QProgressBar>
#include <QFileDialog>
#include "blocksbox.h"
#include "tree.h"
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

    void on_btnAddFile_clicked();

    void on_btnBlocks_clicked();

    void on_btnTrees_clicked();

private:
    Ui::FileSys *ui;
    const static int order = 3;

//    ofstream *diskManager = NULL;
    BlocksBox *blocks=NULL;
    QFileDialog *input=NULL;
    Tree *tree=NULL;
    Btree<NodoFT*, order> *file_table_BTree=NULL;

    QString mounted_disk = "";
    string T_name;
    bool is_mounted_disk=false;
    Inode current_inode;
    double current_inode_ptr=0;
    SuperBlock Super_Block;
    vector<FileData*> file_data_array;
    char *bitmap=NULL;
    char *bitmap_inodes=NULL;
    double start_bitmap=0,start_bitmap_inodes=0,start_filetable=0,start_inodes=0,start_datablocks=0;
    vector<QString> disks;

    QString root_path = "/";
    QString disks_path = "VirtualDisks/";
    QString dirToExport = "Exported_Files/";
    QStringList current_path;
    QString format = ".data";
    QString fdisk_commands = "command' list for fdisk:"
                             "\nfdisk D [name_disk] : show disk's detalls"
                             "\nfdisk n [name_disk] [size_disk] [size_per_block] : create a new disk"
                             "\nfdisk L : show all hard drives created"
                             "\nfdisk d [name_disk] : delete a disk";
    QString fdisk_commands_empty = "we expect: fdisk n [name_disk] [size_disk] [size_per_block]";
    QString last_command_line = "";

    int searchInFileTable(string name);
    int searchInodeInFileTable(string name);
    int get_NextFree_FileTable();

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

    /**
     * @brief writeInode escribe un bloque de DATA del inodo recibido
     * @param inode a quien pertenece el bloque de DATA
     * @param disk el disco montado actualente
     * @param buffer el bloque que va a escribir
     * @param size el tamanio del bloque que va a escribir ya que puede que el buffer no se escribi todo,
     * osea lo que pesa el 'blocksize' sino menos, pero nunca mas de eso.
     */
    void writeInode(Inode *inode, string disk, char *buffer, double size);

    /**
     * @brief mkfile crea un archivo en el directorio situado actualmente con letras random
     * @param name del nombre recibido
     * @param size y tamanio especificado
     */
    void mkfile(string name,int size);

    /**
     * @brief mkDir crea un directorio dentro del directorio actual
     * @param name con el nombre recibido
     */
    void mkDir(string name);
    void mkfile2(string name,int size);

    /**
     * @brief updateFileTableFromDir actualiza los bloques de DATA de un directorio
     * usado a la hora de crear un nuevo 'dir' o 'file' dentro de el.
     * @param inode en el cual se hara el cambio
     * @param data es el nuevo registro para la DATA del 'dir'
     */
    void updateFileTableFromDir(Inode *inode,FileData *data);

    void updateSuperBlock();

    /**
     * @brief ls lista los directorios y achivos que estan dentro del 'dir' actual
     * para ello usa las funciones auxiliares @readDataBlocksFrom(); @getFileTableFrom();
     */
    void ls();
    void cd(string dir_to_move);
    void Export(string file_name);
    void cp(string file,string new_name,QString path);
    void addFile(string filename);
    void rm(string filename);
};

#endif // FILESYS_H
