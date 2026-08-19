#include <cassert>
#include <QDebug>
#include <QFile>
#include <QCoreApplication>

#include "../src/database/DatabaseManager.h"
#include "../src/models/ingredient.h"
#include "../src/models/cake.h"
#include "../src/models/daily.h"


int main(int argc, char *argv[])
{

    QCoreApplication app(argc, argv);
    // =========================================
    // Create Database
    // =========================================

    const QString databasePath =
        "dailyTest.db";

    QFile::remove(databasePath);
    DatabaseManager database(databasePath);

    // =========================================
    // Open Database
    // =========================================

    assert(database.openDatabase());

    // =========================================
    // Create Tables
    // =========================================

    assert(database.createTables());


    // =========================================
    // Add Ingredients
    // =========================================

    Ingredient flour(
        0,
        "Flour",
        Ingredient::Unit::Gram,
        120000,
        0,
        ""
        );

    Ingredient sugar(
        0,
        "Sugar",
        Ingredient::Unit::Gram,
        80000,
        0,
        ""
        );

    Ingredient chocolate(
        0,
        "Chocolate",
        Ingredient::Unit::Gram,
        250000,
        0,
        ""
        );

    Ingredient milk(
        0,
        "Milk",
        Ingredient::Unit::Gram,
        100000,
        0,
        ""
        );

    assert(database.addIngredient(flour));
    assert(database.addIngredient(sugar));
    assert(database.addIngredient(chocolate));
    assert(database.addIngredient(milk));


    // =========================================
    // Load Ingredients
    // =========================================

    auto ingredients = database.getIngredientsMap();

    assert(ingredients.size() == 4);


    // =========================================
    // Get Ingredient IDs
    // =========================================

    qint64 flourId = -1;
    qint64 sugarId = -1;
    qint64 chocolateId = -1;
    qint64 milkId = -1;

    for (const auto& [id, ingredient] : ingredients)
    {
        if (ingredient.getName() == "Flour")
            flourId = id;

        else if (ingredient.getName() == "Sugar")
            sugarId = id;

        else if (ingredient.getName() == "Chocolate")
            chocolateId = id;

        else if (ingredient.getName() == "Milk")
            milkId = id;
    }

    assert(flourId != -1);
    assert(sugarId != -1);
    assert(chocolateId != -1);
    assert(milkId != -1);


    // =========================================
    // Add Cakes
    // =========================================

    std::vector<CakeIngredient> chocolateCakeIngredients =
        {
            {flourId, 1.0},
            {sugarId, 0.5},
            {chocolateId, 0.3}
        };

    Cake chocolateCake(
        0,
        "Chocolate Cake",
        25.0,
        chocolateCakeIngredients,
        ""
        );


    std::vector<CakeIngredient> sugarCakeIngredients =
        {
            {flourId, 1.0},
            {sugarId, 0.8}
        };

    Cake sugarCake(
        0,
        "Sugar Cake",
        15.0,
        sugarCakeIngredients,
        ""
        );


    assert(database.addCake(chocolateCake));
    assert(database.addCake(sugarCake));


    // =========================================
    // Load Cakes
    // =========================================

    auto cakes = database.getCakesMap();

    assert(cakes.size() == 2);


    qDebug() << "Ingredients and Cakes created successfully.";
    // =========================================
    // Get Cake IDs
    // =========================================

    qint64 chocolateCakeId = -1;
    qint64 sugarCakeId = -1;

    for (const auto& [id, cake] : cakes)
    {
        if (cake.getName() == "Chocolate Cake")
        {
            chocolateCakeId = id;
        }
        else if (cake.getName() == "Sugar Cake")
        {
            sugarCakeId = id;
        }
    }

    assert(chocolateCakeId != -1);
    assert(sugarCakeId != -1);
    // =========================================
    // Create Daily
    // =========================================

    std::vector<DailyCake> dailyCakes =
        {
            {chocolateCakeId, 5.0},
            {sugarCakeId, 3.5}
        };

    Daily daily(
        0,
        QDate(2026, 8, 17),
        dailyCakes
        );

    // =========================================
    // Test addDaily
    // =========================================

    assert(database.addDaily(daily));
    // =========================================
    // Check containsDaily
    // =========================================

    auto dailies = database.getDailiesMap();

    assert(!dailies.empty());

    qint64 dailyId = -1;

    for (const auto& [id, d] : dailies)
    {
        if (d.getDate() == QDate(2026, 8, 17))
        {
            dailyId = id;
            break;
        }
    }

    assert(dailyId != -1);

    assert(database.containsDaily(dailyId));
    // =========================================
    // Check Daily Cakes
    // =========================================

    auto savedDailyCakes =
        database.getDailyCakes(dailyId);

    assert(savedDailyCakes.size() == 2);
    bool foundChocolate = false;
    bool foundSugar = false;

    for (const auto& cake : savedDailyCakes)
    {
        if (cake.cakeId == chocolateCakeId)
        {
            assert(cake.quantity == 5.0);
            foundChocolate = true;
        }

        if (cake.cakeId == sugarCakeId)
        {
            assert(cake.quantity == 3.5);
            foundSugar = true;
        }
    }

    assert(foundChocolate);
    assert(foundSugar);
    // =========================================
    // Test findDaily
    // =========================================

    auto foundDaily = database.findDaily(dailyId);

    assert(foundDaily.has_value());

    assert(foundDaily->getId() == dailyId);
    assert(foundDaily->getDate() == QDate(2026, 8, 17));
    // =========================================
    // Check Daily Cakes
    // =========================================

    const auto& foundCakes = foundDaily->getCakes();

    assert(foundCakes.size() == 2);
    bool foundChocolateCake = false;
    bool foundSugarCake = false;

    for (const auto& cake : foundCakes)
    {
        if (cake.cakeId == chocolateCakeId)
        {
            assert(cake.quantity == 5.0);
            foundChocolateCake = true;
        }

        else if (cake.cakeId == sugarCakeId)
        {
            assert(cake.quantity == 3.5);
            foundSugarCake = true;
        }
    }

    assert(foundChocolateCake);
    assert(foundSugarCake);

    // =========================================
    // Find Non-existing Daily
    // =========================================

    auto notFoundDaily = database.findDaily(999999);

    assert(!notFoundDaily.has_value());
    // =========================================
    // Add Second Daily
    // =========================================

    std::vector<DailyCake> secondDailyCakes =
        {
            {chocolateCakeId, 2.0},
            {sugarCakeId, 7.5}
        };

    Daily secondDaily(
        0,
        QDate(2026, 8, 18),
        secondDailyCakes
        );

    assert(database.addDaily(secondDaily));
    // =========================================
    // Test getDailies
    // =========================================

    auto allDailies = database.getDailies();

    assert(allDailies.size() == 2);
    bool foundFirstDaily = false;
    bool foundSecondDaily = false;

    for (const auto& d : allDailies)
    {
        if (d.getDate() == QDate(2026, 8, 17))
        {
            assert(d.getCakes().size() == 2);

            foundFirstDaily = true;
        }

        else if (d.getDate() == QDate(2026, 8, 18))
        {
            assert(d.getCakes().size() == 2);

            foundSecondDaily = true;
        }
    }

    assert(foundFirstDaily);
    assert(foundSecondDaily);
    // =========================================
    // Test getDailiesMap
    // =========================================

    auto dailiesMap = database.getDailiesMap();

    assert(dailiesMap.size() == 2);
    assert(dailiesMap.contains(dailyId));

    qint64 secondDailyId = -1;

    for (const auto& [id, d] : dailiesMap)
    {
        if (d.getDate() == QDate(2026, 8, 18))
        {
            secondDailyId = id;
            break;
        }
    }

    assert(secondDailyId != -1);

    const auto& loadedSecondDaily =
        dailiesMap.at(secondDailyId);

    assert(loadedSecondDaily.getDate() == QDate(2026, 8, 18));

    assert(loadedSecondDaily.getCakes().size() == 2);
    // =========================================
    // Test updateDaily
    // =========================================

    std::vector<DailyCake> updatedDailyCakes =
        {
            {chocolateCakeId, 8.0}
        };

    Daily updatedDaily(
        dailyId,
        QDate(2026, 8, 20),
        updatedDailyCakes
        );

    assert(database.updateDaily(updatedDaily));
    // =========================================
    // Load Updated Daily
    // =========================================

    auto loadedUpdatedDaily =
        database.findDaily(dailyId);

    assert(loadedUpdatedDaily.has_value());
    assert(
        loadedUpdatedDaily->getDate()
        == QDate(2026, 8, 20)
        );

    const auto& updatedCakes =
        loadedUpdatedDaily->getCakes();

    assert(updatedCakes.size() == 1);

    assert(updatedCakes[0].cakeId == chocolateCakeId);
    assert(updatedCakes[0].quantity == 8.0);

    std::vector<DailyCake> updatedAgainCakes =
        {
            {chocolateCakeId, 8.0},
            {sugarCakeId, 4.5}
        };

    Daily updatedAgain(
        dailyId,
        QDate(2026, 8, 21),
        updatedAgainCakes
        );

    assert(database.updateDaily(updatedAgain));

    auto loadedAgain =
        database.findDaily(dailyId);

    assert(loadedAgain.has_value());

    assert(
        loadedAgain->getDate()
        == QDate(2026, 8, 21)
        );

    const auto& cakesAfterUpdate =
        loadedAgain->getCakes();

    assert(cakesAfterUpdate.size() == 2);

     foundChocolate = false;
     foundSugar = false;

    for (const auto& cake : cakesAfterUpdate)
    {
        if (cake.cakeId == chocolateCakeId)
        {
            assert(cake.quantity == 8.0);
            foundChocolate = true;
        }

        else if (cake.cakeId == sugarCakeId)
        {
            assert(cake.quantity == 4.5);
            foundSugar = true;
        }
    }

    assert(foundChocolate);
    assert(foundSugar);
    // =========================================
    // Test deleteDaily
    // =========================================

    assert(database.deleteDaily(dailyId));
    // =========================================
    // Check Daily Was Deleted
    // =========================================

    assert(!database.containsDaily(dailyId));

    auto deletedDaily = database.findDaily(dailyId);

    assert(!deletedDaily.has_value());

    auto deletedDailyCakes =
        database.getDailyCakes(dailyId);

    assert(deletedDailyCakes.empty());

    assert(database.containsDaily(secondDailyId));

    auto remainingDaily =
        database.findDaily(secondDailyId);

    assert(remainingDaily.has_value());
    assert(
        remainingDaily->getDate()
        == QDate(2026, 8, 18)
        );
    // =========================================
    // Test findDailyCake
    // =========================================

    auto foundDailyCake =
        database.findDailyCake(
            secondDailyId,
            chocolateCakeId
            );

    assert(foundDailyCake.has_value());

    assert(foundDailyCake->cakeId == chocolateCakeId);
    assert(foundDailyCake->quantity == 2.0);

    // =========================================
    // Find Non-existing Cake In Daily
    // =========================================

    auto notFoundDailyCake =
        database.findDailyCake(
            secondDailyId,
            999999
            );

    assert(!notFoundDailyCake.has_value());
    // =========================================
    // Find Cake In Non-existing Daily
    // =========================================

    auto notFound =
        database.findDailyCake(
            999999,
            chocolateCakeId
            );

    assert(!notFound.has_value());


    //============================================================================================
    qDebug() <<"test passed successfullly_8";

    return 0;
}