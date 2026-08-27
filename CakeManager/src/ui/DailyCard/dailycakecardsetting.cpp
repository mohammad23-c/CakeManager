#include "dailycakecardsetting.h"
#include "ui_dailycakecardsetting.h"
#include <QMessageBox>
#include <optional>
#include "../CakeCard/ingredientselectiondialog.h"

dailyCakeCardSetting::dailyCakeCardSetting(AppManager &appManager, qint64 dailyId, qint64 cakeId, QWidget *parent):
    QDialog(parent)
    , ui(new Ui::dailyCakeCardSetting)
    , m_appManager(appManager)
    , dailyId(dailyId)
    , cakeId(cakeId)
{
    ui->setupUi(this);
    ui->reduceInventoryCheckBox->setChecked(true);

    connect(
        ui->comboBoxUnit,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &dailyCakeCardSetting::on_unitChanged
        );

    connect(
        ui->doubleSpinBoxQuantity,
        QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        this,
        &dailyCakeCardSetting::on_quantityChanged
        );

    connect(
        ui->doubleSpinBoxWeight,
        QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        this,
        &dailyCakeCardSetting::on_weightChanged
        );

    auto cakeW =
        m_appManager.calculateCakeWeight(cakeId);

    if (!cakeW.has_value() || cakeW.value() <= 0)
    {
        QMessageBox::warning(
            this,
            "Can't Calculate",
            "Invalid cake weight."
            );

        reject();
        return;
    }
    cakeWeight=cakeW.value();

    auto d=m_appManager.findDaily(dailyId);
    if(!d.has_value()){
        QMessageBox::warning(this,"not found","daily not found");
        reject();
        return;
    }
    m_daily=d.value();
    auto Dcake=m_daily.findCake(cakeId);
    if(!Dcake.has_value()){
        QMessageBox::warning(this,"not found","cake not found");
        reject();
        return;
    }
    DailyCake dailyCake=Dcake.value();
    auto c =
        m_appManager.findCake(cakeId);
    if(!c.has_value()){
        QMessageBox::warning(this,"not found","cake not found");
        reject();
        return;
    }
    cake=c.value();
    auto first_quantity=dailyCake.quantity;
    m_cakeIngredients = cake.getIngredients();
    //fil labels
    ui->name->setText(cake.getName());
    ui->PricePercent->setText(
        QString::number(
            cake.getProfitPercentage()
            )
        );
    updatePic();

    //spin box
    ui->doubleSpinBoxQuantity->setEnabled(true);
    ui->doubleSpinBoxWeight->setEnabled(false);

    ui->doubleSpinBoxQuantity->setValue(first_quantity);
    double weight=first_quantity*cakeWeight;
    ui->doubleSpinBoxWeight->setValue(weight);


}
void dailyCakeCardSetting::updatePic()
{
    QPixmap pixmap;
    QString imagePath=cake.getImagePath();

    if (!imagePath.isEmpty() && QFile::exists(imagePath))
    {
        pixmap.load(imagePath);
    }
    else
    {
        pixmap.load(":/images/default.png");
    }

    pixmap = pixmap.scaled(
        ui->Image->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );

    ui->Image->setPixmap(pixmap);
}

dailyCakeCardSetting::~dailyCakeCardSetting()
{
    delete ui;
}

void dailyCakeCardSetting::on_unitChanged(int index)
{
    if (index == 0) // Piece
    {
        ui->doubleSpinBoxQuantity->setEnabled(true);
        ui->doubleSpinBoxWeight->setEnabled(false);
    }
    else if (index == 1) // Kilogram
    {
        ui->doubleSpinBoxQuantity->setEnabled(false);
        ui->doubleSpinBoxWeight->setEnabled(true);
    }
}

void dailyCakeCardSetting::on_showIngerBtn_clicked()
{
    IngredientSelectionDialog dialog(
        m_appManager,
        m_cakeIngredients,
        this
        );

    dialog.exec();
}

//change save
void dailyCakeCardSetting::on_pushButton_clicked()
{
    double quantity =
        ui->doubleSpinBoxQuantity->value();
    bool reduceInventory =
        ui->reduceInventoryCheckBox->isChecked();
    bool success =
        m_appManager.updateCakeToDaily(
            dailyId,
            cakeId,
            quantity,
            reduceInventory
            );

    if (!success)
    {
        QMessageBox::warning(
            this,
            "Cannot Update",
            "There is not enough inventory for this quantity."
            );

        return;
    }

    accept();
}
//quantity changed // this func change weight double spinbox
void dailyCakeCardSetting::on_quantityChanged()
{
    double weight =
        ui->doubleSpinBoxQuantity->value()
        * cakeWeight;

    ui->doubleSpinBoxWeight->blockSignals(true);

    ui->doubleSpinBoxWeight->setValue(weight);

    ui->doubleSpinBoxWeight->blockSignals(false);
}
//weight changed // this func change quantity double spinbox
void dailyCakeCardSetting::on_weightChanged()
{
    double quantity =
        ui->doubleSpinBoxWeight->value()
        / cakeWeight;

    ui->doubleSpinBoxQuantity->blockSignals(true);

    ui->doubleSpinBoxQuantity->setValue(quantity);

    ui->doubleSpinBoxQuantity->blockSignals(false);
}



void dailyCakeCardSetting::on_deleteBtn_clicked()
{
    bool reduceInventory =
        ui->reduceInventoryCheckBox->isChecked();
    bool result=m_appManager.removeCakeFromDaily(dailyId,cakeId,reduceInventory);
    if (!result)
    {
        QMessageBox::warning(
            this,
            "Error",
            "Could not remove cake from daily"
            );

        return;
    }
    accept();
}


void dailyCakeCardSetting::on_pushButton_2_clicked()
{
    reject();
}

