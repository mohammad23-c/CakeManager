#ifndef DAILYCAKECARD_H
#define DAILYCAKECARD_H

#include <QWidget>
#include "../../managers/appmanager.h"

class QLabel;
class QHBoxLayout;
class QMouseEvent;

class DailyCakeCard : public QWidget
{
    Q_OBJECT

public:

    explicit DailyCakeCard(
        qint64 cakeId,
        double quantity,
        AppManager& appManager,
        QWidget* parent = nullptr
        );

    ~DailyCakeCard();

    // =========================================
    // Cake ID
    // =========================================

    qint64 getCakeId() const;

    void setCakeId(qint64 cakeId);

    // =========================================
    // Quantity
    // =========================================

    double getQuantity() const;

    void setQuantity(double quantity);

    // =========================================
    // Update Card
    // =========================================

    void updateCard(
        const QString& name,
        double quantity
        );

signals:

    void clicked(qint64 cakeId);

protected:

    void mousePressEvent(
        QMouseEvent* event
        ) override;

private:

    AppManager& m_appManager;

    QLabel* m_nameLabel;
    QLabel* m_quantityLabel;

    QHBoxLayout* m_layout;

    qint64 m_cakeId;

    QString m_name;

    double m_quantity;
};

#endif // DAILYCAKECARD_H