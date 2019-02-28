#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCommonStyle>
#include <QDesktopWidget>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QTextStream>
#include <QThread>
#include <QtDebug>

main_window::main_window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
                                    size(), qApp->desktop()->availableGeometry()));
    setWindowTitle(QString("Find Duplicates"));
    ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->treeWidget->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->treeWidget->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->treeWidget->header()->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    QCommonStyle style;
    ui->actionScan_Directory->setIcon(style.standardIcon(QCommonStyle::SP_DialogOpenButton));
    ui->actionExit->setIcon(style.standardIcon(QCommonStyle::SP_DialogCloseButton));

    connect(ui->actionScan_Directory, &QAction::triggered, this, &main_window::select_directory);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);
}

main_window::~main_window() {}

void main_window::select_directory() {
    qDebug()<<"select directory";
    dir_path = QFileDialog::getExistingDirectory(this, "Select Directory for Scanning", QString(),
                                                 QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

//    grouper = new FilesGrouper(dir_path);
//    grouper->moveToThread(thread.get());

//    connect(thread.get(), SIGNAL (started()), grouper, SLOT (computeDuplicateGroups()));

//    connect(grouper, SIGNAL (filesChecked(int)), ui->progressBar, SLOT (setValue(int)));
//    connect(grouper, SIGNAL (filesCounted(int)), ui->progressBar, SLOT (setMaximum(int)));

//    connect(grouper, SIGNAL (finished(int)), this, SLOT (show_duplicates()));
//    connect(grouper, SIGNAL (finished(int)), thread.get(), SLOT (quit()));
//    connect(ui->actionStop_search, &QAction::triggered, [=](){thread->requestInterruption();});
//    thread->start();
}
