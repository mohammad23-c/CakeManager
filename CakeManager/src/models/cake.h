#ifndef CAKE_H
#define CAKE_H

#include <QString>
#include <vector>
#include <cstdint>

struct CakeIngredient
{
    qint64 ingredientId;
    double quantity;
};

class Cake
{
public:
    Cake();

    Cake(
        qint64 id,
        const QString& name,
        const std::vector<CakeIngredient>& ingredients
        );

    qint64 getId() const;
    QString getName() const;
    const std::vector<CakeIngredient>& getIngredients() const;

    void setName(const QString& name);
    void setIngredients(
        const std::vector<CakeIngredient>& ingredients
        );

    void addIngredient(
        qint64 ingredientId,
        double quantity
        );

private:
    qint64 m_id;
    QString m_name;
    std::vector<CakeIngredient> m_ingredients;
};

#endif // CAKE_H