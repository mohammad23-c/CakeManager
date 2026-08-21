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

    // Load dailies
    m_dailies = m_database.getDailiesMap();

    qint64 maxDailyId = 0;

    for (const auto& [id, daily] : m_dailies)
    {
        if (id > maxDailyId)
        {
            maxDailyId = id;
        }
    }

    m_nextDailyId = maxDailyId + 1;

    // Load inventory
    m_inventory = m_database.getInventoryMap();

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

        if(!dailySave()){
            return false;
        }
        if (!inventorySave())
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

bool AppManager::dailySave()
{
    const auto databaseDailies =
        m_database.getDailiesMap();

    // Update existing dailies and add new dailies
    for (const auto& [id, daily] : m_dailies)
    {
        if (databaseDailies.contains(id))
        {
            if (!m_database.updateDaily(daily))
            {
                return false;
            }
        }
        else
        {
            if (!m_database.addDaily(daily))
            {
                return false;
            }
        }
    }

    // Delete dailies that no longer exist in memory
    for (const auto& [id, daily] : databaseDailies)
    {
        if (!m_dailies.contains(id))
        {
            if (!m_database.deleteDaily(id))
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

std::optional<double>
AppManager::calculateCakeCost(
    qint64 cakeId,
    double weightKg
    ) const
{
    if (weightKg <= 0)
    {
        return std::nullopt;
    }

    auto cakeWeight = calculateCakeWeight(cakeId);

    if (!cakeWeight.has_value() || *cakeWeight <= 0)
    {
        return std::nullopt;
    }

    auto cakeCost = calculateCakeCost(cakeId);

    if (!cakeCost.has_value())
    {
        return std::nullopt;
    }


    double cakeCount =
        weightKg / *cakeWeight;

    return cakeCount * *cakeCost;
}

std::optional<double>
AppManager::calculateCakeFinalPrice(
    qint64 cakeId,
    double weightKg
    ) const
{
    std::optional<double> price =
        calculateCakeCost(cakeId, weightKg);

    if (!price.has_value())
    {
        return std::nullopt;
    }

    std::optional<Cake> c =
        findCake(cakeId);

    if (!c.has_value())
    {
        return std::nullopt;
    }

    double profit =
        c->getProfitPercentage();

    double finalPrice =
        price.value() *
        (1.0 + (profit / 100.0));

    return finalPrice;
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

bool AppManager::containsDaily(const QDate& date) const
{
    for (const auto& [id, daily] : m_dailies)
    {
        if (daily.getDate() == date)
        {
            return true;
        }
    }

    return false;
}

bool AppManager::containsDaily(
    const QDate& date,
    qint64 exceptId
    ) const
{
    for (const auto& [id, daily] : m_dailies)
    {
        if (id == exceptId)
        {
            continue;
        }

        if (daily.getDate() == date)
        {
            return true;
        }
    }

    return false;
}

std::optional<double>
AppManager::calculateCakeWeight(qint64 cakeId) const
{
    auto cakeIt = m_cakes.find(cakeId);

    if (cakeIt == m_cakes.end())
    {
        return std::nullopt;
    }

    const Cake& cake = cakeIt->second;

    double totalWeight = 0.0;

    for (const auto& cakeIngredient : cake.getIngredients())
    {
        auto ingredientIt =
            m_ingredients.find(cakeIngredient.ingredientId);

        if (ingredientIt == m_ingredients.end())
        {
            return std::nullopt;
        }

        const Ingredient& ingredient =
            ingredientIt->second;

        double quantity = cakeIngredient.quantity;

        switch (ingredient.getUnit())
        {
        case Ingredient::Unit::Kilogram:
            totalWeight += quantity * 1000.0;
            break;

        case Ingredient::Unit::Gram:
            totalWeight += quantity;
            break;

        case Ingredient::Unit::Piece:
            totalWeight += quantity*ingredient.getWeightPerUnit();
            break;
        }
    }

    return totalWeight / 1000.0;
}

DailySummary AppManager::calculateDailySummary(const Daily &daily) const
{

    DailySummary summary;

    for (const auto& dailyCake : daily.getCakes())
    {
        qint64 cakeId = dailyCake.cakeId;
        double quantity = dailyCake.quantity;

        auto cakeCostOpt = calculateCakeCost(cakeId);

        if (!cakeCostOpt.has_value())
        {
            continue;
        }

        double cakeCost = cakeCostOpt.value();

        auto cakeOpt = findCake(cakeId);

        if (!cakeOpt.has_value())
        {
            continue;
        }

        double profitPercentage =
            cakeOpt->getProfitPercentage();

        double finalPrice =
            cakeCost * (1.0 + (profitPercentage / 100.0));

        summary.totalCost += cakeCost * quantity;
        summary.totalSales += finalPrice * quantity;
    }

    return summary;
}

QDate AppManager::getCurrentDate() const
{
    return QDate::currentDate();
}

bool AppManager::addInventory(
    qint64 ingredientId,
    double quantity
    )
{
    if (!containsIngredient(ingredientId))
    {
        return false;
    }

    auto it = m_inventory.find(ingredientId);

    if (it != m_inventory.end())
    {
        it->second = quantity;
    }
    else
    {
        m_inventory.emplace(
            ingredientId,
            quantity
            );
    }

    markAsChanged();

    return true;
}

bool AppManager::updateInventory(
    qint64 ingredientId,
    double quantity
    )
{
    auto it = m_inventory.find(ingredientId);

    if (it == m_inventory.end())
    {
        return false;
    }

    it->second = quantity;

    markAsChanged();

    return true;
}

bool AppManager::deleteInventory(qint64 ingredientId)
{
    auto it = m_inventory.find(ingredientId);

    if (it == m_inventory.end())
    {
        return false;
    }

    m_inventory.erase(it);

    markAsChanged();

    return true;
}

std::optional<double>
AppManager::findInventory(qint64 ingredientId) const
{
    auto it = m_inventory.find(ingredientId);

    if (it == m_inventory.end())
    {
        return std::nullopt;
    }

    return it->second;
}

bool AppManager::containsInventory(qint64 ingredientId) const
{
    return m_inventory.contains(ingredientId);
}

bool AppManager::inventorySave()
{
    const auto databaseInventory =
        m_database.getInventoryMap();

    // Update existing inventory and add new inventory
    for (const auto& [ingredientId, quantity] : m_inventory)
    {
        if (databaseInventory.contains(ingredientId))
        {
            if (!m_database.updateInventory(
                    ingredientId,
                    quantity))
            {
                return false;
            }
        }
        else
        {
            if (!m_database.addInventory(
                    ingredientId,
                    quantity))
            {
                return false;
            }
        }
    }

    // Delete inventory that no longer exists in memory
    for (const auto& [ingredientId, quantity] : databaseInventory)
    {
        if (!m_inventory.contains(ingredientId))
        {
            if (!m_database.deleteInventory(ingredientId))
            {
                return false;
            }
        }
    }

    return true;
}

std::vector<Ingredient> AppManager::getIngredients() const
{
    std::vector<Ingredient> ingredients;

    for (const auto& pair : m_ingredients)
    {
        ingredients.push_back(pair.second);
    }

    return ingredients;
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

bool AppManager::addDaily(const Daily& daily)
{
    if (containsDaily(daily.getDate()))
    {
        return false;
    }

    qint64 id = m_nextDailyId++;

    Daily newDaily = daily;
    newDaily.setId(id);

    m_dailies.emplace(id, newDaily);

    markAsChanged();

    return true;
}

bool AppManager::updateDaily(const Daily& daily)
{
    auto it = m_dailies.find(daily.getId());

    if (it == m_dailies.end())
    {
        return false;
    }
    if(containsDaily(daily.getDate(), daily.getId()))
    {
        return false;
    }
    it->second = daily;

    markAsChanged();

    return true;
}

bool AppManager::deleteDaily(qint64 id)
{
    auto it = m_dailies.find(id);

    if (it == m_dailies.end())
    {
        return false;
    }

    m_dailies.erase(it);

    markAsChanged();

    return true;
}

std::optional<Daily> AppManager::findDaily(qint64 id) const
{
    auto it = m_dailies.find(id);

    if (it == m_dailies.end())
    {
        return std::nullopt;
    }

    return it->second;
}

std::optional<Daily> AppManager::findDaily(const QDate& date) const
{
    for (const auto& [id, daily] : m_dailies)
    {
        if (daily.getDate() == date)
        {
            return daily;
        }
    }

    return std::nullopt;
}

std::vector<Daily> AppManager::getDailies() const
{
    std::vector<Daily> dailies;

    dailies.reserve(m_dailies.size());

    for (const auto& [id, daily] : m_dailies)
    {
        dailies.push_back(daily);
    }

    return dailies;
}

bool AppManager::addCakeToDaily(
    qint64 dailyId,
    qint64 cakeId,
    double quantity,
    bool quantityIsWeight
    )
{
    if (quantity <= 0)
    {
        return false;
    }

    // Find Daily
    auto dailyIt = m_dailies.find(dailyId);

    if (dailyIt == m_dailies.end())
    {
        return false;
    }

    // Find Cake
    auto cakeOpt = findCake(cakeId);

    if (!cakeOpt.has_value())
    {
        return false;
    }

    // Check inventory before changing anything
    for (const auto& cakeIngredient : cakeOpt->getIngredients())
    {
        double requiredQuantity =
            cakeIngredient.quantity * quantity;

        auto inventoryIt =
            m_inventory.find(cakeIngredient.ingredientId);

        if (inventoryIt == m_inventory.end())
        {
            return false;
        }

        if (inventoryIt->second < requiredQuantity)
        {
            return false;
        }
    }
    if (quantityIsWeight)
    {
        auto cakeWeight = calculateCakeWeight(cakeId);

        if (!cakeWeight.has_value() || *cakeWeight <= 0)
        {
            return false;
        }

        quantity /= *cakeWeight;
    }

    // Add cake to Daily
    if (!dailyIt->second.addCake(cakeId, quantity))
    {
        return false;
    }

    // Decrease inventory
    for (const auto& cakeIngredient : cakeOpt->getIngredients())
    {
        double requiredQuantity =
            cakeIngredient.quantity * quantity;

        m_inventory[cakeIngredient.ingredientId]
            -= requiredQuantity;
    }

    markAsChanged();

    return true;
}

bool AppManager::removeCakeFromDaily(
    qint64 dailyId,
    qint64 cakeId
    )
{
    // Find Daily
    auto dailyIt = m_dailies.find(dailyId);

    if (dailyIt == m_dailies.end())
    {
        return false;
    }

    // Find Cake
    auto cakeOpt = findCake(cakeId);

    if (!cakeOpt.has_value())
    {
        return false;
    }

    // Find the cake inside Daily
    auto dailyCakeOpt =
        dailyIt->second.findCake(cakeId);

    if (!dailyCakeOpt.has_value())
    {
        return false;
    }

    double quantity = dailyCakeOpt->quantity;

    // Return ingredients to inventory
    for (const auto& cakeIngredient : cakeOpt->getIngredients())
    {
        double returnedQuantity =
            cakeIngredient.quantity * quantity;

        auto inventoryIt =
            m_inventory.find(cakeIngredient.ingredientId);

        if (inventoryIt == m_inventory.end())
        {
            // Inventory entry does not exist
            m_inventory.emplace(
                cakeIngredient.ingredientId,
                returnedQuantity
                );
        }
        else
        {
            inventoryIt->second += returnedQuantity;
        }
    }

    // Remove cake from Daily
    if (!dailyIt->second.removeCake(cakeId))
    {
        return false;
    }

    markAsChanged();

    return true;
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
