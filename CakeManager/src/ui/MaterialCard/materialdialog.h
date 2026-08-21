#ifndef MATERIALDIALOG_H
#define MATERIALDIALOG_H

#include <QDialog>

#include "../../managers/appmanager.h"

namespace Ui {
class MaterialDialog;
}

class MaterialDialog : public QDialog
{
    Q_OBJECT

public:

    explicit MaterialDialog(
        qint64 ingredientId,
        AppManager& appManager,
        QWidget *parent = nullptr
        );

    ~MaterialDialog();
    void updateImage();
private slots:
    void on_choosePicBtn_clicked();

    void on_save_clicked();

    void on_pushButton_2_clicked();

    void enableWeightLineEdit(int index);

private:

    Ui::MaterialDialog *ui;

    AppManager& m_appManager;

    qint64 m_ingredientId;

    Ingredient* in;

};

#endif // MATERIALDIALOG_H