#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QDir>
#include <QDragEnterEvent>
#include <QDropEvent>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QString folderPath;
    QString browsFolderPath();
    QString destPath;
    bool moveIt;
    quint32 total;
    quint32 succeed;
    quint32 fall;

    QString dragFolderName;
    QString dropFolderName;

    void initData();
    void initUI();
    void searchDir(QString path);
    void setStatus(QString str);
    bool stripFile(QFileInfo info);
    void analysis();

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
private slots:

    void on_action_help_triggered();
    void on_action_about_triggered();
    void on_checkBox_copy_clicked();
    void on_checkBox_move_clicked();
    void on_pushButton_exec_clicked();
    void on_action_open_triggered();
};

#endif // MAINWINDOW_H
