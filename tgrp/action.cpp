#include "./ui_mainwindow.h"
#include "action.hpp"
#include "qnamespace.h" // Qt::ShortcutFocusReason

Action::Action(QObject *mwi, Ui::MainWindow *mwui)
    : QObject{mwi}
    , ui(mwui)
{
}

void Action::goto_tab(int index)
{
    // check that tab with index exist
    if (!(index < ui->tabWidget->count()))
        return;
    ui->tabWidget->setCurrentIndex(index);
}

void Action::goto_tab1() { Action::goto_tab(0); }
void Action::goto_tab2() { Action::goto_tab(1); }

void Action::goto_filter()
{
    Action::goto_tab1();
    ui->filterInput->setFocus(Qt::ShortcutFocusReason);
    ui->filterInput->deselect();
}

void Action::goto_date_fr()
{
    Action::goto_tab1();
    ui->dateFr->setFocus(Qt::ShortcutFocusReason);
}

void Action::goto_date_to()
{
    Action::goto_tab1();
    ui->dateTo->setFocus(Qt::ShortcutFocusReason);
}

void Action::goto_text()
{
    switch (ui->tabWidget->currentIndex()) {
    case 0:
        ui->previewText->setFocus(Qt::ShortcutFocusReason);
        break;
    case 1:
        ui->spentText->setFocus(Qt::ShortcutFocusReason);
        break;
    default:
        qDebug() << "Tab without Ctrl+t shortcut! index:"
                 << ui->tabWidget->currentIndex();
        break;
    }
}

void Action::toggle_merge()
{
    if (ui->tabWidget->currentIndex() != 1)
        return;
    ui->checkBoxMerge->click();
}

