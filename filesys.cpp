#include "filesys.h"
#include "ui_filesys.h"

FileSys::FileSys(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FileSys)
{
    ui->setupUi(this);

    QDir dir(disks_path);

    //cout<<dir.dirName().toStdString()<<endl;
    QFileInfoList list = dir.entryInfoList();

    for (int var = 0; var < list.size(); ++var) {
        QString name = list.at(var).fileName();
        if(name.contains(format))
        {
            disks.push_back((list.at(var).fileName().split(format))[0]);
        }
        //cout<<list.at(var).fileName().toStdString()<<endl;
    }
    //cout<<"List of Disks:"<<endl;
    for (int i = 0; i < disks.size(); ++i) {
        //cout<<disks[i].toStdString()<<endl;
    }

    ui->listTerm->installEventFilter(this);
}

FileSys::~FileSys()
{
    delete ui;
}

void FileSys::exCommand(QString command_line)
{
    ui->listTerm->appendPlainText(mounted_disk + "@root:~" + current_path.join("/") + "$ " + command_line);
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
                        //cout<<"--> Information"<<endl;
                        showInfoDisk(name_disk);
                    }else if(fdisk_command == "d"){ // elimina un disco
                        //cout<<"--> Delete"<<endl;
                        deleteDisk(name_disk);
                    }
                }else if(fdisk_command == "L"){ // enlista todos los discos
                    //cout<<"--> Listar"<<endl;
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
                //cout<<"-> Montar Disco"<<endl;
                mountDisk(commands.at(0));
            }
        }else if(is_mounted_disk){ // comandos si hay un disco esta montado
            if(main_command == "ls") // comand ls -l
            {
                ls();
            }else if(main_command == "cd") // change directory form: cd [path]
            {
                if(commands.size() == 1 )
                {
                    cd(commands.at(0).toStdString());
                }
            }else if(main_command == "mkdir") // crear directory form: mkdir [name_dir]
            {
                if(commands.size() == 1 )
                {
                    mkDir(commands.at(0).toStdString());
                }
            }else if(main_command == "mkfile") // crear file form: mkfile [name_file] [tamanio mb]
            {
                if(commands.size() == 2 )
                {
//                    QtConcurrent::run(this,&FileSys::mkfile,commands.at(0).toStdString(),commands.at(1).toInt());
                    mkfile(commands.at(0).toStdString(),commands.at(1).toInt());
                }
            }else if(main_command == "mkfile2") // crear file form: mkfile [name_file] [tamanio mb]
            {
                if(commands.size() == 2 )
                {
                    mkfile2(commands.at(0).toStdString(),commands.at(1).toInt());
                }
            }else if(main_command == "rm") // delete file or dir form: rm [name]
            {
                if(commands.size()==1)
                    rm(commands.at(0).toStdString());
            }else if(main_command == "export") // exportar fuera del disco form: export [file_name]
            {
                if(commands.size()==1)
                    Export(commands.at(0).toStdString());
            }else if(main_command == "cp") // copiar un archivo de un lugar a otro form: cp [name] [new name] [path]
            {
                if(commands.size()==3)
                {
                    cp(commands.at(0).toStdString(),commands.at(1).toStdString(),commands.at(2));
                }
            }
        }else{
            ui->listTerm->appendPlainText(main_command + " is not a valid command");
        }
        ui->txtcommandLine->setText("");
    }else{
        //cout<<last_command_line.toStdString()<<endl;
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
            mounted_disk = disk_name;
            T_name = (disks_path + mounted_disk + format).toStdString();

//            SuperBlock *SB = new SuperBlock();

            int size_SB = sizeof(SuperBlock);
            char buffer[size_SB];

            in.read((char*)&buffer,size_SB);

            memcpy(&Super_Block,&buffer,size_SB);

            //cout<<"Disk: "<<Super_Block.name<<endl;
            //cout<<"cant of blocks: "<<Super_Block.cantofblock<<endl;
            //cout<<"cant of inodes: "<<Super_Block.cantofinode<<endl;
            //cout<<"Free blocks: "<<Super_Block.freeblock<<endl;
            //cout<<"Free Scpace: "<<Super_Block.freespace<<endl;
            //cout<<"Size: "<<Super_Block.size<<endl;
            //cout<<"Size of Block: "<<Super_Block.sizeofblock<<endl;

            //cout<<"Pointer after read SB: "<<in.tellg()<<endl;
            //cout<<"Size SB: "<<size_SB<<endl;

            double total_size_fs = sizeof(SuperBlock) + sizeof(char)*(Super_Block.cantofblock/8) + sizeof(char)*(Super_Block.cantofinode/8)
                    + sizeof(Inode)*(Super_Block.cantofinode) + sizeof(FileData)*(Super_Block.cantofinode);
            start_datablocks = total_size_fs; //guardando el lugar donde comienza el DataBlock
            //cout<<"start_datablocks: "<<start_datablocks<<endl;

            //Leyendo bitmap
            delete bitmap;
            bitmap = new char[Super_Block.cantofblock/8];
            start_bitmap = in.tellg(); //guardando el lugar donde comienza el bitmap

            in.read(bitmap,sizeof(char)*(Super_Block.cantofblock/8));
            //cout<<"is_in_use 0: "<<is_block_in_use(bitmap,0)<<endl;

            //leyendo bitmap_inodos
            delete bitmap_inodes;
            bitmap_inodes = new char[Super_Block.cantofinode/8];
            start_bitmap_inodes = in.tellg(); //guardando el lugar donde comienza el bitmap_inodes

            in.read(bitmap_inodes,sizeof(char)*(Super_Block.cantofinode/8));
            //cout<<"is_in_use 0 inodes: "<<is_block_in_use(bitmap_inodes,0)<<endl;

            //leyendo FileTable
            int size_DataFile = sizeof(FileData);
            buffer[size_DataFile];
            start_filetable = in.tellg(); //guardando el lugar donde comienza el FileTable

            for (int i = 0; i < file_data_array.size(); ++i) {
                delete file_data_array[i];
            }
            file_data_array.clear();

            for (int i = 0; i < Super_Block.cantofinode; ++i) {
                in.read(buffer,size_DataFile);
                FileData *FD = new FileData();
                memcpy(FD,buffer,size_DataFile);
                file_data_array.push_back(FD);
            }

            start_inodes = in.tellg(); //guardando el lugar donde comienzan los inodos

            //montando el inodo root
            char root_buffer[sizeof(Inode)];
            in.read((char*)&root_buffer,sizeof(Inode));
            memcpy(&current_inode,&root_buffer,sizeof(Inode));

//            current_path.push_back(root_path);

            //cout<<"FD_root_name: "<<file_data_array[0]->name<<endl;
            //cout<<"FD_root_index_inode: "<<file_data_array[0]->index_file<<endl;
            //cout<<"FD_1: "<<file_data_array[1]->name<<endl;
            //cout<<"FD_1_index_inode: "<<file_data_array[1]->index_file<<endl;
            ui->listTerm->appendPlainText("Disco: " + disk_name + " montado!");

        }else{
            ui->listTerm->appendPlainText("No se pudo montar el disco: " + disk_name + "!");
        }

        in.close();
    }else{
        //cout<<"no existe el disco"<<endl;
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
    //cout<<title.toStdString().c_str()<<endl;
    ui->listTerm->appendPlainText(title);
    for (int i = 0; i < disks.size(); ++i) {
        //cout<<disks[i].toStdString()<<endl;
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
            //cout<<"removed file: "<<(disks_path + disk_name + format).toStdString().c_str()<<endl;
        }else{
            //cout<<"no removed"<<endl;
        }
    }else{
        //cout<<"no existe el disco"<<endl;
    }
}

