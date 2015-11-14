#include "disk.h"

SuperBlock createDisk(char name[],double size_disk, int size_block){
    SuperBlock SP;
    double disksizebyte = size_disk*1024*1024;//*1024;
    char block[size_block];
    double cantofblock = disksizebyte / size_block;

    cout<<"Writing Blocks # "<<cantofblock << endl;
    memset(block,'0',size_block);
    string path = "VirtualDisks/"; //[28]
    string name2 = name;
    string format = ".data";
    string name_file = path + name2 + format;
    //strcpy(name_file,name);
    //strcat(name_file,".data");
    ofstream output_file(name_file.c_str(),ios::binary);
    for(int i=0;i<cantofblock;i++){
        output_file.write((char*)block,sizeof(block));
    }

    output_file.close();

    output_file.open(name_file.c_str(), ios::in | ios::out | ios::binary);
    SP.cantofblock = cantofblock;
    SP.cantofinode = (disksizebyte*0.1)/sizeof(Inode);

    int bitmapsize = cantofblock/8;
    char bitmap [bitmapsize];

    int bitmapsize_inodes = (SP.cantofinode)/8;
    char bitmap_inodes [bitmapsize_inodes];

    int inodes_size = sizeof(Inode);
    int FileData_size = sizeof(FileData);

    double total_size_fs = sizeof(SuperBlock) + sizeof(bitmap) + sizeof(bitmap_inodes) + inodes_size*(SP.cantofinode)
                                            + FileData_size*(SP.cantofinode);

    double FS_blocks_WR = total_size_fs/size_block;
    int FS_blocks = FS_blocks_WR+1;

    SP.freeblock = cantofblock - FS_blocks;
    SP.freeinode = SP.cantofinode - 1;
    SP.freespace = disksizebyte - total_size_fs;//sizeof(SuperBlock);
    strcpy(SP.name, name);
    SP.size = disksizebyte;
    SP.sizeofblock = size_block;
    strcpy(SP.endsb, "FIN");
    cout<<"writing Super Block with size: "<<sizeof(SuperBlock)<<endl;
    cout<<"cant blocks: "<<SP.cantofblock<<endl;
    cout<<"cant free-blocks: "<<SP.freeblock<<endl;
    cout<<"cant inodes: "<<SP.cantofinode<<endl;
    cout<<"inode_size: "<<inodes_size<<endl;
    cout<<"FileData_size: "<<FileData_size<<endl;
    cout<<"free Space: "<<SP.freespace<<endl;
    cout<<"FS_blocks: "<<FS_blocks<<endl;
    output_file.write((char*)&SP,sizeof(SuperBlock));

//    int bitmapsize = cantofblock/8;
//    char bitmap [bitmapsize];
    memset(bitmap,0,bitmapsize);
    cout<<"writing Bitmap"<<endl;

    //seteando los bloques usados por el FS al bitmap
    for (int i = 0; i < FS_blocks; ++i) {
        setBlock_use(bitmap,i);
    }

    output_file.write(((char*)bitmap),sizeof(bitmap));
    //output_file.write("end bm",6);

//    int bitmapsize_inodes = SP.cantofinode/8;
//    char bitmap_inodes [bitmapsize_inodes];
    memset(bitmap_inodes,0,bitmapsize_inodes);
    setBlock_use(bitmap_inodes,0); // seteando el primer inodo que sera usado para el root
    cout<<"writing Bitmap Inodes"<<endl;
    output_file.write(((char*)bitmap_inodes),sizeof(bitmap_inodes));
    //output_file.write("end bmi",7);

    FileData root;

    //setendo el primer FilaData que pertenece al root
    strcpy(root.name,"/");
    root.index_file = 0;
    output_file.write(((char*)&root),FileData_size);

    FileData file;
    file.index_file = -1;
//    strcpy(file.name,"");
    memset(file.name,0,sizeof(char)*50); // 50 caracteres maximo para el nombre
    cout<<"writing File'sData of size: "<<FileData_size*SP.cantofinode<<endl;
    for(int d =1;d<SP.cantofinode;d++){
        output_file.write(((char*)&file),FileData_size);
//        output_file.write("ENDFD",5);
    }

    Inode root_inodo;
    root_inodo.filesize = 0;
    root_inodo.blockuse = 0;
    memset(root_inodo.directos,-1,sizeof(int)*10);
    root_inodo.indirectossimples = -1;
    root_inodo.indirectosdobles = -1;
    root_inodo.indirectostriples = -1;
    strcpy(root_inodo.permisos,"drwxrwxrwx");
    output_file.write(((char*)&root_inodo),inodes_size);

    Inode inodes;
    inodes.filesize = -1;
    inodes.blockuse = -1;
    memset(inodes.directos,-1,sizeof(double)*10); //10 inodos directos
    inodes.indirectossimples = -1;
    inodes.indirectosdobles = -1;
    inodes.indirectostriples = -1;
    strcpy(inodes.permisos,"----------");
    cout<<"writing Inodes of size: "<<inodes_size*SP.cantofinode<<endl;
    for(int d =1;d<SP.cantofinode;d++){
        output_file.write(((char*)&inodes),inodes_size);
//        output_file.write("ENDIN",5);
    }

    cout<<"Total Size of FS: "<<total_size_fs<<endl;
    output_file.close();
    return SP;
}

