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
    m_cardWidget(new QWidget(this)),
    m_nameLabel(new QLabel(m_cardWidget)),
    m_quantityLabel(new QLabel(m_cardWidget)),
    m_layout(new QHBoxLayout(this)),
    m_cardLayout(new QHBoxLayout(m_cardWidget)),
    m_cakeId(cakeId),
    m_quantity(quantity)
{
    m_cardWidget->setObjectName("dailyCakeCardWidget");
    qDebug() << m_cardWidget->objectName();
    // =========================================
    // Card Widget Style
    // =========================================

    // m_cardWidget->setStyleSheet(
    //     "background-color: #5A4638;"
    //     "border: none;"
    //     "border-radius: 14px;"
    //     );

    // =========================================
    // Labels
    // =========================================

    // m_nameLabel->setStyleSheet(
    //     "background: transparent;"
    //     "border: none;"
    //     "color: #FFFFFF;"
    //     "font-weight: bold;"
    //     );

    // m_quantityLabel->setStyleSheet(
    //     "background: transparent;"
    //     "border: none;"
    //     "color: #E6C15A;"
    //     "font-weight: bold;"
    //     );

    // =========================================
    // Card Size
    // =========================================

    setMinimumHeight(50);
    setMaximumHeight(50);

    // =========================================
    // Main Layout
    // =========================================

    m_layout->setContentsMargins(
        0, 0, 0, 0
        );

    m_layout->addWidget(
        m_cardWidget
        );

    // =========================================
    // Card Layout
    // =========================================

    m_cardLayout->setContentsMargins(
        15, 5, 15, 5
        );

    m_cardLayout->setSpacing(10);

    m_nameLabel->setAlignment(
        Qt::AlignLeft | Qt::AlignVCenter
        );

    m_quantityLabel->setAlignment(
        Qt::AlignRight | Qt::AlignVCenter
        );

    m_cardLayout->addWidget(
        m_nameLabel
        );

    m_cardLayout->addStretch();

    m_cardLayout->addWidget(
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
