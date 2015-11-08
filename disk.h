#ifndef DISK_H
#define DISK_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include <cstdio>
#include <string.h>
#include <QDir>
#include <vector>
#include <QStringList>
#include <QFileInfoList>
#include <QFile>
#include <math.h>

using namespace std;

struct SuperBlock {
    double size;
    int cantofblock;
    int freeblock;
    double freespace;
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

struct FileData{
    int index_file;
    char name[50];
};

/**
 * @brief createDisk
 * @param name nombre del disco
 * @param size_disk recibe un int que representa el tamanio en mb del disco
 * @param size_block recibe un int que representa el tamanio en byte por bloque de disco
 * @return true si fue creado el disco
 */
SuperBlock createDisk(char name[],double size_disk, int size_block);

/**
 * @brief getTotalBlocksToUse calcula el numero de bloques que se necesitarian para guardar un
 * archivo del tamanio especificado en el parametro, teniendo en cuenta la cantidad de bloques de directos_simples,
 * indirectos simples, dobles y  triples que tendria que usar el inodo
 * @param file_size tamanio del archivo en bytes
 * @param size_block tamanio del bloque en bytes
 * @return numero de bloques calculados para guardar todo el archivo
 */
double getTotalBlocksToUse(double file_size, int size_block);

/**
 * @brief is_block_in_use
 * @param bitmap arreglo de chars del mapa de bits
 * @param blocknum numero de bloque a validar
 * @return true si el bloque esta ocupado
 */
bool is_block_in_use(char* bitmap, int blocknum);

/**
 * @brief setBlock_use
 * @param bitmap arreglo de chars del mapa de bits a modificar
 * @param blocknum numero del bloque a activar
 */
void setBlock_use(char* bitmap, int blocknum);

/**
 * @brief setBlock_unuse
 * @param bitmap arreglo de chars del mapa de bits a modificar
 * @param blocknum numero del bloque a desactivar
 */
void setBlock_unuse(char* bitmap, int blocknum);

#endif // DISK_H
