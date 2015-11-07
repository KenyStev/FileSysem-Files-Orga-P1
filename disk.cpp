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

    int total_size_fs = sizeof(SuperBlock) + sizeof(bitmap) + sizeof(bitmap_inodes) + inodes_size*(SP.cantofinode)
                                            + FileData_size*(SP.cantofinode);

    double FS_blocks_WR = total_size_fs/size_block;
    int FS_blocks = FS_blocks_WR+1;

    SP.freeblock = cantofblock - FS_blocks;
    SP.freeinode = SP.cantofinode;
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

    FileData file;

    //setendo el primer FilaData que pertenece al root
    strcpy(file.name,"/");
    file.index_file = 0;
    output_file.write(((char*)&file),FileData_size);

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
    strcpy(root_inodo.permisos,"drwxrwxrwx");
    output_file.write(((char*)&root_inodo),inodes_size);

    Inode inodes;
    inodes.filesize = -1;
    inodes.blockuse = -1;
    memset(inodes.directos,-1,sizeof(int)*10); //10 inodos directos
    inodes.indirectossimples = -1;
    inodes.indirectosdobles = -1;
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