void FileSys::showInfoDisk(QString disk_name)
{
    if(existDisk(disk_name)>=0)
    {
        ifstream in((disks_path + disk_name + format).toStdString().c_str(), ios::in | ios:: out | ios::binary);

        SuperBlock SB;

        int size_SB = sizeof(SuperBlock);
        char buffer[size_SB];

        in.read((char*)&buffer,size_SB);
        in.close();

        memcpy(&SB,&buffer,size_SB);

        //cout<<"Disk: "<<SB.name<<endl;
        //cout<<"cant of blocks: "<<SB.cantofblock<<endl;
        //cout<<"cant of inodes: "<<SB.cantofinode<<endl;
        //cout<<"Free blocks: "<<SB.freeblock<<endl;
        //cout<<"Free Scpace: "<<SB.freespace<<endl;
        //cout<<"Size: "<<SB.size<<endl;
        //cout<<"Size of Block: "<<SB.sizeofblock<<endl;

        ui->listTerm->appendPlainText("Information Disk: " + QString(SB.name));
        ui->listTerm->appendPlainText("-> Size: " + QString::number(SB.size) + " bytes");
        ui->listTerm->appendPlainText("-> Free Space: " + QString::number(SB.freespace) + " bytes");
        ui->listTerm->appendPlainText("-> Size of block: " + QString::number(SB.sizeofblock) + " bytes");
        ui->listTerm->appendPlainText("-> Blocks: " + QString::number(SB.cantofblock));
        ui->listTerm->appendPlainText("-> Free blocks: " + QString::number(SB.freeblock));
        ui->listTerm->appendPlainText("-> Inodes: " + QString::number(SB.cantofinode));
        ui->listTerm->appendPlainText("-> Free inodes: " + QString::number(SB.freeinode));

    }else{
        //cout<<"no existe el disco"<<endl;
    }
}

