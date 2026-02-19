#pragma once

#include "ui.hpp"

#include "wndx/attila/aliases.hpp"

#include "structs.hpp"          // ss namespace with struct defs
#include "stats.hpp"
#include "attila.hpp"

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

namespace wndx {

class Keys;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();

signals:
    void analyzeTasksSignal(const QString &txt);

private slots:
    void analyzeTasksStarted(const QString &txt);
    void analyzeTasksFinished();
    void dateSpanChanged();
    void filterChanged();
    void mergeToggle(int state);

private:
    void pts(const QString);

    void stylesDefaults();
    void setTabbingOrder();
    void setLastWeekSpan();
    void startup();

    void setTxt(const QString &txt);
    void merge();
    void updateStats(const ss::vtasks_t &vtt);

private:
    ui::MainWindow &ui_self;
    Ui::MainWindow &ui;

    QLineEdit  *fin;
    QString     fin_ss_def;

    QDate date_fr;
    QDate date_to;

    QTimer *typingTimer;
    QRegularExpression re_filter;

    QString TXT_RAW;
    QString TXT_FILTERED;
    QString TXT_SPENT;
    QString TXT_MERGED;

    ss::vtasks_t vtt;
    ss::vtasks_t vtt_merged;
    QFutureWatcher<ss::vtasks_t> vtt_watcher;
};

} // namespace wndx
