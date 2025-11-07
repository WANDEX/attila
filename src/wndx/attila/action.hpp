#ifndef ACTION_HPP
#define ACTION_HPP

#include <QObject>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Action : public QObject
{
    Q_OBJECT
public:
    explicit Action(QObject *mwi = nullptr, Ui::MainWindow *mwui = nullptr);

private:
    void goto_tab(int index);

public slots:
    void goto_tab1();
    void goto_tab2();
    void goto_filter();
    void goto_date_fr();
    void goto_date_to();
    void goto_text();
    void toggle_merge();

private:
    Ui::MainWindow *ui;
};
#endif // ACTION_HPP
