#ifndef LAUNCHWINDOW_H
#define LAUNCHWINDOW_H

#include <memory>

#include <QMainWindow>

namespace Ui {
class LaunchWindow;
}

namespace geometrize
{

namespace dialog
{

/**
 * @brief The LaunchWindow class models the UI for a launchpad for opening new job templates and recent jobs.
 */
class LaunchWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LaunchWindow(QWidget *parent = 0);
    ~LaunchWindow();

    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;

private slots:
    void on_actionPreferences_triggered();
    void on_actionClear_Recents_triggered();
    void on_actionExit_triggered();
    void on_actionTutorials_triggered();
    void on_actionSupport_triggered();
    void on_actionAbout_triggered();
    void on_openImageButton_clicked();

private:
    void closeEvent(QCloseEvent *bar) override;

    Ui::LaunchWindow *ui;

    class LaunchWindowImpl;
    std::unique_ptr<LaunchWindowImpl> d;
};

}

}

#endif // LAUNCHWINDOW_H
