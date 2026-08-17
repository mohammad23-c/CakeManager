#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <unordered_map>
#include <optional>
#include <vector>

#include "../models/ingredient.h"
#include "../models/cake.h"
#include "../database/DatabaseManager.h"

class AppManager
{
public:

    // =========================================
    // Constructor
    // =========================================

    explicit AppManager(const QString& databaseName);


    // =========================================
    // Application
    // =========================================

    // Load all data from database into memory
    bool initialize();

    // Save all current data to database
    bool save();
    bool ingredientSave();
    bool cakeSave();
    // =========================================
    // Ingredient
    // =========================================

    bool addIngredient(const Ingredient& ingredient);

    bool updateIngredient(const Ingredient& ingredient);

    bool deleteIngredient(qint64 id);

    std::optional<Ingredient> findIngredient(qint64 id) const;
    std::optional<Ingredient> findIngredient(QString name) const;

    // =========================================
    // Cake
    // =========================================

    bool addCake(const Cake& cake);

    bool updateCake(const Cake& cake);

    bool deleteCake(qint64 id);

    std::optional<Cake> findCake(qint64 id) const;
    std::optional<Cake> findCake(QString name) const;

    // =========================================
    // Cake / Ingredient Relations
    // =========================================

    bool isIngredientUsedInCake(qint64 ingredientId) const;

    bool isIngredientUsedInThisCake(qint64 ingredientId,qint64 cakeId) const;

    std::vector<qint64>
    getCakesUsingIngredient(qint64 ingredientId) const;


    // =========================================
    // Price
    // =========================================

    std::optional<double>
    calculateCakeCost(qint64 cakeId) const;

    std::optional<double>
    calculateCakeFinalPrice(qint64 cakeId) const;
    //========================================
    // Unsaved Changes
    // ========================================
    //if app have any changes without saveing to database this func retuns true else false
     bool hasUnsavedChanges() const;

    //========================================
    //check contains
    //========================================
        //ingredients
     bool containsIngredient(qint64 id) const;

     bool containsIngredient(const QString& name) const;
     //this contain check that ingredient with name exist in app or not except the ingredient with exceptId, this is useful when we want to update ingredient name and we want to check if the new name already exist in app or not
     bool containsIngredient(
         const QString& name,
         qint64 exceptId
         ) const;

    //cakes
     bool containsCake(qint64 id) const;
     bool containsCake(const QString& name) const;
     bool containsCake(const QString& name,
                      qint64 exceptId) const;

private:

    // =========================================
    // Database
    // =========================================

    QString m_databaseName;

    DatabaseManager m_database;


    // =========================================
    // Application Data
    // =========================================

    std::unordered_map<qint64, Ingredient> m_ingredients;

    std::unordered_map<qint64, Cake> m_cakes;

    qint64 m_nextIngredientId=1;
    qint64 m_nextCakeId=1;
    // =========================================
    // Unsaved Changes
    // ========================================
    //this bool contain save state of app if any changes happend in app without saving to database this bool will be true else false
    bool m_hasUnsavedChanges=false;
    //this func call when any changes happend in app without saving to database this func will set m_hasUnsavedChanges to true
    void markAsChanged();
};

#endif // APPMANAGER_H