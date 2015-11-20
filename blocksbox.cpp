#include "blocksbox.h"
#include "ui_blocksbox.h"

BlocksBox::BlocksBox(QWidget *parent,double cantOfBlocks,double FS_Blocks) :
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
    cout<<"colRow: "<<column<<" "<<row<<endl;
    item->setBackground(QBrush(QColor(r,g,b)));
}

void BlocksBox::setColorTo(double block, int r, int g, int b)
{
    QTableWidgetItem *item = search(QString::number(block));
    item->setBackground(QBrush(QColor(r,g,b)));
}

void BlocksBox::newColor()
{
    srand (time(NULL));
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
