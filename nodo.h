#ifndef NODO
#define NODO

#include <QString>
#include <vector>
#include <iostream>

using namespace std;

class Nodo
{
public:
    Nodo(QString v)
    {
        valor=v;
    }

    QString valor;
    vector<Nodo*> hijos;
};

class NodoFT
{
public:
    NodoFT(int index, double index_FT)
    {
        this->index=index;
        this->index_FT=index_FT;
    }

    int index;
    double index_FT;
};

#endif // NODO

