#include "addingredientdialog.h"
#include "ui_addingredientdialog.h"
#include <QMessageBox>

AddIngredientDialog::AddIngredientDialog(AppManager &appManager, QWidget *parent):
    QDialog(parent),
    ui(new Ui::AddIngredientDialog),
    m_appManager(appManager)
{
    ui->setupUi(this);
    ui->lineEditweightPer->setEnabled(false);
    connect(
        ui->comboBoxunit,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &AddIngredientDialog::enableWeightPerUnitLineEdit
        );

    in = Ingredient(
        0,
        "",
        Ingredient::Unit::Kilogram,
        0,
        0,
        ""
        );
    id=0;
}

void AddIngredientDialog::updateImage()
{
    QPixmap pixmap;
    if (!in.getImagePath().isEmpty() && QFile::exists(in.getImagePath()))
    {
        pixmap.load(in.getImagePath());
    }else{
        pixmap.load(":/images/default.png");
    }
    pixmap = pixmap.scaled(
        ui->labelPicture->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );
    ui->labelPicture->setPixmap(pixmap);
    return ;
}

qint64 AddIngredientDialog::getNewIngredientId() const
{
    return id;
}

AddIngredientDialog::~AddIngredientDialog()
{
    delete ui;
}
//add btn
void AddIngredientDialog::on_pushButton_clicked()
{
    in.setName(ui->lineEditName->text());
    in.setPricePerUnit(ui->lineEditPrice->text().toDouble());
    in.setUnit(static_cast<Ingredient::Unit>(ui->comboBoxunit->currentIndex()));
    in.setWeightPerUnit(0);
    if(in.getUnit()==Ingredient::Unit::Piece){
        if(ui->lineEditweightPer->text().isEmpty()){
            QMessageBox::warning(this,"Weight per unit is empty","Please enter weight per unit");
            ui->lineEditweightPer->setFocus();
            ui->lineEditweightPer->setStyleSheet("border: 1px solid red;");
            return;
        }
        in.setWeightPerUnit(ui->lineEditweightPer->text().toDouble());
    }
    double inventoryQuantity =
        ui->lineEditInventoryWeight->text().toDouble();
    if (m_appManager.addIngredient(in))
    {
        m_appManager.ingredientSave();

        auto ingredient = m_appManager.findIngredient(in.getName());

        if (!ingredient.has_value())
        {
            return;
        }

        m_appManager.addInventory(
            ingredient->getId(),
            inventoryQuantity
            );

        m_appManager.inventorySave();

        id = ingredient->getId();

        accept();
        return;
    }
    QMessageBox::warning(this,"nameNot uniq","cant update : check name is unique or not");
    ui->lineEditName->setFocus();
    ui->lineEditName->setStyleSheet("border: 1px solid red;");

    return ;
}

//cancle
void AddIngredientDialog::on_pushButton_2_clicked()
{
    reject();
}


void AddIngredientDialog::on_chooseImageBtn_clicked()
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
    in.setImagePath(filePath);
    updateImage();
}

void AddIngredientDialog::enableWeightPerUnitLineEdit(int index)
{
    ui->lineEditweightPer->setEnabled(
        index == static_cast<int>(Ingredient::Unit::Piece)
        );
}

