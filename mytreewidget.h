#ifndef MYTREEWIDGET_H
#define MYTREEWIDGET_H


#include <action.h>
#include <qcommonstyle.h>
#include <qtreewidget.h>
#include "dataparser.h"
#include "QKeyEvent"
#include "mainwindow.h"
#include <qdesktopservices.h>
#include <qdebug.h>
#include <qapplication.h>
#include "QClipboard"
class MyTreeWidget : public QTreeWidget {
    Q_OBJECT
public:
    const int SIZE_COL = 2;
    const int DIR_COL = 1;
    const int NAME_COL = 0;

    virtual ~MyTreeWidget(){
    }

   explicit MyTreeWidget(QWidget *parent = nullptr):QTreeWidget (parent) {
        setUniformRowHeights(1);
    }
    void setMainWindow(main_window* mw){
         mainwindow = mw;
    }
    void makeFileSystem();

    void removeFile(QTreeWidgetItem *child);

    void scan_directory(const QString &dir);

    QString getItemName(QTreeWidgetItem *item);

    QString getCurrDir() {
        return currentDir;
    }

    QTreeWidgetItem *getSelectedFile() {
        return selectedFile;
    }

    void setCurDir(QString x) {
        currentDir = x;
    }


    void setSelectedFile(QTreeWidgetItem *x) {
        selectedFile = x;
    }

 public slots:
    void onTreeWidgetClicked();

    void deleteDuplicate(ACTION action);

    void fileSelected(QTreeWidgetItem *curFile);

    void keyPressEvent(QKeyEvent *event);

     void show();
private:

    bool isButtomsWorks =true;
    bool checkItem(ACTION action);
    main_window* mainwindow;
    QThread* thread = nullptr;
    DataParser* worker = nullptr;
    QCommonStyle style;
    QString currentDir = QDir::homePath();
    QTreeWidgetItem *selectedFile = nullptr;


    void noDublicatesMessage(const QString &dir);

    void setItemParameters(QTreeWidgetItem *item, QFileInfo &file_info);
};

#endif // MYTREEWIDGET_H
