#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QString>

#include <vector>
#include <optional>

#include "../models/ingredient.h"

class DatabaseManager
{
public:
    DatabaseManager(const QString& databaseName);

    bool openDatabase();
    bool createTables();

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



private:
    QSqlDatabase m_database;
    QString m_databaseName;
};

#endif // DATABASEMANAGER_H