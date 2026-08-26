#include "ingredientselectiondialog.h"

#include <QLabel>
#include <QInputDialog>
#include <QMessageBox>

IngredientSelectionDialog::IngredientSelectionDialog(
    AppManager& appManager,
    std::vector<CakeIngredient>& cakeIngredients,
    QWidget* parent
    )
    : QDialog(parent),
    m_appManager(appManager),
    m_cakeIngredients(cakeIngredients),
    m_ingredientScrollArea(nullptr),
    m_ingredientContent(nullptr),
    m_ingredientGrid(nullptr)
{
    setWindowTitle("Select Ingredients");

    // =========================================
    // Main Layout
    // =========================================

    auto* mainLayout = new QVBoxLayout(this);

    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // =========================================
    // Scroll Area
    // =========================================

    m_ingredientScrollArea =
        new QScrollArea(this);

    m_ingredientScrollArea->setWidgetResizable(true);

    m_ingredientScrollArea->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAsNeeded
        );

    m_ingredientScrollArea->setVerticalScrollBarPolicy(
        Qt::ScrollBarAsNeeded
        );

    // =========================================
    // Content Widget
    // =========================================

    m_ingredientContent =
        new QWidget();

    // =========================================
    // Grid
    // =========================================

    m_ingredientGrid =
        new QGridLayout(m_ingredientContent);

    m_ingredientGrid->setContentsMargins(
        15, 15, 15, 15
        );

    m_ingredientGrid->setHorizontalSpacing(15);
    m_ingredientGrid->setVerticalSpacing(15);

    // =========================================
    // Set Scroll Content
    // =========================================

    m_ingredientScrollArea->setWidget(
        m_ingredientContent
        );

    mainLayout->addWidget(
        m_ingredientScrollArea
        );

    // =========================================
    // Load Ingredients
    // =========================================

    loadIngredients();
}

IngredientSelectionDialog::~IngredientSelectionDialog()
{
}

// =========================================
// Load Ingredients
// =========================================

void IngredientSelectionDialog::loadIngredients()
{
    auto ingredients =
        m_appManager.getIngredients();

    for (const auto& ingredient : ingredients)
    {
        loadIngredientCard(
            ingredient.getId()
            );
    }
}

// =========================================
// Load One Ingredient
// =========================================

void IngredientSelectionDialog::loadIngredientCard(
    qint64 ingredientId
    )
{
    auto ingredient =
        m_appManager.findIngredient(ingredientId);

    if (!ingredient.has_value())
    {
        return;
    }

    // =========================================
    // Container
    // =========================================

    auto* container =
        new QWidget(m_ingredientContent);

    auto* layout =
        new QVBoxLayout(container);

    layout->setContentsMargins(
        10, 10, 10, 10
        );

    layout->setSpacing(8);

    // =========================================
    // Material Card
    // =========================================

    auto* card =
        new MaterialCard(
            ingredient->getId(),
            ingredient->getName(),
            ingredient->getImagePath(),
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

    selectButton->setChecked(
        isIngredientSelected(
            ingredient->getId()
            )
        );

    // =========================================
    // Add to Layout
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

    if (selectButton->isChecked())
    {
        container->setStyleSheet(
            "QWidget {"
            "border: 2px solid #D4AF37;"
            "border-radius: 8px;"
            "}"
            );
    }
    else
    {
        container->setStyleSheet(
            "QWidget {"
            "border: 1px solid #555;"
            "border-radius: 8px;"
            "}"
            );
    }

    // =========================================
    // Store Item
    // =========================================

    IngredientSelectionItem item;

    item.card = card;
    item.selectButton = selectButton;
    item.container = container;

    m_ingredientItems[
        ingredient->getId()
    ] = item;

    // =========================================
    // Radio Button Connection
    // =========================================

    connect(
        selectButton,
        &QRadioButton::toggled,
        this,
        [this, ingredientId](bool checked)
        {
            if (checked)
            {
                selectIngredient(ingredientId);
            }
            else
            {
                deselectIngredient(ingredientId);
            }
        }
        );

    // =========================================
    // Grid Position
    // =========================================

    int index =
        m_ingredientGrid->count();

    int row =
        index / 4;

    int column =
        index % 4;

    m_ingredientGrid->addWidget(
        container,
        row,
        column
        );
}

// =========================================
// Check Selected
// =========================================

bool IngredientSelectionDialog::isIngredientSelected(
    qint64 ingredientId
    ) const
{
    for (const auto& cakeIngredient : m_cakeIngredients)
    {
        if (cakeIngredient.ingredientId == ingredientId)
        {
            return true;
        }
    }

    return false;
}
// =========================================
// Select Ingredient
// =========================================

void IngredientSelectionDialog::selectIngredient(
    qint64 ingredientId
    )
{
    if (isIngredientSelected(ingredientId))
    {
        return;
    }

    bool ok = false;

    double quantity =
        QInputDialog::getDouble(
            this,
            "Ingredient Quantity",
            "Enter quantity:",
            0.0,
            0.0,
            1000000.0,
            2,
            &ok
            );

    if (!ok)
    {
        auto item =
            m_ingredientItems.find(ingredientId);

        if (item != m_ingredientItems.end())
        {
            item->second.selectButton->setChecked(false);
        }

        return;
    }

    // Create CakeIngredient
    CakeIngredient cakeIngredient;

    cakeIngredient.ingredientId = ingredientId;
    cakeIngredient.quantity = quantity;

    m_cakeIngredients.push_back(
        cakeIngredient
        );

    // Change card appearance
    auto item =
        m_ingredientItems.find(ingredientId);

    if (item != m_ingredientItems.end())
    {
        item->second.container->setStyleSheet(
            "QWidget {"
            "border: 2px solid #D4AF37;"
            "border-radius: 8px;"
            "}"
            );
    }
}

// =========================================
// Deselect Ingredient
// =========================================

void IngredientSelectionDialog::deselectIngredient(
    qint64 ingredientId
    )
{
    for (auto it = m_cakeIngredients.begin();
         it != m_cakeIngredients.end();
         ++it)
    {
        if (it->ingredientId == ingredientId)
        {
            m_cakeIngredients.erase(it);
            break;
        }
    }

    auto item =
        m_ingredientItems.find(ingredientId);

    if (item != m_ingredientItems.end())
    {
        item->second.container->setStyleSheet(
            "QWidget {"
            "border: 1px solid #555;"
            "border-radius: 8px;"
            "}"
            );
    }
}