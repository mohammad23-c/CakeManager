#ifndef MATERIALCARD_H
#define MATERIALCARD_H

#include <QWidget>
#include <QString>
#include <QFile>
#include <QPixmap>
#include <QMouseEvent>

class QLabel;
class QVBoxLayout;

class MaterialCard : public QWidget
{
    Q_OBJECT

public:

    explicit MaterialCard(
        qint64 ingredientId,
        const QString& name,
        const QString& imagePath,
        QWidget *parent = nullptr
        );

    ~MaterialCard();

    // =========================================
    // Ingredient ID
    // =========================================

    qint64 getIngredientId() const;

    void setIngredientId(qint64 ingredientId);

    // =========================================
    // Name
    // =========================================

    QString getName() const;

    void setName(const QString& name);

    void updateName();

    // =========================================
    // Image Path
    // =========================================

    QString getImagePath() const;

    void setImagePath(const QString& imagePath);

    void updateImage();

    // =========================================
    // Update Card UI
    // =========================================

    void updateCard(
        const QString& name,
        const QString& imagePath
        );

signals:

    void clicked(qint64 ingredientId);

protected:

    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:

    QWidget* m_cardWidget;

    QLabel* m_imageLabel;
    QLabel* m_nameLabel;

    QVBoxLayout* m_layout;
    QVBoxLayout* m_cardLayout;

    qint64 m_ingredientId;

    QString m_name;
    QString m_imagePath;
};

#endif // MATERIALCARD_H