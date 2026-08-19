#include "cake.h"

Cake::Cake()
    : m_id(0),
    m_name(""),
    m_ingredients()
{
}

Cake::Cake(qint64 id, const QString &name, double profitPercentage, const std::vector<CakeIngredient> &ingredients, const QString &imagePath)
{
    m_id = id;
    m_name = name;
    m_profitPercentage = profitPercentage;
    m_ingredients = ingredients;
    m_imagePath = imagePath;
}

QString Cake::getImagePath() const
{
    return m_imagePath;
}

void Cake::setImagePath(const QString &imagePath)
{
    this->m_imagePath = imagePath;
}

double Cake::getProfitPercentage() const
{
    return m_profitPercentage;
}

void Cake::setProfitPercentage(double profitPercentage)
{
    this->m_profitPercentage = profitPercentage;
}

void Cake::setId(qint64 id)
{
    this->m_id=id;
}

qint64 Cake::getId() const
{
    return m_id;
}

QString Cake::getName() const
{
    return m_name;
}

const std::vector<CakeIngredient>& Cake::getIngredients() const
{
    return m_ingredients;
}

void Cake::setName(const QString& name)
{
    m_name = name;
}

void Cake::setIngredients(
    const std::vector<CakeIngredient>& ingredients
    )
{
    m_ingredients = ingredients;
}
//this func add ingredient to cake,if ingredient already exist in cake, it will update quantity of ingredient, else it will add new ingredient to cake
void Cake::addIngredient(
    qint64 ingredientId,
    double quantity
    )
{
    auto index = findIngredient(ingredientId);
    if(index.has_value()){
        m_ingredients[index.value()].quantity += quantity;
    }else{
        m_ingredients.push_back({
            ingredientId,
            quantity
        });
    }
}

std::optional<size_t> Cake::findIngredient(qint64 ingredientId) const
{

    for (size_t i = 0; i < m_ingredients.size(); ++i)
    {
        if (m_ingredients[i].ingredientId == ingredientId)
        {
            return i;
        }
    }

    return std::nullopt;
}

