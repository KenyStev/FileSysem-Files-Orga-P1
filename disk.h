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

/**
 * @brief write escribe data en el disco dado
 * @param disk_name nombre del disco a donde va a escribir
 * @param buffer data a escribir
 * @param start posicion donde comenzara a excribir
 * @param bytes_to_write cantidad de bytes a excribir
 */
void write(string disk_name,char *buffer, double start, double bytes_to_write);

/**
 * @brief read
 * @param disk_name
 * @param buffer
 * @param start
 * @param bytes_to_write
 */
void read(string disk_name, char *buffer, double start, double bytes_to_write);

/**
 * @brief getFreeBlocks
 * @param cant_of_blocks cantidad de bloques a asignarles una posicion en el disco
 * @return un vector de tamanio cant_of_blocks con las direcciones de los bloques libres donde se podra guardar el archivo
 */
vector<int> getFreeBlocks(char *bitmap, double size_bitmap, double cant_of_blocks);

/**
 * @brief getFreeSequentialBlocks
 * @param cant_of_blocks cantidad de bloques a asignarles una posicion en el disco
 * @return un vector de tamanio cant_of_blocks con las direcciones de los bloques libres que esten secuencialmente
 * donde se podra guardar el archivo, si no hay la cantidad de bloques especificada en forma secuencial, entonces retorna
 * el vector con la primer posicion: -1
 */
vector<int> getFreeSequentialBlocks(char *bitmap, double size_bitmap, double cant_of_blocks);

/**
 * @brief set_blocks_in_use setea los blocks especificados en el vector es estado ocupado: 1
 * @param bitmap
 * @param blocks
 */
void set_blocks_in_use(char *bitmap, vector<int> blocks);

/**
 * @brief set_blocks_in_unuse setea los blocks especificados en el vector en estado libres: 0
 * @param bitmap
 * @param blocks
 */
void set_blocks_in_unuse(char *bitmap, vector<int> blocks);

#endif // DISK_H
