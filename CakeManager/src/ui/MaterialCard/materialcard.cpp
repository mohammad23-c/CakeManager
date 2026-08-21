#include "MaterialCard.h"
#include "ui_MaterialCard.h"

MaterialCard::MaterialCard(
    qint64 ingredientId,
    const QString& name,
    const QString& imagePath,
    QWidget *parent
    )
    : QWidget(parent),
    ui(new Ui::MaterialCard),
    m_ingredientId(ingredientId),
    m_name(name),
    m_imagePath(imagePath)
{
    ui->setupUi(this);
    updateImage();
    updateName();
}

MaterialCard::~MaterialCard()
{
    delete ui;
}

// =========================================
// Ingredient ID
// =========================================

qint64 MaterialCard::getIngredientId() const
{
    return m_ingredientId;
}

void MaterialCard::setIngredientId(qint64 ingredientId)
{
    m_ingredientId = ingredientId;
}

// =========================================
// Name
// =========================================

QString MaterialCard::getName() const
{
    return m_name;
}

void MaterialCard::setName(const QString& name)
{
    m_name = name;
    updateName();
}

void MaterialCard::updateName()
{
    ui->nameLabel->setText(m_name);
}

// =========================================
// Image Path
// =========================================

QString MaterialCard::getImagePath() const
{
    return m_imagePath;
}

void MaterialCard::setImagePath(const QString& imagePath)
{
    m_imagePath = imagePath;
    updateImage();
}

void MaterialCard::updateImage()
{
    QPixmap pixmap;

    if (!m_imagePath.isEmpty() && QFile::exists(m_imagePath))
    {
        pixmap.load(m_imagePath);
    }
    else
    {
        pixmap.load(":/defultPic/images.png");
    }

    pixmap = pixmap.scaled(
        ui->imageLabel->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );

    ui->imageLabel->setPixmap(pixmap);
}

void MaterialCard::updateCard(
    const QString& name,
    const QString& imagePath
    )
{
    this->m_name=name;
    this->m_imagePath=imagePath;
    updateName();
    updateImage();
}
void MaterialCard::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit clicked(m_ingredientId);
    }

    QWidget::mousePressEvent(event);
}

void MaterialCard::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateImage();
}