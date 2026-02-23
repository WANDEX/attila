#pragma once

#include "ui.hpp"

#include "wndx/attila/aliases.hpp"

#include <QObject>

namespace wndx::attila {

class Action : public QObject
{
    Q_OBJECT
public:
    explicit Action(QObject *mwi, Ui::MainWindow &mwui);
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
    Ui::MainWindow &ui;
};

} // namespace wndx::attila
