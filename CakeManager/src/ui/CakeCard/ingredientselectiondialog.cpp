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
    m_ingredientGrid(nullptr),
    m_searchLineEdit(nullptr)
{
    setWindowTitle("Select Ingredients");

    // Dialog Size
    resize(
        900,
        700
        );


    // =========================================
    // Main Layout
    // =========================================

    auto* mainLayout =
        new QVBoxLayout(this);

    mainLayout->setContentsMargins(
        0,
        0,
        0,
        0
        );

    mainLayout->setSpacing(5);


    // =========================================
    // Search Line Edit
    // =========================================

    m_searchLineEdit =
        new QLineEdit(this);

    m_searchLineEdit->setPlaceholderText(
        "Search..."
        );

    m_searchLineEdit->setFixedHeight(
        40
        );

    connect(
        m_searchLineEdit,
        &QLineEdit::textChanged,
        this,
        &IngredientSelectionDialog::onSearchTextChanged
        );

    auto* searchLayout = new QHBoxLayout;

    searchLayout->addWidget(
        m_searchLineEdit,
        3
        );

    searchLayout->addStretch(
        1
        );

    mainLayout->addLayout(
        searchLayout
        );


    // =========================================
    // Scroll Area
    // =========================================

    m_ingredientScrollArea =
        new QScrollArea(this);

    m_ingredientScrollArea->setWidgetResizable(
        true
        );

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
        new QGridLayout(
            m_ingredientContent
            );

    m_ingredientGrid->setContentsMargins(
        15,
        15,
        15,
        15
        );

    m_ingredientGrid->setHorizontalSpacing(
        15
        );

    m_ingredientGrid->setVerticalSpacing(
        15
        );

    m_ingredientGrid->setAlignment(
        Qt::AlignTop
        );
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

std::vector<qint64>
IngredientSelectionDialog::findIngredientsByName(
    const QString& searchText
    ) const
{
    std::vector<qint64> result;

    QString text =
        searchText.trimmed()
            .toLower();


    for (const auto& pair : m_ingredientItems)
    {
        qint64 id = pair.first;

        auto ingredient =
            m_appManager.findIngredient(id);


        if (!ingredient.has_value())
        {
            continue;
        }


        QString name =
            ingredient->getName()
                .toLower();


        if (text.isEmpty() ||
            name.contains(text))
        {
            result.push_back(id);
        }
    }

    return result;
}

void IngredientSelectionDialog::clearIngredientGrid()
{
    while(m_ingredientGrid->count() > 0)
    {
        QLayoutItem* item =
            m_ingredientGrid->takeAt(0);

        if(item->widget())
        {
            item->widget()->setParent(nullptr);
        }

        delete item;
    }
}

void IngredientSelectionDialog::loadIngredientCards(
    const std::vector<qint64>& ingredientIds
    )
{
    for(qint64 id : ingredientIds)
    {
        auto item =
            m_ingredientItems.find(id);


        if(item == m_ingredientItems.end())
        {
            continue;
        }


        int index =
            m_ingredientGrid->count();


        int row =
            index / 4;


        int column =
            index % 4;


        m_ingredientGrid->addWidget(
            item->second.container,
            row,
            column
            );
    }
}

IngredientSelectionDialog::~IngredientSelectionDialog()
{
}

void IngredientSelectionDialog::onSearchTextChanged(
    const QString& text
    )
{
    auto ids =
        findIngredientsByName(text);


    clearIngredientGrid();


    loadIngredientCards(ids);
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

    container->setSizePolicy(
        QSizePolicy::Fixed,
        QSizePolicy::Fixed
        );

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

    card->setSizePolicy(
        QSizePolicy::Preferred,
        QSizePolicy::Fixed
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

void IngredientSelectionDialog::selectIngredient(qint64 ingredientId)
{
    if (isIngredientSelected(ingredientId))
    {
        return;
    }

    auto ingredient =
        m_appManager.findIngredient(ingredientId);

    if (!ingredient.has_value())
    {
        return;
    }

    // --------------------------------
    // Step 1: Available units
    // --------------------------------

    QStringList units;

    units << "Gram"
          << "Kilogram";

    if (ingredient->getUnit() == Ingredient::Unit::Piece)
    {
        units << "Piece";
    }
    else
    {
        // For Gram/Kilogram ingredients,
        // Piece is also allowed.
        units << "Piece";
    }

    bool ok = false;

    QString selectedUnit =
        QInputDialog::getItem(
            this,
            "واحد مقدار",
            "واحد مقدار را انتخاب کنید:",
            units,
            0,
            false,
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

    // --------------------------------
    // Step 2: Convert text to enum
    // --------------------------------

    Ingredient::Unit inputUnit;

    if (selectedUnit == "Gram")
    {
        inputUnit = Ingredient::Unit::Gram;
    }
    else if (selectedUnit == "Kilogram")
    {
        inputUnit = Ingredient::Unit::Kilogram;
    }
    else
    {
        inputUnit = Ingredient::Unit::Piece;
    }

    // --------------------------------
    // Step 3: Get quantity
    // --------------------------------

    double quantity =
        QInputDialog::getDouble(
            this,
            "مقدار ماده",
            "مقدار را وارد کنید (" + selectedUnit + "):",
            0.0,
            0.0,
            1000000.0,
            3,
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

    // --------------------------------
    // Step 4: Convert to native unit
    // --------------------------------

    Ingredient::Unit ingredientUnit =
        ingredient->getUnit();

    double finalQuantity = quantity;

    // ========================================
    // Native unit = Piece
    // ========================================

    if (ingredientUnit == Ingredient::Unit::Piece)
    {
        qint64 weightPerUnit =
            ingredient->getWeightPerUnit();

        if (weightPerUnit <= 0)
        {
            QMessageBox::warning(
                this,
                "ماده نامعتبر",
                "وزن هر عدد برای این ماده مشخص نشده است."
                );

            auto item =
                m_ingredientItems.find(ingredientId);

            if (item != m_ingredientItems.end())
            {
                item->second.selectButton->setChecked(false);
            }

            return;
        }

        if (inputUnit == Ingredient::Unit::Piece)
        {
            // Piece -> Piece

            finalQuantity = quantity;
        }
        else if (inputUnit == Ingredient::Unit::Gram)
        {
            // Gram -> Piece

            finalQuantity =
                quantity / weightPerUnit;
        }
        else if (inputUnit == Ingredient::Unit::Kilogram)
        {
            // Kilogram -> Gram -> Piece

            finalQuantity =
                (quantity * 1000.0)
                / weightPerUnit;
        }
    }

    // ========================================
    // Native unit = Gram
    // ========================================

    else if (ingredientUnit == Ingredient::Unit::Gram)
    {
        if (inputUnit == Ingredient::Unit::Gram)
        {
            // Gram -> Gram

            finalQuantity = quantity;
        }
        else if (inputUnit == Ingredient::Unit::Kilogram)
        {
            // Kilogram -> Gram

            finalQuantity =
                quantity * 1000.0;
        }
        else if (inputUnit == Ingredient::Unit::Piece)
        {
            // Ask weight of one piece

            double weightPerPiece =
                QInputDialog::getDouble(
                    this,
                    "وزن هر عدد",
                    "وزن هر عدد چند گرم است؟",
                    1.0,
                    0.001,
                    1000000.0,
                    3,
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

            // Piece -> Gram

            finalQuantity =
                quantity * weightPerPiece;
        }
    }

    // ========================================
    // Native unit = Kilogram
    // ========================================

    else if (ingredientUnit == Ingredient::Unit::Kilogram)
    {
        if (inputUnit == Ingredient::Unit::Kilogram)
        {
            // Kilogram -> Kilogram

            finalQuantity = quantity;
        }
        else if (inputUnit == Ingredient::Unit::Gram)
        {
            // Gram -> Kilogram

            finalQuantity =
                quantity / 1000.0;
        }
        else if (inputUnit == Ingredient::Unit::Piece)
        {
            // Ask weight of one piece

            double weightPerPiece =
                QInputDialog::getDouble(
                    this,
                    "وزن هر عدد",
                    "وزن هر عدد چند گرم است؟",
                    1.0,
                    0.001,
                    1000000.0,
                    3,
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

            // Piece -> Gram -> Kilogram

            finalQuantity =
                (quantity * weightPerPiece)
                / 1000.0;
        }
    }

    // --------------------------------
    // Step 5: Create CakeIngredient
    // --------------------------------

    CakeIngredient cakeIngredient;

    cakeIngredient.ingredientId =
        ingredientId;

    cakeIngredient.quantity =
        finalQuantity;

    m_cakeIngredients.push_back(
        cakeIngredient
        );

    // --------------------------------
    // Step 6: Update UI
    // --------------------------------

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