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

                            SuperBlock SB = createDisk((char*)(name_disk.toStdString().c_str()),size_disk.toInt(),size_block.toInt());

                            ui->listTerm->appendPlainText("Disco " + QString(SB.name) + " creado!");
                            ui->listTerm->appendPlainText("-> Size: " + QString::number(SB.size));
                            ui->listTerm->appendPlainText("-> Size of block: " + QString::number(SB.sizeofblock));
                            ui->listTerm->appendPlainText("-> Blocks: " + QString::number(SB.cantofblock));
                            ui->listTerm->appendPlainText("-> Inodes: " + QString::number(SB.cantofinode));
                            ui->listTerm->appendPlainText("-> Free spcae: " + QString::number(SB.freespace));
                            ui->listTerm->appendPlainText("-> Free blocks: " + QString::number(SB.freeblock));
                            ui->listTerm->appendPlainText("-> Free inodes: " + QString::number(SB.freeinode));

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

            }else if(main_command == "mkfile") // crear file form: mkfile [name_file] [tamanio mb]
            {
                if(commands.size() == 2 )
                {
                    mkfile(commands.at(0).toStdString(),commands.at(1).toInt());
                }
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

            double total_size_fs = sizeof(SuperBlock) + sizeof(char)*(Super_Block.cantofblock/8) + sizeof(char)*(Super_Block.cantofinode/8)
                    + sizeof(Inode)*(Super_Block.cantofinode) + sizeof(FileData)*(Super_Block.cantofinode);
            start_datablocks = total_size_fs; //guardando el lugar donde comienza el DataBlock
            cout<<"start_datablocks: "<<start_datablocks<<endl;

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

            for (int i = 0; i < Super_Block.cantofinode; ++i) {
                in.read(buffer,size_DataFile);
                FileData *FD = new FileData();
                memcpy(FD,buffer,size_DataFile);
                file_data_array.push_back(FD);
            }

            start_inodes = in.tellg(); //guardando el lugar donde comienzan los inodos

            //montando el inodo root
            char *root_buffer = new char[sizeof(Inode)];
            in.read(root_buffer,sizeof(Inode));
            memcpy(&current_inode,root_buffer,sizeof(Inode));

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
    output_file.seekp(start_filetable + sizeof(FileData));*
    strcpy(file_data_array[1]->name,"Hola");
    file_data_array[1]->index_file = 1;

//    output_file.write((char*)file_data_array[1],sizeof(FileData));

//    output_file.close();

    write((disks_path + disk_name + format).toStdString(),(char*)file_data_array[1],start_filetable + sizeof(FileData),sizeof(FileData));
    */
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
    if(existDisk(disk_name)>=0)
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
        ui->listTerm->appendPlainText("-> Size: " + QString::number(SB.size) + " bytes");
        ui->listTerm->appendPlainText("-> Free Space: " + QString::number(SB.freespace) + " bytes");
        ui->listTerm->appendPlainText("-> Size of block: " + QString::number(SB.sizeofblock) + " bytes");
        ui->listTerm->appendPlainText("-> Blocks: " + QString::number(SB.cantofblock));
        ui->listTerm->appendPlainText("-> Free blocks: " + QString::number(SB.freeblock));
        ui->listTerm->appendPlainText("-> Inodes: " + QString::number(SB.cantofinode));
        ui->listTerm->appendPlainText("-> Free inodes: " + QString::number(SB.freeinode));

    }else{
        cout<<"no existe el disco"<<endl;
    }
}

