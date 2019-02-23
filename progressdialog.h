#ifndef STOPSEARCH_H
#define STOPSEARCH_H

#include <QDialog>
#include <QThread>

namespace Ui {
class ProgressDialog;
}

class ProgressDialog : public QDialog
{
    Q_OBJECT

public:
    ProgressDialog(QThread* searchThread, QWidget *parent = nullptr);
    ~ProgressDialog();

public slots:
    void stopSearch();
    void update(int);
    void setMaximum(int);
private:
    Ui::ProgressDialog *ui;
    QThread* searchThread;
};

#endif // STOPSEARCH_H
