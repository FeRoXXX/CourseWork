#include "feedviewcontroller.h"
#include "ui_feedviewcontroller.h"

#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <QGraphicsScene>
#include <QSql>
#include <QSqlQuery>
#include <QSqlError>
#include <QBuffer>
#include <QMessageBox>
#include <QSettings>
#include <QVector>


bool buttonCheck = true;

FeedViewController::FeedViewController(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FeedViewController)
{
    ui->setupUi(this);
    updateWindow();
    correctPost = new CorrectPost();
    connect(ui->toAddImage, &QPushButton::clicked, this, &FeedViewController::openFileExplorer);
    connect(ui->publicationButton, &QPushButton::clicked, this, &FeedViewController::on_publicationButton_clicked);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &FeedViewController::onAddTabChanged);
    connect(correctPost, &CorrectPost::firstWindow, this, &FeedViewController::show);
    connect(correctPost, &CorrectPost::aboutToClose, this, &FeedViewController::onCorrectPostAboutToClose);
    m_time.start();

    m_pTimer=new QTimer;
    m_pTimer->setInterval(1000);
    QObject::connect(m_pTimer, SIGNAL(timeout()), this, SLOT(slotTimeout()));
    m_pTimer->start();


}

FeedViewController::~FeedViewController()
{
    delete ui;
    delete model;
}

void FeedViewController::setupTableView() // Инициаллизация таблицы публикаций
{
    model = new QStandardItemModel(0, 4, this);
    model->setHorizontalHeaderItem(0, new QStandardItem(tr("Картинка")));
    model->setHorizontalHeaderItem(1, new QStandardItem(tr("Название")));
    model->setHorizontalHeaderItem(2, new QStandardItem(tr("Описание")));
    model->setHorizontalHeaderItem(3, new QStandardItem(tr("Номер телефона")));

    try{
        database = QSqlDatabase::addDatabase("QSQLITE");
        database.setDatabaseName("//Mac/Home/Documents/Avito/baseUsers.sqlite");
        database.open();

        if (!database.open()) {
            QMessageBox::critical(this, "Ошибка", "Не удалось подключиться к базе данных!");
            qDebug() << "Ошибка открытия базы данных:" << database.lastError().text();
            return;
        }
    } catch (QSqlError& error) {
        QMessageBox::critical(this, "Ошибка", "Произошла ошибка при открытии базы данных: " + error.text());
        return;
    }
    QSqlQuery query(database);
    query.prepare("SELECT id, userID, productName, aboutTextField, numberTextField FROM products");
    if (query.exec()) {
        QVector<int> cellId;
        QVector<int> userIDs;
        QVector<QString> productNames;
        QVector<QPixmap> images;
        QVector<QString> aboutTexts;
        QVector<QString> numberTexts;
        model = new QStandardItemModel(0, 4, this);
        model->setHorizontalHeaderItem(0, new QStandardItem(tr("Картинка")));
        model->setHorizontalHeaderItem(1, new QStandardItem(tr("Название")));
        model->setHorizontalHeaderItem(2, new QStandardItem(tr("Описание")));
        model->setHorizontalHeaderItem(3, new QStandardItem(tr("Номер телефона")));
        ui->tableView->setModel(model);

        while (query.next()) {
            int cell = query.value(0).toInt();
            int userID = query.value(1).toInt();
            QString productName = query.value(2).toString();

            QString aboutText = query.value(3).toString();
            QString numberText = query.value(4).toString();

            cellId.append(cell);
            userIDs.append(userID);
            productNames.append(productName);
            //images.append(image);
            aboutTexts.append(aboutText);
            numberTexts.append(numberText);
            QSqlQuery queryNew(database);
            QString queryStr = QString("SELECT imageViewPub FROM images WHERE id=%1")
                    .arg(query.value(0).toInt());
            if (queryNew.exec(queryStr)) {
                while(queryNew.next()) {
                    QByteArray imageData = queryNew.value(0).toByteArray();
                    QPixmap image;
                    image.loadFromData(imageData);
                    images.append(image);
                }
            }
        }
        for(int index = 0; index < userIDs.count(); index++){
            QList<QStandardItem*> row;
            QStandardItem *item = new QStandardItem();
            item->setData(images[index], Qt::DecorationRole);
            row.append(item);
            QStandardItem *productNameItem = new QStandardItem(productNames[index]);
            productNameItem->setTextAlignment(Qt::AlignTop | Qt::AlignLeft);
            row.append(productNameItem);

            QStandardItem *aboutTextItem = new QStandardItem(aboutTexts[index]);
            aboutTextItem->setTextAlignment(Qt::AlignTop | Qt::AlignLeft);
            row.append(aboutTextItem);

            QStandardItem *numberTextItem = new QStandardItem(numberTexts[index]);
            numberTextItem->setTextAlignment(Qt::AlignTop | Qt::AlignLeft);
            row.append(numberTextItem);

            model->appendRow(row);
            ui->tableView->setModel(model);
            ui->tableView->verticalHeader()->setDefaultSectionSize(192);
            ui->tableView->setColumnWidth(0,256);
            ui->tableView->verticalHeader()->setDefaultAlignment(Qt::AlignTop);
            ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
            QHeaderView *header = ui->tableView->horizontalHeader();
            for (int i = 1; i < header->count(); ++i) {
                header->setSectionResizeMode(i, QHeaderView::Stretch);
            }
        }
        ui->tableView->verticalHeader()->setDefaultSectionSize(192);
        ui->tableView->setColumnWidth(0,256);
        ui->tableView->verticalHeader()->setDefaultAlignment(Qt::AlignTop);
        ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
        QHeaderView *header = ui->tableView->horizontalHeader();
        for (int i = 1; i < header->count(); ++i) {
            header->setSectionResizeMode(i, QHeaderView::Stretch);
        }
    }
    query.finish();
    database.close();
}

