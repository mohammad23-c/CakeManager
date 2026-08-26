#ifndef ADDCAKETODAILY_H
#define ADDCAKETODAILY_H

#include <QDialog>
#include "../../managers/appmanager.h"

namespace Ui {
class AddCakeToDaily;
}

class AddCakeToDaily : public QDialog
{
    Q_OBJECT

public:
    explicit AddCakeToDaily(AppManager& m,const QDate& curr,QWidget *parent = nullptr);
    void updateCake(qint64 cakeId);
    void updatePic(QString imagePath);
    qint64 getCakeId();
    ~AddCakeToDaily();

private slots:

    void on_chooseCake_clicked();
    //cancel
    void on_pushButton_4_clicked();

    void on_unitChanged(int index);
    void on_quantityChanged();
    void on_weightChanged();


    void on_AddBtn_clicked();


private:
    Ui::AddCakeToDaily *ui;
    AppManager &m_appmanager;
    QDate m_currentDailyDate;

    qint64 cakeId;
};

#endif // ADDCAKETODAILY_H
