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
    ui->listTerm->appendPlainText(current_path + "$ " + command_line);
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
            }
        }else if(mounted_disk) // comandos si hay un disco esta montado
        {
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
    if (existDisk(disk_name)>0) {

    }
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
    for (int i = 0; i < disks.size(); ++i) {
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
    if (index>0) {
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

void FileSys::on_txtcommandLine_returnPressed()
{
    exCommand(ui->txtcommandLine->text());
}
