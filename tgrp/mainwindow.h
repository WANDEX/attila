#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>

#include <QDate>
#include <QRegularExpression>
#include <QShortcut>
#include <QKeySequence>

#include <QTabWidget>
#include <QLineEdit>

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
    void filterChanged();
    void gTab1();
    void gTab2();
    void gToFilter();

private:
    Ui::MainWindow *ui;

    void pts(const QString);

    void shortcut(QKeySequence key, const char* execThis);
    void goToTab(int index);
    void hotkeys();

    void startup();
    void stylesDefaults();
    void setTabbingOrder();

    void setLastWeekSpan();
    void setTxt(const QString &txt);

    QLineEdit*  fin;
    QString     fin_ss_def;

    QDate date_fr;
    QDate date_to;

    QRegularExpression re_filter;

    QString TXT_RAW;
    QString TXT_FILTERED;

    std::vector<task_t> vtt;
};
#endif // MAINWINDOW_H
