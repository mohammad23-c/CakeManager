#include "ingredient.h"

Ingredient::Ingredient()
    : m_id(0),
    m_name(""),
    m_unit(Unit::Kilogram),
    m_pricePerUnit(0)
{
}

Ingredient::Ingredient(
    qint64 id,
    const QString& name,
    Unit unit,
    qint64 pricePerUnit,
    const QString& imagePath
    )
    : m_id(id),
    m_name(name),
    m_unit(unit),
    m_pricePerUnit(pricePerUnit),
    m_imagePath(imagePath)
{
}

qint64 Ingredient::getId() const
{
    return m_id;
}

void Ingredient::setId(qint64 id)
{
    m_id = id;
}

QString Ingredient::getName() const
{
    return m_name;
}

void Ingredient::setName(const QString& name)
{
    m_name = name;
}

Ingredient::Unit Ingredient::getUnit() const
{
    return m_unit;
}

void Ingredient::setUnit(Unit unit)
{
    m_unit = unit;
}

qint64 Ingredient::getPricePerUnit() const
{
    return m_pricePerUnit;
}

void Ingredient::setPricePerUnit(qint64 pricePerUnit)
{
    m_pricePerUnit = pricePerUnit;
}

QString Ingredient::getImagePath() const
{
    return m_imagePath;
}

void Ingredient::setImagePath(const QString& imagePath)
{
    m_imagePath = imagePath;
}

QString Ingredient::unitToString(Unit u)
{
    switch (u)
    {
    case Unit::Kilogram:
        return "Kilogram";
    case Unit::Gram:
        return "Gram";
    case Unit::Piece:
        return "Piece";
    default:
        return " Unknown";
    }
}

QString Ingredient::toString() const
{

    return QString("Ingredient{id=%1, name=%2, unit=%3, pricePerUnit=%4, imagePath=%5}")
    .arg(m_id)
        .arg(m_name)
        .arg(Ingredient::unitToString(m_unit))
        .arg(m_pricePerUnit)
        .arg(m_imagePath);

}