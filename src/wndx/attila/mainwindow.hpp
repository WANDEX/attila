#pragma once

#include "ui.hpp"

#include "wndx/attila/aliases.hpp"

#include "structs.hpp" // ss namespace with struct defs

#include <QCheckBox>
#include <QDate>
#include <QDebug>
#include <QFutureWatcher>
#include <QLineEdit>
#include <QMainWindow>
#include <QObject>
#include <QRegularExpression>
#include <QScopedPointer>
#include <QTimer>

namespace wndx::attila {

class Keys;

class MainWindow : public QMainWindow
{
  Q_OBJECT
public:
  explicit MainWindow(QWidget* parent = nullptr);
  virtual ~MainWindow();

signals:
  void analyzeTasksSignal(QString const& txt);

private slots:
  void analyzeTasksStarted(QString const& txt);
  void analyzeTasksFinished();
  void dateSpanChanged();
  void filterChanged();
  void mergeToggle(int state);

private:
  void pts(QString const);

  void stylesDefaults();
  void setTabbingOrder();
  void setLastWeekSpan();
  void startup();

  void setTxt(QString const& txt);
  void merge();
  void updateStats(ss::vtasks_t const& vtt);

private:
  ui::MainWindow& ui_self;
  Ui::MainWindow& ui;

  QLineEdit* fin;
  QString    fin_ss_def;

  QDate date_fr;
  QDate date_to;

  QTimer*            typingTimer;
  QRegularExpression re_filter;

  QString TXT_RAW;
  QString TXT_FILTERED;
  QString TXT_SPENT;
  QString TXT_MERGED;

  ss::vtasks_t                 vtt;
  ss::vtasks_t                 vtt_merged;
  QFutureWatcher<ss::vtasks_t> vtt_watcher;
};

} // namespace wndx::attila
