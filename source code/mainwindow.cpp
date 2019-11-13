#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QUrl>
#include <string>
#include <QMimeData>
#include <QDebug>
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initUI();
    initData();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initData()
{
    total = 0;
    succeed = 0;
    fall = 0;
    destPath = "";
    folderPath = "";
}
void MainWindow::initUI()
{
    this->setWindowTitle("文件夹剥离工具（拖动选择文件夹）");
    ui->checkBox_move->setChecked(true);
    moveIt = true;
    connect(ui->action_quit,SIGNAL(triggered()), this, SLOT(close()));
    this->setAcceptDrops(true);    //允许拖放
}

QString MainWindow::browsFolderPath()
{
    return QFileDialog::getExistingDirectory(this, "选择目录", QDir::homePath(), QFileDialog::ShowDirsOnly);
}

void MainWindow::setStatus(QString str)
{
    statusBar()-> showMessage(str);
}

void MainWindow::on_action_open_triggered()
{
    QString path = browsFolderPath();
    if (path != "")
    {
        ui->lineEdit->setText(path);
    }
}

void MainWindow::on_pushButton_exec_clicked()
{
    folderPath = ui->lineEdit->text();
    QDir dir(folderPath);
    if (!dir.exists())
    {
        return;
    }

    //destPath设置为上层目录
    QDir destDir = dir;
    destDir.cdUp();
    destPath = destDir.absolutePath();

    searchDir(folderPath);
    analysis();
}

bool MainWindow::stripFile(QFileInfo info)
{
    string scrFilePath = info.absoluteFilePath().toStdString();
    char *scr = (char *)scrFilePath.c_str();
    qDebug() << scr;

    string dstFilePath = (destPath + '/' + info.fileName()).toStdString();
    char *dst = (char *)dstFilePath.c_str();
    qDebug() << dst;

    bool bRet = false;
    if (moveIt)
    {
        bRet = QFile::rename(scr, dst);
    }
    else
    {
        bRet = QFile::copy(scr, dst);
    }
    return bRet;
}

void MainWindow::searchDir(QString path)
{
    QStringList str;
    str << "*";

    QDir dir(path);
    QFileInfoList list = dir.entryInfoList(str, QDir::AllEntries | QDir::Hidden | QDir::NoDot, QDir::DirsLast);
    QFileInfo info;

    QDir tempDir;

    for (int i=0; i<list.count(); i++)
    {
        info = list.at(i);
        if (info.isDir() && info.baseName() != "")
        {
            tempDir = dir;
            tempDir.cd(info.absoluteFilePath());
            searchDir(tempDir.path());
        }
        else if(info.isFile())
        {
            total++;

            if (stripFile(info))
            {
                succeed++;
            }
            else
            {
                fall++;
            }
        }
    }
}

void MainWindow::on_checkBox_move_clicked()
{
    ui->checkBox_move->setChecked(true);
    ui->checkBox_copy->setChecked(false);
    moveIt = true;
}

void MainWindow::on_checkBox_copy_clicked()
{
    ui->checkBox_copy->setChecked(true);
    ui->checkBox_move->setChecked(false);
    moveIt = false;
}

void MainWindow::analysis()
{
    QString msg;
    QString str;

    str.setNum(total);
    msg = "找到文件总数：" + str;

    str.setNum(succeed);
    msg += " 个\n----------------------\n成功剥离出：" + str;

    str.setNum(fall);
    msg += " 个\n失败：" + str + " 个\n";

    if (fall!=0)
    {
        msg += "(失败原因可能是此位置已包含同名文件)\n";
    }

    switch( QMessageBox::information( this,"执行结果"
                                      , msg ,"确定", 0))
    {
    case 0:break;
    }

    total = 0;
    succeed = 0;
    fall = 0;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasFormat("text/uri-list"))
    {
        QList<QUrl> urls=event->mimeData()->urls();
        if(urls.isEmpty())
            return;

        dragFolderName=urls.first().toLocalFile();
        if(dragFolderName.isEmpty())
            return;

        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
   dropFolderName = dragFolderName;
   ui->lineEdit->setText(dropFolderName);
}

void MainWindow::on_action_about_triggered()
{
    switch( QMessageBox::information( this,"关于"
                                      , "FolderStriper(文件夹剥离工具) v1.0。\n\t\t作者：zhang35" ,"确定",0))
    {
    case 0:break;
    }
}

void MainWindow::on_action_help_triggered()
{
    switch( QMessageBox::question( this,"帮助"
                                      , "任选一种方法选择一个文件夹\n1、输入文件夹路径\n2、文件-打开，浏览目录\n3、直接将文件夹拖动至软件界面\n点击执行，可以将指定文件夹下的所有文件（包括子文件夹中的文件），移动或复制到文件夹所在目录,操作无法撤销，请慎用！" ,"确定",0))
    {
    case 0:break;
    }
}

