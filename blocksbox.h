#ifndef BLOCKSBOX_H
#define BLOCKSBOX_H

#include <QWidget>
#include <QStringList>
#include <iostream>
#include <QTableWidgetItem>
#include "disk.h"

using namespace std;

namespace Ui {
class BlocksBox;
}

class BlocksBox : public QWidget
{
    Q_OBJECT

public:
    explicit BlocksBox(QWidget *parent = 0,vector<FileData*> *file_data_array=0,double cantOfBlocks=0,double FS_Blocks=0,int sizeofblock=0, double start_inodes=0,string disk="");

    ~BlocksBox();
    void addRow();
    void setColorTo(double block);
    void setColorTo(double block,int r,int g, int b);
    void newColor();
    bool ready();

private:
    Ui::BlocksBox *ui;
    QStringList usedColors;
    QString currentColor;
    QList<QTableWidgetItem*> listItems;
    QTableWidgetItem* search(QString item);
    bool isReady=false;
    int r,g,b;
};

#endif // BLOCKSBOX_H
