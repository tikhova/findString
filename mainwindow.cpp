#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCommonStyle>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QTextStream>
#include <QThread>
#include <QtDebug>

#include "indexator.h"

main_window::main_window(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow),
    index_thread(new QThread()), search_thread(new QThread()), fin(ind) {
    // setup ui
    ui->setupUi(this);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
                                    size(), qApp->desktop()->availableGeometry()));
    setWindowTitle(QString("Find String"));
    ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->treeWidget->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->treeWidget->header()->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    connect(ui->treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(open_file(QTreeWidgetItem*)));
    QCommonStyle style;
    ui->actionScan_Directory->setIcon(style.standardIcon(QCommonStyle::SP_DialogOpenButton));
    ui->actionExit->setIcon(style.standardIcon(QCommonStyle::SP_DialogCloseButton));

    // setup signals
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);
    /// index dir
    connect(ui->actionScan_Directory, &QAction::triggered, this, &main_window::select_directory);
    connect(&ind, SIGNAL (finished(int)), index_thread.get(), SLOT (quit()));
    connect(index_thread.get(), SIGNAL(started()), &ind, SLOT(getTrigrams()));

    /// find string
    connect(ui->searchButton, SIGNAL(clicked()), this, SLOT(select_string()));
    connect(search_thread.get(), SIGNAL(started()), &fin, SLOT(findString()));
    connect(&fin, SIGNAL(finished(int)), this, SLOT(printFindStringResult()));
    connect(&fin, SIGNAL(finished(int)), search_thread.get(), SLOT (quit()));

    /// progress bar
    connect(&ind, SIGNAL (filesCounted(int)), ui->progressBar, SLOT (setMaximum(int)));
    connect(&ind, SIGNAL (filesChecked(int)), ui->progressBar, SLOT (setValue(int)));
    connect(&fin, SIGNAL (filesCounted(int)), ui->progressBar, SLOT (setMaximum(int)));
    connect(&fin, SIGNAL (filesChecked(int)), ui->progressBar, SLOT (setValue(int)));

    /// on stop button
    connect(ui->actionStop_search,  &QAction::triggered, this, &main_window::stop_index_thread);
    connect(ui->actionStop_search,  &QAction::triggered, this, &main_window::stop_search_thread);

    /// move to thread
    ind.moveToThread(index_thread.get());
    fin.moveToThread(search_thread.get());
}

main_window::~main_window() {}

void main_window::select_directory() {
    QString path = QFileDialog::getExistingDirectory(this, "Select Directory for Scanning", QString(),
                                                 QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!path.isEmpty()) {
        dir_path = path;
        qDebug()<<"select directory" << dir_path;
        ind.setDirectory(dir_path);
        index_thread->start();
    }
}

void main_window::select_string() {
    qDebug() << "select string" << ui->lineEdit->text();
    fin.setString(ui->lineEdit->text());
    ui->treeWidget->clear();
    search_thread->start();
}

void main_window::printFindStringResult() {
    QMap<QString, QStringList> result = fin.getResultMap();
    QMapIterator<QString, QStringList> iter(result);
    QDir dir(dir_path);
    while(iter.hasNext()) {
        iter.next();
        QTreeWidgetItem* gr = new QTreeWidgetItem(ui->treeWidget);
        gr->setText(0, QString(dir.relativeFilePath(iter.key())));
        gr->setText(1, QString::number(QFile(iter.key()).size()));
        ui->treeWidget->addTopLevelItem(gr);
    }
    qDebug() << "print result";
}

void main_window::open_file(QTreeWidgetItem *item){
    QDesktopServices::openUrl(QUrl("file:" + QDir(dir_path).absoluteFilePath(item->text(0))));
}

void main_window::stop_index_thread() {
    if(index_thread != nullptr && index_thread->isRunning()){
        index_thread->requestInterruption();
        index_thread->quit();
        index_thread->wait(); // TODO: what is it?
        qDebug() << "index thread interrupted";
    }
}

void main_window::stop_search_thread() {
    if(search_thread != nullptr && search_thread->isRunning()){
        search_thread->requestInterruption();
        search_thread->quit();
        search_thread->wait(); // TODO: what is it?
        qDebug() << "search thread interrupted";
    }
}
