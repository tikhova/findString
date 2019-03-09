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
    thread(new QThread()) {
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

    connect(ui->actionScan_Directory, &QAction::triggered, this, &main_window::select_directory);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);

    connect(ui->searchButton, SIGNAL(clicked()), this, SLOT(findString()));
    connect(&ind, SIGNAL (filesCounted(int)), ui->progressBar, SLOT (setMaximum(int)));
    connect(&ind, SIGNAL (filesChecked(int)), ui->progressBar, SLOT (setValue(int)));
    connect(&ind, SIGNAL (finished(int)), thread.get(), SLOT (quit()));
    connect(ui->actionStop_search,  &QAction::triggered, [=](){thread->requestInterruption();});
}

main_window::~main_window() {}

void main_window::select_directory() {
    dir_path = QFileDialog::getExistingDirectory(this, "Select Directory for Scanning", QString(),
                                                 QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    qDebug()<<"select directory" << dir_path;
    ind.setDirectory(dir_path);
    ind.moveToThread(thread.get());
    thread->start();
    ind.getTrigrams();
}

void main_window::findString() {
    qDebug() << "find string" << ui->lineEdit->text();
    thread->start();
    ui->treeWidget->clear();
    QMap<QString, QStringList> result = ind.findString(ui->lineEdit->text());
    QMapIterator<QString, QStringList> iter(result);
    QDir dir(dir_path);
    while(iter.hasNext()) {
        iter.next();
        QTreeWidgetItem* gr = new QTreeWidgetItem(ui->treeWidget);
        gr->setText(0, QString(dir.relativeFilePath(iter.key())));
        gr->setText(1, QString::number(QFile(iter.key()).size()));
        ui->treeWidget->addTopLevelItem(gr);
    }
    qDebug() << "find string finished"  << ui->lineEdit->text();
}

void main_window::open_file(QTreeWidgetItem *item){
    QDesktopServices::openUrl(QUrl("file:" + QDir(dir_path).absoluteFilePath(item->text(0))));
}
