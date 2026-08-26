#ifndef CAKESELECTIONDIALOG_H
#define CAKESELECTIONDIALOG_H

#include <QDialog>
#include <QRadioButton>
#include <QScrollArea>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QHBoxLayout>
#include <unordered_map>

#include "../../managers/appmanager.h"
#include "../CakeCard/cakecard.h"

namespace Ui {
class CakeSelectionDialog;
}
class QLineEdit;
// =========================================
// Cake Selection Item
// =========================================

struct CakeSelectionItem
{
    CakeCard* card;
    QRadioButton* selectButton;
    QWidget* container;
};

class CakeSelectionDialog : public QDialog
{
    Q_OBJECT

public:

    explicit CakeSelectionDialog(
        AppManager& appManager,
        QWidget* parent = nullptr
        );
    qint64 getSelectedCakeId() const;

    //search
    std::vector<qint64> findCakesByName(
        const QString& searchText
        ) const;


    void clearCakeGrid();


    void loadCakeCards(
        const std::vector<qint64>& cakeIds
        );

    ~CakeSelectionDialog();
private slots:
    void onSearchTextChanged(
    const QString& text
    );
private:

    // =========================================
    // Load Cakes
    // =========================================

    void loadCakes();

    void loadCakeCard(
        qint64 cakeId
        );

    // =========================================
    // Check Cake
    // =========================================

    bool isCakeSelected(
        qint64 cakeId
        ) const;

    // =========================================
    // Cake Selection
    // =========================================

    void selectCake(
        qint64 cakeId
        );

    void deselectCake(
        qint64 cakeId
        );

    // =========================================
    // UI
    // =========================================

    Ui::CakeSelectionDialog* ui;

    AppManager& m_appManager;

    // =========================================
    // Scroll Area
    // =========================================

    QScrollArea* m_cakeScrollArea;

    QWidget* m_cakeContent;

    QGridLayout* m_cakeGrid;

    QLineEdit* m_searchLineEdit;
    // =========================================
    // Cake Items
    // =========================================

    std::unordered_map<
        qint64,
        CakeSelectionItem
        > m_cakeItems;

    // =========================================
    // Selected Cake
    // =========================================

    qint64 m_selectedCakeId;


};

#endif // CAKESELECTIONDIALOG_H