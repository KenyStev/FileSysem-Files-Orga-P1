#ifndef DISK_H
#define DISK_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include <cstdio>
#include <string.h>
#include <string>
#include <QDir>
#include <vector>
#include <QStringList>
#include <QFileInfoList>
#include <QFile>
#include <math.h>

using namespace std;

struct SuperBlock {
    double size;
    double freeblock;
    double freespace;
    double freeinode;
    int FS_Blocks;
    int cantofblock;
    int cantofinode;
    int sizeofblock;
    char name[10];
    char endsb [3];
};

struct Inode{
    double filesize;
    double blockuse;
    double directos[10];
    double indirectossimples;
    double indirectosdobles;
    double indirectostriples;
    double lastDataBlock;
    char permisos[10];
};

struct FileData{
    double index_file;
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
 * @brief getNextFreeBlock busca un bloque libre, lo marca como ocupado y lo devuelve.
 * @param bitmap en donde hara la busqueda
 * @param size_bitmap cantidad de bloques dentro del bitmap
 * @return el primer bloque libre que encuentra en el bitmap recibido de parametro
 */
double getNextFreeBlock(char *bitmap, double size_bitmap);

/**
 * @brief getTotalBlocksToUse calcula el numero de bloques que se necesitarian para guardar un
 * archivo del tamanio especificado en el parametro, teniendo en cuenta la cantidad de bloques de directos_simples,
 * indirectos simples, dobles y  triples que tendria que usar el inodo
 * @param file_size tamanio del archivo en bytes
 * @param size_block tamanio del bloque en bytes
 * @return numero de bloques calculados para guardar todo el archivo
 */
vector<double> getTotalBlocksToUse(double file_size, int size_block);

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
 * @brief writeInodesBlocks
 * @param disk_name
 * @param data_index
 * @param inodes_index
 * @param how_many
 * @param size_block
 * @param inode
 * @param start
 */
void writeInodesBlocks(string disk_name, vector<double> data_index, vector<double> inodes_index, vector<double> how_many,int size_block,Inode *inode, double start);

/**
 * @brief getFreeBlocks
 * @param cant_of_blocks cantidad de bloques a asignarles una posicion en el disco
 * @return un vector de tamanio cant_of_blocks con las direcciones de los bloques libres donde se podra guardar el archivo
 */
vector<double> getFreeBlocks(char *bitmap, double size_bitmap, double cant_of_blocks);

/**
 * @brief getFreeSequentialBlocks
 * @param cant_of_blocks cantidad de bloques a asignarles una posicion en el disco
 * @return un vector de tamanio cant_of_blocks con las direcciones de los bloques libres que esten secuencialmente
 * donde se podra guardar el archivo, si no hay la cantidad de bloques especificada en forma secuencial, entonces retorna
 * el vector con la primer posicion: -1
 */
vector<double> getFreeSequentialBlocks(char *bitmap, double size_bitmap, double cant_of_blocks);

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
void set_blocks_in_unuse(char *bitmap, vector<double> blocks);

/**
 * @brief readDataBlocksFrom lee todos los bloques de data que tiene un inodo y los carga a la RAM.
 * @param disk disco montado
 * @param buffer donde guardara toda la DATA, el buffer se inicializa dentro de la funcion como tamanio el filesize del inodo
 * @param inode de quien extraera la DATA
 * @param sizeblock tamanio del bloque en el disco
 */
void readDataBlocksFrom(string disk,char*&buffer,Inode *inode, double sizeblock);

void ExportFile(string disk,string file_to_export,Inode *inode, double sizeblock);

/**
 * @brief memcpybuffer
 * @param dest
 * @param src
 * @param sizeblock
 * @param init
 * @param size_src
 */
void memcpybuffer(char *&dest, char *src, int sizeblock, double init , double size_src);

/**
 * @brief memtransbuffer va agregando DATA al dest recibido de parametro en cada llamada de diferentes origenes
 * -> usado para ir extraendo los bloques de data de un inodo e irlos agregando a un solo bloque del tamanio del archivo,
 * -> se usara solo para los directorios
 * @param dest es el buffer donde se ira guardando la DATA
 * @param src sera diferente en cada llamada, es cada bloque de data leido del archivo
 * @param init es de donde comenzara a escribir el nuevo bloque de DATA en el buffer 'dest'
 * @param size_src el tamanio en bytes que se va a transferir, o lo que pesa el bloque 'src'
 */
void memtransbuffer(char *&dest, char *src, double init , double size_src);

/**
 * @brief getFileTableFrom lee un buffer y va agregando cada uno de los registros al vector
 * @param dir inodo del directorio del cual se pasara el buffer a vector
 * @param buffer es el buffer generado por: @readDataBlocksFrom();
 * @return un vector con el FileTable de ese directorio
 */
vector<FileData*> getFileTableFrom(Inode dir, char* buffer);

vector<double> getDataBlocksFrom(string disk,Inode *inode,int sizeblock);
vector<double> getAllBlocksUsedFor(string disk, Inode *inode, int sizeblock);
vector<double> get_IBlocks_UsedFor(string disk, Inode *inode, int sizeblock);

void copy(Inode *from, Inode *to);

double getTotalSizeUsed(double filesize,double blocks_data, double sizeblock);
void initInode(Inode *inode);

#endif // DISK_H
