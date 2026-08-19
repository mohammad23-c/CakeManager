#include <QCoreApplication>
#include <QFile>

#include <cassert>

#include "../src/database/DatabaseManager.h"
#include "../src/models/cake.h"
#include "../src/models/ingredient.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // =========================================
    // Clean test database
    // =========================================

    QFile::remove("testCake.db");

    DatabaseManager db("testCake.db");

    assert(db.openDatabase());
    assert(db.createTables());


    // =========================================
    // Add Ingredients
    // =========================================

    Ingredient flour(
        0,
        "Flour",
        Ingredient::Unit::Kilogram,
        50000,
        0,
        "images/flour.png"
        );

    Ingredient chocolate(
        0,
        "Chocolate",
        Ingredient::Unit::Kilogram,
        120000,
        0,
        "images/chocolate.png"
        );

    Ingredient cream(
        0,
        "Cream",
        Ingredient::Unit::Kilogram,
        80000,
        0,
        "images/cream.png"
        );

    assert(db.addIngredient(flour));
    assert(db.addIngredient(chocolate));
    assert(db.addIngredient(cream));


    // =========================================
    // Get Ingredient IDs
    // =========================================

    auto ingredients = db.getIngredients();

    assert(ingredients.size() == 3);

    qint64 flourId = ingredients[0].getId();
    qint64 chocolateId = ingredients[1].getId();
    qint64 creamId = ingredients[2].getId();


    // =========================================
    // Create Cake
    // =========================================

    Cake cake(
        0,
        "Chocolate Cake",
        30.0,
        {},
        ""
        );

    cake.addIngredient(flourId, 500);
    cake.addIngredient(chocolateId, 200);
    cake.addIngredient(creamId, 100);


    // =========================================
    // Test addCake()
    // =========================================

    assert(db.addCake(cake));


    // =========================================
    // Get saved Cake
    // =========================================

    auto cakes = db.getCakes();

    assert(cakes.size() == 1);

    Cake savedCake = cakes[0];

    qint64 cakeId = savedCake.getId();

    assert(cakeId > 0);

    assert(savedCake.getName() == "Chocolate Cake");
    assert(savedCake.getProfitPercentage() == 30.0);
    assert(savedCake.getIngredients().size() == 3);


    // =========================================
    // Test findCake()
    // =========================================

    auto foundCake = db.findCake(cakeId);

    assert(foundCake.has_value());

    assert(foundCake->getId() == cakeId);
    assert(foundCake->getName() == "Chocolate Cake");
    assert(foundCake->getProfitPercentage() == 30.0);
    assert(foundCake->getIngredients().size() == 3);


    // =========================================
    // Test findCakeIngredient()
    // =========================================

    auto foundIngredient =
        db.findCakeIngredient(cakeId, chocolateId);

    assert(foundIngredient.has_value());

    assert(foundIngredient->ingredientId == chocolateId);
    assert(foundIngredient->quantity == 200);


    // =========================================
    // Test getCakeIngredients()
    // =========================================

    auto cakeIngredients =
        db.getCakeIngredients(cakeId);

    assert(cakeIngredients.size() == 3);


    // =========================================
    // Test Update Cake
    // =========================================

    Cake updatedCake(
        cakeId,
        "Big Chocolate Cake",
        40.0,
        {},
        ""
        );

    // Keep flour but change its quantity
    updatedCake.addIngredient(flourId, 700);

    // Keep chocolate but change its quantity
    updatedCake.addIngredient(chocolateId, 300);

    // Cream is intentionally removed

    assert(db.updateCake(updatedCake));


    // =========================================
    // Verify Cake Update
    // =========================================

    auto updatedResult = db.findCake(cakeId);

    assert(updatedResult.has_value());

    assert(updatedResult->getName() == "Big Chocolate Cake");
    assert(updatedResult->getProfitPercentage() == 40.0);

    assert(updatedResult->getIngredients().size() == 2);


    // =========================================
    // Verify updated quantities
    // =========================================

    auto updatedFlour =
        db.findCakeIngredient(cakeId, flourId);

    assert(updatedFlour.has_value());
    assert(updatedFlour->quantity == 700);


    auto updatedChocolate =
        db.findCakeIngredient(cakeId, chocolateId);

    assert(updatedChocolate.has_value());
    assert(updatedChocolate->quantity == 300);


    // =========================================
    // Verify Cream was removed
    // =========================================

    auto removedCream =
        db.findCakeIngredient(cakeId, creamId);

    assert(!removedCream.has_value());


    // =========================================
    // Test Update with non-existing ID
    // =========================================

    Cake invalidCake(
        999999,
        "Invalid Cake",
        50.0,
        {},
        ""
        );

    assert(!db.updateCake(invalidCake));


    // =========================================
    // Test Delete Cake
    // =========================================

    assert(db.deleteCake(cakeId));


    // =========================================
    // Verify Cake was deleted
    // =========================================

    auto deletedCake = db.findCake(cakeId);

    assert(!deletedCake.has_value());


    // =========================================
    // Verify Cake Ingredients were deleted
    // =========================================

    auto deletedIngredients =
        db.getCakeIngredients(cakeId);

    assert(deletedIngredients.empty());


    // =========================================
    // Test Delete with non-existing ID
    // =========================================

    assert(!db.deleteCake(999999));


    // =========================================
    // Success
    // =========================================

    qDebug() << "=================================";
    qDebug() << "All Cake tests passed!";
    qDebug() << "=================================";

    return 0;
}