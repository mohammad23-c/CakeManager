#ifndef INGREDIENTSELECTIONDIALOG_H
#define INGREDIENTSELECTIONDIALOG_H

#include <QDialog>
#include <QRadioButton>
#include <QScrollArea>
#include <QGridLayout>
#include <QVBoxLayout>

#include <unordered_map>
#include <vector>

#include "../../managers/appmanager.h"
#include "../MaterialCard/materialcard.h"
#include <QLineEdit>

namespace Ui {
class IngredientSelectionDialog;
}

struct IngredientSelectionItem
{
    MaterialCard* card;
    QRadioButton* selectButton;
    QWidget* container;
};

class IngredientSelectionDialog : public QDialog
{
    Q_OBJECT

public:

    explicit IngredientSelectionDialog(
        AppManager& appManager,
        std::vector<CakeIngredient>& cakeIngredients,
        QWidget* parent = nullptr
        );

    // =========================================
    // Search
    // =========================================

    std::vector<qint64> findIngredientsByName(
        const QString& searchText
        ) const;


    void clearIngredientGrid();


    void loadIngredientCards(
        const std::vector<qint64>& ingredientIds
        );



    ~IngredientSelectionDialog();

private slots:

    void onSearchTextChanged(
        const QString& text
        );

private:

    // =========================================
    // Load Ingredients
    // =========================================

    void loadIngredients();

    void loadIngredientCard(qint64 ingredientId);

    // =========================================
    // Check Ingredient
    // =========================================

    bool isIngredientSelected(
        qint64 ingredientId
        ) const;

    // =========================================
    // Ingredient Selection
    // =========================================

    void selectIngredient(
        qint64 ingredientId
        );

    void deselectIngredient(
        qint64 ingredientId
        );

    // =========================================
    // UI
    // =========================================

    Ui::IngredientSelectionDialog* ui;

    AppManager& m_appManager;

    std::vector<CakeIngredient>& m_cakeIngredients;

    // =========================================
    // Scroll Area
    // =========================================

    QScrollArea* m_ingredientScrollArea;

    QWidget* m_ingredientContent;

    QGridLayout* m_ingredientGrid;

    // =========================================
    // Ingredient Items
    // =========================================

    std::unordered_map<
        qint64,
        IngredientSelectionItem
        > m_ingredientItems;

    // Search
    QLineEdit* m_searchLineEdit;
};

#endif // INGREDIENTSELECTIONDIALOG_H