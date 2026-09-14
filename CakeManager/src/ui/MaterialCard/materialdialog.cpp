#include "materialdialog.h"
#include "ui_materialdialog.h"
#include "../../utils/validation/inputvalidator.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
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
    connect(
        ui->UnitcomboBox,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &MaterialDialog::setLabelUnit
        );
    //find ingredient by id
    auto ingredient = m_appManager.findIngredient(m_ingredientId);
    auto inventory= m_appManager.findInventory(m_ingredientId);

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
    if(!inventory.has_value()){
        QMessageBox::warning(
            this,
            "Error",
            "inventory not found"
            );
    }
    in=new Ingredient(ingredient.value());
    //default
    ui->PricepuLineE->setEnabled(false);
    //setText
    //set show unit
    ui->showUnit->setText(Ingredient::unitToString(Ingredient::indexToUnit(1)));
    //line edit dialog
    if(inventory.has_value())
        ui->lineEditInventory->setText(QString::number(*inventory));
    //name
    ui->NamelineEdit->setText(ingredient->getName());
    //price
    ui->PriceLineEdit->setText(QString::number(ingredient->getPricePerUnit()));
    //unit
    int index;
    switch(ingredient->getUnit()){
    case Ingredient::Unit::Kilogram:
        index=0;
        break;
    case Ingredient::Unit::Gram:
        index=1;
        break;
    case Ingredient::Unit::Piece:
        index=2;
        break;
    default:
        index=0;
    }
    ui->UnitcomboBox->setCurrentIndex(index);
    //weight per unit
    //default zero
    ui->PricepuLineE->setText(QString::number(0));
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
        //set name
    if(!InputValidator::validateLineEdit(ui->NamelineEdit,InputValidator::InputType::NoType)){
        return;
    }in->setName(ui->NamelineEdit->text());
        //set price per unit
    if(!InputValidator::validateLineEdit(ui->PriceLineEdit,InputValidator::InputType::Double)){
        return;
    }in->setPricePerUnit(ui->PriceLineEdit->text().toDouble());
    //find unit
    Ingredient::Unit u;
    switch (ui->UnitcomboBox->currentIndex()) {
    case 0:
        u=Ingredient::Unit::Kilogram;
        break;
    case 1:
        u=Ingredient::Unit::Gram;
        break;
    case 2:
        u=Ingredient::Unit::Piece;
        break;
    default:
        u=Ingredient::Unit::Kilogram;
        break;
    }
    //no check
    in->setUnit(u);
    double weightPerUnit=0;
    if(in->getUnit()==Ingredient::Unit::Piece){
        if(!InputValidator::validateLineEdit(ui->PricepuLineE,InputValidator::InputType::Integer)){
            QMessageBox::warning(this,"warning","weight per unit cant empty");
            return;
        }
        if(!ui->PricepuLineE->text().isEmpty())
            weightPerUnit=ui->PricepuLineE->text().toInt();
    }
    in->setWeightPerUnit(weightPerUnit);

    //image path already set in on_choosePicBtn_clicked
    if(!InputValidator::validateLineEdit(ui->lineEditInventory,InputValidator::InputType::Double))
    {
        return;
    }
    double inventory=ui->lineEditInventory->text().toDouble();
    if(!m_appManager.updateIngredient(*in)){
        QMessageBox::warning(this,"name not unique","please choose another name ");
        ui->NamelineEdit->setFocus();
        ui->NamelineEdit->setStyleSheet("border: 1px solid red;");
        return;
    }
    if(!m_appManager.updateInventory(in->getId(),inventory)){
        QMessageBox::warning(this,"inventory cant update","inventory cant update");
        return;
    }
    m_appManager.inventorySave();
    m_appManager.ingredientSave();
    accept();
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

