#pragma once

#include "ui_mainwindow.h"      // generated header for Ui::MainWindow

#include "aliases.hpp"

#include <QObject>
#include <QScopedPointer>

namespace wndx {

// namespace Ui { class MainWindow; }

class Action : public QObject
{
    Q_OBJECT
public:
    explicit Action(QObject *mwi, Ui::MainWindow *mwui);
    // explicit Action(QObject *mwi, QScopedPointer<Ui::MainWindow> mwui);
    virtual ~Action();

private:
    void goto_tab(int index);

public slots:
    void goto_tab1();
    void goto_tab2();
    void goto_filter();
    void goto_date_fr();
    void goto_date_to();
    void goto_text();
    void toggle_merge();

private:
    Ui::MainWindow *ui;
    // QScopedPointer<Ui::MainWindow> ui;
};

} // namespace wndx
