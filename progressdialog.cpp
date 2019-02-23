#include "progressdialog.h"
#include "ui_progressdialog.h"
#include <QDebug>

ProgressDialog::ProgressDialog(QThread* searchThread, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProgressDialog),
    searchThread(searchThread) {
    qDebug()<<"progress dialog created";
    ui->setupUi(this);
    update(0);
    connect(ui->stopButton, SIGNAL(clicked()), this, SLOT(stopSearch()));
}

ProgressDialog::~ProgressDialog() {
    qDebug()<<"progress dialog destroyed";
    delete ui;
}

void ProgressDialog::stopSearch() {
    searchThread->requestInterruption();
}

void ProgressDialog::setMaximum(int max){
    ui->progressBar->setMaximum(max);
}

void ProgressDialog::update(int value) {
    ui->progressBar->setValue(value);
}