void MaterialDialog::setLabelUnit(int index)
{
    switch(index){
    case 0:
        ui->showUnit->setText("kilogram");
        break;
    case 1:
        ui->showUnit->setText("gram");
        break;
    case 2:
        ui->showUnit->setText("piece");
        break;
    default:
        ui->showUnit->setText("?");
        break;
    }
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

    if (result == QMessageBox::Yes)
    {
        auto inventoryO=m_appManager.findInventory(m_ingredientId);
        double inventory=0;
        if(!inventoryO.has_value()){
            QMessageBox::warning(this,"inventory not found","inventory not found");
        }else{
            inventory=inventoryO.value();
        }
        if(m_appManager.isIngredientUsedInCake(m_ingredientId)){
            QMessageBox::warning(
                this,
                "Cannot Delete Ingredient",
                "This ingredient is used in a cake and cannot be deleted."
                );

            return;
        }

        bool s = m_appManager.deleteInventory(m_ingredientId);
        if(!s){
            QMessageBox::warning(
                this,
                "Cannot Delete Inventory",
                "its inventory could not be deleted."
                );

            return;
        }
        if(!m_appManager.deleteIngredient(m_ingredientId)){
            QMessageBox::warning(this,
                                 "cant delete ingerdient",
                                 "cant delete ingredient");
            m_appManager.addInventory(m_ingredientId,inventory);
            return;
        }
        emit deleteIng(m_ingredientId);
        m_appManager.ingredientSave();
        m_appManager.inventorySave();

        accept();
    }

    return;
}


