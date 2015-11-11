#include "filesys.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FileSys w;
    w.show();

/*
    QDir dir("./VirtualDisks");
//    dir.mkdir("VirtualDisks");

    cout<<dir.dirName().toStdString()<<endl;
    QFileInfoList list = dir.entryInfoList();

    for (int var = 0; var < list.size(); ++var) {
        cout<<list.at(var).fileName().toStdString()<<endl;
    }*/
/*
    //Bitmap

    char bitmap2[10]; // se crea un bitmap que representa 80 bloques ( cada byte representa a 8 bloques)
    memset(bitmap2,0,10);

    for (int i = 0; i < 8; ++i) {
        setBlock_use(bitmap2,i);
    }
    //setBlock_use(bitmap2,7); // mandar el bitmap y el bloque que se desea marcar como ocupado ( 0 al 79)

    bool isinuse = is_block_in_use(bitmap2,7); // revisa si el bloque esta ocupado o no
    cout << isinuse << endl;

    //setBlock_unuse(bitmap2,7); // manda a marcar el bloque como desocupado

    isinuse = is_block_in_use(bitmap2,7); // revisa si el bloque esta ocupado o no
    cout << isinuse << endl;

//    output_file.open("bitmap.data", ios::binary);
    ofstream output_file("bitmap.data",ios::binary);
    output_file.write((char*)&bitmap2,sizeof(char)*8);
    output_file.close();
    cout<<"document closed"<<endl;
*/
    int blo = getTotalBlocksToUse(368,16);//pow(1024,2) = 1048576 //68176384 byte maximo para bloques de 256 bytes
    cout<<"Total blocks for file_size=1MB, block_size=256bytes: "<<blo<<endl;
    return a.exec();
}
