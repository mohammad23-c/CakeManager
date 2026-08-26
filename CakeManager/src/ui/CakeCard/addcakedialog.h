#ifndef ADDCAKEDIALOG_H
#define ADDCAKEDIALOG_H

#include <QDialog>
#include <QString>
#include <vector>

#include "../../managers/appmanager.h"
#include "../../utils/validation/inputvalidator.h"


namespace Ui {
class AddCakeDialog;
}

class AddCakeDialog : public QDialog
{
    Q_OBJECT

public:

    explicit AddCakeDialog(
        AppManager& appManager,
        QWidget* parent = nullptr
        );
    void updatePic();
    qint64 getNewCakeId() const;
    ~AddCakeDialog();

private slots:

    void on_chooseIngredientsBtn_clicked();

    void on_addCakeBtn_clicked();

    void on_cancelBtn_clicked();

    void on_choosePic_clicked();

private:

    Ui::AddCakeDialog* ui;

    AppManager& m_appManager;

    std::vector<CakeIngredient> m_cakeIngredients;

    qint64 m_cakeId;

    QString imagePath;
};

#endif // ADDCAKEDIALOG_H