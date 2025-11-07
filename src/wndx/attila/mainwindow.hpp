#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QDebug>

#include <QDate>
#include <QFutureWatcher>
#include <QRegularExpression>
#include <QTimer>

#include <QLineEdit>
#include <QCheckBox>

#include "structs.hpp" // ss namespace with struct defs
#include "stats.hpp"
#include "attila.hpp"
#include "keys.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

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
    Ui::MainWindow  *ui;
    class Keys      *ks;

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
#endif // MAINWINDOW_HPP
