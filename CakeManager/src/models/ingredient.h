#ifndef INGREDIENT_H
#define INGREDIENT_H

#include <QObject>
#include <QString>
#include <QtGlobal>

class Ingredient
{
public:
// unit have tree types: kilogram, gram, piece
// person can choose one of them when adding ingredient
    enum class Unit
    {
        Kilogram,
        Gram,
        Piece
    };
//constructors
    Ingredient();
    Ingredient(
        qint64 id,
        const QString& name,
        Unit unit,
        qint64 pricePerUnit,
        qint64 weightPerUnit,
        const QString& imagePath
        );

    // Copy
    Ingredient(const Ingredient& other) = default;
    Ingredient& operator=(const Ingredient& other) = default;

    // Move
    Ingredient(Ingredient&& other) noexcept = default;
    Ingredient& operator=(Ingredient&& other) noexcept = default;

    // Destructor
    ~Ingredient() = default;
//getter and setter
    qint64 getId() const;
    void setId(qint64 id);

    QString getName() const;
    void setName(const QString& name);

    Unit getUnit() const;
    void setUnit(Unit unit);

    qint64 getPricePerUnit() const;
    void setPricePerUnit(qint64 pricePerUnit);


    void setWeightPerUnit(qint64 gram);
    qint64 getWeightPerUnit() const;
//person can add image path for ingredient, this path will be stored in database
    QString getImagePath() const;
    void setImagePath(const QString& imagePath);

//this function will convert unit enum to string, for example: Unit::Kilogram -> "Kilogram"
    static QString unitToString(Unit u) ;
//this func will convert attr of object to string|use to show object in console for test purpose
    QString toString() const;

private:

    qint64 m_id;
    QString m_name;
    Unit m_unit;
    qint64 m_pricePerUnit;
    QString m_imagePath;
    qint64 m_weightPerUnit;
};


#endif // INGREDIENT_H