void FeedViewController::openFileExplorer() // Открытие Проводника для работы с файлами
{
    QString imagePath = QFileDialog::getOpenFileName(this, tr("Выберите картинку"), "", tr("Изображения (*.png *.jpg *.jpeg)"));
    if (!imagePath.isEmpty()) {
        QPixmap pixmap(imagePath);
        QGraphicsScene* scene = new QGraphicsScene();
        scene->setSceneRect(ui->imageViewPub->rect());
        QPixmap scaledPixmap = pixmap.scaled(ui->imageViewPub->rect().size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        scene->addPixmap(scaledPixmap);
        ui->imageViewPub->setScene(scene);
    }
}

void FeedViewController::on_publicationButton_clicked() // Нажатие кнопки опубликовать
{
    if (buttonCheck) {
        buttonCheck = false;
        return;
    }

    QString productName = ui->productName->text();
    QImage image(ui->imageViewPub->grab().toImage());
    QByteArray imageViewPub;
    QBuffer buffer(&imageViewPub);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");
    buffer.close();
    QString aboutTextField = ui->aboutTextField->toPlainText();
    QString numberTextField = ui->numberTextField->text();
    if (productName == "" || image.isNull() || aboutTextField == "" || numberTextField == "") {
        QMessageBox::critical(this, "Ошибка", "Какое то поле не заполнено");
        return;
    }

    try{
        database = QSqlDatabase::addDatabase("QSQLITE");
        database.setDatabaseName("//Mac/Home/Documents/Avito/baseUsers.sqlite");
        database.open();

        if (!database.open()) {
            QMessageBox::critical(this, "Ошибка", "Не удалось подключиться к базе данных!");
            qDebug() << "Ошибка открытия базы данных:" << database.lastError().text();
            return;
        }
    } catch (QSqlError& error) {
        QMessageBox::critical(this, "Ошибка", "Произошла ошибка при открытии базы данных: " + error.text());
        return;
    }


    QSqlQuery query(database);
    query.prepare("INSERT INTO products (userID, productName, aboutTextField, numberTextField) ""VALUES (:userID, :productName, :aboutTextField, :numberTextField)");
    QSettings settings("com.Fedotkin", "Avito");
    query.bindValue(":userID", settings.value("userID"));
    query.bindValue(":productName", productName);
    query.bindValue(":aboutTextField", aboutTextField);
    query.bindValue(":numberTextField", numberTextField);

    if (query.exec()) {
        qDebug() << "first data loaded";
    } else {
        qDebug() << "Ошибка при добавлении данных в базу данных:" << query.lastError().text();
    }

    QSqlQuery queryNew(database);
    queryNew.prepare("INSERT INTO images (imageViewPub) ""VALUES (:imageViewPub)");
    queryNew.bindValue(":imageViewPub", imageViewPub);

    if (queryNew.exec()) {
        qDebug() << "second data loaded!";
        ui->tabWidget->setCurrentIndex(0);
    } else {
        qDebug() << "Ошибка при добавлении данных в базу данных:" << query.lastError().text();
    }

    query.finish();
    database.close();
}

void FeedViewController::onAddTabChanged(int index) { // Перемещение по tab bar
    if (index == 1) {
        QGraphicsScene* scene = new QGraphicsScene;
        QPixmap image("//Mac/Home/Documents/Avito/32339.png");
        scene->addPixmap(image.scaled(256,192,Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        ui->imageViewPub->setScene(scene);
        ui->imageViewPub->show();
        ui->productName->setText("");
        ui->aboutTextField->setText("");
        ui->numberTextField->setText("");
        buttonCheck = true;
    }
    if (index == 0) {
        setupTableView();
        ui->lineEdit->setText("");
    }
    if (index == 2) {
        setupTableViewSecond();
    }

}

void FeedViewController::setupTableViewSecond() // Инициаллизация второй таблицы
{
    model = new QStandardItemModel(0, 5, this);
    model->setHorizontalHeaderItem(0, new QStandardItem(tr("Картинка")));
    model->setHorizontalHeaderItem(1, new QStandardItem(tr("Название")));
    model->setHorizontalHeaderItem(2, new QStandardItem(tr("Описание")));
    model->setHorizontalHeaderItem(3, new QStandardItem(tr("Номер телефона")));
    model->setHorizontalHeaderItem(4, new QStandardItem(tr("id")));


        try{
            database = QSqlDatabase::addDatabase("QSQLITE");
            database.setDatabaseName("//Mac/Home/Documents/Avito/baseUsers.sqlite");
            database.open();

            if (!database.open()) {
                QMessageBox::critical(this, "Ошибка", "Не удалось подключиться к базе данных!");
                qDebug() << "Ошибка открытия базы данных:" << database.lastError().text();
                return;
            }
        } catch (QSqlError& error) {
            QMessageBox::critical(this, "Ошибка", "Произошла ошибка при открытии базы данных: " + error.text());
            return;
        }

    QSqlQuery query(database);
    QSettings settings("com.Fedotkin", "Avito");
    QString queryStr = QString("SELECT id, userID, productName, aboutTextField, numberTextField FROM products WHERE userID=%1")
            .arg(settings.value("userID").toInt());
    if (query.exec(queryStr)) {
        QVector<QString> cellIDs;
        QVector<int> userIDs;
        QVector<QString> productNames;
        QVector<QString> aboutTexts;
        QVector<QString> numberTexts;
        QVector<QPixmap> images;
        model = new QStandardItemModel(0, 5, this);
        model->setHorizontalHeaderItem(0, new QStandardItem(tr("Картинка")));
        model->setHorizontalHeaderItem(1, new QStandardItem(tr("Название")));
        model->setHorizontalHeaderItem(2, new QStandardItem(tr("Описание")));
        model->setHorizontalHeaderItem(3, new QStandardItem(tr("Номер телефона")));
        model->setHorizontalHeaderItem(4, new QStandardItem(tr("id")));
        ui->tableView_2->setModel(model);

        while (query.next()) {
            QString cellID = query.value(0).toString();
            int userID = query.value(1).toInt();
            QString productName = query.value(2).toString();
            QString aboutText = query.value(3).toString();
            QString numberText = query.value(4).toString();

            cellIDs.append(cellID);
            userIDs.append(userID);
            productNames.append(productName);
            aboutTexts.append(aboutText);
            numberTexts.append(numberText);
            QSqlQuery queryNew(database);
            QString queryStr = QString("SELECT imageViewPub FROM images WHERE id=%1")
                    .arg(query.value(0).toInt());
            if (queryNew.exec(queryStr)) {
                while(queryNew.next()) {
                    QByteArray imageData = queryNew.value(0).toByteArray();
                    QPixmap image;
                    image.loadFromData(imageData);
                    images.append(image);
                }
            }
        }
        QString name;
        name.setNum(userIDs.count());
        ui->label_2->setText("Объявлений: " + name);
        for(int index = 0; index < userIDs.count(); index++){
            QList<QStandardItem*> row;
            QStandardItem *item = new QStandardItem();
            item->setData(images[index], Qt::DecorationRole);
            row.append(item);
            QStandardItem *productNameItem = new QStandardItem(productNames[index]);
            productNameItem->setTextAlignment(Qt::AlignTop | Qt::AlignLeft);
            row.append(productNameItem);

            QStandardItem *aboutTextItem = new QStandardItem(aboutTexts[index]);
            aboutTextItem->setTextAlignment(Qt::AlignTop | Qt::AlignLeft);
            row.append(aboutTextItem);

            QStandardItem *numberTextItem = new QStandardItem(numberTexts[index]);
            numberTextItem->setTextAlignment(Qt::AlignTop | Qt::AlignLeft);
            row.append(numberTextItem);
            row.append(new QStandardItem(cellIDs[index]));

            model->appendRow(row);
            ui->tableView_2->setModel(model);
            ui->tableView_2->setColumnHidden(4, true);
            QHeaderView *header = ui->tableView_2->horizontalHeader();
            for (int i = 1; i < header->count(); ++i) {
                header->setSectionResizeMode(i, QHeaderView::Stretch);
            }
            ui->tableView_2->verticalHeader()->setDefaultSectionSize(192);
            ui->tableView_2->setColumnWidth(0,256);
        }
        ui->tableView_2->setColumnHidden(4, true);
        QHeaderView *header = ui->tableView_2->horizontalHeader();
        for (int i = 1; i < header->count(); ++i) {
            header->setSectionResizeMode(i, QHeaderView::Stretch);
        }
        ui->tableView_2->verticalHeader()->setDefaultSectionSize(192);
        ui->tableView_2->setColumnWidth(0,256);
    }

    query.finish();
    database.close();
}

void FeedViewController::on_logoutButtonClicked_clicked() // Нажатие кнопки выход
{
    bool alreadyAuth = false;
    QSettings settings("com.Fedotkin", "Avito");
    settings.setValue("UserAuth", alreadyAuth);
    this->close();
    emit firstWindow();
}

void FeedViewController::updateWindow() { // Обновление всех окон пиложения
    model = new QStandardItemModel();
    ui->tableView->setModel(model);
    ui->tableView_2->setModel(model);
    setupTableView();
    setupTableViewSecond();
    m_time.restart();
}

void FeedViewController::slotTimeout() // Перезапуск таймера работы приложения
{
    int msecs=m_time.elapsed();
    QTime time(0,0,0);
    ui->timerLabel->setText(time.addMSecs(msecs).toString("hh:mm:ss"));
}

void FeedViewController::on_sortedClicked_clicked()
{
    model = new QStandardItemModel(0, 4, this);
    model->setHorizontalHeaderItem(0, new QStandardItem(tr("Картинка")));
    model->setHorizontalHeaderItem(1, new QStandardItem(tr("Название")));
    model->setHorizontalHeaderItem(2, new QStandardItem(tr("Описание")));
    model->setHorizontalHeaderItem(3, new QStandardItem(tr("Номер телефона")));

    try{
        database = QSqlDatabase::addDatabase("QSQLITE");
        database.setDatabaseName("//Mac/Home/Documents/Avito/baseUsers.sqlite");
        database.open();

        if (!database.open()) {
            QMessageBox::critical(this, "Ошибка", "Не удалось подключиться к базе данных!");
            qDebug() << "Ошибка открытия базы данных:" << database.lastError().text();
            return;
        }
    } catch (QSqlError& error) {
        QMessageBox::critical(this, "Ошибка", "Произошла ошибка при открытии базы данных: " + error.text());
        return;
    }
    QSqlQuery query(database);
    QString queryStr;
    if (ui->lineEdit->text() != ""){
        queryStr = QString("SELECT id, userID, productName, aboutTextField, numberTextField FROM products WHERE productName LIKE '%1%'")
            .arg(ui->lineEdit->text());
    } else {
        queryStr = QString("SELECT id, userID, productName, aboutTextField, numberTextField FROM products");
     }
    if (query.exec(queryStr)) {
        QVector<int> userIDs;
        QVector<QString> productNames;
        QVector<QPixmap> images;
        QVector<QString> aboutTexts;
        QVector<QString> numberTexts;
        model = new QStandardItemModel(0, 4, this);
        model->setHorizontalHeaderItem(0, new QStandardItem(tr("Картинка")));
        model->setHorizontalHeaderItem(1, new QStandardItem(tr("Название")));
        model->setHorizontalHeaderItem(2, new QStandardItem(tr("Описание")));
        model->setHorizontalHeaderItem(3, new QStandardItem(tr("Номер телефона")));
        ui->tableView->setModel(model);

        while (query.next()) {
            int userID = query.value(1).toInt();
            QString productName = query.value(2).toString();
            QString aboutText = query.value(3).toString();
            QString numberText = query.value(4).toString();

            userIDs.append(userID);
            productNames.append(productName);
            aboutTexts.append(aboutText);
            numberTexts.append(numberText);
            QSqlQuery queryNew(database);
            QString queryStr = QString("SELECT imageViewPub FROM images WHERE id=%1")
                    .arg(query.value(0).toInt());
            if (queryNew.exec(queryStr)) {
                while(queryNew.next()) {
                    QByteArray imageData = queryNew.value(0).toByteArray();
                    QPixmap image;
                    image.loadFromData(imageData);
                    images.append(image);
                }
            }
        }
        for(int index = 0; index < userIDs.count(); index++){
            QList<QStandardItem*> row;
            QStandardItem *item = new QStandardItem();
            item->setData(images[index], Qt::DecorationRole);
            row.append(item);
            QStandardItem *productNameItem = new QStandardItem(productNames[index]);
            productNameItem->setTextAlignment(Qt::AlignTop | Qt::AlignLeft);
            row.append(productNameItem);

            QStandardItem *aboutTextItem = new QStandardItem(aboutTexts[index]);
            aboutTextItem->setTextAlignment(Qt::AlignTop | Qt::AlignLeft);
            row.append(aboutTextItem);

            QStandardItem *numberTextItem = new QStandardItem(numberTexts[index]);
            numberTextItem->setTextAlignment(Qt::AlignTop | Qt::AlignLeft);
            row.append(numberTextItem);

            model->appendRow(row);
            ui->tableView->setModel(model);
            ui->tableView->verticalHeader()->setDefaultSectionSize(192);
            ui->tableView->setColumnWidth(0,256);
            ui->tableView->verticalHeader()->setDefaultAlignment(Qt::AlignTop);
            ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
            QHeaderView *header = ui->tableView->horizontalHeader();
            for (int i = 1; i < header->count(); ++i) {
                header->setSectionResizeMode(i, QHeaderView::Stretch);
            }
        }
        ui->tableView->verticalHeader()->setDefaultSectionSize(192);
        ui->tableView->setColumnWidth(0,256);
        ui->tableView->verticalHeader()->setDefaultAlignment(Qt::AlignTop);
        ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
        QHeaderView *header = ui->tableView->horizontalHeader();
        for (int i = 1; i < header->count(); ++i) {
            header->setSectionResizeMode(i, QHeaderView::Stretch);
        }
    }
    query.finish();
    database.close();
}

void FeedViewController::on_tableView_2_clicked(const QModelIndex &index)
{
    correctPost->updateData(ui->tableView_2->model()->index(index.row(),4).data().toInt());
    correctPost->show();
    this->close();
}

void FeedViewController::onCorrectPostAboutToClose() {
    updateWindow();
}