void FileSys::mkfile(string name, int size)
{
    double size_bytes = size;//*pow(1024,2);
    double bloques_data = ceil(size_bytes/Super_Block.sizeofblock);
    vector<double> Total_blocks = getTotalBlocksToUse(size_bytes,Super_Block.sizeofblock);
    string T_name = (disks_path + mounted_disk + format).toStdString();

    if(Total_blocks[0] >= 0 && Super_Block.freeinode > 0)
    {
        char *buffer = new char[Super_Block.sizeofblock];
//        memset(buffer,'N',Super_Block.sizeofblock);
        for (int i = 0; i < Super_Block.sizeofblock; ++i) {
            buffer[i] = rand()%25 + 65;
        }
        vector<double> blocks = getFreeBlocks(bitmap,Super_Block.cantofblock,Total_blocks[Total_blocks.size()-1]);
        vector<double> inode = getFreeBlocks(bitmap_inodes,Super_Block.cantofinode,1);

        cout<<"Bloques dados por el bitmap!"<<endl;
        for (double i = 0; i < Total_blocks[Total_blocks.size()-1]; ++i) {
            cout<<"-- "<<blocks[i]<<endl;
        }

        if(blocks[0]>0) // valida que hallan la cantidad de bloques suficientes, para que quepa en el disco
        {
            //guarda el filetable
//            for (int i = 0; i < Super_Block.cantofinode; ++i) {
//                if(file_data_array[i]->index_file==-1)
//                {
                    int index = get_NextFree_FileTable();
                    strcpy(file_data_array[index]->name,name.c_str());
                    file_data_array[index]->index_file = inode[0];
                    write(T_name,(char*)file_data_array[index],start_filetable + index*sizeof(FileData),sizeof(FileData));
//                    break;
//                }
//            }

            //escribe el archivo
            double file_size_bytes_temp = size_bytes;
            for (double i = 0; i < bloques_data-1; ++i) {
                write(T_name,buffer,blocks[i]*(Super_Block.sizeofblock),Super_Block.sizeofblock);
                file_size_bytes_temp -= Super_Block.sizeofblock;
            }
            if(file_size_bytes_temp>0)
            {
                char *buff = new char[(int)(file_size_bytes_temp)];
                memset(buff,'K',file_size_bytes_temp);
                write(T_name,buff,blocks[bloques_data-1]*(Super_Block.sizeofblock),file_size_bytes_temp);
            }


            //comenzar en bloques_data
            //guardar inodo, con sus apuntadores
            Inode new_inode;
            buffer = new char[sizeof(Inode)];
            read(T_name, buffer,start_inodes + inode[0]*sizeof(Inode),sizeof(Inode));
            memcpy(&new_inode,buffer,sizeof(Inode));
            new_inode.blockuse = bloques_data;
            new_inode.filesize = size_bytes;
            strcpy(new_inode.permisos,"-rwxrwxrwx");

            //setear las direcciones de los bloques en el inode

            double Total_blocks_inodes = Total_blocks[Total_blocks.size()-1] - bloques_data;

            cout<<"caso: "<<Total_blocks[0]<<endl;
            if(Total_blocks[0]==0)
            {
                for (int i = 0; i < Total_blocks[1]; ++i) {
                    new_inode.directos[i] = blocks[i];
//                    Total_blocks_inodes--;
                }
                write(T_name,(char*)&new_inode,start_inodes + inode[0]*sizeof(Inode),sizeof(Inode));
            }else{
                for (int i = 0; i < 10; ++i) {
                    new_inode.directos[i] = blocks[i];
//                    Total_blocks_inodes--;
                }

                vector<double> data_index;
                vector<double> inodes_index;
                for (double i = 0; i < Total_blocks[Total_blocks.size()-1]; ++i) {
                    if(i<bloques_data){
                        data_index.push_back(blocks[i]);
                    }else{
                        inodes_index.push_back(blocks[i]);
                    }
                }

                writeInodesBlocks(T_name,data_index,inodes_index,Total_blocks,Super_Block.sizeofblock,&new_inode,start_inodes);
                write(T_name,(char*)&new_inode,start_inodes + inode[0]*sizeof(Inode),sizeof(Inode));
            }


            //guardar bitmaps
            write(T_name,bitmap,start_bitmap,Super_Block.cantofblock/8);
            write(T_name,bitmap_inodes,start_bitmap_inodes,Super_Block.cantofinode/8);

            //guardar Super Block
            Super_Block.freeblock -= Total_blocks[Total_blocks.size()-1];
            (Super_Block.freeinode)--;
            Super_Block.freespace -= (size_bytes + (Total_blocks[Total_blocks.size()-1] - bloques_data)*Super_Block.sizeofblock);
            write(T_name,(char*)&Super_Block,0,sizeof(SuperBlock));

            ui->listTerm->appendPlainText("Archivo creado!");
            cout<<"Archivo creado!"<<endl;
            cout<<"inodo usado: "<<inode[0]<<endl;
            cout<<"bloques usados: "<<Total_blocks[Total_blocks.size()-1]<<endl;
            cout<<"Directos en: "<<endl;
            for (int i = 0; i < 10; ++i) {
                cout<<i<<"- "<<(new_inode.directos)[i]<<endl;
            }
        }else{
            ui->listTerm->appendPlainText("No hay espacio sificiente en el disco!");
        }
    }else{
        ui->listTerm->appendPlainText("Archivo mas grando a lo que puede almacenar el inodo!");
    }

}

void FileSys::on_txtcommandLine_returnPressed()
{
    exCommand(ui->txtcommandLine->text());
}

int FileSys::searchInFileTable(string name)
{
    for (int i = 0; i < file_data_array.size(); ++i) {
        if(strcpy(file_data_array[i]->name,name.c_str())==0)
        {
            return i;
        }
    }
    return -1;
}

int FileSys::searchInodeInFileTable(string name)
{
    for (int i = 0; i < file_data_array.size(); ++i) {
        if(strcpy(file_data_array[i]->name,name.c_str())==0)
        {
            return file_data_array[i]->index_file;
        }
    }
    return -1;
}

int FileSys::get_NextFree_FileTable()
{
    for (int i = 0; i < file_data_array.size(); ++i) {
        if(file_data_array[i]->index_file==-1)
        {
            return i;
        }
    }
    return -1;
}
