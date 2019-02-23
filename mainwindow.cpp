#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "filesgrouper.h"
#include "progressdialog.h"

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
    , ui(new Ui::MainWindow)
    , thread(new QThread()) {
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
    connect(ui->actionDelete_Selected,&QAction::triggered, this, &main_window::delete_selected_files);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);
}

main_window::~main_window() {}

void main_window::select_directory() {
    dir_path = QFileDialog::getExistingDirectory(this, "Select Directory for Scanning", QString(),
                                                 QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    grouper = new FilesGrouper(dir_path);
    ProgressDialog *progressWindow = new ProgressDialog(thread.get(), this);
    connect(grouper, SIGNAL(filesChecked(int)), progressWindow, SLOT(update(int)));
    connect(grouper, SIGNAL(filesCounted(int)), progressWindow, SLOT(setMaximum(int)));

    grouper->moveToThread(thread.get());
    connect(thread.get(), SIGNAL (started()), grouper, SLOT (computeDuplicateGroups()));
    connect(thread.get(), SIGNAL(started()), progressWindow, SLOT (open()));
    connect(grouper, SIGNAL(finished(int)), progressWindow, SLOT (done(int)));

    connect(grouper, SIGNAL (finished(int)), this, SLOT (show_duplicates()));
    connect(grouper, SIGNAL (finished(int)), thread.get(), SLOT (quit()));
    thread->start();
}

void main_window::show_duplicates() {
    auto list = grouper->getDuplicateGroups();
    QDir dir(dir_path);
    ui->treeWidget->clear();
    setWindowTitle(dir_path);
    int current_group = 0;
    for (auto group : list) {
        QTreeWidgetItem* gr = new QTreeWidgetItem(ui->treeWidget);
        gr->setText(0, QString("Group ") + QString::number(++current_group));
        gr->setText(1, QString::number(group.front().size()));
        gr->setText(2, QString::number(group.size()));
        ui->treeWidget->addTopLevelItem(gr);
        for (auto file : group) {
            QTreeWidgetItem* item = new QTreeWidgetItem();
            item->setText(0, dir.relativeFilePath(file.filePath()));
            gr->addChild(item);
        }
    }
}

void main_window::delete_selected_files() {
    auto selected_files = ui->treeWidget->selectedItems();

    QString absolutePath;

    for (auto& item : selected_files) {
        if (item->parent() != nullptr) {
            absolutePath = QDir(dir_path).filePath(item->text(0));
            if (QFile::remove(absolutePath)) {
                item->parent()->setText(2, QString::number(item->parent()->text(2).toInt() - 1));
                delete item;
            }
        }
    }
}
