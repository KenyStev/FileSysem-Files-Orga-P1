#include "filesys.h"
#include "ui_filesys.h"

FileSys::FileSys(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FileSys)
{
    ui->setupUi(this);

    QDir dir(disks_path);

    cout<<dir.dirName().toStdString()<<endl;
    QFileInfoList list = dir.entryInfoList();

    for (int var = 0; var < list.size(); ++var) {
        QString name = list.at(var).fileName();
        if(name.contains(format))
        {
            disks.push_back((list.at(var).fileName().split(format))[0]);
        }
        cout<<list.at(var).fileName().toStdString()<<endl;
    }
    cout<<"List of Disks:"<<endl;
    for (int i = 0; i < disks.size(); ++i) {
        cout<<disks[i].toStdString()<<endl;
    }
}

FileSys::~FileSys()
{
    delete ui;
}

void FileSys::exCommand(QString command_line)
{
    ui->listTerm->appendPlainText(mounted_disk + "@root~" + root_path + current_path.join("/") + "$ " + command_line);
    QStringList commands = command_line.split(" ");

    if(!command_line.isEmpty())
    {
        last_command_line = QString(command_line);
        QString main_command = commands.front();
        commands.pop_front();

        if(main_command == "exit")
        {
            exit(0);
        }else if(main_command == "fdisk")
        {
            if(commands.size()>0)
            {
                QString fdisk_command = commands.front();
                commands.pop_front();
                if(commands.size() > 0)
                {
                    QString name_disk = commands.at(0);
                    if(name_disk.size()>10){
                        ui->listTerm->appendPlainText("name_disk is too large, 10 characters maximun");
                        ui->txtcommandLine->setText("");
                        return;
                    }
                    if(fdisk_command == "n")
                    {
                        if(commands.size() == 3)
                        {
                            //aqui va el codigo para crear el disco
                            QString size_disk = commands.at(1);
                            QString size_block = commands.at(2);

                            createDisk((char*)(name_disk.toStdString().c_str()),size_disk.toInt(),size_block.toInt());
                            disks.push_back(name_disk);
                        }else{
                            ui->listTerm->appendPlainText(fdisk_commands_empty);
                        }
                    }else if(fdisk_command == "D"){ // muestra informacion del disco
                        cout<<"--> Information"<<endl;
                        showInfoDisk(name_disk);
                    }else if(fdisk_command == "d"){ // elimina un disco
                        cout<<"--> Delete"<<endl;
                        deleteDisk(name_disk);
                    }
                }else if(fdisk_command == "L"){ // enlista todos los discos
                    cout<<"--> Listar"<<endl;
                    listDisks();
                }else{
                    ui->listTerm->appendPlainText(fdisk_commands);
                }
            }else{
                ui->listTerm->appendPlainText(fdisk_commands);
            }
        }else if(main_command == "mount"){ // comando para motar el disco
            if(commands.size()>0)
            {
                // aqui va el codigo de montar un disco
                cout<<"-> Montar Disco"<<endl;
                mountDisk(commands.at(0));
            }
        }else if(is_mounted_disk){ // comandos si hay un disco esta montado
            if(main_command == "ls") // comand ls -l
            {

            }else if(main_command == "cd") // change directory form: cd [path]
            {

            }else if(main_command == "mkdir") // crear directory form: mkdir [name_dir]
            {

            }else if(main_command == "mkfile") // crear file form: mkfile [name_file]
            {

            }else if(main_command == "rm") // delete file or dir form: rm [name]
            {

            }else if(main_command == "export") // exportar fuera del disco form: export [file_name]
            {

            }else if(main_command == "cp") // copiar un archivo de un lugar a otro form: cp [name] [path]
            {

            }
        }else{
            ui->listTerm->appendPlainText(main_command + " is not a valid command");
        }
        ui->txtcommandLine->setText("");
    }else{
        cout<<last_command_line.toStdString()<<endl;
        ui->txtcommandLine->setText(last_command_line);
    }
}

