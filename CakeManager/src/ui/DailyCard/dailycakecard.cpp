#include "dailycakecard.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QMouseEvent>

DailyCakeCard::DailyCakeCard(
    qint64 cakeId,
    double quantity,
    AppManager& appManager,
    QWidget* parent
    )
    : QWidget(parent),
    m_appManager(appManager),
    m_nameLabel(new QLabel(this)),
    m_quantityLabel(new QLabel(this)),
    m_layout(new QHBoxLayout(this)),
    m_cakeId(cakeId),
    m_quantity(quantity)
{
    setMinimumHeight(50);
    setMaximumHeight(50);

    m_nameLabel->setAlignment(
        Qt::AlignLeft | Qt::AlignVCenter
        );

    m_quantityLabel->setAlignment(
        Qt::AlignRight | Qt::AlignVCenter
        );

    m_layout->setContentsMargins(
        15, 5, 15, 5
        );

    m_layout->setSpacing(10);

    m_layout->addWidget(
        m_nameLabel
        );

    m_layout->addStretch();

    m_layout->addWidget(
        m_quantityLabel
        );

    // Find cake and get its name
    auto cake = m_appManager.findCake(m_cakeId);

    if (cake.has_value())
    {
        m_name = cake->getName();
    }
    else
    {
        m_name = "Unknown Cake";
    }

    updateCard(
        m_name,
        m_quantity
        );
}

DailyCakeCard::~DailyCakeCard()
{
}

// =========================================
// Cake ID
// =========================================

qint64 DailyCakeCard::getCakeId() const
{
    return m_cakeId;
}

void DailyCakeCard::setCakeId(qint64 cakeId)
{
    m_cakeId = cakeId;
}

// =========================================
// Quantity
// =========================================

double DailyCakeCard::getQuantity() const
{
    return m_quantity;
}

void DailyCakeCard::setQuantity(double quantity)
{
    m_quantity = quantity;

    m_quantityLabel->setText(
        QString::number(m_quantity)
        );
}

// =========================================
// Update Card
// =========================================

void DailyCakeCard::updateCard(
    const QString& name,
    double quantity
    )
{
    m_name = name;
    m_quantity = quantity;

    m_nameLabel->setText(
        m_name
        );

    m_quantityLabel->setText(
        QString::number(m_quantity)
        );
}

// =========================================
// Mouse Click
// =========================================

void DailyCakeCard::mousePressEvent(
    QMouseEvent* event
    )
{
    if (event->button() == Qt::LeftButton)
    {
        emit clicked(m_cakeId);
    }

    QWidget::mousePressEvent(event);
}