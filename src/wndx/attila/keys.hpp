#pragma once

#include "ui.hpp"

#include "wndx/attila/aliases.hpp"

#include <QKeySequence>
#include <QObject>
#include <QPlainTextEdit>
#include <QScopedPointer>
#include <QScrollBar>
#include <QShortcut>
#include <QTabWidget>

namespace wndx::attila {

class Action;

class Keys : public QObject
{
  Q_OBJECT
public:
  explicit Keys(QObject* window, Ui::MainWindow& ui_);
  virtual ~Keys();

private:
  void scrollkey(QKeySequence key);
  void scrollkeys();

  template <typename Func2>
  void sact(QKeySequence key, Func2 slot);

  void hotkeys();

private slots:
  void scroll();

private:
  QObject*        mw;
  Ui::MainWindow& ui;
  Action*         act;

  QPlainTextEdit* sobj;
  QScrollBar*     vsbar;
  QScrollBar*     hsbar;
};

} // namespace wndx::attila
