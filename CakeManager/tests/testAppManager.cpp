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
        Ingredient::Unit::Kilogram,
        100000,
        ""
        );

    Ingredient sugar(
        0,
        "Sugar",
        Ingredient::Unit::Kilogram,
        80000,
        ""
        );

    Ingredient chocolate(
        0,
        "Chocolate",
        Ingredient::Unit::Kilogram,
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


    assert(manager.containsIngredient("Flour"));
    assert(manager.containsIngredient("Sugar"));
    assert(manager.containsIngredient("Chocolate"));
    // =========================================
    // Check Cakes
    // =========================================

    assert(manager.containsCake(1));
    assert(manager.containsCake(2));

    assert(manager.containsCake("Chocolate Cake"));
    assert(manager.containsCake("Sugar Cake"));

    // =========================================
    // Check findIngredient
    // =========================================
    auto ingredients = database.getIngredients();
    qint64 flourId = -1;

    for (const auto& in : ingredients )
    {
        if (in.getName() == "Flour")
        {
            flourId = in.getId();
        }
    }

    assert(flourId != -1);
    auto flour2=manager.findIngredient(flourId);
    assert(flour2.has_value());
    assert(flour2->getName() == "Flour");

    // =========================================
    // Check findCake
    // =========================================

    auto cakes = database.getCakes();

    qint64 chocolateCakeId = -1;

    for (const auto& cake : cakes)
    {
        if (cake.getName() == "Chocolate Cake")
        {
            chocolateCakeId = cake.getId();
        }
    }

    assert(chocolateCakeId != -1);
    auto chocolateCake2 = manager.findCake(chocolateCakeId);

    assert(chocolateCake2.has_value());
    assert(chocolateCake2->getName() == "Chocolate Cake");

    // =========================================
    // Test addIngredient
    // =========================================

    Ingredient milk(
        0,
        "Milk",
        Ingredient::Unit::Kilogram,
        60000,
        ""
        );

    assert(manager.addIngredient(milk));

    assert(manager.containsIngredient("Milk"));

    auto addedMilk = manager.findIngredient(4);

    assert(addedMilk.has_value());
    assert(addedMilk->getName() == "Milk");

    assert(manager.hasUnsavedChanges());

    // =========================================
    // Test duplicate Ingredient
    // =========================================

    Ingredient duplicateMilk(
        0,
        "Milk",
        Ingredient::Unit::Kilogram,
        70000,
        ""
        );

    assert(!manager.addIngredient(duplicateMilk));
    // =========================================
    // Test addCake
    // =========================================

    auto flour3 = manager.findIngredient("Flour");
    auto chocolate3 = manager.findIngredient("Chocolate");

    assert(flour3.has_value());
    assert(chocolate3.has_value());

    qint64 flourId2=flour3->getId();
    qint64 chocolateId2=chocolate3->getId();

    std::vector<CakeIngredient> newCakeIngredients =
        {
            {flourId2, 1.0},
            {chocolateId2, 0.3}
        };

    Cake newCake(
        0,
        "New Chocolate Cake",
        25.0,
        newCakeIngredients
        );

    assert(manager.addCake(newCake));

    assert(manager.containsCake("New Chocolate Cake"));
    // =========================================
    // Test Cake with non-existing Ingredient
    // =========================================

    std::vector<CakeIngredient> invalidIngredients =
        {
            {999999, 2.0}
        };

    Cake invalidCake(
        0,
        "Invalid Cake",
        20.0,
        invalidIngredients
        );

    assert(!manager.addCake(invalidCake));

    assert(!manager.containsCake("Invalid Cake"));
    // =========================================
    // Test duplicate Cake name
    // =========================================

    Cake duplicateCake(
        0,
        "New Chocolate Cake",
        30.0,
        newCakeIngredients
        );

    assert(!manager.addCake(duplicateCake));
    // =========================================
    // Test Ingredient-Cake relation
    // =========================================
    auto choCake = manager.findCake("New Chocolate Cake");
    assert(choCake.has_value());
    qint64 chocolateCakeId2 = choCake->getId();
    assert(
        manager.isIngredientUsedInThisCake(
            flourId2,
            chocolateCakeId2
            )
        );

    assert(
        manager.isIngredientUsedInThisCake(
            chocolateId2,
            chocolateCakeId2
            )
        );
    assert(
        !manager.isIngredientUsedInThisCake(
            999999,
            chocolateCakeId2
            )
        );
    // =========================================
    // Test isIngredientUsedInCake
    // =========================================

    assert(manager.isIngredientUsedInCake(flourId2));
    assert(manager.isIngredientUsedInCake(chocolateId2));

    assert(!manager.isIngredientUsedInCake(999999));

    // =========================================
    // Test getCakesUsingIngredient
    // =========================================

    auto cakesUsingFlour =
        manager.getCakesUsingIngredient(flourId2);

    assert(!cakesUsingFlour.empty());

    bool foundChocolateCake = false;

    for (qint64 id : cakesUsingFlour)
    {
        if (id == chocolateCakeId2)
        {
            foundChocolateCake = true;
            break;
        }
    }

    assert(foundChocolateCake);
    // =========================================
    // Test calculateCakeCost
    // =========================================

    auto cost = manager.calculateCakeCost(chocolateCakeId);

    assert(cost.has_value());

    double expectedCost =
        flour3->getPricePerUnit() * 1.0 +
        chocolate3->getPricePerUnit() * 0.5;

    assert(*cost == expectedCost);
    // =========================================
    // Test calculateCakeFinalPrice
    // =========================================

    auto finalPrice =
        manager.calculateCakeFinalPrice(chocolateCakeId);

    assert(finalPrice.has_value());

    double expectedFinalPrice =
        expectedCost * 1.20;

    assert(*finalPrice == expectedFinalPrice);
    // =========================================
    // Test updateIngredient
    // =========================================

    auto updatedFlour = manager.findIngredient(flourId2);
    assert(updatedFlour.has_value());

    updatedFlour->setPricePerUnit(120000);

    assert(manager.updateIngredient(*updatedFlour));

    auto flourAfterUpdate =
        manager.findIngredient(flourId);

    assert(flourAfterUpdate.has_value());

    assert(
        flourAfterUpdate->getPricePerUnit() == 120000
        );
    // =========================================
    // Test updateCake
    // =========================================

    auto updatedCake =
            manager.findCake(chocolateCakeId);

    assert(updatedCake.has_value());

    updatedCake->setProfitPercentage(30.0);

    assert(manager.updateCake(*updatedCake));

    auto cakeAfterUpdate =
        manager.findCake(chocolateCakeId);

    assert(cakeAfterUpdate.has_value());

    assert(
        cakeAfterUpdate->getProfitPercentage() == 30.0
        );
    // =========================================
    // Test duplicate name on update
    // =========================================

    auto cakeToUpdate =
            manager.findCake(chocolateCakeId);
    assert(cakeToUpdate.has_value());
    cakeToUpdate->setName("New Chocolate Cake");

    assert(!manager.updateCake(*cakeToUpdate));
    // =========================================
    // Test deleteCake
    // =========================================

    auto newCakeFromMemory =
        manager.findCake(chocolateCakeId);

    assert(newCakeFromMemory.has_value());

    assert(manager.deleteCake(chocolateCakeId));

    assert(!manager.containsCake(chocolateCakeId));
    // =========================================
    // Cannot delete used Ingredient
    // =========================================

    assert(manager.isIngredientUsedInCake(flourId2));

    assert(!manager.deleteIngredient(flourId2));

    assert(manager.containsIngredient(flourId2));
    // =========================================
    // Delete unused Ingredient
    // =========================================
    auto milk2=manager.findIngredient("Milk");
    assert(milk2.has_value());

    qint64 milkId=milk2->getId();
    assert(!manager.isIngredientUsedInCake(milkId));

    assert(manager.deleteIngredient(milkId));

    assert(!manager.containsIngredient(milkId));
    // =========================================
    // Test Save
    // =========================================

    assert(manager.hasUnsavedChanges());

    assert(manager.save());

    assert(!manager.hasUnsavedChanges());
    // =========================================
    // Test Load From Database
    // =========================================

    AppManager loadedManager(databaseName);

    assert(loadedManager.initialize());


    // =========================================
    // Check Ingredients
    // =========================================

    assert(loadedManager.containsIngredient(1));
    assert(loadedManager.containsIngredient(2));
    assert(loadedManager.containsIngredient(3));

    assert(loadedManager.containsIngredient("Flour"));
    assert(loadedManager.containsIngredient("Sugar"));
    assert(loadedManager.containsIngredient("Chocolate"));

    assert(!loadedManager.containsIngredient("Milk"));


    // =========================================
    // Check Flour
    // =========================================

    auto flour11 = loadedManager.findIngredient(1);

    assert(flour11.has_value());
    assert(flour11->getName() == "Flour");
    assert(flour11->getPricePerUnit() == 120000);


    // =========================================
    // Check Sugar
    // =========================================

    auto sugar11 = loadedManager.findIngredient(2);

    assert(sugar11.has_value());
    assert(sugar11->getName() == "Sugar");
    assert(sugar11->getPricePerUnit() == 80000);


    // =========================================
    // Check Chocolate
    // =========================================

    auto chocolate11 = loadedManager.findIngredient(3);

    assert(chocolate11.has_value());
    assert(chocolate11->getName() == "Chocolate");
    assert(chocolate11->getPricePerUnit() == 250000);


    // =========================================
    // Check Deleted Milk
    // =========================================

    assert(!loadedManager.findIngredient("Milk").has_value());


    // =========================================
    // Check Cakes
    // =========================================

    assert(loadedManager.containsCake(2));
    assert(loadedManager.containsCake(3));

    assert(loadedManager.containsCake("Sugar Cake"));
    assert(loadedManager.containsCake("New Chocolate Cake"));

    assert(!loadedManager.containsCake("Chocolate Cake"));


    // =========================================
    // Check Sugar Cake
    // =========================================

    auto sugarCake11 =
        loadedManager.findCake(2);

    assert(sugarCake11.has_value());

    assert(sugarCake11->getName() == "Sugar Cake");
    assert(sugarCake11->getProfitPercentage() == 15.0);

    assert(sugarCake11->getIngredients().size() == 2);


    // =========================================
    // Check New Chocolate Cake
    // =========================================

    auto newChocolateCake11 =
        loadedManager.findCake(3);

    assert(newChocolateCake11.has_value());

    assert(newChocolateCake11->getName() == "New Chocolate Cake");
    assert(newChocolateCake11->getProfitPercentage() == 25.0);

    assert(newChocolateCake11->getIngredients().size() == 2);


    // =========================================
    // Check Cake / Ingredient Relations
    // =========================================

    // Sugar Cake
    assert(
        loadedManager.isIngredientUsedInThisCake(
            1,
            2
            )
        );

    assert(
        loadedManager.isIngredientUsedInThisCake(
            2,
            2
            )
        );

    assert(
        !loadedManager.isIngredientUsedInThisCake(
            3,
            2
            )
        );


    // New Chocolate Cake
    assert(
        loadedManager.isIngredientUsedInThisCake(
            1,
            3
            )
        );

    assert(
        loadedManager.isIngredientUsedInThisCake(
            3,
            3
            )
        );

    assert(
        !loadedManager.isIngredientUsedInThisCake(
            2,
            3
            )
        );


    // =========================================
    // Check Ingredient Usage
    // =========================================

    assert(loadedManager.isIngredientUsedInCake(1)); // Flour
    assert(loadedManager.isIngredientUsedInCake(2)); // Sugar
    assert(loadedManager.isIngredientUsedInCake(3)); // Chocolate

    // =========================================
    // Check New Chocolate Cake Ingredients
    // =========================================

    for (const auto& ingredient : newChocolateCake11->getIngredients())
    {
        if (ingredient.ingredientId == 1)
        {
            assert(ingredient.quantity == 1.0);
        }
        else if (ingredient.ingredientId == 3)
        {
            assert(ingredient.quantity == 0.3);
        }
        else
        {
            assert(false);
        }
    }
    // =========================================
    // Check Unsaved Changes
    // =========================================

    assert(!loadedManager.hasUnsavedChanges());
    //====================number of run test successfully==================
    qDebug()<<"test Passed successfully_24";
    return 0;
}