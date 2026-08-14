#include "DatabaseManager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

DatabaseManager::DatabaseManager(const QString& databaseName)
    : m_databaseName(databaseName)
{
    m_database = QSqlDatabase::addDatabase("QSQLITE");
}

bool DatabaseManager::openDatabase()
{
    m_database.setDatabaseName(m_databaseName);

    if (!m_database.open())
    {
        qDebug() << "Database error:" << m_database.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::createTables()
{
    QSqlQuery query(m_database);

    QString sql = R"(
        CREATE TABLE IF NOT EXISTS ingredients (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            unit INTEGER NOT NULL,
            price_per_unit INTEGER NOT NULL,
            image_path TEXT
        )
    )";

    if (!query.exec(sql))
    {
        qDebug() << "Table creation error:"
                 << query.lastError().text();

        return false;
    }

    return true;
}

bool DatabaseManager::addIngredient(const Ingredient& ingredient)
{
    QSqlQuery query(m_database);

    query.prepare(R"(
        INSERT INTO ingredients
        (name, unit, price_per_unit, image_path)
        VALUES (:name, :unit, :price, :imagePath)
    )");

    query.bindValue(":name", ingredient.getName());
    query.bindValue(":unit", static_cast<int>(ingredient.getUnit()));
    query.bindValue(":price", ingredient.getPricePerUnit());
    query.bindValue(":imagePath", ingredient.getImagePath());

    if (!query.exec())
    {
        qDebug() << "Failed to add ingredient:"
                 << query.lastError().text();

        return false;
    }

    return true;
}

bool DatabaseManager::deleteIngredient(qint64 id)
{
    QSqlQuery query(m_database);

    query.prepare(R"(
        DELETE FROM ingredients
        WHERE id = :id
    )");

    query.bindValue(":id", id);

    if (!query.exec())
    {
        qDebug() << "Failed to delete ingredient:"
                 << query.lastError().text();

        return false;
    }

    return query.numRowsAffected() > 0;
}

bool DatabaseManager::updateIngredient(const Ingredient &ingredient)
{
    QSqlQuery query(m_database);

    query.prepare(R"(
        UPDATE ingredients
        SET name = :name,
            unit = :unit,
            price_per_unit = :price,
            image_path = :imagePath
        WHERE id = :id
    )");

    query.bindValue(":id", ingredient.getId());
    query.bindValue(":name", ingredient.getName());
    query.bindValue(":unit", static_cast<int>(ingredient.getUnit()));
    query.bindValue(":price", ingredient.getPricePerUnit());
    query.bindValue(":imagePath", ingredient.getImagePath());

    if (!query.exec())
    {
        qDebug() << "Failed to update ingredient:"
                 << query.lastError().text();

        return false;
    }

return query.numRowsAffected() > 0;}

std::optional<Ingredient> DatabaseManager::getIngredient(qint64 id)
{
    QSqlQuery query(m_database);

    query.prepare(R"(
        SELECT id, name, unit, price_per_unit, image_path
        FROM ingredients
        WHERE id = :id
    )");

    query.bindValue(":id", id);

    if (!query.exec())
    {
        qDebug() << "Failed to get ingredient:"
                 << query.lastError().text();

        return std::nullopt;
    }

    if (!query.next())
    {
        return std::nullopt;
    }

    Ingredient ingredient(
        query.value("id").toLongLong(),
        query.value("name").toString(),
        static_cast<Ingredient::Unit>(
            query.value("unit").toInt()
            ),
        query.value("price_per_unit").toLongLong(),
        query.value("image_path").toString()
        );

    return ingredient;
}

std::vector<Ingredient> DatabaseManager::getIngredients()
{
    std::vector<Ingredient> ingredients;

    QSqlQuery query(m_database);

    if (!query.exec("SELECT id FROM ingredients"))
    {
        qDebug() << "Failed to get ingredients:"
                 << query.lastError().text();

        return ingredients;
    }

    while (query.next())
    {
        qint64 id = query.value("id").toLongLong();

        auto ingredient = getIngredient(id);

        if (ingredient.has_value())
        {
            ingredients.push_back(ingredient.value());
        }
    }

    return ingredients;
}

bool DatabaseManager::updateIngredientName(
    qint64 id,
    const QString& name)
{
    QSqlQuery query(m_database);

    query.prepare(R"(
        UPDATE ingredients
        SET name = :name
        WHERE id = :id
    )");

    query.bindValue(":id", id);
    query.bindValue(":name", name);

    if (!query.exec())
    {
        qDebug() << "Failed to update ingredient name:"
                 << query.lastError().text();

        return false;
    }

    return query.numRowsAffected() > 0;
}


bool DatabaseManager::updateIngredientUnit(
    qint64 id,
    Ingredient::Unit unit)
{
    QSqlQuery query(m_database);

    query.prepare(R"(
        UPDATE ingredients
        SET unit = :unit
        WHERE id = :id
    )");

    query.bindValue(":id", id);
    query.bindValue(":unit", static_cast<int>(unit));

    if (!query.exec())
    {
        qDebug() << "Failed to update ingredient unit:"
                 << query.lastError().text();

        return false;
    }

    return query.numRowsAffected() > 0;
}


bool DatabaseManager::updateIngredientPrice(
    qint64 id,
    qint64 pricePerUnit)
{
    QSqlQuery query(m_database);

    query.prepare(R"(
        UPDATE ingredients
        SET price_per_unit = :price
        WHERE id = :id
    )");

    query.bindValue(":id", id);
    query.bindValue(":price", pricePerUnit);

    if (!query.exec())
    {
        qDebug() << "Failed to update ingredient price:"
                 << query.lastError().text();

        return false;
    }

    return query.numRowsAffected() > 0;
}


bool DatabaseManager::updateIngredientImage(
    qint64 id,
    const QString& imagePath)
{
    QSqlQuery query(m_database);

    query.prepare(R"(
        UPDATE ingredients
        SET image_path = :imagePath
        WHERE id = :id
    )");

    query.bindValue(":id", id);
    query.bindValue(":imagePath", imagePath);

    if (!query.exec())
    {
        qDebug() << "Failed to update ingredient image:"
                 << query.lastError().text();

        return false;
    }

    return query.numRowsAffected() > 0;
}