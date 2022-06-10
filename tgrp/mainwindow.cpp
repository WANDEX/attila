#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "tgrp.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    startup();

    connect(ui->dateFr, &QDateEdit::dateChanged, this, &MainWindow::dateSpanChanged);
    connect(ui->dateTo, &QDateEdit::dateChanged, this, &MainWindow::dateSpanChanged);
    connect(ui->filterInput, &QLineEdit::textChanged, this, &MainWindow::filterChanged);
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

void MainWindow::goToTab(int index)
{
    // check that tab with index exist
    if (!(index < ui->tabWidget->count()))
        return;
    ui->tabWidget->setCurrentIndex(index);
}

void MainWindow::gTab1() { goToTab(0); }
void MainWindow::gTab2() { goToTab(1); }

void MainWindow::gToFilter()
{
    gTab1();
    ui->filterInput->setFocus(Qt::ShortcutFocusReason);
    ui->filterInput->deselect();
}

void MainWindow::gToDateFr()
{
    gTab1();
    ui->dateFr->setFocus(Qt::ShortcutFocusReason);
}

void MainWindow::gToDateTo()
{
    gTab1();
    ui->dateTo->setFocus(Qt::ShortcutFocusReason);
}

void MainWindow::gToTxt()
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

void MainWindow::gScroll()
{
    switch (ui->tabWidget->currentIndex()) {
    case 0:
        pte_of_cur_tab = ui->previewText;
        break;
    case 1:
        pte_of_cur_tab = ui->spentText;
        break;
    default:
        qDebug() << "Tab without scroll shortcut! index:"
                 << ui->tabWidget->currentIndex();
        return;
    }

    QShortcut* shcut = qobject_cast<QShortcut*>(sender());
    QKeySequence seq = shcut->key();

    vsb_of_cur_tab = pte_of_cur_tab->verticalScrollBar();
    hsb_of_cur_tab = pte_of_cur_tab->horizontalScrollBar();
    int vsb_val = vsb_of_cur_tab->value();
    int hsb_val = hsb_of_cur_tab->value();

    // focus scrollable text element
    pte_of_cur_tab->setFocus(Qt::ShortcutFocusReason);

    /*
       vertical scrolling
    */
    if (seq.matches(Qt::Key_J) || seq.matches(Qt::Key_N)) {
        vsb_of_cur_tab->setValue(vsb_val + vsb_of_cur_tab->singleStep());
        return;
    }
    if (seq.matches(Qt::Key_K) || seq.matches(Qt::Key_E)) {
        vsb_of_cur_tab->setValue(vsb_val - vsb_of_cur_tab->singleStep());
        return;
    }
    // ^ same by page step
    if (seq.matches(Qt::SHIFT | Qt::Key_J) || seq.matches(Qt::SHIFT | Qt::Key_N)) {
        vsb_of_cur_tab->setValue(vsb_val + vsb_of_cur_tab->pageStep());
        return;
    }
    if (seq.matches(Qt::SHIFT | Qt::Key_K) || seq.matches(Qt::SHIFT | Qt::Key_E)) {
        vsb_of_cur_tab->setValue(vsb_val - vsb_of_cur_tab->pageStep());
        return;
    }
    /*
       horizontal scrolling
    */
    if (seq.matches(Qt::Key_H)) {
        hsb_of_cur_tab->setValue(hsb_val - (hsb_of_cur_tab->singleStep() * 4));
        return;
    }
    if (seq.matches(Qt::Key_L) || seq.matches(Qt::Key_I)) {
        hsb_of_cur_tab->setValue(hsb_val + (hsb_of_cur_tab->singleStep() * 4));
        return;
    }
    // ^ same by page step
    if (seq.matches(Qt::SHIFT | Qt::Key_H)) {
        hsb_of_cur_tab->setValue(hsb_val - hsb_of_cur_tab->pageStep());
        return;
    }
    if (seq.matches(Qt::SHIFT | Qt::Key_L) || seq.matches(Qt::SHIFT | Qt::Key_I)) {
        hsb_of_cur_tab->setValue(hsb_val + hsb_of_cur_tab->pageStep());
        return;
    }
    /*
       scroll top/bot
    */
    if (seq.matches(Qt::Key_G)) {
        vsb_of_cur_tab->setValue(vsb_of_cur_tab->minimum());
        return;
    }
    if (seq.matches(Qt::SHIFT | Qt::Key_G)) {
        vsb_of_cur_tab->setValue(vsb_of_cur_tab->maximum());
        return;
    }
}

