#ifndef INPUTVALIDATOR_H
#define INPUTVALIDATOR_H

#include <QLineEdit>

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
};

#endif // INPUTVALIDATOR_H