void MaterialDialog::on_tabdilVahed_clicked()
{
    // ========================================
    // 1. Determine ingredient's native unit
    // ========================================

    Ingredient::Unit ingredientUnit;

    switch (ui->UnitcomboBox->currentIndex())
    {
    case 0:
        ingredientUnit = Ingredient::Unit::Kilogram;
        break;

    case 1:
        ingredientUnit = Ingredient::Unit::Gram;
        break;

    case 2:
        ingredientUnit = Ingredient::Unit::Piece;
        break;

    default:
        ingredientUnit = Ingredient::Unit::Kilogram;
        break;
    }

    // ========================================
    // 2. Ask user which unit they want to enter
    // ========================================

    QStringList units;

    units << "کیلوگرم"
          << "گرم"
          << "تعداد";

    bool ok = false;

    QString selectedUnit =
        QInputDialog::getItem(
            this,
            "واحد مقدار",
            "مقدار را به چه واحدی می‌خواهید وارد کنید؟",
            units,
            0,
            false,
            &ok
            );

    if (!ok)
    {
        return;
    }

    // ========================================
    // 3. Determine input unit
    // ========================================

    Ingredient::Unit inputUnit;

    if (selectedUnit == "کیلوگرم")
    {
        inputUnit = Ingredient::Unit::Kilogram;
    }
    else if (selectedUnit == "گرم")
    {
        inputUnit = Ingredient::Unit::Gram;
    }
    else
    {
        inputUnit = Ingredient::Unit::Piece;
    }

    // ========================================
    // 4. Ask for quantity
    // ========================================

    QString quantityText;

    if (inputUnit == Ingredient::Unit::Kilogram)
    {
        quantityText = "چند کیلوگرم؟";
    }
    else if (inputUnit == Ingredient::Unit::Gram)
    {
        quantityText = "چند گرم؟";
    }
    else
    {
        quantityText = "چند عدد؟";
    }

    double quantity =
        QInputDialog::getDouble(
            this,
            "مقدار",
            quantityText,
            1.0,
            0.001,
            1000000.0,
            3,
            &ok
            );

    if (!ok)
    {
        return;
    }

    if (quantity <= 0)
    {
        QMessageBox::warning(
            this,
            "خطا",
            "مقدار باید بیشتر از صفر باشد."
            );

        return;
    }

    // ========================================
    // 5. Ask for TOTAL price
    // ========================================

    double totalPrice =
        QInputDialog::getDouble(
            this,
            "قیمت کل",
            "قیمت کل این مقدار را وارد کنید:",
            0.0,
            0.0,
            1000000000000.0,
            0,
            &ok
            );

    if (!ok)
    {
        return;
    }

    if (totalPrice <= 0)
    {
        QMessageBox::warning(
            this,
            "خطا",
            "قیمت باید بیشتر از صفر باشد."
            );

        return;
    }

    // ========================================
    // 6. Calculate price per input unit
    // ========================================

    double pricePerInputUnit =
        totalPrice / quantity;

    // ========================================
    // 7. Check if weight per unit is needed
    // ========================================

    bool needsWeightPerUnit =
        (ingredientUnit == Ingredient::Unit::Piece &&
         inputUnit != Ingredient::Unit::Piece)
        ||
        (ingredientUnit != Ingredient::Unit::Piece &&
         inputUnit == Ingredient::Unit::Piece);

    double weightPerUnit = 0.0;

    if (needsWeightPerUnit)
    {
        bool ok = false;

        weightPerUnit = QInputDialog::getDouble(
            this,
            "وزن هر عدد",
            "وزن هر عدد چند گرم است؟",
            1.0,      // مقدار پیش‌فرض
            0.01,     // حداقل
            100000.0, // حداکثر
            2,        // تعداد رقم اعشار
            &ok
            );

        if (!ok)
        {
            return;
        }

        if (weightPerUnit <= 0)
        {
            QMessageBox::warning(
                this,
                "خطا",
                "وزن هر عدد باید بیشتر از صفر باشد."
                );

            return;
        }
    }

    // ========================================
    // 8. Convert price to ingredient's unit
    // ========================================

    double finalPrice = pricePerInputUnit;

    // ----------------------------------------
    // Native unit = Kilogram
    // ----------------------------------------

    if (ingredientUnit == Ingredient::Unit::Kilogram)
    {
        if (inputUnit == Ingredient::Unit::Kilogram)
        {
            // Kg -> Kg

            finalPrice = pricePerInputUnit;
        }
        else if (inputUnit == Ingredient::Unit::Gram)
        {
            // Gram -> Kg

            finalPrice =
                pricePerInputUnit * 1000.0;
        }
        else if (inputUnit == Ingredient::Unit::Piece)
        {
            // Piece -> Gram -> Kg

            double pricePerGram =
                pricePerInputUnit / weightPerUnit;

            finalPrice =
                pricePerGram * 1000.0;
        }
    }

    // ----------------------------------------
    // Native unit = Gram
    // ----------------------------------------

    else if (ingredientUnit == Ingredient::Unit::Gram)
    {
        if (inputUnit == Ingredient::Unit::Gram)
        {
            // Gram -> Gram

            finalPrice = pricePerInputUnit;
        }
        else if (inputUnit == Ingredient::Unit::Kilogram)
        {
            // Kg -> Gram

            finalPrice =
                pricePerInputUnit / 1000.0;
        }
        else if (inputUnit == Ingredient::Unit::Piece)
        {
            // Piece -> Gram

            finalPrice =
                pricePerInputUnit / weightPerUnit;
        }
    }

    // ----------------------------------------
    // Native unit = Piece
    // ----------------------------------------

    else if (ingredientUnit == Ingredient::Unit::Piece)
    {
        if (inputUnit == Ingredient::Unit::Piece)
        {
            // Piece -> Piece

            finalPrice = pricePerInputUnit;
        }
        else if (inputUnit == Ingredient::Unit::Gram)
        {
            // Gram -> Piece

            finalPrice =
                pricePerInputUnit * weightPerUnit;
        }
        else if (inputUnit == Ingredient::Unit::Kilogram)
        {
            // Kg -> Gram -> Piece

            double pricePerGram =
                pricePerInputUnit / 1000.0;

            finalPrice =
                pricePerGram * weightPerUnit;
        }
    }

    // ========================================
    // 9. Put final price into lineEditPrice
    // ========================================
    ui->PriceLineEdit->setText(
        QString::number(
            finalPrice,
            'f',
            0
            )
        );
}