//size: MB
void FileSys::mkfile(string name, int size)
{
    double size_bytes = size;//*pow(1024,2);
    double bloques_data = ceil(size_bytes/Super_Block.sizeofblock);
    vector<double> Total_blocks = getTotalBlocksToUse(size_bytes,Super_Block.sizeofblock);
    string T_name = (disks_path + mounted_disk + format).toStdString();
    int index = searchInFileTable(name);
    //cout<<"Free size disk antes: "<<Super_Block.freespace<<endl;
    int x = Super_Block.sizeofblock/8;
    double totalSizeInode = (10 + x + pow(x,2) + pow(x,3))*Super_Block.sizeofblock;

    if(index == -1 && Super_Block.freeinode > 0 && Total_blocks[Total_blocks.size()-1] <= Super_Block.freeblock && current_inode.filesize < totalSizeInode)
    {
//        Super_Block.freespace -= size_bytes;

        //creando filetable e inodo
        index = get_NextFree_FileTable();
        double inode = getNextFreeBlock(bitmap_inodes,Super_Block.cantofinode);
        Super_Block.freeinode--;
        strcpy(file_data_array[index]->name,name.c_str());
        file_data_array[index]->index_file = inode;

        Inode new_inode;
        char buffer[sizeof(Inode)];
        read(T_name, (char*)&buffer,start_inodes + inode*sizeof(Inode),sizeof(Inode));
        memcpy(&new_inode,&buffer,sizeof(Inode));
        strcpy(new_inode.permisos,"-rwxrwxrwx");

        //escribiendo filedata en el disco
        write(T_name,(char*)file_data_array[index],start_filetable + index*sizeof(FileData),sizeof(FileData));

        //actualizamos el FileTable del directorio actual
        FileData fd;
        strcpy(fd.name,name.c_str());
        fd.index_file = inode;
        updateFileTableFromDir(&current_inode,&fd);
        write(T_name,(char*)&current_inode,start_inodes + current_inode_ptr*sizeof(Inode),sizeof(Inode));

        double size_bytes_temp = size_bytes;
        double size_to_write = Super_Block.sizeofblock;
        char buffer_data[Super_Block.sizeofblock];
        double percent=0;
        for (int i = 0; i < bloques_data; ++i) {
            for (int i = 0; i < Super_Block.sizeofblock; ++i) {
                buffer_data[i] = 'K';//rand()%25 + 65;
            }
            if(size_bytes_temp<size_to_write)
                size_to_write = size_bytes_temp;
            size_bytes_temp-=size_to_write;
            writeInode(&new_inode,T_name,(char*)&buffer_data,size_to_write);
            percent+=size_to_write;
//            ui->listTerm->appendPlainText(QString::number((percent/size)*100) + "%");
        }
        //escribiendo inodo en el disco
        write(T_name,(char*)&new_inode,start_inodes + inode*sizeof(Inode),sizeof(Inode));

        //guardar bitmaps
        write(T_name,bitmap,start_bitmap,Super_Block.cantofblock/8);
        write(T_name,bitmap_inodes,start_bitmap_inodes,Super_Block.cantofinode/8);

        //guardar Super Block 'en duda aun'
//        Super_Block.freeblock -= Total_blocks[Total_blocks.size()-1];
//        (Super_Block.freeinode)--;
//        Super_Block.freespace -= (size_bytes + (Total_blocks[Total_blocks.size()-1] - bloques_data)*Super_Block.sizeofblock);
//        write(T_name,(char*)&Super_Block,0,sizeof(SuperBlock));
        updateSuperBlock();

        ui->listTerm->appendPlainText("Archivo creado!");
        //cout<<"Archivo creado!"<<endl;
        //cout<<"inodo usado: "<<inode<<endl;
        //cout<<"bloques usados: "<<Total_blocks[Total_blocks.size()-1]<<endl;
        //cout<<"Directos en: "<<endl;
        for (int i = 0; i < 10; ++i) {
            //cout<<i<<"- "<<(new_inode.directos)[i]<<endl;
        }

        //lee IS
        int x = Super_Block.sizeofblock/8;
        double buf[x];
        read(T_name,(char*)&buf,new_inode.indirectossimples*Super_Block.sizeofblock,Super_Block.sizeofblock);
    //    memcpy(&ino,buf,size_block);

        if(new_inode.indirectossimples!=-1)
        {
            //cout<<"Leido del Disco IS en!: "<<new_inode.indirectossimples<<endl;
            for (int i = 0; i < x; ++i) {
                //cout<<"data en- "<<buf[i]<<endl;
    //            if(buf[i]==-1) //cout<<"-nan = -1"<<endl;
            }
        }

        //lee ID
        read(T_name,(char*)&buf,new_inode.indirectosdobles*Super_Block.sizeofblock,Super_Block.sizeofblock);
    //    memcpy(&ino,buf,size_block);
        //cout<<"Leido del Disco ID en!: "<<new_inode.indirectosdobles<<endl;

        if(new_inode.indirectosdobles!=-1)
        {
            for (int i = 0; i < x; ++i) {
                //cout<<"IS en- "<<buf[i]<<endl;
                double buf2[x];
                read(T_name,(char*)&buf2,buf[i]*Super_Block.sizeofblock,Super_Block.sizeofblock);
        //        //cout<<"IS del ID"<<endl;
                if(buf[i]!=-1)
                {
                    for (int j = 0; j < x; ++j) {
                        //cout<<"data en:- "<<buf2[j]<<endl;
                    }
                }
            }
        }

        //lee IT
        read(T_name,(char*)&buf,new_inode.indirectostriples*Super_Block.sizeofblock,Super_Block.sizeofblock);
    //    memcpy(&ino,buf,size_block);
        //cout<<"Leido del Disco IT en!: "<<new_inode.indirectostriples<<endl;
        if(new_inode.indirectostriples!=-1)
        {
            for (int i = 0; i < x; ++i) {
                //cout<<"ID en- "<<buf[i]<<endl;
                double buf2[x];
                read(T_name,(char*)&buf2,buf[i]*Super_Block.sizeofblock,Super_Block.sizeofblock);
        //        //cout<<"ID del IT"<<endl;
                if(buf[i]!=-1)
                {
                    for (int j = 0; j < x; ++j) {
                        //cout<<"IS en- "<<buf2[j]<<endl;
                        double buf3[x];
                        read(T_name,(char*)&buf3,buf2[j]*Super_Block.sizeofblock,Super_Block.sizeofblock);
            //            //cout<<"IS del ID del IT"<<endl;
                        if(buf2[j]!=-1)
                        {
                            for (int k = 0; k < x; ++k) {
                                //cout<<"data en- "<<buf3[k]<<endl;
                            }
                        }
                    }
                }
            }
        }
    }
//    else{
//        //cout<<"Ya existe el nombre"<<endl;
//        double inode = file_data_array[index]->index_file;
//        char *buffer = new char[sizeof(Inode)];
//        Inode new_inode;
//        read(T_name, buffer,start_inodes + inode*sizeof(Inode),sizeof(Inode));
//        memcpy(&new_inode,buffer,sizeof(Inode));
//        //cout<<"lastDataBlock: "<<new_inode.lastDataBlock<<endl;

//        double size_bytes_temp = size_bytes;
//        double size_to_write = Super_Block.sizeofblock;
//        for (int i = 0; i < bloques_data; ++i) {
//            char *buffer = new char[Super_Block.sizeofblock];
//            for (int i = 0; i < Super_Block.sizeofblock; ++i) {
//                buffer[i] = 'K';//rand()%25 + 65;
//            }
//            if(size_bytes_temp<size_to_write)
//                size_to_write = size_bytes_temp;
//            size_bytes_temp-=size_to_write;
//            writeInode(&new_inode,T_name,buffer,size_to_write);
//        }
//        //escribiendo inodo en el disco
//        write(T_name,(char*)&new_inode,start_inodes + inode*sizeof(Inode),sizeof(Inode));
//        //cout<<"termino de escribir!"<<endl;

//        //escribimos el superblock
//        updateSuperBlock();
//    }
    //cout<<"Free size disk: "<<Super_Block.freespace<<endl;
}

