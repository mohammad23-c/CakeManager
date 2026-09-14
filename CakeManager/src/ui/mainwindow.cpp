#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MaterialCard/addingredientdialog.h"
#include <QMessageBox>
#include "CakeCard/addcakedialog.h"
#include "CakeCard/cakeCardSetting.h"
#include "DailyCard/dailyCakeCardSetting.h"
#include "DailyCard/AddCakeToDaily.h"
#include "../utils/outputcontroll.h"
#include <QLabel>
#include <QStandardPaths>
#include <QDir>


//this func use to create path for database
QString getDatabasePath()
{
    QString path =
        QStandardPaths::writableLocation(
            QStandardPaths::AppLocalDataLocation
            );

    QDir().mkpath(path);
    qDebug() << "Database path:" << path;
    return path + "/primaryDataBase.db";
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,m_appManager(getDatabasePath())
    , m_ingredientScrollArea(nullptr)
    , m_ingredientContent(nullptr)
    , m_ingredientGrid(nullptr)
{

    ui->setupUi(this);

    QWidget* cakesPage = new QWidget();
    QWidget* dailyPage= new QWidget();

    ui->stackedWidget->addWidget(cakesPage);
    ui->stackedWidget->addWidget(dailyPage);

    m_appManager.initialize();

    createIngredientPage();
    createCakePage();
    createDailyPage();
    // ==============================
    // Load Ingredients
    // ==============================
    loadIngredients();
    loadCakes();

    m_currentDailyDate=QDate::currentDate();
    loadCurrentDaily();
    qDebug() << "Qt version:" << QT_VERSION_STR;
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

    // =========================================
    // Main Layout
    // =========================================

    auto* ingredientsLayout =
        new QVBoxLayout(ingredientsPage);

    ingredientsLayout->setContentsMargins(
        0, 0, 0, 0
        );

    ingredientsLayout->setSpacing(0);

    // =========================================
    // Top Layout
    // =========================================

    auto* topLayout =
        new QHBoxLayout;

    // Search
    m_ingredientSearch =
        new QLineEdit(ingredientsPage);

    m_ingredientSearch->setPlaceholderText(
        "جستجو"
        );

    m_ingredientSearch->setFixedHeight(40);

    // Add Button
    m_addIngredientButton =
        new QPushButton(
            "اضافه کردن مواد اولیه",
            ingredientsPage
            );

    m_addIngredientButton->setFixedHeight(40);

    connect(
        m_addIngredientButton,
        &QPushButton::clicked,
        this,
        &MainWindow::on_pushButton_clicked
        );

    // =========================================
    // Top Layout
    // =========================================

    topLayout->addWidget(
        m_ingredientSearch,
        1
        );

    topLayout->addWidget(
        m_addIngredientButton
        );

    ingredientsLayout->addLayout(
        topLayout
        );

    // =========================================
    // Scroll Area
    // =========================================

    m_ingredientScrollArea =
        new QScrollArea(ingredientsPage);

    m_ingredientScrollArea->setWidgetResizable(true);

    m_ingredientScrollArea->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
        );

    m_ingredientScrollArea->setVerticalScrollBarPolicy(
        Qt::ScrollBarAsNeeded
        );

    // =========================================
    // Content Widget
    // =========================================

    m_ingredientContent =
        new QWidget();

    // =========================================
    // Grid
    // =========================================

    m_ingredientGrid =
        new QGridLayout(m_ingredientContent);

    m_ingredientGrid->setContentsMargins(
        15, 15, 15, 15
        );

    m_ingredientGrid->setHorizontalSpacing(15);
    m_ingredientGrid->setVerticalSpacing(15);

    // =========================================
    // Set Content
    // =========================================

    m_ingredientScrollArea->setWidget(
        m_ingredientContent
        );

    // =========================================
    // Add Scroll Area
    // =========================================

    ingredientsLayout->addWidget(
        m_ingredientScrollArea
        );


    //connect search line to its slot
    connect(
        m_ingredientSearch,
        &QLineEdit::textChanged,
        this,
        &MainWindow::onIngredientSearchChanged
        );
}

