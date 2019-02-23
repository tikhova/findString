#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "filesgrouper.h"

#include <QFileInfoList>
#include <QMainWindow>
#include <memory>

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
    void show_duplicates();
    void delete_selected_files();
private:
    std::unique_ptr<Ui::MainWindow> ui;
    QString dir_path;
    FilesGrouper * grouper;
    std::unique_ptr<QThread> thread;
};

#endif // MAINWINDOW_H
