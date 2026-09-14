#include "outputcontroll.h"

outPutControll::outPutControll() {}
//this func calculate cost as tooman
QString outPutControll::formatPrice(double value)
{
    value /= 1000.0;

    QString number =
        QString::number(
            value,
            'f',
            2
            );

    int decimalIndex =
        number.indexOf('.');

    QString integerPart =
        number.left(decimalIndex);

    QString decimalPart =
        number.mid(decimalIndex);

    for (int i = integerPart.length() - 3;
         i > 0;
         i -= 3)
    {
        integerPart.insert(i, ',');
    }

    return integerPart + decimalPart;
}
