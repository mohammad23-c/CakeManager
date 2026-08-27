#ifndef DAILYCAKECARDSETTING_H
#define DAILYCAKECARDSETTING_H

#include <QDialog>
#include "../../managers/appmanager.h"



namespace Ui {
class dailyCakeCardSetting;
}

class dailyCakeCardSetting : public QDialog
{
    Q_OBJECT

public:
    dailyCakeCardSetting(
        AppManager& appManager,
        qint64 dailyId,
        qint64 cakeId,
        QWidget* parent = nullptr
        );
    void updatePic();
    ~dailyCakeCardSetting();
private slots:
    void on_unitChanged(int index);
    void on_showIngerBtn_clicked();

    void on_pushButton_clicked();
    void on_quantityChanged();
    void on_weightChanged();


    void on_deleteBtn_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::dailyCakeCardSetting *ui;

    AppManager& m_appManager;

    qint64 dailyId;
    qint64 cakeId;

    Cake cake;
    Daily m_daily;

    double cakeWeight;
    std::vector<CakeIngredient> m_cakeIngredients;

};

#endif // DAILYCAKECARDSETTING_H
