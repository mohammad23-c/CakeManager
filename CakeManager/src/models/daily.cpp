#include "daily.h"

// =========================================
// Constructors
// =========================================

Daily::Daily()
    : m_id(0),
    m_date(),
    m_cakes(),
    m_summary{0.0, 0.0}
{
}

Daily::Daily(
    qint64 id,
    const QDate& date,
    const std::vector<DailyCake>& cakes
    )
    : m_id(id),
    m_date(date),
    m_cakes(cakes),
    m_summary{0.0, 0.0}
{
}

// =========================================
// Getters
// =========================================

qint64 Daily::getId() const
{
    return m_id;
}

QDate Daily::getDate() const
{
    return m_date;
}

const std::vector<DailyCake>& Daily::getCakes() const
{
    return m_cakes;
}

// =========================================
// Summary
// =========================================

DailySummary Daily::getSummary() const
{
    return m_summary;
}


// =========================================
// Setters
// =========================================

void Daily::setDate(const QDate& date)
{
    m_date = date;
}

void Daily::setCakes(const std::vector<DailyCake>& cakes)
{
    m_cakes = cakes;
}

void Daily::setSummary(const DailySummary& summary)
{
    m_summary = summary;
}

void Daily::setId(qint64 id)
{
    this->m_id=id;
}

// =========================================
// Cake Management
// =========================================

bool Daily::addCake(qint64 cakeId, double quantity)
{
    // Quantity must be positive
    if (quantity <= 0)
    {
        return false;
    }

    // Check if cake already exists
    for (auto& cake : m_cakes)
    {
        if (cake.cakeId == cakeId)
        {
            cake.quantity += quantity;
            return true;
        }
    }

    // Cake does not exist
    m_cakes.push_back({cakeId, quantity});

    return true;
}


bool Daily::removeCake(qint64 cakeId)
{
    for (auto it = m_cakes.begin(); it != m_cakes.end(); ++it)
    {
        if (it->cakeId == cakeId)
        {
            m_cakes.erase(it);
            return true;
        }
    }

    return false;
}


bool Daily::containsCake(qint64 cakeId) const
{
    for (const auto& cake : m_cakes)
    {
        if (cake.cakeId == cakeId)
        {
            return true;
        }
    }

    return false;
}

std::optional<DailyCake> Daily::findCake(qint64 cakeId) const
{
    for (const auto& cake : m_cakes)
    {
        if (cake.cakeId == cakeId)
        {
            return cake;
        }
    }

    return std::nullopt;
}