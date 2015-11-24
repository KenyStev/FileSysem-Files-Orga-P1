#ifndef TREE_H
#define TREE_H

#include <QWidget>
#include <iostream>
#include <QTreeWidgetItem>
#include "disk.h"
#include "nodo.h"

using namespace std;

namespace Ui {
class Tree;
}

class Tree : public QWidget
{
    Q_OBJECT

public:
    explicit Tree(QWidget *parent = 0,string disk=0, double start_inodes=0, double root=0, int b_size=0);
    ~Tree();

private:
    Ui::Tree *ui;
    string disk;
    double start_inode;
    double inode;
    int blocksize;

    void fillTree();
    void fillTree(Inode*,QString,QTreeWidgetItem*&root);
};

#endif // TREE_H
