#include "cakecardsetting.h"
#include "ui_cakecardsetting.h"
#include "ingredientselectiondialog.h"
#include <QMessageBox>
#include <QFileDialog>

#include "../../utils/validation/inputvalidator.h"

CakeCardSetting::CakeCardSetting(
    AppManager& appManager,
    qint64 cakeId,
    QWidget* parent
    )
    : QDialog(parent),
    m_appManager(appManager),
    m_cakeId(cakeId),
    ui(new Ui::CakeCardSetting)
{
    ui->setupUi(this);

    auto cake = m_appManager.findCake(cakeId);

    if (!cake.has_value())
    {
        return;
    }

    c = cake.value();

    imagePath = c.getImagePath();

    ui->lineEditName->setText(
        c.getName()
        );

    ui->lineEditProfit->setText(
        QString::number(c.getProfitPercentage())
        );

    m_cakeIngredients = c.getIngredients();

    updatePic();
    updateInfromation();
}

CakeCardSetting::~CakeCardSetting()
{
    delete ui;
}

void CakeCardSetting::updatePic()
{
    QPixmap pixmap;

    if (!imagePath.isEmpty() && QFile::exists(imagePath))
    {
        pixmap.load(imagePath);
    }else{
        pixmap.load(":/images/default.png");
    }
    pixmap = pixmap.scaled(
        ui->Image->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );
    ui->Image->setPixmap(pixmap);
    return ;
}
//
void CakeCardSetting::updateInfromation()
{
    std::optional<double>  weight=m_appManager.calculateCakeWeight(m_cakeIngredients);
    if(!weight.has_value()){
        return;
    }
    std::optional<double> price=m_appManager.calculateCakePrice(m_cakeIngredients);
    if(!price.has_value()){
        return;
    }
    ui->weight->setText(QString::number(weight.value())+"kilogram");
    ui->Price->setText(QString::number(price.value()));
}

void CakeCardSetting::on_changeBtn_clicked()
{
    c.setId(m_cakeId);

    if (!InputValidator::validateLineEdit(
            ui->lineEditName,
            InputValidator::InputType::NoType))
    {
        return;
    }

    c.setName(
        ui->lineEditName->text()
        );

    if (!InputValidator::validateLineEdit(
            ui->lineEditProfit,
            InputValidator::InputType::Double))
    {
        return;
    }

    c.setProfitPercentage(
        ui->lineEditProfit->text().toDouble()
        );

    c.setIngredients(
        m_cakeIngredients
        );

    c.setImagePath(
        imagePath
        );

    if (!m_appManager.updateCake(c))
    {
        QMessageBox::warning(
            this,
            "Name Not Unique",
            "Cannot update cake. Check that the name is unique."
            );

        InputValidator::setFocusAndRedBorder(
            ui->lineEditName
            );

        return;
    }

    m_appManager.cakeSave();

    accept();
}

void CakeCardSetting::on_cancelBtn_clicked()
{
    reject();
}


void CakeCardSetting::on_changeIngreditenBtn_clicked()
{
    IngredientSelectionDialog dialog(
        m_appManager,
        m_cakeIngredients,
        this
        );

    dialog.exec();
    updateInfromation();
}

void CakeCardSetting::on_deleteBtn_clicked()
{
    QMessageBox::StandardButton result =
        QMessageBox::question(
            this,
            "Delete Cake",
            "Are you sure you want to delete this cake?",
            QMessageBox::Yes | QMessageBox::No
            );

    if (result == QMessageBox::Yes)
    {
        bool success =
            m_appManager.deleteCake(m_cakeId);

        if (!success)
        {
            QMessageBox::warning(
                this,
                "Cannot Delete Cake",
                "This cake cannot be deleted."
                );

            return;
        }

        m_appManager.cakeSave();

        emit deleteCake(m_cakeId);

        accept();
    }
}


void CakeCardSetting::on_changePicBtn_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Choose Image",
        "",
        "Images (*.png *.jpg *.jpeg *.bmp *.webp)"
        );

    if (filePath.isEmpty())
    {
        return;
    }
    imagePath=filePath;
    updatePic();
}

