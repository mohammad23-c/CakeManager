#include <QCoreApplication>
#include <cassert>
#include <QFile>
#include "../src/database/DatabaseManager.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QFile::remove("testIngredient.db");
    DatabaseManager database("testIngredient.db");

    // =========================
    // Database
    // =========================

    assert(database.openDatabase());
    assert(database.createTables());


    // =========================
    // Add Ingredient
    // =========================

    Ingredient flour(
        0,
        "Flour",
        Ingredient::Unit::Kilogram,
        50000,
        0,
        "images/flour.png"
        );

    assert(database.addIngredient(flour));


    // =========================
    // Get Ingredient
    // =========================

    auto ingredient = database.getIngredient(1);

    assert(ingredient.has_value());

    //assert(ingredient->getName() == "Flour");
    assert(ingredient->getUnit() == Ingredient::Unit::Kilogram);
    assert(ingredient->getPricePerUnit() == 50000);
    assert(ingredient->getImagePath() == "images/flour.png");


    // =========================
    // Update Name
    // =========================

    assert(
        database.updateIngredientName(
            1,
            "Premium Flour"
            )
        );

    ingredient = database.getIngredient(1);

    assert(ingredient.has_value());
    assert(ingredient->getName() == "Premium Flour");


    // =========================
    // Update Unit
    // =========================

    assert(
        database.updateIngredientUnit(
            1,
            Ingredient::Unit::Gram
            )
        );

    ingredient = database.getIngredient(1);

    assert(ingredient.has_value());
    assert(ingredient->getUnit() == Ingredient::Unit::Gram);


    // =========================
    // Update Price
    // =========================

    assert(
        database.updateIngredientPrice(
            1,
            70000
            )
        );

    ingredient = database.getIngredient(1);

    assert(ingredient.has_value());
    assert(ingredient->getPricePerUnit() == 70000);


    // =========================
    // Update Image
    // =========================

    assert(
        database.updateIngredientImage(
            1,
            "images/premium_flour.png"
            )
        );

    ingredient = database.getIngredient(1);

    assert(ingredient.has_value());
    assert(
        ingredient->getImagePath()
        == "images/premium_flour.png"
        );


    // =========================
    // Update All
    // =========================

    Ingredient updatedFlour(
        1,
        "Super Flour",
        Ingredient::Unit::Kilogram,
        90000,
        0,
        "images/super_flour.png"
        );

    assert(database.updateIngredient(updatedFlour));

    ingredient = database.getIngredient(1);

    assert(ingredient.has_value());

    assert(ingredient->getName() == "Super Flour");
    assert(ingredient->getUnit() == Ingredient::Unit::Kilogram);
    assert(ingredient->getPricePerUnit() == 90000);
    assert(
        ingredient->getImagePath()
        == "images/super_flour.png"
        );


    // =========================
    // Invalid ID
    // =========================

    assert(
        !database.updateIngredientPrice(
            999,
            100000
            )
        );

    // =========================
    // Delete Ingredient
    // =========================

    assert(database.getIngredient(1).has_value());

    assert(database.deleteIngredient(1));

    assert(!database.getIngredient(1).has_value());

    // Invalid ID
    assert(!database.deleteIngredient(999));
    qDebug()<<"test passed seccussfully";
    return 0;
}