void FileSys::mkDir(string name)
{
    //falta validar que quepa otro archivo en el 'dir' actual
    //cout<<"Free size disk antes: "<<Super_Block.freespace<<endl;

    int x = Super_Block.sizeofblock/8;
    double totalSizeInode = (10 + x + pow(x,2) + pow(x,3))*Super_Block.sizeofblock;
    int index = searchInFileTable(name);

    if(index==-1 && current_inode.filesize < totalSizeInode )
    {
        string T_name = (disks_path + mounted_disk + format).toStdString();

        //creando filetable e inodo
        int index = get_NextFree_FileTable();
        double inode = getNextFreeBlock(bitmap_inodes,Super_Block.cantofinode);
        Super_Block.freeinode--;
        strcpy(file_data_array[index]->name,name.c_str());
        file_data_array[index]->index_file = inode;

        //escribiendo filedata en el disco
        write(T_name,(char*)file_data_array[index],start_filetable + index*sizeof(FileData),sizeof(FileData));

        //nuevo inodo para el nuevo 'dir'
        Inode new_dir;
        initInode(&new_dir);
        strcpy(new_dir.permisos,"drwxrwxrwx");
//        for (int i = 0; i < 10; ++i) {
//            (new_dir.directos)[i] = -1;
//        }
//        new_dir.filesize = -1;
//        new_dir.blockuse = -1;
//        new_dir.indirectossimples = -1;
//        new_dir.indirectosdobles = -1;
//        new_dir.indirectostriples = -1;
//        new_dir.lastDataBlock = -1;

        FileData refer_to_father;
        strcpy(refer_to_father.name,"..");
        refer_to_father.index_file = current_inode_ptr;

        updateFileTableFromDir(&new_dir,&refer_to_father);

        //escribiendo inodo en el disco
        write(T_name,(char*)&new_dir,start_inodes + inode*sizeof(Inode),sizeof(Inode));

        //referencia para el 'dir' actual
        FileData refer;
        strcpy(refer.name,name.c_str());
        refer.index_file = inode;

        //agregamos la referencia al FileTable del 'dir' actual
        updateFileTableFromDir(&current_inode,&refer);

        //actualizando inodo actual en el disco
        write(T_name,(char*)&current_inode,start_inodes + current_inode_ptr*sizeof(Inode),sizeof(Inode));

        //actualizando SuperBlock
        updateSuperBlock();
    }else{
        ui->listTerm->appendPlainText(QString(("El dir: " + name + " ya existe!").c_str()));
    }

    //cout<<"Free size disk: "<<Super_Block.freespace<<endl;
}

void FileSys::mkfile2(string name, int size)
{
    double size_bytes = size;//*pow(1024,2);
    double bloques_data = ceil(size_bytes/Super_Block.sizeofblock);
    vector<double> Total_blocks = getTotalBlocksToUse(size_bytes,Super_Block.sizeofblock);
//    string T_name = (disks_path + mounted_disk + format).toStdString();

    if(Total_blocks[0] >= 0 && Super_Block.freeinode > 0)
    {
        char *buffer = new char[Super_Block.sizeofblock];
//        memset(buffer,'N',Super_Block.sizeofblock);
        for (int i = 0; i < Super_Block.sizeofblock; ++i) {
            buffer[i] = rand()%25 + 65;
        }
        vector<double> blocks = getFreeBlocks(bitmap,Super_Block.cantofblock,Total_blocks[Total_blocks.size()-1]);
        vector<double> inode = getFreeBlocks(bitmap_inodes,Super_Block.cantofinode,1);

        //cout<<"Bloques dados por el bitmap!"<<endl;
        for (double i = 0; i < Total_blocks[Total_blocks.size()-1]; ++i) {
            //cout<<"-- "<<blocks[i]<<endl;
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

            //cout<<"caso: "<<Total_blocks[0]<<endl;
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
            //cout<<"Archivo creado!"<<endl;
            //cout<<"inodo usado: "<<inode[0]<<endl;
            //cout<<"bloques usados: "<<Total_blocks[Total_blocks.size()-1]<<endl;
            //cout<<"Directos en: "<<endl;
            for (int i = 0; i < 10; ++i) {
                //cout<<i<<"- "<<(new_inode.directos)[i]<<endl;
            }
        }else{
            ui->listTerm->appendPlainText("No hay espacio sificiente en el disco!");
        }
    }else{
        ui->listTerm->appendPlainText("Archivo mas grando a lo que puede almacenar el inodo!");
    }

}

void FileSys::updateFileTableFromDir(Inode *inode, FileData *data)
{
    string T_name = (disks_path + mounted_disk + format).toStdString();
    double size = sizeof(FileData);
//    Super_Block.freespace-=size;
    double size_temp = size;
    double size_to_write = Super_Block.sizeofblock;
    double dataBlocks = ceil(size/size_to_write);
    char *src_buffer = (char*)data;
    char *buffer = new char[(int)size_to_write];
    double iterate=0;

    for (int i = 0; i < dataBlocks; ++i) {
        if(size_temp<size_to_write)
            size_to_write = size_temp;
        size_temp-=size_to_write;
        memcpybuffer(buffer,src_buffer,size_to_write,iterate,size);
        iterate+=size_to_write;
        writeInode(inode,T_name,buffer,size_to_write);
    }
    delete buffer;

}

void FileSys::updateSuperBlock()
{
    string T_name = (disks_path + mounted_disk + format).toStdString();
    write(T_name,(char*)&Super_Block,0,sizeof(SuperBlock));
}

void FileSys::ls()
{
    if(is_mounted_disk){
        string titles = "permisos\towner\tgroup\tsize\tname";
        ui->listTerm->appendPlainText(QString(titles.c_str()));
        if(current_inode.filesize>0){
            string T_name = (disks_path + mounted_disk + format).toStdString();
            char *all_datablocks;
            readDataBlocksFrom(T_name,all_datablocks,&current_inode,Super_Block.sizeofblock);

            vector<FileData*> filetable = getFileTableFrom(current_inode,all_datablocks);
            //cout<<"current path: "<<current_path.join("/").toStdString().c_str()<<endl;
            for (int i = 0; i < filetable.size(); ++i) {
                //cout<<"Nombre: "<<filetable[i]->name<<" index: "<<filetable[i]->index_file<<endl;
                if(strcmp(filetable[i]->name,"..")!=0)
                {
                    char buffer[sizeof(Inode)];
                    Inode inode;// = new Inode();
                    initInode(&inode);
                    read(T_name,buffer,start_inodes + (filetable[i]->index_file)*sizeof(Inode),sizeof(Inode));
                    memcpy(&inode,buffer,sizeof(Inode));
                    string file = string(inode.permisos) + "\troot\troot\t" + QString::number(inode.filesize).toStdString() + "\t" + string(filetable[i]->name);
                    //cout<<file.c_str()<<endl;
                    ui->listTerm->appendPlainText(QString(file.c_str()));
                }
            }
            delete all_datablocks;
            filetable.clear();
        }
    }
}