std::vector<qint64> MainWindow::findIngredientsByName(
    const QString& searchText
    ) const
{
    std::vector<qint64> result;

    for (const auto& [ingredientId, card] : m_materialCards)
    {
        if (card->getName().contains(
                searchText,
                Qt::CaseInsensitive
                ))
        {
            result.push_back(ingredientId);
        }
    }

    return result;
}

void MainWindow::clearIngredientGrid()
{
    while (QLayoutItem* item = m_ingredientGrid->takeAt(0))
    {
        if (QWidget* widget = item->widget())
        {
            widget->hide();
        }

        delete item;
    }
}

void MainWindow::loadIngredientCards(
    const std::vector<qint64>& ingredientIds
    )
{
    int index = 0;

    for (qint64 ingredientId : ingredientIds)
    {
        auto it =
            m_materialCards.find(ingredientId);

        if (it == m_materialCards.end())
        {
            continue;
        }

        MaterialCard* card =
            it->second;

        int row = index / 4;
        int column = index % 4;

        m_ingredientGrid->addWidget(
            card,
            row,
            column
            );

        card->show();

        ++index;
    }
}

void MainWindow::reloadIngredientCards()
{
    // حذف تمام کارت‌های فعلی
    while (QLayoutItem* item = m_ingredientGrid->takeAt(0))
    {
        if (QWidget* widget = item->widget())
        {
            delete widget;
        }

        delete item;
    }

    m_materialCards.clear();

    // ساخت دوباره کارت‌ها از روی AppManager
    loadIngredients();
}

void MainWindow::createCakePage()
{
    QWidget* cakesPage =
        ui->stackedWidget->widget(CakesPage);

    // =========================================
    // Main Layout
    // =========================================

    auto* cakesLayout =
        new QVBoxLayout(cakesPage);

    cakesLayout->setContentsMargins(
        0, 0, 0, 0
        );

    cakesLayout->setSpacing(0);

    // =========================================
    // Scroll Area
    // =========================================

    m_cakeScrollArea =
        new QScrollArea(cakesPage);

    m_cakeScrollArea->setWidgetResizable(true);

    m_cakeScrollArea->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
        );

    m_cakeScrollArea->setVerticalScrollBarPolicy(
        Qt::ScrollBarAsNeeded
        );

    // =========================================
    // Content Widget
    // =========================================

    m_cakeContent =
        new QWidget();

    // =========================================
    // Grid
    // =========================================

    m_cakeGrid =
        new QGridLayout(m_cakeContent);

    m_cakeGrid->setContentsMargins(
        15, 15, 15, 15
        );

    m_cakeGrid->setHorizontalSpacing(15);
    m_cakeGrid->setVerticalSpacing(15);

    // =========================================
    // Set Scroll Content
    // =========================================

    m_cakeScrollArea->setWidget(
        m_cakeContent
        );

    // =========================================
    // Top Layout
    // =========================================

    auto* topLayout =
        new QHBoxLayout;

    // Search
    m_cakeSearch =
        new QLineEdit(cakesPage);

    m_cakeSearch->setPlaceholderText(
        "جستجو"
        );

    m_cakeSearch->setFixedHeight(40);

    // Add Cake Button
    m_addCakeButton =
        new QPushButton(
            "اضافه کردن کیک",
            cakesPage
            );

    m_addCakeButton->setFixedHeight(40);

    connect(
        m_addCakeButton,
        &QPushButton::clicked,
        this,
        &MainWindow::on_addCakeButton_clicked
        );

    // =========================================
    // Top Layout
    // =========================================

    topLayout->addWidget(
        m_cakeSearch,
        1
        );

    topLayout->addWidget(
        m_addCakeButton
        );

    // =========================================
    // Add To Page
    // =========================================

    cakesLayout->addLayout(
        topLayout
        );

    cakesLayout->addWidget(
        m_cakeScrollArea
        );

    //conncet cake search box to its slot
    connect(
        m_cakeSearch,
        &QLineEdit::textChanged,
        this,
        &MainWindow::onCakeSearchChanged
        );
}

void MainWindow::loadCakes()
{
    auto cakes = m_appManager.getCakes();

    for (const auto& cake : cakes)
    {
        loadNewCakeCard(cake.getId());
    }
}

