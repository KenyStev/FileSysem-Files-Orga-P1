#include "blocksbox.h"
#include "ui_blocksbox.h"

BlocksBox::BlocksBox(QWidget *parent,vector<FileData*> *file_data_array,double cantOfBlocks,double FS_Blocks,int sizeofblock, double start_inodes,string disk) :
    QWidget(parent),
    ui(new Ui::BlocksBox)
{
    ui->setupUi(this);
    int cols = 10;
    ui->tableBlocks->setColumnCount(cols);
    double cont=0;
    usedColors.push_back("0 100 0");
    usedColors.push_back("104 104 104");

    for (int i = 0; i < cantOfBlocks; ++i) {
        if(i%cols==0){addRow();cont=0;}
        QTableWidgetItem *item = new QTableWidgetItem(QString::number(i));
        if(i<FS_Blocks)
        {
            QBrush brush(QColor(104,104,104));
            item->setBackground(brush);
        }else{
            QBrush brush(QColor(0,100,0));
            item->setBackground(brush);
        }
        ui->tableBlocks->setItem(ui->tableBlocks->rowCount()-1,cont++,item);
        listItems.push_back(item);
    }

    srand (time(NULL));
    if(file_data_array->size()>0){
        for (int i = 0; i < file_data_array->size(); ++i) {
            if((*file_data_array)[i]->index_file!=-1){
                newColor();
                char buf[sizeof(Inode)];
                Inode inode;
                initInode(&inode);

                read(disk,(char*)&buf,start_inodes + (*file_data_array)[i]->index_file*sizeof(Inode),sizeof(Inode));
                memcpy(&inode,buf,sizeof(Inode));

                vector<double> AllBlocks = getAllBlocksUsedFor(disk,&inode,sizeofblock);

                for (int j = 0; j < AllBlocks.size(); ++j) {
                    setColorTo(AllBlocks[j]);
                }
            }
        }
    }

    isReady=true;
}

BlocksBox::~BlocksBox()
{
    delete ui;
}

void BlocksBox::addRow()
{
    ui->tableBlocks->insertRow(ui->tableBlocks->rowCount());
}

void BlocksBox::setColorTo(double block)
{
    QTableWidgetItem *item = search(QString::number(block));
    int column = ui->tableBlocks->column(item);
    int row = ui->tableBlocks->row(item);
    //cout<<"colRow: "<<column<<" "<<row<<endl;
    item->setBackground(QBrush(QColor(r,g,b)));
}

void BlocksBox::setColorTo(double block, int r, int g, int b)
{
    QTableWidgetItem *item = search(QString::number(block));
    item->setBackground(QBrush(QColor(r,g,b)));
}

void BlocksBox::newColor()
{
    while(true){
        r = rand()%256;
        g = rand()%256;
        b = rand()%256;
        QString color = QString::number(r) + " " + QString::number(g) + " " + QString::number(b);
        if(!usedColors.contains(color))
        {
            usedColors.push_back(color);
            break;
        }
    }
}

bool BlocksBox::ready()
{
    return isReady;
}

QTableWidgetItem *BlocksBox::search(QString item)
{
    return ui->tableBlocks->findItems(item,Qt::MatchExactly).first();
}