void FileSys::cd(string dir_to_move)
{
    string T_name = (disks_path + mounted_disk + format).toStdString();
    if(dir_to_move==".." && current_path.size()==0)
    {
        return;
    }else{
        char *all_datablocks;
        readDataBlocksFrom(T_name,all_datablocks,&current_inode,Super_Block.sizeofblock);

        vector<FileData*> filetable = getFileTableFrom(current_inode,all_datablocks);
        for (int i = 0; i < filetable.size(); ++i) {
            if(strcmp(filetable[i]->name,dir_to_move.c_str())==0)
            {
                char buffer[sizeof(Inode)];
                Inode inode;// = new Inode();
                read(T_name,(char*)&buffer,start_inodes + (filetable[i]->index_file)*sizeof(Inode),sizeof(Inode));
                memcpy(&inode,(char*)&buffer,sizeof(Inode));
                if((inode.permisos)[0]=='d'){
                    memcpy(&current_inode,(char*)&buffer,sizeof(Inode));
                    current_inode_ptr = filetable[i]->index_file;
                    if(dir_to_move=="..")
                    {
                        current_path.pop_back();
                    }else{
                        current_path.push_back(QString(dir_to_move.c_str()));
                    }
                }else{
                    ui->listTerm->appendPlainText(QString((dir_to_move + " no es un 'dir'!").c_str()));
                }
                return;
            }
        }
        ui->listTerm->appendPlainText(QString((dir_to_move + " no existe!").c_str()));
        for (int i = 0; i < filetable.size(); ++i) {
            delete filetable[i];
        }
        filetable.clear();
    }
}

void FileSys::Export(string file_name)
{
    string T_name = (disks_path + mounted_disk + format).toStdString();
    string exportTo = dirToExport.toStdString() + file_name;
    double index_inode = searchInodeInFileTable(file_name);
    if(index_inode!=-1){
        //cout<<"Exportando..."<<endl;
        Inode inode;
        char buff[sizeof(Inode)];
        read(T_name,(char*)&buff,start_inodes + index_inode*sizeof(Inode),sizeof(Inode));
        memcpy(&inode,buff,sizeof(Inode));
        ExportFile(T_name,exportTo,&inode,Super_Block.sizeofblock);
    }
}

void FileSys::cp(string file, string new_name, QString path)
{
//    string T_name = (disks_path + mounted_disk + format).toStdString();
    QStringList path_list = path.split("/");
    QString dir = path_list.last();

    int index = searchInFileTable(dir.toStdString());
    if(index!=-1)
    {
        if(Super_Block.freeinode>0)
        {


            char buff[sizeof(Inode)];
            Inode dir_to;
            //cout<<"DIR to COPY: "<<dir.toStdString().c_str()<<endl;
            read(T_name,(char*)&buff,start_inodes + (file_data_array[index]->index_file)*sizeof(Inode),sizeof(Inode));
            memcpy(&dir_to,buff,sizeof(Inode));

            char *all_datablocks;
            readDataBlocksFrom(T_name,all_datablocks,&current_inode,Super_Block.sizeofblock);

            vector<FileData*> filetable = getFileTableFrom(current_inode,all_datablocks);
            Inode from,to;
            initInode(&to);
            bool found=false;

            for (int i = 0; i < filetable.size(); ++i) {
                if(strcmp(filetable[i]->name,file.c_str())==0)
                {
                    read(T_name,(char*)&buff,start_inodes + (filetable[i]->index_file)*sizeof(Inode),sizeof(Inode));
                    memcpy(&from,&buff,sizeof(Inode));
                    found=true;
                    break;
                }
            }

            if(found)
            {
                //cout<<"encontrado: "<<file.c_str()<<endl;
                if(from.permisos[0]=='-')
                {
                    strcpy(to.permisos,from.permisos);
                    if((getTotalSizeUsed(from.filesize,from.blockuse,Super_Block.sizeofblock) + sizeof(FileData)) <= Super_Block.freespace)
                    {
                        double inode = getNextFreeBlock(bitmap_inodes,Super_Block.cantofinode);
                        double size_to_write = Super_Block.sizeofblock;
                        double size = from.filesize;
                        vector<double> blocks = getDataBlocksFrom(T_name,&from,size_to_write);

                        for (int i = 0; i < blocks.size(); ++i) {
                            if(size<size_to_write)
                                size_to_write=size;
                            size-=size_to_write;

                            char buffer[(int)size_to_write];
                            read(T_name,(char*)buffer,blocks[i]*(Super_Block.sizeofblock),size_to_write);
                            //cout<<"DATA leida: "<<buffer<<endl;
                            writeInode(&to,T_name,(char*)buffer,size_to_write);
                        }
                        //escribimos el inodo
                        write(T_name,(char*)&to,start_inodes + inode*sizeof(Inode),sizeof(Inode));
                        //escribimos filetable
                        double index_FT = get_NextFree_FileTable();
                        strcpy(file_data_array[(int)index_FT]->name,new_name.c_str());
                        file_data_array[(int)index_FT]->index_file = inode;
                        write(T_name,(char*)file_data_array[(int)index_FT],start_filetable + index_FT*sizeof(FileData),sizeof(FileData));

                        //escribiendo filedata del directorio donde se copiara
                        updateFileTableFromDir(&dir_to,file_data_array[(int)index_FT]);

                        //escribiendo inode dir_to
                        write(T_name,(char*)&dir_to,start_inodes + (file_data_array[index]->index_file)*sizeof(Inode),sizeof(Inode));

                        //guardar bitmaps
                        write(T_name,bitmap,start_bitmap,Super_Block.cantofblock/8);
                        write(T_name,bitmap_inodes,start_bitmap_inodes,Super_Block.cantofinode/8);

                        updateSuperBlock();
                    }
                }
            }
            for (int i = 0; i < filetable.size(); ++i) {
                delete filetable[i];
            }
            filetable.clear();
        }
    }
}

