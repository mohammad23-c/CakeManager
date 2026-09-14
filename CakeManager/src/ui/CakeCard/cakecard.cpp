#include "cakecard.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QSizePolicy>
#include <QPainter>
#include <QPainterPath>

CakeCard::CakeCard(
    qint64 cakeId,
    const QString& name,
    const QString& imagePath,
    QWidget *parent
    )
    : QWidget(parent),
    m_cakeId(cakeId),
    m_name(name),
    m_imagePath(imagePath),
    m_cardWidget(new QWidget(this)),
    m_imageLabel(new QLabel(m_cardWidget)),
    m_nameLabel(new QLabel(m_cardWidget)),
    m_layout(new QVBoxLayout(this)),
    m_cardLayout(new QVBoxLayout(m_cardWidget))
{
    m_cardWidget->setObjectName("cakeCardWidget");

    // setStyleSheet(
    //     "background-color: #3A2920;"
    //     "border: 2px solid #B8863B;"
    //     "border-radius: 16px;"
    //     );

    // m_imageLabel->setStyleSheet(
    //     "background-color: #25272D;"
    //     "border: 2px solid #B8863B;"
    //     "border-radius: 12px;"
    //     );

    // m_nameLabel->setStyleSheet(
    //     "background-color: transparent;"
    //     "border: none;"
    //     "color: #FFFFFF;"
    //     "font-weight: bold;"
    //     );
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
    // Layout
    // =========================================

    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addWidget(m_cardWidget);
    /////////////////////////////////////////////

    m_cardLayout->setContentsMargins(10, 10, 10, 10);
    m_cardLayout->setSpacing(10);

    m_cardLayout->addWidget(m_imageLabel);
    m_cardLayout->addWidget(m_nameLabel);

    // =========================================
    // Initial UI
    // =========================================

    updateImage();
    updateName();
}

CakeCard::~CakeCard()
{
}

// =========================================
// Cake ID
// =========================================

qint64 CakeCard::getCakeId() const
{
    return m_cakeId;
}

void CakeCard::setCakeId(qint64 cakeId)
{
    m_cakeId = cakeId;
}

// =========================================
// Name
// =========================================

QString CakeCard::getName() const
{
    return m_name;
}

void CakeCard::setName(const QString& name)
{
    m_name = name;
    updateName();
}

void CakeCard::updateName()
{
    m_nameLabel->setText(m_name);
}

// =========================================
// Image Path
// =========================================

QString CakeCard::getImagePath() const
{
    return m_imagePath;
}

void CakeCard::setImagePath(const QString& imagePath)
{
    m_imagePath = imagePath;
    updateImage();
}

void CakeCard::updateImage()
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

    m_imageLabel->setPixmap(roundedPixmap);
    qDebug() << "IMAGE 15";

}

// =========================================
// Update Card
// =========================================

void CakeCard::updateCard(
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

void CakeCard::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit clicked(m_cakeId);
    }

    QWidget::mousePressEvent(event);
}

// =========================================
// Resize Event
// =========================================

void CakeCard::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    updateImage();
}