
#include "mytreewidget.h"
#include "progressdialog.h"

#include <QThread>
#include <qprogressbar.h>



void MyTreeWidget::makeFileSystem() {
    clear();
    //mainwindow->setWindowTitle(QString("Directory Content - %1").arg(currentDir));
    QDir d(currentDir);
    QFileInfoList list = d.entryInfoList();
    QList < QTreeWidgetItem * > items;
    for (QFileInfo file_info : list) {
        if (file_info.fileName() == "..")
            continue;
        QTreeWidgetItem *item = new QTreeWidgetItem(this);
        if (file_info.fileName() == ".") {
            item->setIcon(NAME_COL, style.standardIcon(QStyle::SP_ArrowBack));
        } else {
            file_info.isDir() ? item->setIcon(NAME_COL, style.standardIcon(QStyle::SP_DirIcon)) : item->setIcon(
                    NAME_COL, style.standardIcon(QStyle::SP_FileIcon));
        }
        setItemParameters(item, file_info);
        items.append(item);
    }
    insertTopLevelItems(0, items);
}

bool MyTreeWidget::checkItem(ACTION action) {
    if (selectedFile == nullptr || selectedFile->text(DIR_COL) == "") {
        QMessageBox::warning(this, "Select file", "Select file", QMessageBox::Ok);
        return false;
    }
    return !(QMessageBox::warning(this, "Deleting",
                                  QString("Do you really want to delete %1").arg(
                                 action == ACTION::THIS ? "file %1" : "dublicates of \n\n %1").arg(
                                 getItemName(selectedFile)),
                         QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel);
}

void MyTreeWidget::fileSelected(QTreeWidgetItem *curFile) {
   selectedFile = curFile;
}

void MyTreeWidget::deleteDuplicate(ACTION action) {
    if (!checkItem(action))
        return;
    if (action == THIS) {
        QTreeWidgetItem *parent = selectedFile->parent();
        //QList<QTreeWidgetItem*> children = parent->takeChildren();
        removeFile(selectedFile);
        long delSize = selectedFile->text(SIZE_COL).toInt();
        parent->removeChild(selectedFile);
        parent->setText(NAME_COL, QString("%1  dublicated files").arg(parent->childCount()));
        if (parent->childCount() == 1) {
            delete parent;
        } else {
            parent->setText(NAME_COL, QString("%1  dublicated files").arg(parent->childCount()));
            parent->setText(SIZE_COL, QString::number(parent->childCount() * delSize));
        }

        QMessageBox::information(this, "Congratulations",
                                 QString("file was successfuly deleted\n"), QMessageBox::Ok);
    }

    if (action == EXCEPT_THIS) {
        QTreeWidgetItem *parent = selectedFile->parent();
        QList < QTreeWidgetItem * > children = parent->takeChildren();
        for (QTreeWidgetItem *child : children) {
            if (selectedFile == child)
                continue;
            removeFile(child);
        }
        delete parent;
        QMessageBox::information(this, "Congratulations",
                                 QString("%1 files was successfuly deleted\n").arg(QString::number(children.size())),
                                 QMessageBox::Ok);
    }

    if (topLevelItemCount() == 0) {
        noDublicatesMessage(currentDir);
    }
}


void MyTreeWidget::removeFile(QTreeWidgetItem *child) {
    QString delFilePath = getItemName(child);
    QFile file(delFilePath);
    if (!file.remove()) {
        QMessageBox::warning(this, "Can't delete file",
                             QString("File \n\n %1 \n\n can't be removed.").arg(delFilePath),
                             QMessageBox::Ok);
    }
}

void MyTreeWidget::scan_directory(QString const &dir) {
    mainwindow->setModeType(false);
    auto *thread = new QThread();
    worker = new DataParser(dir);
    auto *progressWindow = new ProgressDialog(thread, this);
    worker->moveToThread(thread);

    connect(thread, SIGNAL (started()), worker, SLOT (find_dublicate()));

    connect(worker, SIGNAL(filesChecked(int)), progressWindow, SLOT(update(int)));
    connect(worker, SIGNAL(filesCounted(int)), progressWindow, SLOT(setRange(int)));


    connect(worker, SIGNAL (finished(int)), this, SLOT (show()));
    connect(worker, SIGNAL (finished(int)), thread, SLOT (quit()));
    connect(worker, SIGNAL(finished(int)),  progressWindow, SLOT(done(int)));
    connect(thread, SIGNAL (finished()), thread, SLOT (deleteLater()));

    thread->start();
    if (progressWindow->exec() == QDialog::Rejected) {
            progressWindow->stopSearch();
        }
    delete progressWindow;
}
void MyTreeWidget::show(){
    qDebug()<<"kek";
    clear();
    bool isDublicate = false;
    for (auto& comp : worker->getDuplicateMap()) {
        if (comp.size() < 2) continue;
        isDublicate = true;
        QTreeWidgetItem *group = new QTreeWidgetItem();

        comp.size() * comp.begin()->size() > 1000 ? group->setIcon(NAME_COL,
                                                                   style.standardIcon(QStyle::SP_MessageBoxCritical))
                                                  : group->setIcon(NAME_COL,
                                                                   style.standardIcon(QStyle::SP_MessageBoxWarning));
        group->setText(NAME_COL, QString("%1 duplicated files").arg(comp.size()));
        group->setTextColor(NAME_COL, Qt::red);
        qDebug()<<comp.size();
        for (auto& fileDir : comp) {
           QFileInfo file_info(fileDir);
            QTreeWidgetItem *item = new QTreeWidgetItem();
            setItemParameters(item, file_info);
            group->addChild(item);
        }
       group->setText(SIZE_COL, QString::number(comp.size() * group->child(0)->text(SIZE_COL).toLong()));
        addTopLevelItem(group);
    }
    sortItems(SIZE_COL, Qt::SortOrder::DescendingOrder);

    selectedFile = nullptr;
    if (!isDublicate) {
         mainwindow->setModeType(true);
        noDublicatesMessage(worker->rootPath);
    }
    isButtomsWorks = true;
    delete worker;
}

void MyTreeWidget::onTreeWidgetClicked() {
    if (selectedFile == nullptr) return;
    QString prevDir = currentDir;
    prevDir.truncate(prevDir.lastIndexOf("/"));
    if(prevDir == ""){
        prevDir = "/";
    }
    QString newOdj = selectedFile->text(NAME_COL) == "." ? prevDir : getItemName(selectedFile);
    if(newOdj.startsWith("//")){
        newOdj = newOdj.mid(1);
    }
    QFileInfo tmp(newOdj);
    if (mainwindow->getModeType() && tmp.isDir()) {
            currentDir = newOdj;
            mainwindow->makeFileSystem();
            return;
    }
    if (!tmp.isDir()){
        if(!QDesktopServices::openUrl(QUrl::fromLocalFile(newOdj))){
            if (QMessageBox::information(this, "Msg",
                                         QString("Can't open file \n\n %1").arg(newOdj), QMessageBox::Ok)) {
                this->close();
            }
        }
    }
}

void MyTreeWidget::noDublicatesMessage(QString const &dir) {
      mainwindow->makeFileSystem();
    if (QMessageBox::information(this, "Msg",
                                 QString("No duplicated files found in directory:\n\n %1 \n\nDo you want to exit from application?").arg(
                                         dir), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        this->close();
    }
}



QString MyTreeWidget::getItemName(QTreeWidgetItem *item) {
    return QString("%1/%2").arg(item->text(DIR_COL), item->text(NAME_COL));
}


void MyTreeWidget::keyPressEvent(QKeyEvent * event){
    QString x =event->text();
    if ( event->key() == Qt::Key_Return){
        this->onTreeWidgetClicked();
    }
    //f
    if ( event->key() == Qt::Key(1040)||  event->key() ==Qt::Key_F){
        mainwindow->scan_directory();
    }
    if ( event->key() == Qt::Key_Escape && !mainwindow->getModeType()){
        mainwindow->makeFileSystem();
    }
    if ( event->key() == Qt::Key_Escape && mainwindow->getModeType()){
        currentDir.truncate(currentDir.lastIndexOf("/"));
        if(currentDir.size()==0){
            currentDir = "/";
        };
        mainwindow->makeFileSystem();
    }
    qDebug() << Qt::Key(event->key());
    if(Qt::Key(event->key()) == Qt::Key_Backspace && !mainwindow->getModeType()){
        mainwindow->deleteCurrent();
    }
    QTreeWidget::keyPressEvent(event);
}
void MyTreeWidget::setItemParameters(QTreeWidgetItem *item, QFileInfo &file_info) {
    item->setText(DIR_COL, file_info.path());
    item->setText(NAME_COL, file_info.fileName());
    if (!file_info.isDir()){
      item->setText(SIZE_COL, QString::number(file_info.size()));
    }
}
