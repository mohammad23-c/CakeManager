#include "materialdialog.h"
#include "ui_materialdialog.h"
#include <QFileDialog>
#include <QMessageBox>
MaterialDialog::MaterialDialog(
    qint64 ingredientId,
    AppManager& appManager,
    QWidget *parent
    )
    : QDialog(parent),
    ui(new Ui::MaterialDialog),
    m_appManager(appManager),
    m_ingredientId(ingredientId)
{
    ui->setupUi(this);
    //connects
    connect(
        ui->UnitcomboBox,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &MaterialDialog::enableWeightLineEdit
        );
    //find ingredient by id
    auto ingredient = m_appManager.findIngredient(m_ingredientId);

    if (!ingredient.has_value())
    {
        QMessageBox::warning(
            this,
            "Error",
            "Ingredient not found."
            );

        reject();
        return;
    }
    in=new Ingredient(ingredient.value());
    //default
    ui->PricepuLineE->setEnabled(false);
    //setText
    //name
    ui->NamelineEdit->setText(ingredient->getName());
    //price
    ui->PriceLineEdit->setText(QString::number(ingredient->getPricePerUnit()));
    //unit
    ui->UnitcomboBox->setCurrentIndex(static_cast<int>(ingredient->getUnit()));
    //weight per unit
    //default zero
    ui->PricepuLineE->setText("0");
    if(ingredient->getUnit()==Ingredient::Unit::Piece){
        ui->PricepuLineE->setEnabled(true);
        //weight per uinit always in kilogram
        ui->PricepuLineE->setText(QString::number(ingredient->getWeightPerUnit()));
    }
    //update image
    updateImage();

}

MaterialDialog::~MaterialDialog()
{
    delete in;
    delete ui;
}

void MaterialDialog::updateImage()
{
    QPixmap pixmap;
    if (!in->getImagePath().isEmpty() && QFile::exists(in->getImagePath()))
    {
        pixmap.load(in->getImagePath());
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

void MaterialDialog::on_choosePicBtn_clicked()
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
    in->setImagePath(filePath);
    updateImage();
}

//save ingredient
void MaterialDialog::on_save_clicked()
{
    //update in with line edits
    in->setName(ui->NamelineEdit->text());
    in->setPricePerUnit(ui->PriceLineEdit->text().toDouble());
    in->setUnit(static_cast<Ingredient::Unit>(ui->UnitcomboBox->currentIndex()));
    if(in->getUnit()==Ingredient::Unit::Piece){
        if(ui->PricepuLineE->text().isEmpty()){
            QMessageBox::warning(this,"Weight per unit is empty","Please enter weight per unit");
            ui->PricepuLineE->setFocus();
            ui->PricepuLineE->setStyleSheet("border: 1px solid red;");
            return;
        }
        in->setWeightPerUnit(ui->PricepuLineE->text().toDouble());
    }else{
        in->setWeightPerUnit(0);
    }
    //image path already set in on_choosePicBtn_clicked

    if(m_appManager.updateIngredient(*in)){

        m_appManager.ingredientSave();
        accept();
        return ;
    }
    QMessageBox::warning(this,"nameNot uniq","cant update : check name is unique or not");
    ui->NamelineEdit->setFocus();
    ui->NamelineEdit->setStyleSheet("border: 1px solid red;");
    return ;
}


void MaterialDialog::on_pushButton_2_clicked()
{
    reject();
}

void MaterialDialog::enableWeightLineEdit(int index)
{
    ui->PricepuLineE->setEnabled(
        index == static_cast<int>(Ingredient::Unit::Piece)
        );
}


void MaterialDialog::on_deleteIngredent_clicked()
{
    QMessageBox::StandardButton result =
        QMessageBox::question(
            this,
            "Delete Ingredient",
            "Are you sure you want to delete this ingredient?",
            QMessageBox::Yes | QMessageBox::No
            );
    if(result==QMessageBox::Yes){
        bool s=m_appManager.deleteIngredient(m_ingredientId);
        if(!s){
            QMessageBox::warning(
                this,
                "Cannot Delete Ingredient",
                "This ingredient is used in a cake and cannot be deleted."
                );
            return ;
        }
        m_appManager.ingredientSave();
        emit deleteIng(m_ingredientId);
        accept();
    }
    return ;
}

