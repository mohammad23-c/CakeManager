#ifndef CAKECARDSETTING_H
#define CAKECARDSETTING_H

#include <QDialog>
#include "../../managers/appmanager.h"

namespace Ui {
class CakeCardSetting;
}

class CakeCardSetting : public QDialog
{
    Q_OBJECT

public:
    explicit CakeCardSetting(
        AppManager& appManager,
        qint64 cakeId,
        QWidget *parent = nullptr);
    ~CakeCardSetting();
    void updatePic();
    void updateInfromation();


private slots:
    void on_changePicBtn_clicked();

    void on_cancelBtn_clicked();

    void on_changeIngreditenBtn_clicked();

    void on_changeBtn_clicked();

    void on_deleteBtn_clicked();


signals:
    void deleteCake(qint64 cakeId);

private:
    Ui::CakeCardSetting *ui;
    AppManager& m_appManager;

    std::vector<CakeIngredient> m_cakeIngredients;

    qint64 m_cakeId;

    QString imagePath;

    Cake c;
};

#endif // CAKECARDSETTING_H
