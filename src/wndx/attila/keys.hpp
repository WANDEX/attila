#ifndef KEYS_HPP
#define KEYS_HPP

#include <QObject>
#include <QShortcut>
#include <QKeySequence>

#include <QPlainTextEdit>
#include <QScrollBar>
#include <QTabWidget>

#include "action.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow;

class Keys : public QObject
{
    Q_OBJECT
public:
    explicit Keys(QObject *mwi = nullptr, Ui::MainWindow *mwui = nullptr);

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
    QObject        *mw;
    Action         *act;

    QPlainTextEdit *sobj;
    QScrollBar     *vsbar;
    QScrollBar     *hsbar;
};
#endif // KEYS_HPP
