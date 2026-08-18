#ifndef DAILY_H
#define DAILY_H

#include <QDate>
#include <QString>

#include <vector>
#include <optional>

struct DailyCake
{
    qint64 cakeId;
    double quantity;
};

struct DailySummary
{
    double totalCost;
    double totalSales;
};

class Daily
{
public:

    // =========================================
    // Constructors
    // =========================================

    Daily();

    Daily(
        qint64 id,
        const QDate& date,
        const std::vector<DailyCake>& cakes
        );


    // =========================================
    // Getters
    // =========================================

    qint64 getId() const;

    QDate getDate() const;

    const std::vector<DailyCake>& getCakes() const;

    DailySummary getSummary() const;


    // =========================================
    // Setters
    // =========================================

    void setDate(const QDate& date);

    void setCakes(const std::vector<DailyCake>& cakes);

    void setSummary(const DailySummary& summary);

    // =========================================
    // Cake Management
    // =========================================

    // Adds a cake to the daily orders.
    // If the cake already exists, its quantity is increased.
    bool addCake(qint64 cakeId, double quantity);

    // Removes a cake from the daily orders.
    bool removeCake(qint64 cakeId);

    // Checks whether the cake exists in this daily.
    bool containsCake(qint64 cakeId) const;

    //this func find cake by id
    std::optional<DailyCake> findCake(qint64 cakeId) const;

private:

    // =========================================
    // Data
    // =========================================

    qint64 m_id;

    QDate m_date;

    std::vector<DailyCake> m_cakes;

    DailySummary m_summary;
};


#endif // DAILY_H