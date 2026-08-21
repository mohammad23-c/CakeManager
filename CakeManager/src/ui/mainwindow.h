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

    //load all ingredient and show in page ingredient
    void loadIngredients();
    void loadNewIngredientCard(qint64 id);
    void updateCard(qint64 ingredientId);
    void createIngredientPage();

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


    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    AppManager m_appManager;

    std::unordered_map<qint64, MaterialCard*> m_materialCards;

    // Ingredients page
    QScrollArea* m_ingredientScrollArea;
    QWidget* m_ingredientContent;
    QGridLayout* m_ingredientGrid;
    QPushButton* m_addIngredientButton;
};
#endif // MAINWINDOW_H
