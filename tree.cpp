#include "tree.h"
#include "ui_tree.h"

Tree::Tree(QWidget *parent,string disk, double start_inodes, double root, int b_size) :
    QWidget(parent),
    ui(new Ui::Tree)
{
    ui->setupUi(this);
    this->disk = disk;
    start_inode = start_inodes;
    inode = root;
    blocksize=b_size;

    fillTree();
}

Tree::~Tree()
{
    delete ui;
}

void Tree::fillTree()
{
    ui->tree->clear();

    char buff[sizeof(Inode)];
    read(disk,(char*)&buff,start_inode + inode*sizeof(Inode),sizeof(Inode));
    Inode root_inode;
    memcpy(&root_inode,&buff,sizeof(Inode));
//    Nodo *root = new Nodo("/");

    QTreeWidgetItem *rootWidget = new QTreeWidgetItem();
    rootWidget->setText(0,"/");
    fillTree(&root_inode,"/",rootWidget);
    ui->tree->addTopLevelItem(rootWidget);
}

void Tree::fillTree(Inode* curr_inode,QString name,QTreeWidgetItem*&root)
{
    QList<QTreeWidgetItem*>hijos;
    vector<FileData*>FT;
    char *data;
    readDataBlocksFrom(disk,data,curr_inode,blocksize);
    FT = getFileTableFrom(*curr_inode,data);

    if(curr_inode->permisos[0]=='d')
    {
        for (int i = 0; i < FT.size(); ++i) {
            char buff[sizeof(Inode)];
            read(disk,(char*)&buff,start_inode + FT[i]->index_file*sizeof(Inode),sizeof(Inode));
            Inode *child_inode = new Inode();
            memcpy(child_inode,&buff,sizeof(Inode));

            QTreeWidgetItem* item = new QTreeWidgetItem();
            item->setText(0,QString(FT[i]->name));

            if((strcmp(FT[i]->name,".."))!=0)
            {
                if((child_inode->permisos)[0]=='d')
                {
                    fillTree(child_inode,QString(FT[i]->name),item);
                }
                root->addChild(item);
            }
        }
    }
}
