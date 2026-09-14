#include "addingredientdialog.h"
#include "ui_addingredientdialog.h"
#include <QMessageBox>
#include <QInputDialog>
#include "../../utils/validation/inputvalidator.h"

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
    connect(
         ui->comboBoxunit,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &AddIngredientDialog::setLabelUnit
        );
    in = Ingredient(
        0,
        "",
        Ingredient::Unit::Kilogram,
        0,
        0,
        ":/defultPic/images.png"
        );

    id=0;
    ui->showUnit->setText(Ingredient::unitToString(Ingredient::indexToUnit(1)));
    updateImage();

}

void AddIngredientDialog::updateImage()
{
    QPixmap pixmap;
    if (!in.getImagePath().isEmpty() && QFile::exists(in.getImagePath()))
    {
        pixmap.load(in.getImagePath());
    }else{
        pixmap.load(":/defultPic/images.png");
    }
    pixmap = pixmap.scaled(
        ui->labelPicture->size(),
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
    ui->labelPicture->setPixmap(roundedPixmap);
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
    // Validate name
    if (!InputValidator::validateLineEdit(
            ui->lineEditName,
            InputValidator::InputType::NoType))
    {
        return;
    }

    // Validate price
    if (!InputValidator::validateLineEdit(
            ui->lineEditPrice,
            InputValidator::InputType::Double))
    {
        return;
    }

    // Validate inventory
    if (!InputValidator::validateLineEdit(
            ui->lineEditInventoryWeight,
            InputValidator::InputType::Double))
    {
        return;
    }

    // Validate weight per unit only for Piece
    if (
        static_cast<Ingredient::Unit>(
            ui->comboBoxunit->currentIndex()
            ) == Ingredient::Unit::Piece
        )
    {
        if (!InputValidator::validateLineEdit(
                ui->lineEditweightPer,
                InputValidator::InputType::Integer))
        {
            return;
        }
    }

    // =========================
    // Create Ingredient
    // =========================

    in.setName(
        ui->lineEditName->text()
        );

    in.setPricePerUnit(
        ui->lineEditPrice->text().toDouble()
        );
    //find unit
    Ingredient::Unit u;
    switch (ui->comboBoxunit->currentIndex()) {
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

    in.setUnit(
        u
        );

    in.setWeightPerUnit(0);

    if (in.getUnit() == Ingredient::Unit::Piece)
    {
        in.setWeightPerUnit(
            ui->lineEditweightPer->text().toInt()
            );
    }

    double inventoryQuantity =
        ui->lineEditInventoryWeight->text().toDouble();

    // =========================
    // Add Ingredient
    // =========================

    if (m_appManager.addIngredient(in))
    {
        m_appManager.ingredientSave();

        auto ingredient =
            m_appManager.findIngredient(in.getName());

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

    // =========================
    // Duplicate name
    // =========================

    QMessageBox::warning(
        this,
        "Name Not Unique",
        "An ingredient with this name already exists."
        );

    ui->lineEditName->setFocus();
    ui->lineEditName->setStyleSheet(
        "border: 1px solid red;"
        );
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

void AddIngredientDialog::setLabelUnit(int index)
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


void AddIngredientDialog::on_tabdilVahed_clicked()
{
    // ========================================
    // 1. Determine ingredient's native unit
    // ========================================

    Ingredient::Unit ingredientUnit;

    switch (ui->comboBoxunit->currentIndex())
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

    ui->lineEditPrice->setText(
        QString::number(
            finalPrice,
            'f',
            0
            )
        );
}