void FileSys::mountDisk(QString disk_name)
{
    int index = existDisk(disk_name);
    if (index>=0) {
        ifstream in((disks_path + disk_name + format).toStdString().c_str(), ios::in | ios:: out | ios::binary);
        is_mounted_disk = in.is_open();

        if(is_mounted_disk)
        {
            ui->listTerm->appendPlainText("Disco: " + disk_name + " montado!");
            mounted_disk = disk_name;

//            SuperBlock *SB = new SuperBlock();

            int size_SB = sizeof(SuperBlock);
            char *buffer = new char[size_SB];

            in.read(buffer,size_SB);

            memcpy(&Super_Block,buffer,size_SB);

            cout<<"Disk: "<<Super_Block.name<<endl;
            cout<<"cant of blocks: "<<Super_Block.cantofblock<<endl;
            cout<<"cant of inodes: "<<Super_Block.cantofinode<<endl;
            cout<<"Free blocks: "<<Super_Block.freeblock<<endl;
            cout<<"Free Scpace: "<<Super_Block.freespace<<endl;
            cout<<"Size: "<<Super_Block.size<<endl;
            cout<<"Size of Block: "<<Super_Block.sizeofblock<<endl;

            cout<<"Pointer after read SB: "<<in.tellg()<<endl;
            cout<<"Size SB: "<<size_SB<<endl;

            start_datablocks = Super_Block.cantofblock; //guardando el lugar donde comienza el DataBlock

            //Leyendo bitmap
            bitmap = new char[Super_Block.cantofblock/8];
            start_bitmap = in.tellg(); //guardando el lugar donde comienza el bitmap

            in.read(bitmap,sizeof(char)*(Super_Block.cantofblock/8));
            cout<<"is_in_use 0: "<<is_block_in_use(bitmap,0)<<endl;

            //leyendo bitmap_inodos
            bitmap_inodes = new char[Super_Block.cantofinode/8];
            start_bitmap_inodes = in.tellg(); //guardando el lugar donde comienza el bitmap_inodes

            in.read(bitmap_inodes,sizeof(char)*(Super_Block.cantofinode/8));
            cout<<"is_in_use 0 inodes: "<<is_block_in_use(bitmap_inodes,0)<<endl;

            //leyendo FileTable
            int size_DataFile = sizeof(FileData);
            buffer = new char[size_DataFile];
            start_filetable = in.tellg(); //guardando el lugar donde comienza el FileTable

            for (int i = 1; i < Super_Block.cantofinode; ++i) {
                in.read(buffer,size_DataFile);
                FileData *FD = new FileData();
                memcpy(FD,buffer,size_DataFile);
                file_data_array.push_back(FD);
            }

            cout<<"FD_root_name: "<<file_data_array[0]->name<<endl;
            cout<<"FD_root_index_inode: "<<file_data_array[0]->index_file<<endl;
            cout<<"FD_1: "<<file_data_array[1]->name<<endl;
            cout<<"FD_1_index_inode: "<<file_data_array[1]->index_file<<endl;

        }else{
            ui->listTerm->appendPlainText("No se pudo montar el disco: " + disk_name + "!");
        }

        in.close();
    }else{
        cout<<"no existe el disco"<<endl;
    }

    //probando cambiar el filetable
    /*//si funciona.
    ofstream output_file((disks_path + disk_name + format).toStdString().c_str(), ios::in | ios::out | ios::binary);
    output_file.seekp(start_filetable + sizeof(FileData));
    strcpy(file_data_array[1]->name,"Hola");
    file_data_array[1]->index_file = 1;

    output_file.write((char*)file_data_array[1],sizeof(FileData));

    output_file.close();*/
}

void FileSys::listDisks()
{
    QString title = "List of Disks:";
    cout<<title.toStdString().c_str()<<endl;
    ui->listTerm->appendPlainText(title);
    for (int i = 0; i < disks.size(); ++i) {
        cout<<disks[i].toStdString()<<endl;
        ui->listTerm->appendPlainText("->" + disks[i]);
    }
}

int FileSys::existDisk(QString disk_name)
{
    for (int i = 0; i < disks.size(); i++) {
        if(QString::compare(disks[i], disk_name,Qt::CaseSensitive)==0)
        {
            return i;
        }
    }
    return -1;
}

void FileSys::deleteDisk(QString disk_name)
{
    int index = existDisk(disk_name);
    if (index>=0) {
        if(remove((disks_path + disk_name + format).toStdString().c_str())==0)
        {
            disks.erase(disks.begin()+index);
            cout<<"removed file: "<<(disks_path + disk_name + format).toStdString().c_str()<<endl;
        }else{
            cout<<"no removed"<<endl;
        }
    }else{
        cout<<"no existe el disco"<<endl;
    }
}

void FileSys::showInfoDisk(QString disk_name)
{
    if(existDisk(disk_name))
    {
        ifstream in((disks_path + disk_name + format).toStdString().c_str(), ios::in | ios:: out | ios::binary);

        SuperBlock SB;

        int size_SB = sizeof(SuperBlock);
        char *buffer = new char[size_SB];

        in.read(buffer,size_SB);
        in.close();

        memcpy(&SB,buffer,size_SB);

        cout<<"Disk: "<<SB.name<<endl;
        cout<<"cant of blocks: "<<SB.cantofblock<<endl;
        cout<<"cant of inodes: "<<SB.cantofinode<<endl;
        cout<<"Free blocks: "<<SB.freeblock<<endl;
        cout<<"Free Scpace: "<<SB.freespace<<endl;
        cout<<"Size: "<<SB.size<<endl;
        cout<<"Size of Block: "<<SB.sizeofblock<<endl;

        ui->listTerm->appendPlainText("Information Disk: " + QString(SB.name));
        ui->listTerm->appendPlainText("Size: " + QString::number(SB.size) + " bytes");
        ui->listTerm->appendPlainText("Free Space: " + QString::number(SB.freespace) + " bytes");
        ui->listTerm->appendPlainText("Size of block: " + QString::number(SB.sizeofblock) + " bytes");
        ui->listTerm->appendPlainText("blocks: " + QString::number(SB.cantofblock));
        ui->listTerm->appendPlainText("Free blocks: " + QString::number(SB.freeblock));
        ui->listTerm->appendPlainText("inodes: " + QString::number(SB.cantofinode));

    }else{
        cout<<"no existe el disco"<<endl;
    }
}

void FileSys::on_txtcommandLine_returnPressed()
{
    exCommand(ui->txtcommandLine->text());
}
