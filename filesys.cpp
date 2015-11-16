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
    ui->listTerm->appendPlainText(mounted_disk + "@root~" + current_path.join("/") + "$ " + command_line);
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
                ls();
            }else if(main_command == "cd") // change directory form: cd [path]
            {

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

            current_path.push_back(root_path);

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
    int index = searchInFileTable(name);

    if(index == -1 && Super_Block.freeinode > 0)
    {
        //creando filetable e inodo
        index = get_NextFree_FileTable();
        double inode = getNextFreeBlock(bitmap_inodes,Super_Block.cantofinode);
        strcpy(file_data_array[index]->name,name.c_str());
        file_data_array[index]->index_file = inode;

        Inode new_inode;
        char *buffer = new char[sizeof(Inode)];
        read(T_name, buffer,start_inodes + inode*sizeof(Inode),sizeof(Inode));
        memcpy(&new_inode,buffer,sizeof(Inode));
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
        for (int i = 0; i < bloques_data; ++i) {
            char *buffer = new char[Super_Block.sizeofblock];
            for (int i = 0; i < Super_Block.sizeofblock; ++i) {
                buffer[i] = 'K';//rand()%25 + 65;
            }
            if(size_bytes_temp<size_to_write)
                size_to_write = size_bytes_temp;
            size_bytes_temp-=size_to_write;
            writeInode(&new_inode,T_name,buffer,size_to_write);
        }
        //escribiendo inodo en el disco
        write(T_name,(char*)&new_inode,start_inodes + inode*sizeof(Inode),sizeof(Inode));

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
        cout<<"inodo usado: "<<inode<<endl;
        cout<<"bloques usados: "<<Total_blocks[Total_blocks.size()-1]<<endl;
        cout<<"Directos en: "<<endl;
        for (int i = 0; i < 10; ++i) {
            cout<<i<<"- "<<(new_inode.directos)[i]<<endl;
        }

        //lee IS
        int x = Super_Block.sizeofblock/8;
        double *buf = new double[x];
        read(T_name,(char*)buf,new_inode.indirectossimples*Super_Block.sizeofblock,Super_Block.sizeofblock);
    //    memcpy(&ino,buf,size_block);

        if(new_inode.indirectossimples!=-1)
        {
            cout<<"Leido del Disco IS en!: "<<new_inode.indirectossimples<<endl;
            for (int i = 0; i < x; ++i) {
                cout<<"data en- "<<buf[i]<<endl;
    //            if(buf[i]==-1) cout<<"-nan = -1"<<endl;
            }
        }

        //lee ID
        read(T_name,(char*)buf,new_inode.indirectosdobles*Super_Block.sizeofblock,Super_Block.sizeofblock);
    //    memcpy(&ino,buf,size_block);
        cout<<"Leido del Disco ID en!: "<<new_inode.indirectosdobles<<endl;

        if(new_inode.indirectosdobles!=-1)
        {
            for (int i = 0; i < x; ++i) {
                cout<<"IS en- "<<buf[i]<<endl;
                double *buf2 = new double[x];
                read(T_name,(char*)buf2,buf[i]*Super_Block.sizeofblock,Super_Block.sizeofblock);
        //        cout<<"IS del ID"<<endl;
                if(buf[i]!=-1)
                {
                    for (int j = 0; j < x; ++j) {
                        cout<<"data en:- "<<buf2[j]<<endl;
                    }
                }
            }
        }

        //lee IT
        read(T_name,(char*)buf,new_inode.indirectostriples*Super_Block.sizeofblock,Super_Block.sizeofblock);
    //    memcpy(&ino,buf,size_block);
        cout<<"Leido del Disco IT en!: "<<new_inode.indirectostriples<<endl;
        if(new_inode.indirectostriples!=-1)
        {
            for (int i = 0; i < x; ++i) {
                cout<<"ID en- "<<buf[i]<<endl;
                double *buf2 = new double[x];
                read(T_name,(char*)buf2,buf[i]*Super_Block.sizeofblock,Super_Block.sizeofblock);
        //        cout<<"ID del IT"<<endl;
                if(buf[i]!=-1)
                {
                    for (int j = 0; j < x; ++j) {
                        cout<<"IS en- "<<buf2[j]<<endl;
                        double *buf3 = new double[x];
                        read(T_name,(char*)buf3,buf2[j]*Super_Block.sizeofblock,Super_Block.sizeofblock);
            //            cout<<"IS del ID del IT"<<endl;
                        if(buf2[j]!=-1)
                        {
                            for (int k = 0; k < x; ++k) {
                                cout<<"data en- "<<buf3[k]<<endl;
                            }
                        }
                    }
                }
            }
        }
    }else{
        cout<<"Ya existe el nombre"<<endl;
        double inode = file_data_array[index]->index_file;
        char *buffer = new char[sizeof(Inode)];
        Inode new_inode;
        read(T_name, buffer,start_inodes + inode*sizeof(Inode),sizeof(Inode));
        memcpy(&new_inode,buffer,sizeof(Inode));
        cout<<"lastDataBlock: "<<new_inode.lastDataBlock<<endl;

        double size_bytes_temp = size_bytes;
        double size_to_write = Super_Block.sizeofblock;
        for (int i = 0; i < bloques_data; ++i) {
            char *buffer = new char[Super_Block.sizeofblock];
            for (int i = 0; i < Super_Block.sizeofblock; ++i) {
                buffer[i] = 'K';//rand()%25 + 65;
            }
            if(size_bytes_temp<size_to_write)
                size_to_write = size_bytes_temp;
            size_bytes_temp-=size_to_write;
            writeInode(&new_inode,T_name,buffer,size_to_write);
        }
        //escribiendo inodo en el disco
        write(T_name,(char*)&new_inode,start_inodes + inode*sizeof(Inode),sizeof(Inode));
        cout<<"termino de escribir!"<<endl;
    }
}

