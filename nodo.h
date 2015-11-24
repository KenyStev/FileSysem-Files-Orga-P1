#ifndef NODO
#define NODO

#include <QString>
#include <vector>

class Nodo
{
public:
    Nodo(QString v)
    {
        valor=v;
//        izq=NULL;
//        der=NULL;
    }

    QString valor;
//    Nodo *izq, *der;
    vector<Nodo*> hijos;
};

#endif // NODO

