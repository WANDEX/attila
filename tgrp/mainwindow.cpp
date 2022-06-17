#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

#include <QtConcurrent/QtConcurrent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , ks(new Keys(this, ui)) // init Keys class & bind hotkeys
{
    ui->setupUi(this);
    MainWindow::startup();

    typingTimer = new QTimer(this);
    typingTimer->setSingleShot(true); // timer will fire only once after it was started

    // filter only after the user has stopped typing for at least a short time (filter as you type)
    connect(ui->filterInput, &QLineEdit::textChanged, this, [&](){ typingTimer->start(300); });
    connect(typingTimer,     &QTimer::timeout,        this, &MainWindow::filterChanged);

    connect(ui->dateFr, &QDateEdit::dateChanged, this, &MainWindow::dateSpanChanged);
    connect(ui->dateTo, &QDateEdit::dateChanged, this, &MainWindow::dateSpanChanged);

    // parallel analysis of tasks in the background (non-blocking behavior)
    connect(this, &MainWindow::analyzeTasksSignal, this, &MainWindow::analyzeTasksStarted);
    connect(&vtt_watcher, &QFutureWatcher<vtasks_t>::finished, this, &MainWindow::analyzeTasksFinished);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*
   print timestamp into debug output (used to measure speed between calls)
*/
void MainWindow::pts(const QString msg="")
{
    if (msg.isEmpty()) {
        qDebug() << QDateTime::currentDateTime().toString("[hh:mm]:ss.zzz");
    } else {
        qDebug() << QDateTime::currentDateTime().toString("[hh:mm]:ss.zzz") << ":" << msg;
    }
}

void MainWindow::stylesDefaults()
{
    fin = ui->filterInput;
    fin_ss_def = "QLineEdit{ color: white; }\nQLineEdit[text=\"\"]{ color: gray; }";
    fin->setStyleSheet(fin_ss_def); // fix: override placeholderText color by gray
}

/*
    set order in which we switch between widgets by pressing Tab key
*/
void MainWindow::setTabbingOrder()
{
    QWidget::setTabOrder(fin, ui->dateFr);
    QWidget::setTabOrder(ui->dateFr, ui->dateTo);
    QWidget::setTabOrder(ui->dateTo, ui->scrollArea);
    QWidget::setTabOrder(ui->scrollArea, ui->scrollAreaWidgetContents);
}

void MainWindow::setLastWeekSpan()
{
    date_to = QDate::currentDate();
    date_fr = date_to.addDays(1-date_to.dayOfWeek()); // monday
    ui->dateFr->setDate(date_fr);
    ui->dateTo->setDate(date_to);
}

void MainWindow::startup()
{
    stylesDefaults();
    setTabbingOrder();
    setLastWeekSpan();
    dateSpanChanged();
}

void MainWindow::setTxt(const QString &txt)
{
    ui->previewText->setPlainText(txt);
    qDebug() << "New text was set!";
    emit analyzeTasksSignal(txt);
}

void MainWindow::analyzeTasksStarted(const QString &txt)
{
    pts("[TASKS ANALYZING] started");
    const std::string stdstr = txt.toStdString();
    QFuture<vtasks_t> future = QtConcurrent::run(parse_tasks_parallel, stdstr);
    vtt_watcher.setFuture(future); // when computation is finished -> emit finished
}

void MainWindow::analyzeTasksFinished()
{
    pts("[TASKS ANALYZING] finished");
    vtt = vtt_watcher.result();
    const QString spent_text = QString::fromStdString(tasks_to_mulstr(vtt));
    ui->spentText->setPlainText(spent_text);
    pts("[TASKS ANALYZING] spent text is set!");
}

void MainWindow::dateSpanChanged()
{
    date_fr = ui->dateFr->date();
    date_to = ui->dateTo->date();
    if (!date_fr.isValid() || !date_to.isValid()) {
        qDebug() << "Not valid date, processing was skipped.";
        return;
    }
    // allow fr-to dates exchange - swap variables
    if (date_to < date_fr) {
        QDate tmpdate = date_fr;
        date_fr = date_to;
        date_to = tmpdate;
    }

    std::string fr = date_fr.toString("yyyy-MM-dd").toStdString();
    std::string to = date_to.toString("yyyy-MM-dd").toStdString();
    std::string content = concat_span(fr, to);
    TXT_RAW = QString::fromStdString(content);
    setTxt(TXT_RAW);
    // try to apply filter back after changing the date span
    if (!fin->text().isEmpty())
        filterChanged();
}

void MainWindow::filterChanged()
{
    const QString pattern = fin->text();
    if (pattern.isEmpty()) {
        setTxt(TXT_RAW); // set back not filtered text after clearing filter pattern
        fin->setStyleSheet(fin_ss_def);
        return;
    }

    re_filter = QRegularExpression(pattern);
    if (!re_filter.isValid()) {
        fin->setStyleSheet("color: red"); // indicate not valid regex by the red text color
        return;
    } else {
        fin->setStyleSheet(fin_ss_def);
    }

    std::string filtered = filter_find(TXT_RAW.toStdString(), re_filter.pattern().toStdString());
    TXT_FILTERED = QString::fromStdString(filtered);
    setTxt(TXT_FILTERED);
}