bool is_block_in_use(char* bitmap, int blocknum)
{
    int indexonbitmap = blocknum / 8;
    int posinchar = blocknum%8;

     if(bitmap[indexonbitmap] & 1<<posinchar){
         return true;
     }else{
         return false;
     }

}

void setBlock_use(char* bitmap, int blocknum){
    int indexonbitmap = blocknum / 8;
    int posinchar = blocknum%8;
    bitmap[indexonbitmap] = bitmap[indexonbitmap] | 1 << posinchar;
}

void setBlock_unuse(char* bitmap, int blocknum){
    int indexonbitmap = blocknum / 8;
    int posinchar = blocknum%8;
    bitmap[indexonbitmap]= bitmap[indexonbitmap] & ~(1<<posinchar);
}

double getNextFreeBlock(char *bitmap, double size_bitmap)
{
    for (int i = 0; i < size_bitmap; ++i) {
        if (!is_block_in_use(bitmap,i)) {
            setBlock_use(bitmap,i);
            return i;
        }
    }
    return -1;
}

vector<double> getTotalBlocksToUse(double file_size, int block_size)
{
    vector<double> T_blocks;
    int x = block_size/8;
    double blocks_data = ceil(file_size/block_size);
    double extra_blocks = 0;

    if(blocks_data <= 10){ //case: 0
        extra_blocks = 0;
        T_blocks.push_back(0);
        T_blocks.push_back(ceil(file_size/block_size));   //cuantos bloques de data los Directos simples uso? index: 1
    }else if(blocks_data <= (10 + x)){ //case: 1
        extra_blocks = 1;
        T_blocks.push_back(1);
        T_blocks.push_back(blocks_data-10);               //cuantos bloques de data va a usar el IS? index: 1
    }else if(blocks_data <= (10 + x + pow(x,2))){ //case: 2
        double blocks_temp = blocks_data - (10 + x);
        extra_blocks = ceil(blocks_temp/x) + 2;
        T_blocks.push_back(2);
        T_blocks.push_back(x);                            //cuantos bloques de data va a usar el IS? index: 1
        T_blocks.push_back(ceil(blocks_temp/x));          //cuantos IS va a usar el ID? index: 2
        T_blocks.push_back(blocks_temp);                  //cuantos bloques de data van a usar los IS del ID? index: 3
    }else if(blocks_data <= (10 + x + pow(x,2) + pow(x,3))){ //case: 3
        double blocks_temp = blocks_data - (10 + x + pow(x,2));
        extra_blocks = ceil(blocks_temp/(pow(x,2))) + ceil(blocks_temp/x) + 3 + x;//(blocks_temp*(1+x))/(pow(x,2)) + 3 + x;//blocks_temp/(pow(x,2)) + blocks_temp/x + 3;
        T_blocks.push_back(3);
        T_blocks.push_back(x);                            //cuantos bloques de data va a usar el IS? index: 1
        T_blocks.push_back(x);                            //cuantos IS va a usar el ID? index: 2
        T_blocks.push_back(pow(x,2));                     //cuantos bloques de data van a usar los IS del ID? index: 3
        T_blocks.push_back(ceil(blocks_temp/(pow(x,2)))); //cuantos ID va usar el IT? index: 4
        T_blocks.push_back(ceil(blocks_temp/x));          //cuantos IS van a usar los ID del IT? index: 5
        T_blocks.push_back(blocks_temp);                  //cuantos bloques de data van a usar los Is de los ID del IT? index: 6
    }else{
//        return -1;
        T_blocks.push_back(-1);
    }

//    return ceil(blocks_data + extra_blocks);
    T_blocks.push_back(ceil(blocks_data + extra_blocks));
    return T_blocks;
}