void FileSys::writeInode(Inode *inode, string disk, char *buffer, double size)
{
    //los permisos deben darsele afuera de esta funcion
    //el inodo como tal debe guardarse afuera de esta funcion
    Super_Block.freespace -= size;
    if(inode->filesize==-1) //virgen
    {
        inode->filesize = size;
        inode->blockuse = 1;
        double block = getNextFreeBlock(bitmap,Super_Block.cantofblock);
        Super_Block.freeblock--;
        inode->directos[0] = block;
        inode->lastDataBlock = block;
        write(disk,buffer,block*Super_Block.sizeofblock,size);
    }else{ //no virgen
        double used_blocks = inode->filesize/Super_Block.sizeofblock;
        int x = Super_Block.sizeofblock/8;
        double block_used_percent = used_blocks - floor(used_blocks);
        double free_of_block = Super_Block.sizeofblock - block_used_percent*Super_Block.sizeofblock;


        if(block_used_percent>0)
        {
            if(size <= free_of_block)
            {
                write(disk,buffer,inode->lastDataBlock*Super_Block.sizeofblock + block_used_percent*Super_Block.sizeofblock,size);
                inode->filesize += size;
                return;
            }else{
                write(disk,buffer,inode->lastDataBlock*Super_Block.sizeofblock + block_used_percent*Super_Block.sizeofblock,free_of_block);
                inode->filesize += free_of_block;

//                char *buf_temp = new char[Super_Block.sizeofblock];
//                strcpy(buf_temp,buffer);
////                buffer = new char[Super_Block.sizeofblock];
//                memset(buffer,0,Super_Block.sizeofblock);
                //cout<<"buffer antes: "<<buffer<<endl;
                //cout<<"free of blocks: "<<free_of_block<<endl;
                //cout<<"size: "<<size<<endl;
//                for (int i = (block_used_percent*Super_Block.sizeofblock),j=0; i < size; ++i, j++) {
//                    buffer[j] = buf_temp[i];
//                }
                char buf_temp[(int)size];
                memcpy(buf_temp,buffer,size);
                size -= free_of_block;
                buffer = new char[(int)size];
                memcpy(buffer,&buf_temp[(int)(free_of_block-1)],size);

//                string buf_temp(buffer);
//                string buf = buf_temp.substr(free_of_block,size);
//                buffer = const_cast<char *>(buf.c_str());
                buffer[0] = 'Y';
                //cout<<"bytes copiados: "<<buffer<<endl;
            }
        }

//        if(block_used_percent==0) // si los bloques usados estan todos llenos
//        {
            if(inode->blockuse < 10) //CASE 0:si los bloques de data usados son menorea a los DIRECTOS
            {
                double block = getNextFreeBlock(bitmap,Super_Block.cantofblock); //pide bloque libre al bitmap
                Super_Block.freeblock--;
                int save_to = inode->blockuse++;    //guardamos el index de la siguente posicion libre de los DIRECTOS
                inode->directos[save_to] = block;   //guardamos la direccion del bloque en los directos
                inode->filesize += size;            //aumentamos el filesize del archivo
                inode->lastDataBlock = block;
                write(disk,buffer,block*Super_Block.sizeofblock,size);   //escribimos el bloque de data
            }else if(inode->blockuse < (10 + x)){   //CASE 1: si bloques de data usados es menor I_SIMPLES mas los anteriores
                double IS_ptr = inode->indirectossimples;
                double IS[x];         //bloque de IS con las direcciones a los bloques de data
                if(IS_ptr==-1)                      //si aun no hemos utilizado los I_SIMPLES
                {
                    IS_ptr = getNextFreeBlock(bitmap,Super_Block.cantofblock); //le pedimos un bloque al bitmap para el IS
                    Super_Block.freespace -= Super_Block.sizeofblock;
                    Super_Block.freeblock--;
                    inode->indirectossimples = IS_ptr; //actualizamos el inodo
                    //llenamos el bloque con -1 el bloque
                    for (int i = 0; i < x; ++i) {
                        IS[i] = -1;
                    }
                }else{ //si ya habiamos utilizado antes el IS solo leemos lo que ya tenia
                    read(disk,(char*)&IS,IS_ptr*Super_Block.sizeofblock,Super_Block.sizeofblock);
                }

                double block = getNextFreeBlock(bitmap,Super_Block.cantofblock); //bloque para la data
                Super_Block.freeblock--;
                int save_to = inode->blockuse++ - 10; //sacamos el index en los IS donde guardaremos la direccion del bloque de data
                IS[save_to] = block;
                inode->filesize += size;
                inode->lastDataBlock = block;

                //escribimos el bloque de data y el bloque de IS como corresponda
                write(disk,buffer,block*Super_Block.sizeofblock,size);
                write(disk,(char*)&IS,IS_ptr*Super_Block.sizeofblock,Super_Block.sizeofblock);
            }else if(inode->blockuse < (10 + x + pow(x,2))){ //CASE 2: si bloques de data usados es menor I_DOBLES mas los anteriores
                double ID_ptr = inode->indirectosdobles;
                double ID[x];
                double ID_IS[x];
                if (ID_ptr==-1) { //si no hemos usado I_DOBLES antes
                    ID_ptr = getNextFreeBlock(bitmap,Super_Block.cantofblock); //pedimos un bloque para los ID
                    Super_Block.freespace -= Super_Block.sizeofblock;
                    Super_Block.freeblock--;
                    inode->indirectosdobles = ID_ptr; //actualizamod el inodo
                    for (int i = 0; i < x; ++i) { //lo inicializamos
                        ID[i] = -1;
                    }
                }else{ //si ya lo habiamos usado solo lo leemos
                    read(disk,(char*)&ID,ID_ptr*Super_Block.sizeofblock,Super_Block.sizeofblock);
                }
                double b_data_ID = inode->blockuse++ - 10 - x;  //cuantos bloques de DATA tiene el ID?
                double IS_to_write = b_data_ID/x;               //cuantos IS esta usando para tenerlos?
                if((IS_to_write - floor(IS_to_write))==0)       //los IS que esta usando ya estan llenos?
                {
                    //pide un bloque para añadirlo a los IS que usa el ID
                    double addIS_toDoble = getNextFreeBlock(bitmap,Super_Block.cantofblock);
                    Super_Block.freespace -= Super_Block.sizeofblock;
                    Super_Block.freeblock--;
                    ID[(int)IS_to_write] = addIS_toDoble; //lo agrega
                    for (int i = 0; i < x; ++i) { //lo inicializa con -1
                        ID_IS[i] = -1;
                    }
                    //escribe el ID en el disco
                    write(disk,(char*)&ID,inode->indirectosdobles*Super_Block.sizeofblock,Super_Block.sizeofblock);
                }else{ //sino, entonces hay espacio aun en el ultimo IS que esta usando el ID
                    IS_to_write = floor(IS_to_write); //lo redondeamos para abajo para obtener el index de ese IS en el ID
                    read(disk,(char*)&ID_IS,ID[(int)IS_to_write]*Super_Block.sizeofblock,Super_Block.sizeofblock); //lemos el IS
                }
                int indexInIS = b_data_ID - x*IS_to_write; //sacamos el index siguiente libre en el IS

                double block = getNextFreeBlock(bitmap,Super_Block.cantofblock); //pedimos bloque para la data
                Super_Block.freeblock--;
                ID_IS[indexInIS] = block; //lo guardamos el los IS que tiene el ID
                inode->filesize += size;
                inode->lastDataBlock = block;

                //escribimos el bloque de data y el bloque del IS que pertenece al ID en el disco como corresponde
                write(disk,buffer,block*Super_Block.sizeofblock,size);
                write(disk,(char*)&ID_IS,ID[(int)IS_to_write]*Super_Block.sizeofblock,Super_Block.sizeofblock);
            }else if(inode->blockuse < (10 + x + pow(x,2) + pow(x,3))) //CASE 3: si bloques de data usados es menor I_TRIPLES mas los anteriores
            {
                double IT_ptr = inode->indirectostriples;
                double IT[x];
                double IT_ID[x];
                double IT_ID_IS[x];
                if(IT_ptr == -1) //si no hemos usado IT antes
                {
                    IT_ptr = getNextFreeBlock(bitmap,Super_Block.cantofblock); //pedimos bloque para IT
                    Super_Block.freespace -= Super_Block.sizeofblock;
                    Super_Block.freeblock--;
                    inode->indirectostriples = IT_ptr; //actualizamos el inodo
                    for (int i = 0; i < x; ++i) { //lo inicializamos
                        IT[i] = -1;
                    }
                }else{ //si ya lo habiamos usado solo lo leemos
                    read(disk,(char*)&IT,IT_ptr*Super_Block.sizeofblock,Super_Block.sizeofblock);
                }
                double b_data_IT = inode->blockuse++ - 10 - x - pow(x,2); //cuantos bloques de DATA tiene el IT?
                double ID_to_write = b_data_IT/pow(x,2);                  //cuantos ID esta usando el IT?
                if((ID_to_write - floor(ID_to_write))==0)                 //los ID que esta usando ya estan llenos?
                {
                    //pide un bloque para añadirlo a los ID que usa el IT
                    double addID_toTriple = getNextFreeBlock(bitmap,Super_Block.cantofblock);
                    Super_Block.freespace -= Super_Block.sizeofblock;
                    Super_Block.freeblock--;
                    IT[(int)ID_to_write] = addID_toTriple; //lo agrega
                    for (int i = 0; i < x; ++i) { //lo inicializa
                        IT_ID[i] = -1;
                    }
                    write(disk,(char*)&IT,IT_ptr*Super_Block.sizeofblock,Super_Block.sizeofblock);
                }else{ //sino, entonces hay espacio aun en el ultimo ID que esta usando el IT
                    ID_to_write = floor(ID_to_write); //lo redondeamos hacia abajo para obtener el index en el IT
                    read(disk,(char*)&IT_ID,IT[(int)ID_to_write]*Super_Block.sizeofblock,Super_Block.sizeofblock); //lo leemos
                }

                double IS_to_write = b_data_IT/x;                         //cuantos IS esta usando el IT?
                double indexIS_inID = IS_to_write - x*ID_to_write;        //cual es el index del IS en el ID correspondiente?
                if((IS_to_write - floor(IS_to_write))==0)                 //los IS que esta usando ya estan llenos?
                {
                    //pide un bloque para añadirlo a los IS que usa el ID correspondiente
                    double addIS_toIDoble = getNextFreeBlock(bitmap,Super_Block.cantofblock);
                    Super_Block.freespace -= Super_Block.sizeofblock;
                    Super_Block.freeblock--;
                    IT_ID[(int)indexIS_inID] = addIS_toIDoble; //lo agrega
                    for (int i = 0; i < x; ++i) { //lo inicializa
                        IT_ID_IS[i] = -1;
                    }
                    write(disk,(char*)&IT_ID,IT[(int)ID_to_write]*Super_Block.sizeofblock,Super_Block.sizeofblock);
                }else{ //sino, entonces hay espacio aun en el ultimo IS del ID correspondiente que esta usando el IT
                    IS_to_write = floor(IS_to_write); //redondeamos hacia abajo para luego sacar el index donde va ir la data en ese IS
                    indexIS_inID = floor(indexIS_inID); //redondeamos hacia abajo para sacar el index del IS en el ID
                    read(disk,(char*)&IT_ID_IS,IT_ID[(int)indexIS_inID]*Super_Block.sizeofblock,Super_Block.sizeofblock); //lo leemos
                }
                int indexInIS = b_data_IT - x*IS_to_write; //calculamos el index de la DATA dentro del IS

                double block = getNextFreeBlock(bitmap,Super_Block.cantofblock); //pide bloque para la DATA
                Super_Block.freeblock--;
                IT_ID_IS[indexInIS] = block; //lo agrega al IS
                inode->filesize += size;
                inode->lastDataBlock = block;

                //escribimos en el disco el bloque de DATA y el IS correcpondiente
                write(disk,buffer,block*Super_Block.sizeofblock,size);
                write(disk,(char*)&IT_ID_IS,IT_ID[(int)indexIS_inID]*Super_Block.sizeofblock,Super_Block.sizeofblock);
            }
//        }else{ //si al ultimo bloque le falta por llenarse

//        }

    }
}

