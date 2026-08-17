#ifndef CAKE_H
#define CAKE_H

#include <QString>
#include <vector>
#include <cstdint>
#include <optional>

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
        double profitPercentage,
        const std::vector<CakeIngredient>& ingredients
        );
    Cake(const Cake &other)=default;

    // Copy assignment operator
    Cake& operator=(const Cake& other)=default;

    // Move constructor
    Cake(Cake&& other) noexcept=default;

    // Move assignment operator
    Cake& operator=(Cake&& other) noexcept=default;

    ~Cake() = default;


    double getProfitPercentage() const;
    void setProfitPercentage(double profitPercentage);

    void setId(qint64 id);
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
    //this func find ingredient in cake by id, if found return index of ingredient in vector, else return std::nullopt
    std::optional<size_t> findIngredient(qint64 ingredientId) const;
private:
    qint64 m_id;
    QString m_name;
    //this profitPercentage is percentage of profit that cake will give, it is used to calculate selling price of cake
    double m_profitPercentage;

    //this vec store ingredients of cake, each ingredient has id and quantity
    std::vector<CakeIngredient> m_ingredients;
};

#endif // CAKE_H