void write(string disk_name, char *buffer, double start, double bytes_to_write)
{
    ofstream out(disk_name.c_str(), ios::in | ios::out | ios::binary);

    out.seekp(start);
    out.write(buffer,bytes_to_write);

    out.close();
}

void read(string disk_name, char *buffer, double start, double bytes_to_read)
{
    ifstream in(disk_name.c_str(), ios::in | ios::out | ios::binary);

    in.seekg(start);
    in.read(buffer,bytes_to_read);

    in.close();
}


vector<double> getFreeBlocks(char *bitmap, double size_bitmap, double cant_of_blocks)
{
    vector<double> blocks;
    for (double i = 0; i < size_bitmap; ++i) {
        if(cant_of_blocks==0) break;
        if(!is_block_in_use(bitmap,i))
        {
            blocks.push_back(i);
            cant_of_blocks--;
            setBlock_use(bitmap,i); // lo cambiamos a usado
        }
    }
    if(blocks.size() < cant_of_blocks)
    {
        blocks.clear();
        blocks.push_back(-1);
    }
    return blocks;
}


vector<double> getFreeSequentialBlocks(char *bitmap, double size_bitmap, double cant_of_blocks)
{
    vector<double> blocks;
    double T_blocks = cant_of_blocks; // Total de bloques a buscar
    double start=0; //comienza a buscar en 0
    while (T_blocks>0) { //mientras el total de bloques a buscar sea mayor que cero, sequira buscando
        bool start_encontrado=false;
        //recorremos el mapa de bits hasta encontrar el primer bloque libre
        for (double i = start; i < size_bitmap; ++i) {
            if(!is_block_in_use(bitmap,i)) //si el bloque no esta usado
            {
                start = i; //seteamos el comienzo en ese bloque
                start_encontrado = true;
                break;
            }
        }

        if(start_encontrado)
        {
            //verificamos si la catidad de bloques pedida esta disponible secuencialmente desde el comienzo
            //que encontramos prbiamente.
            for (int i = 0; i < cant_of_blocks; ++i) {
                if(!is_block_in_use(bitmap,start + i)) //si no esta usado
                {
                    blocks.push_back(start + i); //lo vamos agregando a la lista
                    T_blocks--; //y lo restamos de la cantidad de bloques a buscar
                }else{
                    start = start + i; // cambiamos el comienzo para volver a buscar el siguente primer bloque libre a partir de alli
                    T_blocks = cant_of_blocks; //volvemos a setear el total de bloques = cantidad de bloques a buscar
                    blocks.clear(); //limpiamos la lista de los bloques que teniamos hasta el momento
                    break; //paramos el ciclo para que todo comience de nuevo
                }
            }
        }else{
            blocks.clear(); //limpiamos la lista por si habian datos guardados
            blocks.push_back(-1); //seteamos el primer valor: -1 para saber que no se pudo encontrar una secuencia de bloques libres de la cantidad especificada
            break;
        }
    }
    return blocks;
}