void FileSys::addFile(string filename)
{
    if(is_mounted_disk){
        ifstream in(filename.c_str(),ios::in | ios::out | ios::binary);

        if(in.is_open())
        {
            in.seekg(0,ios::end);
            double filesize = in.tellg();
            double dataBlocks = ceil(filesize/Super_Block.sizeofblock);
            double size_to_write = Super_Block.sizeofblock;
            in.seekg(0,ios::beg);

            //cout<<"FileSize addFile: "<<filesize<<endl;

            if((getTotalSizeUsed(filesize,dataBlocks,Super_Block.sizeofblock) + sizeof(FileData)) <= Super_Block.freespace)
            {
                string name = QString(filename.c_str()).split("/").last().toStdString();
                double index = searchInFileTable(name);

                if(index==-1)
                {
                    index = get_NextFree_FileTable();
                    double inode = getNextFreeBlock(bitmap_inodes,Super_Block.cantofinode);
                    Super_Block.freeinode--;

                    //escribiendo en el filetable general
                    strcpy(file_data_array[index]->name,name.c_str());
                    file_data_array[index]->index_file = inode;
                    write(T_name,(char*)file_data_array[index],start_filetable + index*sizeof(FileData),sizeof(FileData));

                    Inode new_inode;
                    initInode(&new_inode);
                    strcpy(new_inode.permisos,"-rwxrwxrwx");

                    for (int i = 0; i < dataBlocks; ++i) {
                        if(filesize<size_to_write)
                            size_to_write=filesize;
                        filesize-=size_to_write;

                        char buff[(int)size_to_write];
                        in.read((char*)&buff,size_to_write);
                        writeInode(&new_inode,T_name,(char*)&buff,size_to_write);
                    }
                    //escribiendo el inodo
                    write(T_name,(char*)&new_inode,start_inodes + inode*sizeof(Inode),sizeof(Inode));

                    //actualizando filetable del dir actual
                    updateFileTableFromDir(&current_inode,file_data_array[index]);

                    //escribiendo el actual
                    write(T_name,(char*)&current_inode,start_inodes + current_inode_ptr*sizeof(Inode),sizeof(Inode));

                    //guardar bitmaps
                    write(T_name,bitmap,start_bitmap,Super_Block.cantofblock/8);
                    write(T_name,bitmap_inodes,start_bitmap_inodes,Super_Block.cantofinode/8);

                    updateSuperBlock();
                }
            }
        }

        in.close();
    }
}

