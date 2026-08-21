#include "inputvalidator.h"

bool InputValidator::validateLineEdit(
    QLineEdit* lineEdit,
    InputType expectedType
    )
{
    // Check if LineEdit is empty
    if (lineEdit->text().trimmed().isEmpty())
    {
        lineEdit->setFocus();
        lineEdit->setStyleSheet(
            "border: 1px solid red;"
            );

        return false;
    }

    bool valid = true;

    switch (expectedType)
    {
    case InputType::NoType:
        // Type doesn't matter
        valid = true;
        break;

    case InputType::String:
        // Already a QString
        valid = true;
        break;

    case InputType::Integer:
    {
        bool ok = false;

        lineEdit->text().toInt(&ok);

        valid = ok;
        break;
    }

    case InputType::Double:
    {
        bool ok = false;

        lineEdit->text().toDouble(&ok);

        valid = ok;
        break;
    }
    }

    if (!valid)
    {
        lineEdit->setFocus();
        lineEdit->setStyleSheet(
            "border: 1px solid red;"
            );

        return false;
    }

    // Input is valid
    lineEdit->setStyleSheet("");

    return true;
}