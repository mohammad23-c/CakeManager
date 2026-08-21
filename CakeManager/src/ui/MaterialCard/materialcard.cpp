#include "MaterialCard.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QSizePolicy>

MaterialCard::MaterialCard(
    qint64 ingredientId,
    const QString& name,
    const QString& imagePath,
    QWidget *parent
    )
    : QWidget(parent),
    m_ingredientId(ingredientId),
    m_name(name),
    m_imagePath(imagePath),
    m_imageLabel(new QLabel(this)),
    m_nameLabel(new QLabel(this)),
    m_layout(new QVBoxLayout(this))
{
    setMinimumSize(220, 280);
    setMaximumSize(220, 280);

    m_imageLabel->setMinimumSize(200, 220);
    m_imageLabel->setMaximumSize(200, 220);

    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setScaledContents(false);

    m_nameLabel->setAlignment(Qt::AlignCenter);

    m_layout->setContentsMargins(10, 10, 10, 10);
    m_layout->setSpacing(10);

    m_layout->addWidget(m_imageLabel);
    m_layout->addWidget(m_nameLabel);

    updateImage();
    updateName();
}

MaterialCard::~MaterialCard()
{
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
    m_nameLabel->setText(m_name);
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
        m_imageLabel->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );

    m_imageLabel->setPixmap(pixmap);
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