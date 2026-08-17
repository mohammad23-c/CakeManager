#include "appmanager.h"


AppManager::AppManager(const QString &databaseName): m_databaseName(databaseName),
    m_database(databaseName),
    m_hasUnsavedChanges(false)
{
}

bool AppManager::initialize()
{
    if (!m_database.openDatabase())
    {
        return false;
    }

    if (!m_database.createTables())
    {
        return false;
    }

    // Load ingredients
    m_ingredients = m_database.getIngredientsMap();

    qint64 maxIngredientId = 0;

    for (const auto& [id, ingredient] : m_ingredients)
    {
        if (id > maxIngredientId)
        {
            maxIngredientId = id;
        }
    }

    m_nextIngredientId = maxIngredientId + 1;

    // Load cakes
    m_cakes = m_database.getCakesMap();

    qint64 maxCakeId = 0;

    for (const auto& [id, cake] : m_cakes)
    {
        if (id > maxCakeId)
        {
            maxCakeId = id;
        }
    }

    m_nextCakeId = maxCakeId + 1;

    m_hasUnsavedChanges = false;

    return true;
}

bool AppManager::save()
{

        if (!ingredientSave())
        {
            return false;
        }

        if (!cakeSave())
        {
            return false;
        }

        m_hasUnsavedChanges = false;

        return true;}

bool AppManager::ingredientSave()
{
    const auto databaseIngredients =
        m_database.getIngredientsMap();

    for (const auto& [id, ingredient] : m_ingredients)
    {
        if (databaseIngredients.contains(id))
        {
            if (!m_database.updateIngredient(ingredient))
                return false;
        }
        else
        {
            if (!m_database.addIngredient(ingredient))
                return false;
        }
    }

    for (const auto& [id, ingredient] : databaseIngredients)
    {
        if (!containsIngredient(id))
        {
            if (!m_database.deleteIngredient(id))
                return false;
        }
    }

    return true;
}

bool AppManager::cakeSave()
{
    const auto databaseCakes = m_database.getCakesMap();

    // Update existing cakes and add new cakes
    for (const auto& [id, cake] : m_cakes)
    {
        if (databaseCakes.contains(id))
        {
            if (!m_database.updateCake(cake))
            {
                return false;
            }
        }
        else
        {
            if (!m_database.addCake(cake))
            {
                return false;
            }
        }
    }

    // Delete cakes that no longer exist in memory
    for (const auto& [id, cake] : databaseCakes)
    {
        if (!containsCake(id))
        {
            if (!m_database.deleteCake(id))
            {
                return false;
            }
        }
    }

    return true;
}

bool AppManager::addIngredient(const Ingredient &ingredient)
{
    // Name must be unique
    if (containsIngredient(ingredient.getName()))
    {
        return false;
    }

    Ingredient newIngredient = ingredient;

    // AppManager assigns the ID
    newIngredient.setId(m_nextIngredientId);

    ++m_nextIngredientId;

    m_ingredients.emplace(
        newIngredient.getId(),
        newIngredient
        );

    AppManager::markAsChanged();

    return true;
}

bool AppManager::updateIngredient(const Ingredient &ingredient)
{
    // Check if ingredient exists
    auto it = m_ingredients.find(ingredient.getId());

    if (it == m_ingredients.end())
    {
        return false;
    }

    // Check if another ingredient already has this name
    if (containsIngredient(
            ingredient.getName(),
            ingredient.getId()))
    {
        return false;
    }

    // Update ingredient in memory
    it->second = ingredient;

    // There are unsaved changes
    markAsChanged();

    return true;
}

bool AppManager::deleteIngredient(qint64 id)
{
    // Check if ingredient exists
    auto it = m_ingredients.find(id);

    if (it == m_ingredients.end())
    {
        return false;
    }

    // Ingredient cannot be deleted if it is used in a cake
    if (isIngredientUsedInCake(id))
    {
        return false;
    }

    // Remove ingredient from memory
    m_ingredients.erase(it);

    // There are unsaved changes
    markAsChanged();

    return true;
}

std::optional<Ingredient> AppManager::findIngredient(qint64 id) const
{
    auto it = m_ingredients.find(id);

    if (it == m_ingredients.end())
    {
        return std::nullopt;
    }

    return it->second;
}

std::optional<Ingredient> AppManager::findIngredient(QString name) const
{
    for (const auto& in : m_ingredients)
    {
        if (in.second.getName() == name)
        {
            return in.second;
        }
    }

    return std::nullopt;
}

bool AppManager::addCake(const Cake &cake)
{

    // Check if cake name already exists
    if (containsCake(cake.getName()))
    {
        return false;
    }

    // Check if all ingredients exist
    for (const auto& ingredient : cake.getIngredients())
    {
        if (!containsIngredient(ingredient.ingredientId))
        {
            return false;
        }
    }

    Cake newCake = cake;

    // AppManager assigns the ID
    newCake.setId(m_nextCakeId);

    ++m_nextCakeId;

    // Add cake to memory
    m_cakes.emplace(
        newCake.getId(),
        newCake
        );

    // There are unsaved changes
    markAsChanged();

    return true;
}