void set_blocks_in_use(char *bitmap, vector<int> blocks)
{
    for (int i = 0; i < blocks.size(); ++i) {
        setBlock_use(bitmap,blocks[i]);
    }
}


void set_blocks_in_unuse(char *bitmap, vector<int> blocks)
{
    for (int i = 0; i < blocks.size(); ++i) {
        setBlock_unuse(bitmap,blocks[i]);
    }
}


void writeInodesBlocks(string disk_name, vector<double> data_index, vector<double> inodes_index, vector<double> how_many, int size_block, Inode *inode, double start)
{
    double iterate = 10;
    int x = size_block/8;

    double *IS = new double[x];
    double *ID = new double[x];
    double *ID_IS = new double[x];
    double *IT = new double[x];
    double *IT_ID = new double[x];
    double *IT_ID_IS = new double[x];

    for (int i = 0; i < x; ++i) {
        IS[i] = -1;
        ID[i] = -1;
        ID_IS[i] = -1;
        IT[i] = -1;
        IT_ID[i] = -1;
        IT_ID_IS[i] = -1;
    }
//    memset(IS,-1,size_block);
//    memset(ID,-1,size_block);
//    memset(ID_IS,-1,size_block);
//    memset(IT,-1,size_block);
//    memset(IT_ID,-1,size_block);
//    memset(IT_ID_IS,-1,size_block);

    if(how_many[0]>=1) //IS
    {
        inode->indirectossimples = inodes_index[0];
        cout<<"IS en: "<<inode->indirectossimples<<endl;
        for (int i = 0; i < how_many[1]; ++i) {
            IS[i] = data_index[iterate++];
        }
        write(disk_name,(char*)IS,inode->indirectossimples*size_block,size_block);
        inodes_index.erase(inodes_index.begin()); //borramos del vector el que ya fue usado
    }

    if(how_many[0]>=2) //ID
    {
        inode->indirectosdobles = inodes_index[0];
        cout<<"ID en: "<<inode->indirectosdobles<<endl;
        inodes_index.erase(inodes_index.begin());
        cout<<"seteando IS del ID en: "<<endl;
        for (int i = 0; i < how_many[2]; ++i) {
            ID[i] = inodes_index[i];
            cout<<"- "<<ID[i]<<endl;
        }
        for (int i = 0; i < how_many[2]; ++i) {
            inodes_index.erase(inodes_index.begin());
        }

        int d_b = how_many[3];
        for (int j = 0; j < how_many[2]; ++j) {
            memset(ID_IS,-1,x);
            cout<<"seteando data para IS: "<<j<<endl;
            for (int i = 0; i < x; ++i) {
                if(d_b>0)
                {
                    ID_IS[i] = data_index[iterate++];
                    d_b--;
                    cout<<"- "<<ID_IS[i]<<endl;
                }else{
//                    break;
                    ID_IS[i] = -1;
                }
            }
            write(disk_name,(char*)ID_IS,ID[j]*size_block,size_block);
        }
        write(disk_name,(char*)ID,(inode->indirectosdobles)*size_block,size_block);
    }

    if(how_many[0]>=3) //IT
    {
        inode->indirectostriples = inodes_index[0];
        cout<<"IT en: "<<inode->indirectostriples<<endl;
        inodes_index.erase(inodes_index.begin());
        cout<<"seteando ID del IT en: "<<"cant: "<<how_many[4]<<endl;
        for (int i = 0; i < how_many[4]; ++i) {
            IT[i] = inodes_index[i];
            cout<<"- "<<IT[i]<<endl;
        }
        for (int i = 0; i < how_many[4]; ++i) {
            inodes_index.erase(inodes_index.begin());
        }

        int is_b = how_many[5];
        int d_b = how_many[6];
        for (int j = 0; j < how_many[4]; ++j) {
            memset(IT_ID,-1,x);
            cout<<"seteando IS de ID de IT: "<<j<<endl;
            for (int i = 0; i < x; ++i) {
                if(is_b>0)
                {
                    IT_ID[i] = inodes_index[0];
                    is_b--;
                    cout<<"- "<<IT_ID[i]<<endl;
                    inodes_index.erase(inodes_index.begin());
                }else{
//                    break;
                    IT_ID[i] = -1;
                }
                memset(IT_ID_IS,-1,x);
                cout<<"seteando data para IS de ID de IT: "<<j<<endl;
                for (int k = 0; k < x; ++k) {
                    if(d_b>0)
                    {
                        IT_ID_IS[k] = data_index[iterate++];
                        d_b--;
                        cout<<"- "<<IT_ID_IS[k]<<endl;
                    }else{
//                        break;
                        IT_ID_IS[k] = -1;
                    }
                }
                write(disk_name,(char*)IT_ID_IS,IT_ID[i]*size_block,size_block);
            }
            write(disk_name,(char*)IT_ID,IT[j]*size_block,size_block);
        }
        write(disk_name,(char*)IT,inode->indirectostriples*size_block,size_block);
    }

//    Inode ino;

    //lee IS
    double *buf = new double[x];
    read(disk_name,(char*)buf,inode->indirectossimples*size_block,size_block);
//    memcpy(&ino,buf,size_block);

    if(inode->indirectossimples!=-1)
    {
        cout<<"Leido del Disco IS en!: "<<inode->indirectossimples<<endl;
        for (int i = 0; i < x; ++i) {
            cout<<"data en- "<<buf[i]<<endl;
//            if(buf[i]==-1) cout<<"-nan = -1"<<endl;
        }
    }

    //lee ID
    read(disk_name,(char*)buf,inode->indirectosdobles*size_block,size_block);
//    memcpy(&ino,buf,size_block);
    cout<<"Leido del Disco ID en!: "<<inode->indirectosdobles<<endl;

    if(inode->indirectosdobles!=-1)
    {
        for (int i = 0; i < x; ++i) {
            cout<<"IS en- "<<buf[i]<<endl;
            double *buf2 = new double[x];
            read(disk_name,(char*)buf2,buf[i]*size_block,size_block);
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
    read(disk_name,(char*)buf,inode->indirectostriples*size_block,size_block);
//    memcpy(&ino,buf,size_block);
    cout<<"Leido del Disco IT en!: "<<inode->indirectostriples<<endl;
    if(inode->indirectostriples!=-1)
    {
        for (int i = 0; i < x; ++i) {
            cout<<"ID en- "<<buf[i]<<endl;
            double *buf2 = new double[x];
            read(disk_name,(char*)buf2,buf[i]*size_block,size_block);
    //        cout<<"ID del IT"<<endl;
            if(buf[i]!=-1)
            {
                for (int j = 0; j < x; ++j) {
                    cout<<"IS en- "<<buf2[j]<<endl;
                    double *buf3 = new double[x];
                    read(disk_name,(char*)buf3,buf2[j]*size_block,size_block);
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
}

//void writeInode(Inode *inode, char *buffer, double size)
//{
//    //los permisos deben darsele afuera de esta funcion
//    if(inode->filesize==-1) //virgen
//    {
//        inode->filesize = size;
//        inode->blockuse = 1;
////        getNextFreeBlock(bi)
//    }else{ //no virgen

//    }
//}

void memcpybuffer(char *&dest, char *src, int sizeblock, double init , double size_src)
{
    memset(dest,0,sizeblock*sizeof(char));
    for(int i =0; i< sizeblock;i++){
        if(size_src==init+i)
            return;
        dest[i]=src[(int)(init+i)];
    }
}
