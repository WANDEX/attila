#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setLastWeekSpan()
{
    QDate date_to = QDate::currentDate();
    QDate date_fr = date_to.addDays(1-date_to.dayOfWeek()); // monday
    ui->dateFr->setDate(date_fr);
    ui->dateTo->setDate(date_to);
}

void MainWindow::setTxt(const QString &txt)
{
    ui->plainTextEdit->setPlainText(txt);
}
