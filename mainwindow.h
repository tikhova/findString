#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileInfoList>
#include <QMainWindow>
#include <QTreeWidgetItem>
#include <memory>
#include "indexator.h"

namespace Ui {
class MainWindow;
}

class main_window : public QMainWindow
{
    Q_OBJECT

public:
    explicit main_window(QWidget *parent = nullptr);
    ~main_window();

private slots:
    void select_directory();
    void findString();
    void open_file(QTreeWidgetItem *);
private:
    std::unique_ptr<Ui::MainWindow> ui;
    QString dir_path;    
    std::unique_ptr<QThread> thread;
    indexator ind;
};

#endif // MAINWINDOW_H
