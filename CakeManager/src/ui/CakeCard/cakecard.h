#ifndef CAKECARD_H
#define CAKECARD_H

#include <QWidget>
#include <QString>
#include <QFile>
#include <QPixmap>
#include <QMouseEvent>

class QLabel;
class QVBoxLayout;

class CakeCard : public QWidget
{
    Q_OBJECT

public:

    explicit CakeCard(
        qint64 cakeId,
        const QString& name,
        const QString& imagePath,
        QWidget *parent = nullptr
        );

    ~CakeCard();

    // =========================================
    // Cake ID
    // =========================================

    qint64 getCakeId() const;

    void setCakeId(qint64 cakeId);

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

    void clicked(qint64 cakeId);

protected:

    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:

    QLabel* m_imageLabel;
    QLabel* m_nameLabel;
    QVBoxLayout* m_layout;

    qint64 m_cakeId;

    QString m_name;

    QString m_imagePath;
};

#endif // CAKECARD_H