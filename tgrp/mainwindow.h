#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QDate>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

void setLastWeekSpan();

std::pair<std::string, std::string> getDateFrTo();

void setTxt(const QString &txt);
    QString txt() const;

private:
    Ui::MainWindow *ui;

    QDate date_fr;
    QDate date_to;
};
#endif // MAINWINDOW_H
