#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QPushButton>

#include "../managers/appmanager.h"
#include "MaterialCard/materialcard.h"
#include "MaterialCard/materialdialog.h"
#include "DailyCard/dailyCakeCard.h"
#include "CakeCard/cakecard.h"
#include <QDateEdit>


QString getDatabasePath();

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

enum Page
{
    HomePage = 0,
    IngredientsPage,
    CakesPage,
    DailyPage
};
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    //============================================
    //ingredients
    //============================================

    //load all ingredient and show in page ingredient
    void loadIngredients();
    void loadNewIngredientCard(qint64 id);
    void updateCard(qint64 ingredientId);
    void createIngredientPage();
    //find ingredient card by its name
    std::vector<qint64> findIngredientsByName(
        const QString& searchText
        ) const;
    //clear page but not cardsMap
    void clearIngredientGrid();
    //load cakes that be in the vector
    void loadIngredientCards(const std::vector<qint64>& ingredientIds);

    void reloadIngredientCards();

    //=================================================================
    //cakes
    //=================================================================
    //load all cake and show in page cake
    void createCakePage();
    void loadCakes();
    void loadNewCakeCard(qint64 id);
    void updateCakeCard(qint64 id);
    // find cake card by its name
    std::vector<qint64> findCakesByName(
        const QString& searchText
        ) const;

    // clear page but not cardsMap
    void clearCakeGrid();

    // load cakes that be in the vector
    void loadCakeCards(
        const std::vector<qint64>& cakeIds
        );

    //=================================================================
    //daily
    //=================================================================
    //load all cake and show page date
    void createDailyPage();
    void loadCurrentDaily();
    void loadNewDailyCakeCard(
        qint64 cakeId,
        double quantity
        );
    void saveCurrentDaily();
    void clearDailyCakeCards();
    std::optional<Daily> getOrCreateDaily(
        const QDate& date
        );
    void updateDailyCakeCard(qint64 cakeId);
    void updateDailySummary();
    void removeDailyCakeCard(qint64 cakeId);
    ~MainWindow() override;

private slots:
    void on_Homebtn_clicked();

    void on_Ingredientbtn_clicked();

    void on_Cakebtn_clicked();

    void on_DAILYbtn_clicked();
    //this slot active when click on ingredient card and open edit ingredient dialog
    void createIngredientEditDialog(qint64 ingredientId);
    //when the signal from MaterialDialog emit that ingredient deleted, this slot will delete the card from ui and also from m_materialCards map
    void deleteIngredientCard(qint64 ingredientId);


    //cake
    void createCakeEditDialog(qint64 cakeId);
    void on_addCakeButton_clicked();
    void deleteCakeCard(qint64 cakeId);
    //add dialog push btn
    void on_pushButton_clicked();


    //daily
    void onDailyDateChanged(const QDate& date);
    void createDailyCakeEditDialog(qint64 cakeId);
    void onAddDailyCakeClicked();


    //search lines
    void onIngredientSearchChanged(const QString& text);
    void onCakeSearchChanged(const QString& text);

private:
    Ui::MainWindow *ui;

    AppManager m_appManager;

    std::unordered_map<qint64, MaterialCard*> m_materialCards;
    std::unordered_map<qint64, CakeCard*> m_cakeCards;
    std::unordered_map<qint64, DailyCakeCard*> m_dailyCakeCards;

    // Ingredients page
    QScrollArea* m_ingredientScrollArea;
    QWidget* m_ingredientContent;
    QGridLayout* m_ingredientGrid;
    QPushButton* m_addIngredientButton;
    QLineEdit* m_ingredientSearch;

    // Cakes page
    QScrollArea* m_cakeScrollArea;
    QWidget* m_cakeContent;
    QGridLayout* m_cakeGrid;
    QPushButton* m_addCakeButton;
    QLineEdit* m_cakeSearch;

    // Daily page
    QDateEdit* m_dailyDateEdit;
    QDate m_currentDailyDate;
    Daily m_currentDaily;

    QScrollArea* m_dailyScrollArea;
    QWidget* m_dailyContent;
    QVBoxLayout* m_dailyLayout;

    QPushButton* m_addDailyCakeButton;
    QLabel* m_dailyTotalCostLabel;
    QLabel* m_dailyTotalSalesLabel;


};
#endif // MAINWINDOW_H