void FileSys::rm(string filename)
{
    if(is_mounted_disk)
    {
        double indexFileTableGlobal = searchInFileTable(filename);

        if(indexFileTableGlobal>=0)
        {
            //cout<<"Eliminando "<<filename.c_str()<<endl;
            char *all_datablocks;
            readDataBlocksFrom(T_name,all_datablocks,&current_inode,Super_Block.sizeofblock);

            vector<FileData*> filetable = getFileTableFrom(current_inode,all_datablocks);
            int indexFileInFileTable = -1;
            Inode toDelete;
            char buff[sizeof(Inode)];

            for (int i = 0; i < filetable.size(); ++i) {
                if(strcmp(filetable[i]->name,filename.c_str())==0)
                {
                    read(T_name,(char*)&buff,start_inodes + (filetable[i]->index_file)*sizeof(Inode),sizeof(Inode));
                    memcpy(&toDelete,&buff,sizeof(Inode));
                    indexFileInFileTable = i;
                    //cout<<"indexFileInFileTable: "<<indexFileInFileTable<<endl;
                    break;
                }
            }

            if(indexFileInFileTable>=0)
            {
                if(toDelete.permisos[0]=='-')
                {
                    double inode_index = filetable[indexFileInFileTable]->index_file;
                    //cout<<"inode_index: "<<inode_index<<endl;
                    filetable.erase(filetable.begin() + indexFileInFileTable);
                    strcpy(file_data_array[(int)indexFileTableGlobal]->name,"");
                    file_data_array[(int)indexFileTableGlobal]->index_file = -1;
                    vector<double> blocksUsedForDir = getAllBlocksUsedFor(T_name,&current_inode,Super_Block.sizeofblock);
                    vector<double> blocksUdedForFile = getAllBlocksUsedFor(T_name,&toDelete,Super_Block.sizeofblock);
                    initInode(&toDelete);
                    initInode(&current_inode);
                    strcpy(current_inode.permisos,"drwxrwxrwx");
                    write(T_name,(char*)&toDelete,start_inodes + inode_index*sizeof(Inode),sizeof(Inode));
                    write(T_name,(char*)file_data_array[(int)indexFileTableGlobal],start_filetable + indexFileTableGlobal*sizeof(FileData),sizeof(FileData));
                    setBlock_unuse(bitmap_inodes,inode_index);
                    set_blocks_in_unuse(bitmap,blocksUsedForDir);
                    set_blocks_in_unuse(bitmap,blocksUdedForFile);

                    //guardar bitmaps
                    write(T_name,bitmap,start_bitmap,Super_Block.cantofblock/8);
                    write(T_name,bitmap_inodes,start_bitmap_inodes,Super_Block.cantofinode/8);

                    for (int i = 0; i < filetable.size(); ++i) {
                        updateFileTableFromDir(&current_inode,filetable[i]);
                    }
                    write(T_name,(char*)&current_inode,start_inodes + current_inode_ptr*sizeof(Inode),sizeof(Inode));

                    updateSuperBlock();
                }
            }
            for (int i = 0; i < filetable.size(); ++i) {
                delete filetable[i];
            }
            filetable.clear();
            delete all_datablocks;
        }
    }
}

int FileSys::searchInFileTable(string name)
{
    for (int i = 0; i < file_data_array.size(); ++i) {
        if(strcmp(file_data_array[i]->name,name.c_str())==0)
        {
            return i;
        }
    }
    return -1;
}

int FileSys::searchInodeInFileTable(string name)
{
    for (int i = 0; i < file_data_array.size(); ++i) {
        if(strcmp(file_data_array[i]->name,name.c_str())==0)
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

void FileSys::on_txtcommandLine_returnPressed()
{
    exCommand(ui->txtcommandLine->text());
}

void FileSys::on_btnAddFile_clicked()
{
    if(is_mounted_disk)
    {
        delete input;
        input = new QFileDialog();
        string filePath = input->getOpenFileName().toStdString();
        //cout<<filePath.c_str()<<endl;
        addFile(filePath);
    }else{
        ui->listTerm->appendPlainText("No hay disco montado.");
    }
}

void FileSys::on_btnBlocks_clicked()
{
    if(is_mounted_disk){
        delete blocks;
        blocks = new BlocksBox(NULL,&file_data_array,Super_Block.cantofblock,Super_Block.FS_Blocks,Super_Block.sizeofblock,start_inodes,T_name);
        blocks->show();
    }else{
        ui->listTerm->appendPlainText("No hay disco montado.");
    }
}

void FileSys::on_btnTrees_clicked()
{
    if(is_mounted_disk){
        delete tree;
        tree = new Tree(NULL,T_name,start_inodes,0,Super_Block.sizeofblock);
        tree->show();
    }else{
        ui->listTerm->appendPlainText("No hay disco montado.");
    }
}
