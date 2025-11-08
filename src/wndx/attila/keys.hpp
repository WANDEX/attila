#pragma once

#include "ui_mainwindow.h"      // generated header for Ui::MainWindow

#include "aliases.hpp"

#include <QKeySequence>
#include <QObject>
#include <QPlainTextEdit>
#include <QScopedPointer>
#include <QScrollBar>
#include <QShortcut>
#include <QTabWidget>

namespace wndx {

class Action;
// namespace Ui { class MainWindow; }

class Keys : public QObject
{
    Q_OBJECT
public:
    explicit Keys(QObject *mwi, Ui::MainWindow *mwui);
    // explicit Keys(QObject *mwi, QScopedPointer<Ui::MainWindow> mwui);
    virtual ~Keys();

private:
    void scrollkey(QKeySequence key);
    void scrollkeys();

    template<typename Func2>
    void sact(QKeySequence key, Func2 slot);

    void hotkeys();

private slots:
    void scroll();

private:
    Ui::MainWindow *ui;
    // QScopedPointer<Ui::MainWindow> ui;
    QObject        *mw;
    Action         *act;

    // QScopedPointer<QObject> mw;
    // QScopedPointer<Action> act;

    QPlainTextEdit *sobj;
    QScrollBar     *vsbar;
    QScrollBar     *hsbar;
};

} // namespace wndx
