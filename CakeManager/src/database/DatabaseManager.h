#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QString>
#include <unordered_map>

#include <vector>
#include <optional>
#include "../models/cake.h"
#include "../models/ingredient.h"

class DatabaseManager
{
public:
    DatabaseManager(const QString& databaseName);

    bool openDatabase();
    bool createTables();
    //===========================================================================
    //ingerdient functions
    //===========================================================================

    //to get and find cake without ingredient
    std::optional<Cake> findCake(qint64 id);
    //to check if cake has ingredient or not, if it has return CakeIngredient, else return std::nullopt
    std::optional<CakeIngredient> findCakeIngredient(
        qint64 cakeId,
        qint64 ingredientId
        );
    //this func return all ingredients of cake by cake id, if cake has no ingredient, it will return empty vector
    std::vector<CakeIngredient> getCakeIngredients(qint64 cakeId);
    //this func return all cakes in database, if database has no cake, it will return empty vector
    std::vector<Cake> getCakes();
    //get cakes and get ingredient with onordered map
    std::unordered_map<qint64, Ingredient> getIngredientsMap();
    std::unordered_map<qint64, Cake> getCakesMap();

    //this func add new cake to func
    bool addCake(const Cake& cake);
    //this func update cake in database, it will update name, profitPercentage and ingredients of cake
    bool updateCake(const Cake& cake);
    //this func delete ingredient from database by id, it will delete cake and all ingredients of cake
    bool deleteCake(qint64 id);

    //===========================================================================
    //ingerdient functions
    //===========================================================================
    // Adds an ingredient to the database
    bool addIngredient(const Ingredient& ingredient);

    //delete ingredient by id
    // Deletes an ingredient from the database by ID
    bool deleteIngredient(qint64 id);

    //update ingeriant whit id
    bool updateIngredient(const Ingredient& ingredient);

    //update
    // Updates only the name
    bool updateIngredientName(qint64 id, const QString& name);

    // Updates only the unit
    bool updateIngredientUnit(qint64 id, Ingredient::Unit unit);

    // Updates only the price
    bool updateIngredientPrice(qint64 id, qint64 pricePerUnit);

    // Updates only the image path
    bool updateIngredientImage(qint64 id, const QString& imagePath);

    // Gets one ingredient from the database by ID.
    // Returns std::nullopt if the ingredient is not found.
    std::optional<Ingredient> getIngredient(qint64 id);

    // Gets all ingredients from the database.
    std::vector<Ingredient> getIngredients();

    //=======================================================================
    //save all from memory to database (from unordered_map memory to database)
    //===========================================================================

    //contains
    bool containsIngredient(qint64 id) const;
    bool containsCake(qint64 id) const;


private:
    QSqlDatabase m_database;
    QString m_databaseName;
};

#endif // DATABASEMANAGER_H