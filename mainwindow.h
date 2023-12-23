#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QLineEdit>
#include "feedviewcontroller.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool chechAuth();

    private slots:
    void onSignInClicked();
    void onSingUpClicked();

    private:
    Ui::MainWindow *ui;
    QSqlDatabase database;
    FeedViewController *feedViewController;
};

#endif // MAINWINDOW_H