void MainWindow::gToMerge()
{
    if (ui->tabWidget->currentIndex() != 1)
        return;
    ui->checkBoxMerge->click();
}

/*
usage:
shortcut(QKeySequence(Qt::Key_Escape), SLOT(keyHandle()));
*/
void MainWindow::shortcut(QKeySequence key, const char* execThis)
{
    QShortcut *shortcut = new QShortcut(key, this);
    shortcut->setContext(Qt::ApplicationShortcut);
    connect(shortcut, SIGNAL(activated()), this, execThis);
}

void MainWindow::hotkeys()
{
    shortcut(tr("Ctrl+1"), SLOT(gTab1()));
    shortcut(tr("Ctrl+2"), SLOT(gTab2()));
    shortcut(tr("Ctrl+f"), SLOT(gToFilter()));
    shortcut(tr("Ctrl+d"),       SLOT(gToDateFr()));
    shortcut(tr("Ctrl+Shift+D"), SLOT(gToDateTo()));
    shortcut(tr("Ctrl+t"), SLOT(gToTxt()));
    shortcut(tr("Ctrl+m"), SLOT(gToMerge()));

    // hjkl scrolling of the text
    shortcut(Qt::Key_H, SLOT(gScroll()));
    shortcut(Qt::Key_J, SLOT(gScroll())); shortcut(Qt::Key_N, SLOT(gScroll()));
    shortcut(Qt::Key_K, SLOT(gScroll())); shortcut(Qt::Key_E, SLOT(gScroll()));
    shortcut(Qt::Key_L, SLOT(gScroll())); shortcut(Qt::Key_I, SLOT(gScroll()));
    // HJKL by page step (same as PageUp/PageDown)
    shortcut(Qt::SHIFT | Qt::Key_H, SLOT(gScroll()));
    shortcut(Qt::SHIFT | Qt::Key_J, SLOT(gScroll())); shortcut(Qt::SHIFT | Qt::Key_N, SLOT(gScroll()));
    shortcut(Qt::SHIFT | Qt::Key_K, SLOT(gScroll())); shortcut(Qt::SHIFT | Qt::Key_E, SLOT(gScroll()));
    shortcut(Qt::SHIFT | Qt::Key_L, SLOT(gScroll())); shortcut(Qt::SHIFT | Qt::Key_I, SLOT(gScroll()));
    // scroll top/bot - g/G
    shortcut(Qt::Key_G, SLOT(gScroll()));
    shortcut(Qt::SHIFT | Qt::Key_G, SLOT(gScroll()));
}

void MainWindow::startup()
{
    hotkeys();
    stylesDefaults();
    setTabbingOrder();
    setLastWeekSpan();
    dateSpanChanged();
}

void MainWindow::setLastWeekSpan()
{
    date_to = QDate::currentDate();
    date_fr = date_to.addDays(1-date_to.dayOfWeek()); // monday
    ui->dateFr->setDate(date_fr);
    ui->dateTo->setDate(date_to);
}

void MainWindow::setTxt(const QString &txt)
{
    ui->previewText->setPlainText(txt);
    pts("[TASKS ANALYZING] before");
    vtt = parse_tasks_parallel(txt.toStdString());
    pts("[TASKS ANALYZING] after");
    const QString spent_text = QString::fromStdString(tasks_to_mulstr(vtt));
    ui->spentText->setPlainText(spent_text);
    qDebug() << "New text was set!";
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
