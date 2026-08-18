#include "DatabaseManager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "../models/cake.h"

DatabaseManager::DatabaseManager(const QString& databaseName)
    : m_databaseName(databaseName),
    m_connectionName(
        "DatabaseManager_" +
        QString::number(reinterpret_cast<quintptr>(this))
        )
{
    m_database = QSqlDatabase::addDatabase(
        "QSQLITE",
        m_connectionName
        );

    m_database.setDatabaseName(m_databaseName);
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

    // =========================================
    // Ingredient table
    // =========================================

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


    // =========================================
    // Cake table
    // =========================================

    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS cakes (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            profit_percentage REAL NOT NULL
        )
    )"))
    {
        qDebug() << "Cakes table creation error:"
                 << query.lastError().text();

        return false;
    }


    // =========================================
    // Cake - Ingredient relation
    // =========================================

    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS cake_ingredients (
            cake_id INTEGER NOT NULL,
            ingredient_id INTEGER NOT NULL,
            quantity REAL NOT NULL,

            PRIMARY KEY (cake_id, ingredient_id),

            FOREIGN KEY (cake_id)
                REFERENCES cakes(id),

            FOREIGN KEY (ingredient_id)
                REFERENCES ingredients(id)
        )
    )"))
    {
        qDebug() << "Cake ingredients table creation error:"
                 << query.lastError().text();

        return false;
    }


    // =========================================
    // Daily table
    // =========================================

    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS dailies (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            date TEXT NOT NULL,
            total_cost REAL NOT NULL,
            total_sales REAL NOT NULL
        )
    )"))
    {
        qDebug() << "Dailies table creation error:"
                 << query.lastError().text();

        return false;
    }


    // =========================================
    // Daily - Cake relation
    // =========================================

    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS daily_cakes (
            daily_id INTEGER NOT NULL,
            cake_id INTEGER NOT NULL,
            quantity REAL NOT NULL,

            PRIMARY KEY (daily_id, cake_id),

            FOREIGN KEY (daily_id)
                REFERENCES dailies(id),

            FOREIGN KEY (cake_id)
                REFERENCES cakes(id)
        )
    )"))
    {
        qDebug() << "Daily cakes table creation error:"
                 << query.lastError().text();

        return false;
    }


    return true;
}

std::optional<Cake> DatabaseManager::findCake(qint64 id)
{QSqlQuery query(m_database);

    query.prepare(R"(
        SELECT id, name, profit_percentage
        FROM cakes
        WHERE id = :id
    )");

    query.bindValue(":id", id);

    if (!query.exec())
    {
        qDebug() << "Failed to find cake:"
                 << query.lastError().text();

        return std::nullopt;
    }

    if (!query.next())
    {
        return std::nullopt;
    }
    std::vector<CakeIngredient> ingredients =
        getCakeIngredients(query.value("id").toLongLong());
    Cake cake(
        query.value("id").toLongLong(),
        query.value("name").toString(),
        query.value("profit_percentage").toDouble(),
        ingredients
        );

    return cake;

}

std::optional<CakeIngredient> DatabaseManager::findCakeIngredient(qint64 cakeId, qint64 ingredientId)
{
    QSqlQuery query(m_database);

    query.prepare(R"(
        SELECT ingredient_id, quantity
        FROM cake_ingredients
        WHERE cake_id = :cakeId
          AND ingredient_id = :ingredientId
    )");

    query.bindValue(":cakeId", cakeId);
    query.bindValue(":ingredientId", ingredientId);

    if (!query.exec())
    {
        qDebug() << "Failed to find cake ingredient:"
                 << query.lastError().text();

        return std::nullopt;
    }

    if (!query.next())
    {
        return std::nullopt;
    }

    CakeIngredient result;

    result.ingredientId =
        query.value("ingredient_id").toLongLong();

    result.quantity =
        query.value("quantity").toDouble();

    return result;
}

std::vector<CakeIngredient> DatabaseManager::getCakeIngredients(qint64 cakeId)
{
    std::vector<CakeIngredient> ingredients;

    QSqlQuery query(m_database);

    query.prepare(R"(
        SELECT ingredient_id, quantity
        FROM cake_ingredients
        WHERE cake_id = :cakeId
    )");

    query.bindValue(":cakeId", cakeId);

    if (!query.exec())
    {
        qDebug() << "Failed to get cake ingredients:"
                 << query.lastError().text();

        return ingredients;
    }

    while (query.next())
    {
        CakeIngredient ingredient;

        ingredient.ingredientId =
            query.value("ingredient_id").toLongLong();

        ingredient.quantity =
            query.value("quantity").toDouble();

        ingredients.push_back(ingredient);
    }

    return ingredients;
}