void FileSys::mkDir(string name)
{
    //falta validar que quepa otro archivo en el 'dir' actual
    string T_name = (disks_path + mounted_disk + format).toStdString();

    //creando filetable e inodo
    int index = get_NextFree_FileTable();
    double inode = getNextFreeBlock(bitmap_inodes,Super_Block.cantofinode);
    strcpy(file_data_array[index]->name,name.c_str());
    file_data_array[index]->index_file = inode;

    //escribiendo filedata en el disco
    write(T_name,(char*)file_data_array[index],start_filetable + index*sizeof(FileData),sizeof(FileData));

    //nuevo inodo para el nuevo 'dir'
    Inode new_dir;
    strcpy(new_dir.permisos,"drwxrwxrwx");

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
}

void FileSys::mkfile2(string name, int size)
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

void FileSys::updateFileTableFromDir(Inode *inode, FileData *data)
{
    string T_name = (disks_path + mounted_disk + format).toStdString();
    double size = sizeof(FileData);
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
}

void FileSys::ls()
{
    string T_name = (disks_path + mounted_disk + format).toStdString();
    char *all_datablocks;
    readDataBlocksFrom(T_name,all_datablocks,&current_inode,Super_Block.sizeofblock);

    vector<FileData*> filetable = getFileTableFrom(current_inode,all_datablocks);
    cout<<"current path: "<<current_path.join("/").toStdString().c_str()<<endl;
    for (int i = 0; i < filetable.size(); ++i) {
        cout<<"Nombre: "<<filetable[i]->name<<" index: "<<filetable[i]->index_file<<endl;
        char *buffer = new char[sizeof(Inode)];
        Inode *inode = new Inode();
        read(T_name,buffer,start_inodes + (filetable[i]->index_file)*sizeof(Inode),sizeof(Inode));
        memcpy(inode,buffer,sizeof(Inode));
        string file = string(inode->permisos) + "\troot  root\t" + QString::number(inode->filesize).toStdString() + "    " + string(filetable[i]->name);
        cout<<file.c_str()<<endl;
        ui->listTerm->appendPlainText(QString(file.c_str()));
    }
}

void FileSys::writeInode(Inode *inode, string disk, char *buffer, double size)
{
    //los permisos deben darsele afuera de esta funcion
    //el inodo como tal debe guardarse afuera de esta funcion
    if(inode->filesize==-1) //virgen
    {
        inode->filesize = size;
        inode->blockuse = 1;
        double block = getNextFreeBlock(bitmap,Super_Block.cantofblock);
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
                cout<<"buffer antes: "<<buffer<<endl;
                cout<<"free of blocks: "<<free_of_block<<endl;
                cout<<"size: "<<size<<endl;
//                for (int i = (block_used_percent*Super_Block.sizeofblock),j=0; i < size; ++i, j++) {
//                    buffer[j] = buf_temp[i];
//                }
                size -= free_of_block;
                string buf_temp(buffer);
                string buf = buf_temp.substr(free_of_block,size);
                buffer = const_cast<char *>(buf.c_str());
                buffer[0] = 'Y';
                cout<<"bytes copiados: "<<buffer<<endl;
            }
        }