void MainWindow::loadNewCakeCard(qint64 id)
{
    auto cake = m_appManager.findCake(id);

    if (!cake.has_value())
    {
        return;
    }

    auto* card = new CakeCard(
        cake->getId(),
        cake->getName(),
        cake->getImagePath(),
        m_cakeContent
        );

    m_cakeCards[cake->getId()] = card;

    connect(
        card,
        &CakeCard::clicked,
        this,
        &MainWindow::createCakeEditDialog
        );

    int index = m_cakeGrid->count();

    int row = index / 4;
    int column = index % 4;

    m_cakeGrid->addWidget(
        card,
        row,
        column
        );
}

void MainWindow::updateCakeCard(qint64 cakeId)
{
    auto cake = m_appManager.findCake(cakeId);

    if (!cake.has_value())
    {
        return;
    }

    auto card = m_cakeCards.find(cakeId);

    if (card == m_cakeCards.end())
    {
        return;
    }

    card->second->updateCard(
        cake->getName(),
        cake->getImagePath()
        );
}

std::vector<qint64> MainWindow::findCakesByName(
    const QString& searchText
    ) const
{
    std::vector<qint64> result;

    QString text =
        searchText.trimmed();

    for (const auto& [id, card] : m_cakeCards)
    {
        if (card->getName()
                .contains(
                    text,
                    Qt::CaseInsensitive
                    ))
        {
            result.push_back(id);
        }
    }

    return result;
}

void MainWindow::clearCakeGrid()
{
    while (QLayoutItem* item = m_cakeGrid->takeAt(0))
    {
        if (QWidget* widget = item->widget())
        {
            widget->hide();
        }

        delete item;
    }
}

void MainWindow::loadCakeCards(
    const std::vector<qint64>& cakeIds
    )
{
    int index = 0;

    for (qint64 cakeId : cakeIds)
    {
        auto it =
            m_cakeCards.find(cakeId);

        if (it == m_cakeCards.end())
        {
            continue;
        }

        CakeCard* card =
            it->second;

        int row =
            index / 4;

        int column =
            index % 4;

        m_cakeGrid->addWidget(
            card,
            row,
            column
            );

        card->show();

        ++index;
    }
}

void MainWindow::createDailyPage()
{
    QWidget* dailyPage =
        ui->stackedWidget->widget(DailyPage);

    // =========================================
    // Main Layout
    // =========================================

    auto* dailyLayout =
        new QVBoxLayout(dailyPage);

    dailyLayout->setContentsMargins(
        0, 0, 0, 0
        );

    dailyLayout->setSpacing(0);

    // =========================================
    // Top Layout
    // =========================================

    auto* topLayout =
        new QHBoxLayout;

    // =========================================
    // Date Edit
    // =========================================

    m_dailyDateEdit =
        new QDateEdit(dailyPage);

    m_dailyDateEdit->setCalendar(
        QCalendar(QCalendar::System::Jalali)
        );

    m_dailyDateEdit->setDisplayFormat(
        "yyyy/MM/dd"
        );

    m_dailyDateEdit->setDate(
        QDate::currentDate()
        );

    m_dailyDateEdit->setCalendarPopup(true);

    // =========================================
    // Add Cake Button
    // =========================================

    m_addDailyCakeButton =
        new QPushButton(
            "اضافه کردن کیک",
            dailyPage
            );

    m_addDailyCakeButton->setFixedHeight(40);

    // =========================================
    // Top Layout
    // =========================================

    topLayout->addWidget(
        m_dailyDateEdit
        );

    topLayout->addStretch();

    topLayout->addWidget(
        m_addDailyCakeButton
        );

    // =========================================
    // Scroll Area
    // =========================================

    m_dailyScrollArea =
        new QScrollArea(dailyPage);

    m_dailyScrollArea->setWidgetResizable(true);

    m_dailyScrollArea->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
        );

    m_dailyScrollArea->setVerticalScrollBarPolicy(
        Qt::ScrollBarAsNeeded
        );

    // =========================================
    // Content
    // =========================================

    m_dailyContent =
        new QWidget();

    m_dailyLayout =
        new QVBoxLayout(m_dailyContent);

    m_dailyLayout->setContentsMargins(
        15, 15, 15, 15
        );

    m_dailyLayout->setSpacing(10);

    m_dailyScrollArea->setWidget(
        m_dailyContent
        );

    // =========================================
    // Summary Layout
    // =========================================

    auto* summaryLayout =
        new QHBoxLayout;

    summaryLayout->setContentsMargins(
        15, 10, 15, 10
        );

    summaryLayout->setSpacing(20);

    // =========================================
    // Total Cost
    // =========================================

    m_dailyTotalCostLabel =
        new QLabel(
            "Total Cost: 0",
            dailyPage
            );

    // =========================================
    // Total Profit
    // =========================================

    m_dailyTotalSalesLabel =
        new QLabel(
            "Total Sales: 0",
            dailyPage
            );

    summaryLayout->addWidget(
        m_dailyTotalCostLabel
        );

    summaryLayout->addStretch();

    summaryLayout->addWidget(
        m_dailyTotalSalesLabel
        );

    // =========================================
    // Add To Page
    // =========================================

    dailyLayout->addLayout(
        topLayout
        );

    dailyLayout->addWidget(
        m_dailyScrollArea
        );

    dailyLayout->addLayout(
        summaryLayout
        );

    // =========================================
    // Date Changed Signal
    // =========================================

    connect(
        m_dailyDateEdit,
        &QDateEdit::dateChanged,
        this,
        &MainWindow::onDailyDateChanged
        );

    // =========================================
    // Add Cake Button
    // =========================================

    connect(
        m_addDailyCakeButton,
        &QPushButton::clicked,
        this,
        &MainWindow::onAddDailyCakeClicked
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
    loadCurrentDaily();
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
    Q_UNUSED(ingredientId);

    reloadIngredientCards();
}

