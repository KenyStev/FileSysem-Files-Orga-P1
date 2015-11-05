#ifndef DISK_H
#define DISK_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include <string.h>

using namespace std;

struct SuperBlock {
    int size;
    int cantofblock;
    int freeblock;
    int freespace;
    int cantofinode;
    int freeinode;
    int sizeofblock;
    char name[10];
    char endsb [3];
};

struct Inode{
    int filesize;
    int blockuse;
    int directos[10];
    int indirectossimples;
    int indirectosdobles;
    char permisos[10];
};

/**
 * param: size_block recibe un int que representa el tamanio en mb del disco
 * param: size:blocl recibe un int que representa el tamanio en byte por bloque de disco
 */
SuperBlock createDisk(char name[],double size_disk, int size_block);


#endif // DISK_H
