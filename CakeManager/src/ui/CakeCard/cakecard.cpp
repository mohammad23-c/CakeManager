#include "cakecard.h"
#include "ui_cakecard.h"

CakeCard::CakeCard(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CakeCard)
{
    ui->setupUi(this);
}

CakeCard::~CakeCard()
{
    delete ui;
}
