#ifndef ADDINGREDIENTDIALOG_H
#define ADDINGREDIENTDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include "../../models/ingredient.h"
#include "../../managers/appmanager.h"

namespace Ui {
class AddIngredientDialog;
}

class AddIngredientDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddIngredientDialog(AppManager &appManager, QWidget *parent);
    void updateImage();
    qint64 getNewIngredientId() const;
    ~AddIngredientDialog();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_chooseImageBtn_clicked();

    void enableWeightPerUnitLineEdit(int index);

private:

    Ui::AddIngredientDialog *ui;
    AppManager& m_appManager;
    Ingredient in;
    qint64 id;

};

#endif // ADDINGREDIENTDIALOG_H