bool AppManager::updateCake(const Cake &cake)
{
    //chek cake exit
    auto it = m_cakes.find(cake.getId());

    if (it == m_cakes.end())
    {
        return false;
    }

    // Check if another cake already has this name
    if (containsCake(cake.getName(),cake.getId()))
    {
        return false;
    }

    // Check if all ingredients exist
    for (const auto& ingredient : cake.getIngredients())
    {
        if (!containsIngredient(ingredient.ingredientId))
        {
            return false;
        }
    }
    // Update cake in memory
    it->second = cake;

    // There are unsaved changes
    markAsChanged();

    return true;
}

bool AppManager::deleteCake(qint64 id)
{

    // Check if cake exists
    auto it = m_cakes.find(id);

    if (it == m_cakes.end())
    {
        return false;
    }

    // TODO:
    // Check if this cake was used in today's orders.
    // If it was used today, deletion should not be allowed.

    // Remove cake from memory
    m_cakes.erase(it);

    // There are unsaved changes
    markAsChanged();

    return true;
}

std::optional<Cake> AppManager::findCake(qint64 id) const
{
    auto it = m_cakes.find(id);

    if (it == m_cakes.end())
    {
        return std::nullopt;
    }

    return it->second;
}

std::optional<Cake> AppManager::findCake(QString name) const
{
    for(const auto& c:m_cakes){
        if(c.second.getName()==name){
            return c.second;
        }
    }
    return std::nullopt;
}

bool AppManager::isIngredientUsedInCake(qint64 ingredientId) const
{
    for (const auto& [cakeId, cake] : m_cakes)
    {
        if(isIngredientUsedInThisCake(ingredientId,cakeId))
            return true;
    }

    return false;
}

std::optional<double> AppManager::calculateCakeCost(qint64 cakeId) const
{
    std::optional<Cake> c = findCake(cakeId);

    if (!c.has_value())
    {
        return std::nullopt;
    }

    double sum = 0;

    const std::vector<CakeIngredient>& ingredients =
        c->getIngredients();

    for (const auto& in : ingredients)
    {
        qint64 ingredientId = in.ingredientId;

        auto ingredient = findIngredient(ingredientId);

        if (!ingredient.has_value())
        {
            return std::nullopt;
        }

        double pricePerUnit =
            ingredient->getPricePerUnit();

        double quantity = in.quantity;

        double cost = pricePerUnit * quantity;

        sum += cost;
    }

    return sum;
}
bool AppManager::containsIngredient(qint64 id) const
{
    return m_ingredients.find(id) != m_ingredients.end();
}

bool AppManager::containsIngredient(const QString &name) const
{
    for (const auto& [id, ingredient] : m_ingredients)
    {
        if (ingredient.getName() == name)
        {
            return true;
        }
    }

    return false;
}

bool AppManager::containsIngredient(const QString &name, qint64 exceptId) const
{

    for (const auto& [id, ingredient] : m_ingredients)
    {
        if (id != exceptId &&
            ingredient.getName() == name)
        {
            return true;
        }
    }

    return false;
}

bool AppManager::containsCake(qint64 id) const
{
    return m_cakes.find(id) != m_cakes.end();
}

bool AppManager::containsCake(const QString &name) const
{
    for (const auto& [id, cake] : m_cakes)
    {
        if (cake.getName() == name)
        {
            return true;
        }
    }

    return false;
}

bool AppManager::containsCake(const QString &name, qint64 exceptId) const
{
    for (const auto& [id, cake] : m_cakes)
    {
        if (id != exceptId &&
            cake.getName() == name)
        {
            return true;
        }
    }

    return false;
}

void AppManager::markAsChanged()
{
    this->m_hasUnsavedChanges=true;
}
std::optional<double> AppManager::calculateCakeFinalPrice(qint64 cakeId) const{
    std::optional<double> price=calculateCakeCost(cakeId);
    if(!price.has_value()){
        return std::nullopt;
    }
    std::optional<Cake> c=findCake(cakeId);
    if(!c.has_value()){
        return std::nullopt;
    }
    double profit=c->getProfitPercentage();
    double finalPrice=(price.value())*(1.0+(profit/100));
    return finalPrice;
}
bool AppManager::hasUnsavedChanges() const{
    return m_hasUnsavedChanges;
}
std::vector<qint64>
AppManager::getCakesUsingIngredient(qint64 ingredientId) const
{
    std::vector<qint64> cakes;

    for (const auto& [id, cake] : m_cakes)
    {
        if (isIngredientUsedInThisCake(ingredientId, id))
        {
            cakes.push_back(id);
        }
    }

    return cakes;
}
bool AppManager::isIngredientUsedInThisCake(
    qint64 ingredientId,
    qint64 cakeId
) const
{
    auto it = m_cakes.find(cakeId);

    if (it == m_cakes.end())
    {
        return false;
    }

    const auto& ingredients = it->second.getIngredients();

    for (const auto& ingredient : ingredients)
    {
        if (ingredient.ingredientId == ingredientId)
        {
            return true;
        }
    }

    return false;
}
