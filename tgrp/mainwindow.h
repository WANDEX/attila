#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>

#include <QDate>

#include "tgrp.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void dateSpanChanged();

private:
    Ui::MainWindow *ui;

    void startup();
    void setLastWeekSpan();
    void setTxt(const QString &txt);
    std::pair<std::string, std::string> getDateFrTo();

    QDate date_fr;
    QDate date_to;
};
#endif // MAINWINDOW_H
