#pragma once

#include <QObject>

namespace Ui { // forward declare the Qt-generated class
    class MainWindow;
}

namespace wndx {

class Keys;

namespace ui {

class MainWindow
{
public:
    explicit MainWindow(QObject *);
    virtual ~MainWindow();

    // THE MAGIC: Overload the arrow operator
    // This allows: myWrapper->anyQtWidget (avoids ui->ui->).
    Ui::MainWindow*       operator->()       { return ui_ptr; }
    Ui::MainWindow const* operator->() const { return ui_ptr; }

    Ui::MainWindow*       ptr()       { return  ui_ptr; }
    Ui::MainWindow const* ptr() const { return  ui_ptr; }

    Ui::MainWindow&       ref()       { return *ui_ptr; }
    Ui::MainWindow&       ref() const { return *ui_ptr; }

    auto keys(QObject *) -> Keys&;

    decltype(auto) self_t() { return   *this; }
    decltype(auto) self_p() { return &m_self; }
    decltype(auto) self_r() { return  m_self; }

    void cleanup();

private:
    ui::MainWindow  &m_self;
    Ui::MainWindow  *ui_ptr;
    Keys            *keys_ptr;
};

} // namespace ui
} // namespace wndx
