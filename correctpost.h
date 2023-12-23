#ifndef CORRECTPOST_H
#define CORRECTPOST_H

#include <QWidget>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtCore>

namespace Ui {
class CorrectPost;
}

class CorrectPost : public QWidget
{
    Q_OBJECT

public:
    explicit CorrectPost(QWidget *parent = 0);
    ~CorrectPost();

    void updateData(int id);

private:
    Ui::CorrectPost *ui;
    QSqlDatabase database;
    int id;

signals:
    void firstWindow();
    void aboutToClose();
private slots:
    void on_publicationButton_clicked();
    void openFileExplorer();
    void on_pushButton_clicked();
};

#endif // CORRECTPOST_H
