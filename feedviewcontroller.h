#ifndef FEEDVIEWCONTROLLER_H
#define FEEDVIEWCONTROLLER_H

#include <QWidget>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtCore>
#include "correctpost.h"

namespace Ui {
class FeedViewController;
}

class FeedViewController : public QWidget
{
    Q_OBJECT

public:
    void updateWindow();
    explicit FeedViewController(QWidget *parent = nullptr);
    ~FeedViewController();

private:
    Ui::FeedViewController *ui;
    QStandardItemModel *model;
    QSqlDatabase database;
    QTime m_time;
    QTimer* m_pTimer;
    CorrectPost *correctPost;


    void setupTableView();
    void setupTableViewSecond();
private slots:
    void slotTimeout();
    void openFileExplorer();
    void on_publicationButton_clicked();
    void onAddTabChanged(int index);
    void on_logoutButtonClicked_clicked();

    void on_sortedClicked_clicked();

    void on_tableView_2_clicked(const QModelIndex &index);

public slots:
    void onCorrectPostAboutToClose();
signals:
    void firstWindow();
};

#endif // FEEDVIEWCONTROLLER_H
