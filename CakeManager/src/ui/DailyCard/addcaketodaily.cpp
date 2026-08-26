#include "addcaketodaily.h"
#include "ui_addcaketodaily.h"
#include "CakeSelectionDialog.h"
#include "../../utils/validation/inputvalidator.h"
#include <QMessageBox>


AddCakeToDaily::AddCakeToDaily(
    AppManager& m,
    const QDate& curr,
    QWidget* parent
    )
    : QDialog(parent)
    , ui(new Ui::AddCakeToDaily)
    , m_appmanager(m)
    , m_currentDailyDate(curr)
    , cakeId(-1)
{
    ui->setupUi(this);

    connect(
        ui->comboBox,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &AddCakeToDaily::on_unitChanged
        );

    connect(
        ui->doubleSpinBoxQuantity,
        QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        this,
        &AddCakeToDaily::on_quantityChanged
        );

    connect(
        ui->doubleSpinBoxWeight,
        QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        this,
        &AddCakeToDaily::on_weightChanged
        );

    ui->comboBox->setEnabled(false);
    ui->doubleSpinBoxWeight->setEnabled(false);
    ui->doubleSpinBoxQuantity->setEnabled(false);
}

void AddCakeToDaily::updateCake(qint64 cakeid)
{
    auto c=m_appmanager.findCake(cakeid);
    if(!c.has_value()){
        return ;
    }
    Cake cake=c.value();
    ui->Name->setText(cake.getName());
    ui->PricePercent->setText(QString::number(cake.getProfitPercentage()));
    updatePic(cake.getImagePath());
    ui->doubleSpinBoxQuantity->setValue(0);
    ui->doubleSpinBoxWeight->setValue(0);
}

void AddCakeToDaily::updatePic(QString imagePath)
{
    QPixmap pixmap;

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

qint64 AddCakeToDaily::getCakeId()
{
    return cakeId;
}

AddCakeToDaily::~AddCakeToDaily()
{
    delete ui;
}



void AddCakeToDaily::on_chooseCake_clicked()
{
    CakeSelectionDialog sd(m_appmanager);
    sd.exec();
    cakeId =
        sd.getSelectedCakeId();

    if (cakeId == -1)
    {
        QMessageBox::warning(this,"No cake choosed","select one cake again");
        return;
    }else{
        ui->comboBox->setEnabled(true);
        updateCake(cakeId);
        ui->comboBox->setCurrentIndex(0);
        on_unitChanged(0);
    }
}


void AddCakeToDaily::on_pushButton_4_clicked()
{
    reject();
}

void AddCakeToDaily::on_unitChanged(int index)
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
//quantity changed // this func change weight double spinbox
void AddCakeToDaily::on_quantityChanged()
{

    auto cakeWeight =
        m_appmanager.calculateCakeWeight(cakeId);

    if (!cakeWeight.has_value())
    {
        return;
    }
    double weight =
        ui->doubleSpinBoxQuantity->value()
        * cakeWeight.value();

    ui->doubleSpinBoxWeight->blockSignals(true);

    ui->doubleSpinBoxWeight->setValue(weight);

    ui->doubleSpinBoxWeight->blockSignals(false);
}
//weight changed // this func change quantity double spinbox
void AddCakeToDaily::on_weightChanged()
{
    auto cakeWeight =
        m_appmanager.calculateCakeWeight(cakeId);

    if (!cakeWeight.has_value())
    {
        return;
    }

    double quantity =
        ui->doubleSpinBoxWeight->value()
        / cakeWeight.value();

    ui->doubleSpinBoxQuantity->blockSignals(true);

    ui->doubleSpinBoxQuantity->setValue(quantity);

    ui->doubleSpinBoxQuantity->blockSignals(false);
}


void AddCakeToDaily::on_AddBtn_clicked()
{
    double quantity=ui->doubleSpinBoxQuantity->value();
    if(cakeId==-1){
        InputValidator::setFocusAndRedBorder(ui->chooseCake);
        return;
    }
    if(quantity<=0){
        if (ui->comboBox->currentIndex() == 0)
        {
            InputValidator::setFocusAndRedBorder(
                ui->doubleSpinBoxQuantity
                );
        }
        else
        {
            InputValidator::setFocusAndRedBorder(
                ui->doubleSpinBoxWeight
                );
        }
        return;
    }
    DailyCake dc;
    dc.cakeId=cakeId;
    dc.quantity=ui->doubleSpinBoxQuantity->value();

    auto day=m_appmanager.findDaily(m_currentDailyDate);
    if (!day.has_value())
    {
        QMessageBox::warning(
            this,
            "Day Not Found",
            "Daily record was not found."
            );

        return;
    }
    bool success =
        m_appmanager.addCakeToDaily(
            day->getId(),
            dc.cakeId,
            dc.quantity
            );

    if (!success)
    {
        QMessageBox::warning(
            this,
            "Cannot Add Cake",
            "There is not enough inventory for this cake."
            );

        return;
    }

    accept();
}

