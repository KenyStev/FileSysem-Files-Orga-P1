#include "disk.h"

SuperBlock createDisk(char name[],double size_disk, int size_block){
    SuperBlock SP;
    double disksizebyte = size_disk*1024*1024*1024;
    char block[size_block];
    double cantofblock = disksizebyte / size_block;

    cout<<"Writing Blocks # "<<cantofblock << endl;
    memset(block,'0',size_block);
    char name_file[30];
    strcpy(name_file,name);
    strcat(name_file,".data");
    ofstream output_file(name_file,ios::binary);
    for(int i=0;i<cantofblock;i++){
        output_file.write((char*)block,sizeof(block));
    }

    output_file.close();

    output_file.open(name_file, ios::in | ios::out | ios::binary);
    SP.cantofblock = cantofblock;
    SP.cantofinode = (disksizebyte*0.1)/sizeof(Inode);

    int bitmapsize = cantofblock/8;
    char bitmap [bitmapsize];

    int bitmapsize_inodes = SP.cantofinode/8;
    char bitmap_inodes [bitmapsize_inodes];

    int total_size_fs = sizeof(SuperBlock) + sizeof(bitmap) + sizeof(bitmap_inodes);

    SP.freeblock = cantofblock;
    SP.freeinode = SP.cantofinode;
    SP.freespace = disksizebyte - total_size_fs;//sizeof(SuperBlock);
    strcpy(SP.name, name);
    SP.size = disksizebyte;
    SP.sizeofblock = size_block;
    strcpy(SP.endsb, "FIN");
    cout<<"writing Super Block with size: "<<sizeof(SP)<<endl;
    cout<<"cant blocks: "<<SP.cantofblock<<endl;
    cout<<"cant inodes: "<<SP.cantofinode<<endl;
    output_file.write((char*)&SP,sizeof(SuperBlock));

//    int bitmapsize = cantofblock/8;
//    char bitmap [bitmapsize];
    memset(bitmap,0,bitmapsize);
    cout<<"writing Bitmap"<<endl;
    output_file.write(((char*)bitmap),sizeof(bitmap));
    //output_file.write("end bm",6);

//    int bitmapsize_inodes = SP.cantofinode/8;
//    char bitmap_inodes [bitmapsize_inodes];
    memset(bitmap_inodes,0,bitmapsize_inodes);
    cout<<"writing Bitmap Inodes"<<endl;
    output_file.write(((char*)bitmap_inodes),sizeof(bitmap_inodes));
    //output_file.write("end bmi",7);

    cout<<"Total Size of FS: "<<total_size_fs<<endl;
    output_file.close();
    return SP;
}


