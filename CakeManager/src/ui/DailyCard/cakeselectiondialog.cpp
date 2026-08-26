#include "cakeselectiondialog.h"
#include "ui_cakeselectiondialog.h"

CakeSelectionDialog::CakeSelectionDialog(
    AppManager& appManager,
    QWidget* parent
    )
    : QDialog(parent),
    ui(new Ui::CakeSelectionDialog),
    m_appManager(appManager),
    m_cakeScrollArea(nullptr),
    m_cakeContent(nullptr),
    m_cakeGrid(nullptr),
    m_selectedCakeId(-1)
{
    ui->setupUi(this);

    setWindowTitle("Select Cake");

    // =========================================
    // Main Layout
    // =========================================

    auto* mainLayout =
        new QVBoxLayout(this);

    mainLayout->setContentsMargins(
        0, 0, 0, 0
        );

    mainLayout->setSpacing(0);

    // =========================================
    // Scroll Area
    // =========================================

    m_cakeScrollArea =
        new QScrollArea(this);

    m_cakeScrollArea->setWidgetResizable(true);

    m_cakeScrollArea->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAsNeeded
        );

    m_cakeScrollArea->setVerticalScrollBarPolicy(
        Qt::ScrollBarAsNeeded
        );

    // =========================================
    // Content Widget
    // =========================================

    m_cakeContent =
        new QWidget();

    // =========================================
    // Grid
    // =========================================

    m_cakeGrid =
        new QGridLayout(m_cakeContent);

    m_cakeGrid->setContentsMargins(
        15, 15, 15, 15
        );

    m_cakeGrid->setHorizontalSpacing(15);
    m_cakeGrid->setVerticalSpacing(15);

    // =========================================
    // Set Scroll Content
    // =========================================

    m_cakeScrollArea->setWidget(
        m_cakeContent
        );

    mainLayout->addWidget(
        m_cakeScrollArea
        );

    // =========================================
    // Load Cakes
    // =========================================
    loadCakes();
}

CakeSelectionDialog::~CakeSelectionDialog()
{
    delete ui;
}

// =========================================
// Get Selected Cake
// =========================================

qint64 CakeSelectionDialog::getSelectedCakeId() const
{
    return m_selectedCakeId;
}

// =========================================
// Load Cakes
// =========================================

void CakeSelectionDialog::loadCakes()
{
    auto cakes =
        m_appManager.getCakes();

    for (const auto& cake : cakes)
    {
        loadCakeCard(
            cake.getId()
            );
    }
}

// =========================================
// Load One Cake
// =========================================

void CakeSelectionDialog::loadCakeCard(
    qint64 cakeId
    )
{
    auto cake =
        m_appManager.findCake(cakeId);

    if (!cake.has_value())
    {
        return;
    }

    // =========================================
    // Container
    // =========================================

    auto* container =
        new QWidget(m_cakeContent);

    auto* layout =
        new QVBoxLayout(container);

    layout->setContentsMargins(
        10, 10, 10, 10
        );

    layout->setSpacing(8);

    // =========================================
    // Cake Card
    // =========================================

    auto* card =
        new CakeCard(
            cake->getId(),
            cake->getName(),
            cake->getImagePath(),
            container
            );

    // =========================================
    // Radio Button
    // =========================================

    auto* selectButton =
        new QRadioButton(
            "Select",
            container
            );

    // =========================================
    // Add To Layout
    // =========================================

    layout->addWidget(card);

    layout->addWidget(
        selectButton,
        0,
        Qt::AlignCenter
        );

    // =========================================
    // Initial Style
    // =========================================

    container->setStyleSheet(
        "QWidget {"
        "border: 1px solid #555;"
        "border-radius: 8px;"
        "}"
        );

    // =========================================
    // Store Item
    // =========================================

    CakeSelectionItem item;

    item.card = card;
    item.selectButton = selectButton;
    item.container = container;

    m_cakeItems[cakeId] = item;

    // =========================================
    // Radio Button Connection
    // =========================================

    connect(
        selectButton,
        &QRadioButton::toggled,
        this,
        [this, cakeId](bool checked)
        {
            if (checked)
            {
                selectCake(cakeId);
            }
        }
        );

    // =========================================
    // Grid Position
    // =========================================

    int index =
        m_cakeGrid->count();

    int row =
        index / 4;

    int column =
        index % 4;

    m_cakeGrid->addWidget(
        container,
        row,
        column
        );
}

// =========================================
// Check Selected Cake
// =========================================

bool CakeSelectionDialog::isCakeSelected(
    qint64 cakeId
    ) const
{
    return m_selectedCakeId == cakeId;
}

// =========================================
// Select Cake
// =========================================

void CakeSelectionDialog::selectCake(
    qint64 cakeId
    )
{
    // =========================================
    // Deselect Previous Cake
    // =========================================

    if (m_selectedCakeId != -1 &&
        m_selectedCakeId != cakeId)
    {
        auto previous =
            m_cakeItems.find(
                m_selectedCakeId
                );

        if (previous != m_cakeItems.end())
        {
            previous->second.selectButton
                ->blockSignals(true);

            previous->second.selectButton
                ->setChecked(false);

            previous->second.selectButton
                ->blockSignals(false);

            previous->second.container
                ->setStyleSheet(
                    "QWidget {"
                    "border: 1px solid #555;"
                    "border-radius: 8px;"
                    "}"
                    );
        }
    }

    // =========================================
    // Set New Selection
    // =========================================

    m_selectedCakeId = cakeId;

    auto item =
        m_cakeItems.find(cakeId);

    if (item == m_cakeItems.end())
    {
        return;
    }

    item->second.container
        ->setStyleSheet(
            "QWidget {"
            "border: 2px solid #D4AF37;"
            "border-radius: 8px;"
            "}"
            );
}

// =========================================
// Deselect Cake
// =========================================

void CakeSelectionDialog::deselectCake(
    qint64 cakeId
    )
{
    if (m_selectedCakeId != cakeId)
    {
        return;
    }

    m_selectedCakeId = -1;

    auto item =
        m_cakeItems.find(cakeId);

    if (item != m_cakeItems.end())
    {
        item->second.container
            ->setStyleSheet(
                "QWidget {"
                "border: 1px solid #555;"
                "border-radius: 8px;"
                "}"
                );
    }
}