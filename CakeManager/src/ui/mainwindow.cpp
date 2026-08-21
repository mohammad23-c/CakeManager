#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MaterialCard/addingredientdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_appManager("primaryDataBase.db")
    , m_ingredientScrollArea(nullptr)
    , m_ingredientContent(nullptr)
    , m_ingredientGrid(nullptr)
{
    ui->setupUi(this);

    m_appManager.initialize();

    createIngredientPage();
    // ==============================
    // Load Ingredients
    // ==============================

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
        m_ingredientContent
        );

    m_materialCards[ingredient->getId()] = card;

    connect(
        card,
        &MaterialCard::clicked,
        this,
        &MainWindow::createIngredientEditDialog
        );

    int index = m_ingredientGrid->count();

    int row = index / 4;
    int column = index % 4;

    m_ingredientGrid->addWidget(
        card,
        row,
        column
        );
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

void MainWindow::createIngredientPage()
{
    QWidget* ingredientsPage =
        ui->stackedWidget->widget(IngredientsPage);

    // Main layout
    auto* ingredientsLayout =
        new QVBoxLayout(ingredientsPage);

    ingredientsLayout->setContentsMargins(
        0, 0, 0, 0
        );

    ingredientsLayout->setSpacing(0);

    // Scroll Area
    m_ingredientScrollArea =
        new QScrollArea(ingredientsPage);

    m_ingredientScrollArea->setWidgetResizable(true);

    m_ingredientScrollArea->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
        );

    m_ingredientScrollArea->setVerticalScrollBarPolicy(
        Qt::ScrollBarAsNeeded
        );

    // Content Widget
    m_ingredientContent =
        new QWidget();

    // Grid
    m_ingredientGrid =
        new QGridLayout(m_ingredientContent);

    m_ingredientGrid->setContentsMargins(
        15, 15, 15, 15
        );

    m_ingredientGrid->setHorizontalSpacing(15);
    m_ingredientGrid->setVerticalSpacing(15);

    // Set content
    m_ingredientScrollArea->setWidget(
        m_ingredientContent
        );

    //add btn
    auto* topLayout = new QHBoxLayout;

    m_addIngredientButton =
        new QPushButton("Add Ingredient", ingredientsPage);

    m_addIngredientButton->setFixedHeight(40);

    connect(
        m_addIngredientButton,
        &QPushButton::clicked,
        this,
        &MainWindow::on_pushButton_clicked
        );


    topLayout->addStretch();
    topLayout->addWidget(m_addIngredientButton);

    ingredientsLayout->addLayout(topLayout);
    // Add scroll area to page
    ingredientsLayout->addWidget(
        m_ingredientScrollArea
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
    connect(
        &dialog,
        &MaterialDialog::deleteIng,
        this,
        &MainWindow::deleteIngredientCard
        );
    if (dialog.exec() == QDialog::Accepted)
    {
        updateCard(ingredientId);
    }
}

void MainWindow::deleteIngredientCard(qint64 ingredientId)
{
    auto it = m_materialCards.find(ingredientId);

    if (it == m_materialCards.end())
    {
        return;
    }

    MaterialCard* card = it->second;

    m_ingredientGrid->removeWidget(card);

    m_materialCards.erase(it);

    delete card;
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

