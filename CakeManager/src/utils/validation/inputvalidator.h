#ifndef INPUTVALIDATOR_H
#define INPUTVALIDATOR_H

#include <QLineEdit>
#include <QPainter>
#include <QPainterPath>

class InputValidator
{
public:

    enum class InputType
    {
        NoType,
        String,
        Integer,
        Double
    };

    static bool validateLineEdit(
        QLineEdit* lineEdit,
        InputType expectedType
        );
    static void setFocusAndRedBorder(QWidget* widget);
};

#endif // INPUTVALIDATOR_H