std::vector<Cake> DatabaseManager::getCakes()
{
    std::vector<Cake> cakes;

    QSqlQuery query(m_database);

    if (!query.exec(R"(
        SELECT id, name, profit_percentage
        FROM cakes
    )"))
    {
        qDebug() << "Failed to get cakes:"
                 << query.lastError().text();

        return cakes;
    }

    while (query.next())
    {
        qint64 id =
            query.value("id").toLongLong();

        QString name =
            query.value("name").toString();

        double profitPercentage =
            query.value("profit_percentage").toDouble();

        std::vector<CakeIngredient> ingredients =
            getCakeIngredients(id);

        Cake cake(
            id,
            name,
            profitPercentage,
            ingredients
            );

        cakes.push_back(cake);
    }

    return cakes;
}

std::unordered_map<qint64, Ingredient> DatabaseManager::getIngredientsMap()
{
    std::unordered_map<qint64, Ingredient> ingredients;

    const auto databaseIngredients = getIngredients();

    for (const auto& ingredient : databaseIngredients)
    {
        ingredients.emplace(
            ingredient.getId(),
            ingredient
            );
    }

    return ingredients;
}

std::unordered_map<qint64, Cake> DatabaseManager::getCakesMap()
{
    std::unordered_map<qint64, Cake> cakes;

    const auto databaseCakes = getCakes();

    for (const auto& cake : databaseCakes)
    {
        cakes.emplace(
            cake.getId(),
            cake
            );
    }

    return cakes;
}

bool DatabaseManager::addCake(const Cake &cake)
{
    if (!m_database.transaction())
    {
        qDebug() << "Failed to start transaction:"
                 << m_database.lastError().text();

        return false;
    }

    QSqlQuery query(m_database);

    // Add cake
    query.prepare(R"(
        INSERT INTO cakes
        (name, profit_percentage)
        VALUES (:name, :profit)
    )");

    query.bindValue(":name", cake.getName());
    query.bindValue(":profit", cake.getProfitPercentage());

    if (!query.exec())
    {
        qDebug() << "Failed to add cake:"
                 << query.lastError().text();

        m_database.rollback();
        return false;
    }

    // Get generated cake ID
    qint64 cakeId = query.lastInsertId().toLongLong();

    // Add ingredients
    for (const auto& ingredient : cake.getIngredients())
    {
        query.prepare(R"(
            INSERT INTO cake_ingredients
            (cake_id, ingredient_id, quantity)
            VALUES (:cakeId, :ingredientId, :quantity)
        )");

        query.bindValue(":cakeId", cakeId);
        query.bindValue(":ingredientId", ingredient.ingredientId);
        query.bindValue(":quantity", ingredient.quantity);

        if (!query.exec())
        {
            qDebug() << "Failed to add cake ingredient:"
                     << query.lastError().text();

            m_database.rollback();
            return false;
        }
    }

    if (!m_database.commit())
    {
        qDebug() << "Failed to commit transaction:"
                 << m_database.lastError().text();

        m_database.rollback();
        return false;
    }

    return true;
}

bool DatabaseManager::updateCake(const Cake &cake)
{
    if (!m_database.transaction())
    {
        return false;
    }

    QSqlQuery query(m_database);

    // Update cake information
    query.prepare(R"(
        UPDATE cakes
        SET name = :name,
            profit_percentage = :profit
        WHERE id = :id
    )");

    query.bindValue(":id", cake.getId());
    query.bindValue(":name", cake.getName());
    query.bindValue(":profit", cake.getProfitPercentage());

    if (!query.exec())
    {
        m_database.rollback();
        return false;
    }

    if (query.numRowsAffected() == 0)
    {
        m_database.rollback();
        return false;
    }

    // Remove old ingredients
    query.prepare(R"(
        DELETE FROM cake_ingredients
        WHERE cake_id = :cakeId
    )");

    query.bindValue(":cakeId", cake.getId());

    if (!query.exec())
    {
        m_database.rollback();
        return false;
    }

    // Add new ingredients
    for (const auto& ingredient : cake.getIngredients())
    {
        query.prepare(R"(
            INSERT INTO cake_ingredients
            (cake_id, ingredient_id, quantity)
            VALUES (:cakeId, :ingredientId, :quantity)
        )");

        query.bindValue(":cakeId", cake.getId());
        query.bindValue(":ingredientId", ingredient.ingredientId);
        query.bindValue(":quantity", ingredient.quantity);

        if (!query.exec())
        {
            m_database.rollback();
            return false;
        }
    }

    if (!m_database.commit())
    {
        m_database.rollback();
        return false;
    }

    return true;
}

bool DatabaseManager::deleteCake(qint64 id)
{
    if (!m_database.transaction())
    {
        return false;
    }

    QSqlQuery query(m_database);

    // Delete cake ingredients
    query.prepare(R"(
        DELETE FROM cake_ingredients
        WHERE cake_id = :id
    )");

    query.bindValue(":id", id);

    if (!query.exec())
    {
        m_database.rollback();
        return false;
    }

    // Delete cake
    query.prepare(R"(
        DELETE FROM cakes
        WHERE id = :id
    )");

    query.bindValue(":id", id);

    if (!query.exec())
    {
        m_database.rollback();
        return false;
    }

    if (query.numRowsAffected() == 0)
    {
        m_database.rollback();
        return false;
    }

    if (!m_database.commit())
    {
        m_database.rollback();
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

bool DatabaseManager::containsIngredient(qint64 id) const
{
    QSqlQuery query(m_database);

    query.prepare(R"(
        SELECT 1
        FROM ingredients
        WHERE id = :id
        LIMIT 1
    )");

    query.bindValue(":id", id);

    if (!query.exec())
    {
        return false;
    }

    return query.next();
}

bool DatabaseManager::containsCake(qint64 id) const
{
    QSqlQuery query(m_database);

    query.prepare(R"(
        SELECT 1
        FROM cakes
        WHERE id = :id
        LIMIT 1
    )");

    query.bindValue(":id", id);

    if (!query.exec())
    {
        return false;
    }

    return query.next();
}

DatabaseManager::~DatabaseManager()
{
    if (m_database.isOpen())
    {
        m_database.close();
    }

    m_database = QSqlDatabase();

    QSqlDatabase::removeDatabase(m_connectionName);
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