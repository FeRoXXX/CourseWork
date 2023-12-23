#ifndef CUSTOMTABLEMODEL_H
#define CUSTOMTABLEMODEL_H
#include <QAbstractTableModel>


class CustomTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    CustomTableModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    QVector<QString> names;
    QVector<QString> descriptions;
    QVector<QString> phoneNumbers;
    QVector<QPixmap> images;
};

#endif // CUSTOMTABLEMODEL_H
