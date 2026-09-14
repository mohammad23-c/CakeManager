#include "cakecardsetting.h"
#include "ui_cakecardsetting.h"
#include "ingredientselectiondialog.h"
#include <QMessageBox>
#include <QFileDialog>

#include "../../utils/validation/inputvalidator.h"
#include "../../utils/outputcontroll.h"

CakeCardSetting::CakeCardSetting(
    AppManager& appManager,
    qint64 cakeId,
    QWidget* parent
    )
    : QDialog(parent),
    m_appManager(appManager),
    m_cakeId(cakeId),
    ui(new Ui::CakeCardSetting),
    imagePath(":/defultPic/images.png")
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
        pixmap.load(":/defultPic/images.png");
    }
    pixmap = pixmap.scaled(
        ui->Image->size(),
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        );

    // Round corners
    QPixmap roundedPixmap(pixmap.size());
    roundedPixmap.fill(Qt::transparent);

    QPainter painter(&roundedPixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.addRoundedRect(
        roundedPixmap.rect(),
        12,
        12
        );

    painter.setClipPath(path);
    painter.drawPixmap(0, 0, pixmap);

    painter.end();

    ui->Image->setPixmap(roundedPixmap);
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
    double percent=0;
    if(!ui->lineEditProfit->text().isEmpty()){
        percent=ui->lineEditProfit->text().toDouble();
    }
    ui->weight->setText(QString::number(weight.value())+" kilogram");
    ui->Price->setText(outPutControll::formatPrice((price.value())));

    double finalprice=price.value()*(1+percent/100.0);
    ui->finalPrice->setText(outPutControll::formatPrice((finalprice)));
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

