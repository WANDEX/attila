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
    date_to = QDate::currentDate();
    date_fr = date_to.addDays(1-date_to.dayOfWeek()); // monday
    ui->dateFr->setDate(date_fr);
    ui->dateTo->setDate(date_to);
}

std::pair<std::string, std::string> MainWindow::getDateFrTo()
{
    date_fr = ui->dateFr->date();
    date_to = ui->dateTo->date();
    std::string fr = date_fr.toString("yyyy-MM-dd").toStdString();
    std::string to = date_to.toString("yyyy-MM-dd").toStdString();
    return std::make_pair(fr, to);
}

void MainWindow::setTxt(const QString &txt)
{
    ui->plainTextEdit->setPlainText(txt);
}
