#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <memory>
#include "indexator.h"
#include "finder.h"

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
    void select_string();
    void printFindStringResult();
    void open_file(QTreeWidgetItem *);
    void stop_index_thread();
    void stop_search_thread();
private:
    std::unique_ptr<Ui::MainWindow> ui;
    QString dir_path;    
    std::unique_ptr<QThread> index_thread;
    std::unique_ptr<QThread> search_thread;
    indexator ind;
    finder fin;
};

#endif // MAINWINDOW_H