void MainWindow::createCakeEditDialog(qint64 cakeId)
{
    CakeCardSetting dialog(
        m_appManager,
        cakeId,
        this
        );

    connect(
        &dialog,
        &CakeCardSetting::deleteCake,
        this,
        &MainWindow::deleteCakeCard
        );

    if (dialog.exec() == QDialog::Accepted)
    {
        updateCakeCard(cakeId);
    }
}

void MainWindow::on_addCakeButton_clicked()
{
    AddCakeDialog dialog(
        m_appManager,
        this
        );

    if (dialog.exec() == QDialog::Accepted)
    {
        loadNewCakeCard(
            dialog.getNewCakeId()
            );
    }
}

void MainWindow::deleteCakeCard(qint64 cakeId)
{
    auto it=m_cakeCards.find(cakeId);
    if(it==m_cakeCards.end())
        return;

    CakeCard * card=it->second;
    m_cakeGrid->removeWidget(card);
    m_cakeCards.erase(it);
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

void MainWindow::onDailyDateChanged(const QDate& date)
{
    // Save previous date
    saveCurrentDaily();

    // Change current date
    m_currentDailyDate = date;

    // Load new date
    loadCurrentDaily();
}

void MainWindow::createDailyCakeEditDialog(qint64 cakeId)
{
    dailyCakeCardSetting dialog(
        m_appManager,
        m_currentDaily.getId(),
        cakeId,
        this
        );

    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }

    auto daily =
        m_appManager.findDaily(
            m_currentDaily.getDate()
            );

    if (!daily.has_value())
    {
        return;
    }
    m_currentDaily = daily.value();
    if(!m_currentDaily.containsCake(cakeId)){
        removeDailyCakeCard(cakeId);
        return;
    }
    updateDailyCakeCard(cakeId);
}

void MainWindow::onAddDailyCakeClicked()
{
    AddCakeToDaily dialog(
        m_appManager,
        m_currentDailyDate,
        this
        );

    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }

    loadCurrentDaily();
}

void MainWindow::onIngredientSearchChanged(
    const QString& text
    )
{
    auto ingredientIds =
        findIngredientsByName(text);

    clearIngredientGrid();

    loadIngredientCards(
        ingredientIds
        );
}

void MainWindow::onCakeSearchChanged(
    const QString& text
    )
{
    auto cakeIds =
        findCakesByName(text);

    clearCakeGrid();

    loadCakeCards(
        cakeIds
        );
}

