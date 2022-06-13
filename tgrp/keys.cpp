#include "./ui_mainwindow.h"
#include "keys.hpp"
#include "qnamespace.h" // Qt::ShortcutFocusReason

Keys::Keys(QObject *mwi, Ui::MainWindow *mwui)
    : QObject{mwi}
    , mw(mwi)
    , ui(mwui)
    , act(new Action(mwi, mwui)) // Action class instance
{
    Keys::hotkeys(); // bind hotkeys
}


/*
    change scroll position based on active tab & scroll key
*/
void Keys::scroll()
{
    switch (ui->tabWidget->currentIndex()) {
    case 0:
        sobj = ui->previewText;
        break;
    case 1:
        sobj = ui->spentText;
        break;
    default:
        qDebug() << "Tab without scroll shortcut! index:"
                 << ui->tabWidget->currentIndex();
        return;
    }

    QShortcut* shcut = qobject_cast<QShortcut*>(sender());
    QKeySequence seq = shcut->key();

    vsbar = sobj->verticalScrollBar();
    hsbar = sobj->horizontalScrollBar();
    int vsb_val = vsbar->value();
    int hsb_val = hsbar->value();

    // focus scrollable text element
    sobj->setFocus(Qt::ShortcutFocusReason);

    //
    // vertical scrolling
    //
    if (seq.matches(Qt::Key_J) || seq.matches(Qt::Key_N)) {
        vsbar->setValue(vsb_val + vsbar->singleStep());
        return;
    }
    if (seq.matches(Qt::Key_K) || seq.matches(Qt::Key_E)) {
        vsbar->setValue(vsb_val - vsbar->singleStep());
        return;
    }
    // ^ same by page step
    if (seq.matches(Qt::SHIFT | Qt::Key_J) || seq.matches(Qt::SHIFT | Qt::Key_N)) {
        vsbar->setValue(vsb_val + vsbar->pageStep());
        return;
    }
    if (seq.matches(Qt::SHIFT | Qt::Key_K) || seq.matches(Qt::SHIFT | Qt::Key_E)) {
        vsbar->setValue(vsb_val - vsbar->pageStep());
        return;
    }
    //
    // horizontal scrolling
    //
    if (seq.matches(Qt::Key_H)) {
        hsbar->setValue(hsb_val - (hsbar->singleStep() * 4));
        return;
    }
    if (seq.matches(Qt::Key_L) || seq.matches(Qt::Key_I)) {
        hsbar->setValue(hsb_val + (hsbar->singleStep() * 4));
        return;
    }
    // ^ same by page step
    if (seq.matches(Qt::SHIFT | Qt::Key_H)) {
        hsbar->setValue(hsb_val - hsbar->pageStep());
        return;
    }
    if (seq.matches(Qt::SHIFT | Qt::Key_L) || seq.matches(Qt::SHIFT | Qt::Key_I)) {
        hsbar->setValue(hsb_val + hsbar->pageStep());
        return;
    }
    //
    // scroll top/bot
    //
    if (seq.matches(Qt::Key_G)) {
        vsbar->setValue(vsbar->minimum());
        return;
    }
    if (seq.matches(Qt::SHIFT | Qt::Key_G)) {
        vsbar->setValue(vsbar->maximum());
        return;
    }
}

/*
    bind scroll key
*/
void Keys::scrollkey(QKeySequence key)
{
    QObject::connect(new QShortcut(key, mw), &QShortcut::activated, this, &Keys::scroll);
}

/*
    bind scroll keys
*/
void Keys::scrollkeys()
{
    // hjkl scrolling of the text
    scrollkey(Qt::Key_H);
    scrollkey(Qt::Key_J); scrollkey(Qt::Key_N);
    scrollkey(Qt::Key_K); scrollkey(Qt::Key_E);
    scrollkey(Qt::Key_L); scrollkey(Qt::Key_I);
    // HJKL by page step (same as PageUp/PageDown)
    scrollkey(Qt::SHIFT | Qt::Key_H);
    scrollkey(Qt::SHIFT | Qt::Key_J); scrollkey(Qt::SHIFT | Qt::Key_N);
    scrollkey(Qt::SHIFT | Qt::Key_K); scrollkey(Qt::SHIFT | Qt::Key_E);
    scrollkey(Qt::SHIFT | Qt::Key_L); scrollkey(Qt::SHIFT | Qt::Key_I);
    // scroll top/bot - g/G
    scrollkey(Qt::Key_G);
    scrollkey(Qt::SHIFT | Qt::Key_G);
}

/*
    bind key shortcut to action from Action class
*/
template<typename Func2>
void Keys::sact(QKeySequence key, Func2 slot)
{
    QObject::connect(new QShortcut(key, mw), &QShortcut::activated, act, slot);
}

/*
    bind hotkeys
*/
void Keys::hotkeys()
{
    Keys::scrollkeys(); // bind scroll keys

    sact(tr("Ctrl+1"), &Action::goto_tab1);
    sact(tr("Ctrl+2"), &Action::goto_tab2);
    sact(tr("Ctrl+t"), &Action::goto_text);
    sact(tr("Ctrl+f"), &Action::goto_filter);
    sact(tr("Ctrl+d"),       &Action::goto_date_fr);
    sact(tr("Ctrl+Shift+D"), &Action::goto_date_to);
    sact(tr("Ctrl+m"), &Action::toggle_merge);
}

