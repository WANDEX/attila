#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "tgrp.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    startup();

    connect(ui->dateFr, &QDateEdit::dateChanged, this, &MainWindow::dateSpanChanged);
    connect(ui->dateTo, &QDateEdit::dateChanged, this, &MainWindow::dateSpanChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startup()
{
    setLastWeekSpan();
    std::pair<std::string, std::string> datestr_fr_to = getDateFrTo();
    std::vector<Task> tasks = file_tasks(find_last_week_file());
    setTxt(QString::fromStdString(tasks_to_mulstr(tasks)));
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

void MainWindow::dateSpanChanged()
{
    // TODO: verify that fr/to is really changed -> return otherwise

    date_fr = ui->dateFr->date();
    date_to = ui->dateTo->date();

    std::string fr = date_fr.toString("yyyy-MM-dd").toStdString();
    std::string to = date_to.toString("yyyy-MM-dd").toStdString();
    std::string content = concat_span(fr, to);

    setTxt(QString::fromStdString(content));
}
