#include "addcakedialog.h"
#include "ui_addcakedialog.h"
#include "ingredientselectiondialog.h"
#include <QMessageBox>
#include <QFileDialog>

AddCakeDialog::AddCakeDialog(
    AppManager& appManager,
    QWidget* parent
    )
    : QDialog(parent),
    ui(new Ui::AddCakeDialog),
    m_appManager(appManager),
    m_cakeId(0)
{
    ui->setupUi(this);
}

void AddCakeDialog::updatePic()
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
        ui->labelPicture->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );

    ui->labelPicture->setPixmap(pixmap);
}

qint64 AddCakeDialog::getNewCakeId() const
{
    return m_cakeId;
}

AddCakeDialog::~AddCakeDialog()
{
    delete ui;
}

void AddCakeDialog::on_chooseIngredientsBtn_clicked()
{
    IngredientSelectionDialog dialog(
        m_appManager,
        m_cakeIngredients,
        this
        );

    dialog.exec();
}

void AddCakeDialog::on_addCakeBtn_clicked()
{
    // =========================================
    // Validate Name
    // =========================================

    if (!InputValidator::validateLineEdit(
            ui->lineEditName,
            InputValidator::InputType::String))
    {
        return;
    }

    // =========================================
    // Validate Profit
    // =========================================

    if (!InputValidator::validateLineEdit(
            ui->lineEditProfit,
            InputValidator::InputType::Double))
    {
        return;
    }

    // =========================================
    // Get Values
    // =========================================

    QString name =
        ui->lineEditName->text();

    double profitPercentage =
        ui->lineEditProfit->text().toDouble();

    // =========================================
    // Create Cake
    // =========================================

    Cake cake(
        0,
        name,
        profitPercentage,
        m_cakeIngredients,
        imagePath
        );
    if(m_cakeIngredients.empty())
    {
        QMessageBox::warning(
            this,
            "No Ingredients",
            "Please add at least one ingredient to the cake."
            );
        return;
    }
    // =========================================
    // Add Cake
    // =========================================

    if (m_appManager.addCake(cake))
    {
        m_appManager.cakeSave();
        m_cakeId = m_appManager.findCake(cake.getName())->getId();
        accept();
        return;
    }

    QMessageBox::warning(
        this,
        "Cannot Add Cake",
        "A cake with this name already exists."
        );
}
void AddCakeDialog::on_cancelBtn_clicked()
{
    reject();
}

void AddCakeDialog::on_choosePic_clicked()
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

    imagePath = filePath;

    updatePic();
}

