#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MaterialCard/addingredientdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      m_appManager("primaryDataBase.db")
{
    ui->setupUi(this);
    m_appManager.initialize();
    //load ingredient cards from app manager
    loadIngredients();
}

void MainWindow::loadIngredients()
{
    auto ingredients = m_appManager.getIngredients();

    for (const auto& ingredient : ingredients)
    {
        loadNewIngredientCard(ingredient.getId());
    }
}

void MainWindow::loadNewIngredientCard(qint64 id)
{
    auto ingredient = m_appManager.findIngredient(id);

    if (!ingredient.has_value())
    {
        return;
    }

    auto* card = new MaterialCard(
        ingredient->getId(),
        ingredient->getName(),
        ingredient->getImagePath(),
        ui->materialGridLayout->parentWidget()
        );

    m_materialCards[ingredient->getId()] = card;

    connect(
        card,
        &MaterialCard::clicked,
        this,
        &MainWindow::createIngredientEditDialog
        );

    int index = ui->materialGridLayout->count();

    int row = index / 1;
    int column = index % 1;

    ui->materialGridLayout->addWidget(card, row, column);
}

void MainWindow::updateCard(qint64 ingredientId)
{
    auto ingredient = m_appManager.findIngredient(ingredientId);

    if (!ingredient.has_value())
    {
        return;
    }

    auto card = m_materialCards.find(ingredientId);

    if (card == m_materialCards.end())
    {
        return;
    }

    card->second->updateCard(
        ingredient->getName(),
        ingredient->getImagePath()
        );
}

MainWindow::~MainWindow()
{
    if(m_appManager.hasUnsavedChanges()){
        m_appManager.save();
    }
    delete ui;
}

void MainWindow::on_Homebtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(HomePage);
}


void MainWindow::on_Ingredientbtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(IngredientsPage);
}


void MainWindow::on_Cakebtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(CakesPage);
}


void MainWindow::on_DAILYbtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(DailyPage);
}

void MainWindow::createIngredientEditDialog(qint64 ingredientId)
{
    MaterialDialog dialog(
        ingredientId,
        m_appManager,
        this
        );

    if (dialog.exec() == QDialog::Accepted)
    {
        updateCard(ingredientId);
    }
}
//add dialog push btn
void MainWindow::on_pushButton_clicked()
{
    AddIngredientDialog dialog(
    m_appManager,
    this
    );
    if(dialog.exec() == QDialog::Accepted){
        loadNewIngredientCard(dialog.getNewIngredientId());
    }
}

