#include "MaterialCard.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>
#include <QPainterPath>

MaterialCard::MaterialCard(
    qint64 ingredientId,
    const QString& name,
    const QString& imagePath,
    QWidget *parent
    )
    : QWidget(parent),
    m_cardWidget(new QWidget(this)),
    m_imageLabel(new QLabel(m_cardWidget)),
    m_nameLabel(new QLabel(m_cardWidget)),
    m_layout(new QVBoxLayout(this)),
    m_cardLayout(new QVBoxLayout(m_cardWidget)),
    m_ingredientId(ingredientId),
    m_name(name),
    m_imagePath(imagePath)
{
    qDebug() << "MATERIAL START";

    // =========================================
    // Object Names
    // =========================================

    m_cardWidget->setObjectName("materialCardWidget");
    m_imageLabel->setObjectName("materialCardImage");
    m_nameLabel->setObjectName("materialCardName");

    // =========================================
    // Card Size
    // =========================================

    setMinimumSize(220, 280);
    setMaximumSize(220, 280);

    // =========================================
    // Image
    // =========================================

    m_imageLabel->setMinimumSize(200, 220);
    m_imageLabel->setMaximumSize(200, 220);

    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setScaledContents(false);

    // =========================================
    // Name
    // =========================================

    m_nameLabel->setAlignment(Qt::AlignCenter);

    // =========================================
    // Main Layout
    // =========================================

    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addWidget(m_cardWidget);

    // =========================================
    // Card Layout
    // =========================================

    m_cardLayout->setContentsMargins(10, 10, 10, 10);
    m_cardLayout->setSpacing(10);

    m_cardLayout->addWidget(m_imageLabel);
    m_cardLayout->addWidget(m_nameLabel);

    // =========================================
    // Initial UI
    // =========================================
    qDebug() << "BEFORE UPDATE IMAGE";
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
    qDebug() << "IMAGE 1";

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
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        );

    // =========================================
    // Round Image Corners
    // =========================================

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

    m_imageLabel->setPixmap(roundedPixmap);

    qDebug() << "IMAGE 15";
}

// =========================================
// Update Card
// =========================================

void MaterialCard::updateCard(
    const QString& name,
    const QString& imagePath
    )
{
    m_name = name;
    m_imagePath = imagePath;

    updateName();
    updateImage();
}

// =========================================
// Mouse Event
// =========================================

void MaterialCard::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit clicked(m_ingredientId);
    }

    QWidget::mousePressEvent(event);
}

// =========================================
// Resize Event
// =========================================

void MaterialCard::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    updateImage();
}