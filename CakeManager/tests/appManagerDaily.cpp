#include <QCoreApplication>
#include <QFile>
#include <cassert>
#include <cmath>

#include "../src/managers/AppManager.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QFile::remove("testAppManagerDaily.db");

    AppManager manager("testAppManagerDaily.db");

    // =========================================================
    // Initialize
    // =========================================================

    assert(manager.initialize());


    // =========================================================
    // Ingredients
    // =========================================================

    Ingredient zargol(
        0,
        "ZargolX",
        Ingredient::Unit::Piece,
        5000,
        50,
        ""
        );

    Ingredient blorpMilk(
        0,
        "BlorpMilk",
        Ingredient::Unit::Kilogram,
        60000,
        0,
        ""
        );

    Ingredient chocoMango(
        0,
        "ChocoMango",
        Ingredient::Unit::Gram,
        300,
        0,
        ""
        );

    assert(manager.addIngredient(zargol));
    assert(manager.addIngredient(blorpMilk));
    assert(manager.addIngredient(chocoMango));


    // =========================================================
    // Find Ingredient IDs
    // =========================================================

    auto zargolOpt =
        manager.findIngredient("ZargolX");

    auto blorpMilkOpt =
        manager.findIngredient("BlorpMilk");

    auto chocoMangoOpt =
        manager.findIngredient("ChocoMango");

    assert(zargolOpt.has_value());
    assert(blorpMilkOpt.has_value());
    assert(chocoMangoOpt.has_value());

    qint64 zargolId =
        zargolOpt->getId();

    qint64 blorpMilkId =
        blorpMilkOpt->getId();

    qint64 chocoMangoId =
        chocoMangoOpt->getId();


    // =========================================================
    // Inventory
    // =========================================================

    // ZargolX -> Piece
    // BlorpMilk -> Kilogram
    // ChocoMango -> Gram

    assert(
        manager.addInventory(
            zargolId,
            20.0
            )
        );

    assert(
        manager.addInventory(
            blorpMilkId,
            10.0
            )
        );

    assert(
        manager.addInventory(
            chocoMangoId,
            2000.0
            )
        );


    // =========================================================
    // Cakes
    // =========================================================

    // ---------------------------------------------------------
    // Cake 1
    //
    // 2 eggs      -> 100 g
    // 0.5 kg milk -> 500 g
    // 200 g choc  -> 200 g
    //
    // Total = 800 g = 0.8 kg
    // ---------------------------------------------------------

    std::vector<CakeIngredient> cakeIngredients1 =
        {
            {zargolId, 2.0},
            {blorpMilkId, 0.5},
            {chocoMangoId, 200.0}
        };

    Cake qromboCake(
        0,
        "QromboCake",
        25.0,
        cakeIngredients1,
        ""
        );

    assert(manager.addCake(qromboCake));


    // ---------------------------------------------------------
    // Cake 2
    //
    // 3 eggs      -> 150 g
    // 0.25 kg milk -> 250 g
    // 300 g choc  -> 300 g
    //
    // Total = 700 g = 0.7 kg
    // ---------------------------------------------------------

    std::vector<CakeIngredient> cakeIngredients2 =
        {
            {zargolId, 3.0},
            {blorpMilkId, 0.25},
            {chocoMangoId, 300.0}
        };

    Cake flonkyCake(
        0,
        "FlonkyCake",
        30.0,
        cakeIngredients2,
        ""
        );

    assert(manager.addCake(flonkyCake));


    // ---------------------------------------------------------
    // Cake 3
    //
    // 1 egg       -> 50 g
    // 1 kg milk   -> 1000 g
    // 150 g choc  -> 150 g
    //
    // Total = 1200 g = 1.2 kg
    // ---------------------------------------------------------

    std::vector<CakeIngredient> cakeIngredients3 =
        {
            {zargolId, 1.0},
            {blorpMilkId, 1.0},
            {chocoMangoId, 150.0}
        };

    Cake grumpleCake(
        0,
        "GrumpleCake",
        20.0,
        cakeIngredients3,
        ""
        );

    assert(manager.addCake(grumpleCake));


    // =========================================================
    // Find Cake IDs
    // =========================================================

    auto qromboOpt =
        manager.findCake("QromboCake");

    auto flonkyOpt =
        manager.findCake("FlonkyCake");

    auto grumpleOpt =
        manager.findCake("GrumpleCake");

    assert(qromboOpt.has_value());
    assert(flonkyOpt.has_value());
    assert(grumpleOpt.has_value());

    qint64 qromboId =
        qromboOpt->getId();

    qint64 flonkyId =
        flonkyOpt->getId();

    qint64 grumpleId =
        grumpleOpt->getId();


    // =========================================================
    // Test Cake Weights
    // =========================================================

    auto qromboWeight =
        manager.calculateCakeWeight(qromboId);

    auto flonkyWeight =
        manager.calculateCakeWeight(flonkyId);

    auto grumpleWeight =
        manager.calculateCakeWeight(grumpleId);

    assert(qromboWeight.has_value());
    assert(flonkyWeight.has_value());
    assert(grumpleWeight.has_value());

    assert(
        std::abs(*qromboWeight - 0.8)
        < 0.000001
        );

    assert(
        std::abs(*flonkyWeight - 0.7)
        < 0.000001
        );

    assert(
        std::abs(*grumpleWeight - 1.2)
        < 0.000001
        );


    // =========================================================
    // Daily
    // =========================================================

    QDate testDate(2026, 8, 19);

    Daily testDaily(
        0,
        testDate,
        {}
        );

    assert(manager.addDaily(testDaily));

    auto dailyOpt =
        manager.findDaily(testDate);

    assert(dailyOpt.has_value());

    qint64 dailyId =
        dailyOpt->getId();


    // =========================================================
    // Add Cake By Quantity
    // =========================================================

    // Add 2 QromboCake

    assert(
        manager.addCakeToDaily(
            dailyId,
            qromboId,
            2.0,
            false
            )
        );


    auto dailyAfterQrombo =
        manager.findDaily(dailyId);

    assert(dailyAfterQrombo.has_value());

    auto qromboDailyCake =
        dailyAfterQrombo->findCake(qromboId);

    assert(qromboDailyCake.has_value());

    assert(
        qromboDailyCake->quantity == 2.0
        );


    // Inventory after 2 QromboCake:
    //
    // Egg:
    // 20 - (2 eggs × 2 cakes) = 16
    //
    // Milk:
    // 10 - (0.5 × 2) = 9
    //
    // Chocolate:
    // 2000 - (200 × 2) = 1600

    auto eggInventory =
        manager.findInventory(zargolId);

    auto milkInventory =
        manager.findInventory(blorpMilkId);

    auto chocolateInventory =
        manager.findInventory(chocoMangoId);

    assert(eggInventory.has_value());
    assert(milkInventory.has_value());
    assert(chocolateInventory.has_value());

    assert(*eggInventory == 16.0);
    assert(*milkInventory == 9.0);
    assert(*chocolateInventory == 1600.0);


    // =========================================================
    // Add Cake By Weight
    // =========================================================

    // QromboCake weighs 0.8 kg.
    //
    // User enters 2.4 kg.
    //
    // 2.4 / 0.8 = 3 cakes

    assert(
        manager.addCakeToDaily(
            dailyId,
            qromboId,
            2.4,
            true
            )
        );

    dailyAfterQrombo =
        manager.findDaily(dailyId);

    assert(dailyAfterQrombo.has_value());

    qromboDailyCake =
        dailyAfterQrombo->findCake(qromboId);

    assert(qromboDailyCake.has_value());

    // Previous 2 + new 3 = 5

    assert(
        qromboDailyCake->quantity == 5.0
        );


    // Inventory should now contain:
    //
    // Egg:
    // 16 - (2 × 3) = 10
    //
    // Milk:
    // 9 - (0.5 × 3) = 7.5
    //
    // Chocolate:
    // 1600 - (200 × 3) = 1000

    eggInventory =
        manager.findInventory(zargolId);

    milkInventory =
        manager.findInventory(blorpMilkId);

    chocolateInventory =
        manager.findInventory(chocoMangoId);

    assert(eggInventory.has_value());
    assert(milkInventory.has_value());
    assert(chocolateInventory.has_value());

    assert(*eggInventory == 10.0);
    assert(*milkInventory == 7.5);
    assert(
        std::abs(*chocolateInventory - 1000.0)
        < 0.000001
        );


    // =========================================================
    // Add Another Cake
    // =========================================================

    // Add 2 FlonkyCake

    assert(
        manager.addCakeToDaily(
            dailyId,
            flonkyId,
            2.0,
            false
            )
        );

    auto flonkyDailyCake =
        manager.findDaily(dailyId)
            ->findCake(flonkyId);

    assert(flonkyDailyCake.has_value());

    assert(
        flonkyDailyCake->quantity == 2.0
        );


    // Inventory:
    //
    // Egg:
    // 10 - (3 × 2) = 4
    //
    // Milk:
    // 7.5 - (0.25 × 2) = 7
    //
    // Chocolate:
    // 1000 - (300 × 2) = 400

    eggInventory =
        manager.findInventory(zargolId);

    milkInventory =
        manager.findInventory(blorpMilkId);

    chocolateInventory =
        manager.findInventory(chocoMangoId);

    assert(*eggInventory == 4.0);
    assert(*milkInventory == 7.0);
    assert(*chocolateInventory - 400.0<0.0000001);


    // =========================================================
    // Insufficient Inventory
    // =========================================================

    // One GrumpleCake requires:
    //
    // Egg       -> 1
    // Milk      -> 1 kg
    // Chocolate -> 150 g
    //
    // Egg = 4
    // Milk = 7
    // Chocolate = 400
    //
    // 3 GrumpleCake would require:
    //
    // Egg       -> 3
    // Milk      -> 3
    // Chocolate -> 450
    //
    // Chocolate is insufficient.

    auto beforeFailedDaily =
        manager.findDaily(dailyId);

    assert(beforeFailedDaily.has_value());

    assert(
        !manager.addCakeToDaily(
            dailyId,
            grumpleId,
            3.0,
            false
            )
        );


    // Daily must remain unchanged.

    auto afterFailedDaily =
        manager.findDaily(dailyId);

    assert(afterFailedDaily.has_value());

    auto failedCake =
        afterFailedDaily->findCake(grumpleId);

    assert(!failedCake.has_value());


    // Inventory must also remain unchanged.

    eggInventory =
        manager.findInventory(zargolId);

    milkInventory =
        manager.findInventory(blorpMilkId);

    chocolateInventory =
        manager.findInventory(chocoMangoId);

    assert(*eggInventory == 4.0);
    assert(*milkInventory == 7.0);
    assert(*chocolateInventory - 400.0<0.0000001);


    // =========================================================
    // Remove Cake From Daily
    // =========================================================

    // Remove all 5 QromboCake.
    //
    // QromboCake:
    // Egg       -> 2
    // Milk      -> 0.5
    // Chocolate -> 200
    //
    // 5 cakes return:
    //
    // Egg       -> +10
    // Milk      -> +2.5
    // Chocolate -> +1000

    assert(
        manager.removeCakeFromDaily(
            dailyId,
            qromboId
            )
        );


    auto dailyAfterRemove =
        manager.findDaily(dailyId);

    assert(dailyAfterRemove.has_value());

    auto removedQrombo =
        dailyAfterRemove->findCake(qromboId);

    assert(!removedQrombo.has_value());


    // Inventory should return to:

    // Egg:
    // 4 + 10 = 14

    // Milk:
    // 7 + 2.5 = 9.5

    // Chocolate:
    // 400 + 1000 = 1400

    eggInventory =
        manager.findInventory(zargolId);

    milkInventory =
        manager.findInventory(blorpMilkId);

    chocolateInventory =
        manager.findInventory(chocoMangoId);

    assert(*eggInventory == 14.0);
    assert(*milkInventory == 9.5);
    assert(*chocolateInventory == 1400.0);


    // =========================================================
    // Remove Non-existing Cake
    // =========================================================

    assert(
        !manager.removeCakeFromDaily(
            dailyId,
            qromboId
            )
        );


    // =========================================================
    // Daily Find By Date
    // =========================================================

    auto dateSearch =
        manager.findDaily(testDate);

    assert(dateSearch.has_value());

    assert(
        dateSearch->getDate() == testDate
        );


    // =========================================================
    // Daily Contains
    // =========================================================

    assert(
        manager.containsDaily(testDate)
        );

    assert(
        !manager.containsDaily(
            QDate(2030, 1, 1)
            )
        );


    // =========================================================
    // Current Date
    // =========================================================

    QDate currentDate =
        manager.getCurrentDate();

    assert(currentDate.isValid());


    // =========================================================
    // Unsaved Changes
    // =========================================================

    assert(
        manager.hasUnsavedChanges()
        );


    qDebug()
        << "Daily AppManager test passed successfully!";

    return 0;
}