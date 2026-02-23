#include "ui.hpp"

#include "keys.hpp"

#include "ui_mainwindow.h"      // generated header for Ui::MainWindow

namespace wndx::attila::ui {

MainWindow::MainWindow(QObject *window)
    : m_self(self_t())
    , ui_ptr(new Ui::MainWindow) // Now valid because Ui::MainWindow is complete
{
    keys(window);
}

MainWindow::~MainWindow()
{
}

/**
 * init Keys class & bind hotkeys.
 */
auto MainWindow::keys(QObject *window) -> Keys&
{
    keys_ptr = new Keys(window, *ui_ptr);
    return *keys_ptr;
}

void MainWindow::cleanup()
{
    if (keys_ptr) delete keys_ptr;
    if (  ui_ptr) delete   ui_ptr;
}

} // namespace wndx::attila::ui