void MainWindow::loadCurrentDaily()
{
    // Remove cards from previous date
    clearDailyCakeCards();

    // Find or create Daily for current date
    auto daily =
        getOrCreateDaily(
            m_currentDailyDate
            );

    if (!daily.has_value())
    {
        return;
    }

    // Store current Daily
    m_currentDaily =
        daily.value();

    // Load cakes of current Daily
    const auto& cakes =
        m_currentDaily.getCakes();

    for (const auto& dailyCake : cakes)
    {
        loadNewDailyCakeCard(
            dailyCake.cakeId,
            dailyCake.quantity
            );
    }
    updateDailySummary();
}

void MainWindow::loadNewDailyCakeCard(
    qint64 cakeId,
    double quantity
    )
{
    auto cake =
        m_appManager.findCake(cakeId);

    if (!cake.has_value())
    {
        return;
    }

    auto* card =
        new DailyCakeCard(
            cakeId,
            quantity,
            m_appManager,
            m_dailyContent
            );

    m_dailyCakeCards[cakeId] =
        card;

    m_dailyLayout->addWidget(
        card
        );

    connect(
        card,
        &DailyCakeCard::clicked,
        this,
        &MainWindow::createDailyCakeEditDialog
        );
}

void MainWindow::saveCurrentDaily()
{
    m_appManager.updateDaily(
        m_currentDaily
        );
    m_appManager.save();
}

void MainWindow::clearDailyCakeCards()
{
    for (auto& pair : m_dailyCakeCards)
    {
        m_dailyLayout->removeWidget(
            pair.second
            );

        delete pair.second;
    }

    m_dailyCakeCards.clear();
}

std::optional<Daily> MainWindow::getOrCreateDaily(
    const QDate& date
    )
{
    // Check if Daily already exists
    auto daily =
        m_appManager.findDaily(date);

    if (daily.has_value())
    {
        return daily.value();
    }

    // Daily doesn't exist -> create it
    Daily newDaily(
        0,
        date,
        {}
        );

    if (!m_appManager.addDaily(newDaily))
    {
        return std::nullopt;
    }

    // Find the newly created Daily
    auto createdDaily =
        m_appManager.findDaily(date);

    if (!createdDaily.has_value())
    {
        return std::nullopt;
    }

    return createdDaily.value();
}

void MainWindow::updateDailyCakeCard(qint64 cakeId)
{
    auto it =
        m_dailyCakeCards.find(cakeId);

    if (it == m_dailyCakeCards.end())
    {
        return;
    }

    auto dailyCake =
        m_currentDaily.findCake(cakeId);

    if (!dailyCake.has_value())
    {
        DailyCakeCard* card = it->second;

        m_dailyLayout->removeWidget(card);

        m_dailyCakeCards.erase(it);

        delete card;

        return;
    }

    auto cake =
        m_appManager.findCake(cakeId);

    if (!cake.has_value())
    {
        return;
    }

    it->second->updateCard(
        cake->getName(),
        dailyCake->quantity
        );
    updateDailySummary();
}

void MainWindow::updateDailySummary()
{
    bool success =
        m_appManager.updateDailySummary(
            m_currentDaily.getId()
            );

    if (!success)
    {
        return;
    }

    auto daily =
        m_appManager.findDaily(
            m_currentDaily.getId()
            );

    if (!daily.has_value())
    {
        return;
    }

    m_currentDaily =
        daily.value();

    DailySummary summary =
        m_currentDaily.getSummary();

    m_dailyTotalCostLabel->setText(
        "Total Cost: " +
        outPutControll::formatPrice(summary.totalCost)
        );

    m_dailyTotalSalesLabel->setText(
        "Total Sales: " +
        outPutControll::formatPrice(summary.totalSales)
        );
}

void MainWindow::removeDailyCakeCard(qint64 cakeId)
{
    auto it = m_dailyCakeCards.find(cakeId);

    if(it == m_dailyCakeCards.end())
    {
        return;
    }

    auto* card = it->second;

    m_dailyLayout->removeWidget(card);

    m_dailyCakeCards.erase(it);
    delete card;
    updateDailySummary();
}

