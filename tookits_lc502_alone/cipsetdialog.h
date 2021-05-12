#ifndef CIPSETDIALOG_H
#define CIPSETDIALOG_H

#include <QDialog>

namespace Ui {
class CIpSetDialog;
}

class CIpSetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CIpSetDialog(QWidget *parent = nullptr);
    ~CIpSetDialog();

    void ShowDlg();
private:
    void _SearchAllLocalIp();
private slots:
    void on_requery_btn_clicked();

    void on_ok_btn_clicked();

    void on_cancel_btn_clicked();

private:
    Ui::CIpSetDialog *ui;
};

#endif // CIPSETDIALOG_H
