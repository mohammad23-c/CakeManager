#include <cassert>
#include <QCoreApplication>
#include <QFile>

#include "../src/managers/appmanager.h"
#include "../src/database/DatabaseManager.h"
#include "../src/models/ingredient.h"
#include "../src/models/cake.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // =========================================
    // Test Database
    // =========================================

    const QString databaseName =
        "D:/CppAllProject/QT/CakeManager/CakeManager/tests/test_app_manager.db";

    // Delete old test database
    QFile::remove(databaseName);

    // =========================================
    // Create Database
    // =========================================

    DatabaseManager database(databaseName);

    assert(database.openDatabase());
    assert(database.createTables());

    // =========================================
    // Add Ingredients
    // Database generates IDs
    // =========================================

    Ingredient flour(
        0,
        "Flour",
        Ingredient::Unit::Kg,
        100000,
        ""
        );

    Ingredient sugar(
        0,
        "Sugar",
        Ingredient::Unit::Kg,
        80000,
        ""
        );

    Ingredient chocolate(
        0,
        "Chocolate",
        Ingredient::Unit::Kg,
        250000,
        ""
        );

    assert(database.addIngredient(flour));
    assert(database.addIngredient(sugar));
    assert(database.addIngredient(chocolate));

    // =========================================
    // Add Cakes
    // Database generates IDs
    // =========================================

    std::vector<CakeIngredient> chocolateCakeIngredients =
        {
            {1, 1.0},
            {3, 0.5}
        };

    Cake chocolateCake(
        0,
        "Chocolate Cake",
        20.0,
        chocolateCakeIngredients
        );

    std::vector<CakeIngredient> sugarCakeIngredients =
        {
            {1, 0.8},
            {2, 0.4}
        };

    Cake sugarCake(
        0,
        "Sugar Cake",
        15.0,
        sugarCakeIngredients
        );

    assert(database.addCake(chocolateCake));
    assert(database.addCake(sugarCake));

    // =========================================
    // Create AppManager
    // =========================================

    AppManager manager(databaseName);

    // =========================================
    // Initialize Test
    // =========================================

    assert(manager.initialize());

    // =========================================
    // Check Ingredients
    // =========================================

    assert(manager.containsIngredient(1));
    assert(manager.containsIngredient(2));
    assert(manager.containsIngredient(3));

    // =========================================
    // Check Cakes
    // =========================================

    assert(manager.containsCake(1));
    assert(manager.containsCake(2));

    // =========================================
    // No Unsaved Changes After Initialize
    // =========================================

    assert(!manager.hasUnsavedChanges());

    return 0;
}