//        if(block_used_percent==0) // si los bloques usados estan todos llenos
//        {
            if(inode->blockuse < 10) //CASE 0:si los bloques de data usados son menorea a los DIRECTOS
            {
                double block = getNextFreeBlock(bitmap,Super_Block.cantofblock); //pide bloque libre al bitmap
                int save_to = inode->blockuse++;    //guardamos el index de la siguente posicion libre de los DIRECTOS
                inode->directos[save_to] = block;   //guardamos la direccion del bloque en los directos
                inode->filesize += size;            //aumentamos el filesize del archivo
                inode->lastDataBlock = block;
                write(disk,buffer,block*Super_Block.sizeofblock,size);   //escribimos el bloque de data
            }else if(inode->blockuse < (10 + x)){   //CASE 1: si bloques de data usados es menor I_SIMPLES mas los anteriores
                double IS_ptr = inode->indirectossimples;
                double *IS = new double[x];         //bloque de IS con las direcciones a los bloques de data
                if(IS_ptr==-1)                      //si aun no hemos utilizado los I_SIMPLES
                {
                    IS_ptr = getNextFreeBlock(bitmap,Super_Block.cantofblock); //le pedimos un bloque al bitmap para el IS
                    inode->indirectossimples = IS_ptr; //actualizamos el inodo
                    //llenamos el bloque con -1 el bloque
                    for (int i = 0; i < x; ++i) {
                        IS[i] = -1;
                    }
                }else{ //si ya habiamos utilizado antes el IS solo leemos lo que ya tenia
                    read(disk,(char*)IS,IS_ptr*Super_Block.sizeofblock,Super_Block.sizeofblock);
                }

                double block = getNextFreeBlock(bitmap,Super_Block.cantofblock); //bloque para la data
                int save_to = inode->blockuse++ - 10; //sacamos el index en los IS donde guardaremos la direccion del bloque de data
                IS[save_to] = block;
                inode->filesize += size;
                inode->lastDataBlock = block;

                //escribimos el bloque de data y el bloque de IS como corresponda
                write(disk,buffer,block*Super_Block.sizeofblock,size);
                write(disk,(char*)IS,IS_ptr*Super_Block.sizeofblock,Super_Block.sizeofblock);
            }else if(inode->blockuse < (10 + x + pow(x,2))){ //CASE 2: si bloques de data usados es menor I_DOBLES mas los anteriores
                double ID_ptr = inode->indirectosdobles;
                double *ID = new double[x];
                double *ID_IS = new double[x];
                if (ID_ptr==-1) { //si no hemos usado I_DOBLES antes
                    ID_ptr = getNextFreeBlock(bitmap,Super_Block.cantofblock); //pedimos un bloque para los ID
                    inode->indirectosdobles = ID_ptr; //actualizamod el inodo
                    for (int i = 0; i < x; ++i) { //lo inicializamos
                        ID[i] = -1;
                    }
                }else{ //si ya lo habiamos usado solo lo leemos
                    read(disk,(char*)ID,ID_ptr*Super_Block.sizeofblock,Super_Block.sizeofblock);
                }
                double b_data_ID = inode->blockuse++ - 10 - x;  //cuantos bloques de DATA tiene el ID?
                double IS_to_write = b_data_ID/x;               //cuantos IS esta usando para tenerlos?
                if((IS_to_write - floor(IS_to_write))==0)       //los IS que esta usando ya estan llenos?
                {
                    //pide un bloque para añadirlo a los IS que usa el ID
                    double addIS_toDoble = getNextFreeBlock(bitmap,Super_Block.cantofblock);
                    ID[(int)IS_to_write] = addIS_toDoble; //lo agrega
                    for (int i = 0; i < x; ++i) { //lo inicializa con -1
                        ID_IS[i] = -1;
                    }
                    //escribe el ID en el disco
                    write(disk,(char*)ID,inode->indirectosdobles*Super_Block.sizeofblock,Super_Block.sizeofblock);
                }else{ //sino, entonces hay espacio aun en el ultimo IS que esta usando el ID
                    IS_to_write = floor(IS_to_write); //lo redondeamos para abajo para obtener el index de ese IS en el ID
                    read(disk,(char*)ID_IS,ID[(int)IS_to_write]*Super_Block.sizeofblock,Super_Block.sizeofblock); //lemos el IS
                }
                int indexInIS = b_data_ID - x*IS_to_write; //sacamos el index siguiente libre en el IS

                double block = getNextFreeBlock(bitmap,Super_Block.cantofblock); //pedimos bloque para la data
                ID_IS[indexInIS] = block; //lo guardamos el los IS que tiene el ID
                inode->filesize += size;
                inode->lastDataBlock = block;

                //escribimos el bloque de data y el bloque del IS que pertenece al ID en el disco como corresponde
                write(disk,buffer,block*Super_Block.sizeofblock,size);
                write(disk,(char*)ID_IS,ID[(int)IS_to_write]*Super_Block.sizeofblock,Super_Block.sizeofblock);
            }else if(inode->blockuse < (10 + x + pow(x,2) + pow(x,3))) //CASE 3: si bloques de data usados es menor I_TRIPLES mas los anteriores
            {
                double IT_ptr = inode->indirectostriples;
                double *IT = new double[x];
                double *IT_ID = new double[x];
                double *IT_ID_IS = new double[x];
                if(IT_ptr == -1) //si no hemos usado IT antes
                {
                    IT_ptr = getNextFreeBlock(bitmap,Super_Block.cantofblock); //pedimos bloque para IT
                    inode->indirectostriples = IT_ptr; //actualizamos el inodo
                    for (int i = 0; i < x; ++i) { //lo inicializamos
                        IT[i] = -1;
                    }
                }else{ //si ya lo habiamos usado solo lo leemos
                    read(disk,(char*)IT,IT_ptr*Super_Block.sizeofblock,Super_Block.sizeofblock);
                }
                double b_data_IT = inode->blockuse++ - 10 - x - pow(x,2); //cuantos bloques de DATA tiene el IT?
                double ID_to_write = b_data_IT/pow(x,2);                  //cuantos ID esta usando el IT?
                if((ID_to_write - floor(ID_to_write))==0)                 //los ID que esta usando ya estan llenos?
                {
                    //pide un bloque para añadirlo a los ID que usa el IT
                    double addID_toTriple = getNextFreeBlock(bitmap,Super_Block.cantofblock);
                    IT[(int)ID_to_write] = addID_toTriple; //lo agrega
                    for (int i = 0; i < x; ++i) { //lo inicializa
                        IT_ID[i] = -1;
                    }
                    write(disk,(char*)IT,IT_ptr*Super_Block.sizeofblock,Super_Block.sizeofblock);
                }else{ //sino, entonces hay espacio aun en el ultimo ID que esta usando el IT
                    ID_to_write = floor(ID_to_write); //lo redondeamos hacia abajo para obtener el index en el IT
                    read(disk,(char*)IT_ID,IT[(int)ID_to_write]*Super_Block.sizeofblock,Super_Block.sizeofblock); //lo leemos
                }

                double IS_to_write = b_data_IT/x;                         //cuantos IS esta usando el IT?
                double indexIS_inID = IS_to_write - x*ID_to_write;        //cual es el index del IS en el ID correspondiente?
                if((IS_to_write - floor(IS_to_write))==0)                 //los IS que esta usando ya estan llenos?
                {
                    //pide un bloque para añadirlo a los IS que usa el ID correspondiente
                    double addIS_toIDoble = getNextFreeBlock(bitmap,Super_Block.cantofblock);
                    IT_ID[(int)indexIS_inID] = addIS_toIDoble; //lo agrega
                    for (int i = 0; i < x; ++i) { //lo inicializa
                        IT_ID_IS[i] = -1;
                    }
                    write(disk,(char*)IT_ID,IT[(int)ID_to_write]*Super_Block.sizeofblock,Super_Block.sizeofblock);
                }else{ //sino, entonces hay espacio aun en el ultimo IS del ID correspondiente que esta usando el IT
                    IS_to_write = floor(IS_to_write); //redondeamos hacia abajo para luego sacar el index donde va ir la data en ese IS
                    indexIS_inID = floor(indexIS_inID); //redondeamos hacia abajo para sacar el index del IS en el ID
                    read(disk,(char*)IT_ID_IS,IT_ID[(int)indexIS_inID]*Super_Block.sizeofblock,Super_Block.sizeofblock); //lo leemos
                }
                int indexInIS = b_data_IT - x*IS_to_write; //calculamos el index de la DATA dentro del IS

                double block = getNextFreeBlock(bitmap,Super_Block.cantofblock); //pide bloque para la DATA
                IT_ID_IS[indexInIS] = block; //lo agrega al IS
                inode->filesize += size;
                inode->lastDataBlock = block;

                //escribimos en el disco el bloque de DATA y el IS correcpondiente
                write(disk,buffer,block*Super_Block.sizeofblock,size);
                write(disk,(char*)IT_ID_IS,IT_ID[(int)indexIS_inID]*Super_Block.sizeofblock,Super_Block.sizeofblock);
            }
//        }else{ //si al ultimo bloque le falta por llenarse

//        }

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
