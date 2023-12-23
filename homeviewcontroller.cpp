//#include "homeviewcontroller.h"
//#include "ui_homeviewcontroller.h"
//#include <QStandardItemModel>
//#include <QStyledItemDelegate>
//#include <QPainter>
//#include <QPixmap>
//#include <QDebug>
//#include <QGraphicsScene>
//#include <QSql>
//#include <QSqlQuery>
//#include <QSqlError>
//#include <QBuffer>
//#include <QMessageBox>
//#include <QSettings>
//#include <QVector>


//bool buttonCheck = true;

//HomeViewController::HomeViewController(QWidget *parent) :
//    QWidget(parent),
//    ui(new Ui::HomeViewController)
//{
//    ui->setupUi(this);
//    setupTableView();
//    setupTableViewSecond();
//    connect(ui->toAddImage, &QPushButton::clicked, this, &HomeViewController::openFileExplorer);
//    connect(ui->publicationButton, &QPushButton::clicked, this, &HomeViewController::on_publicationButton_clicked);
//    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &HomeViewController::onAddTabChanged);
//}

//HomeViewController::~HomeViewController()
//{
//    delete ui;
//    delete model;
//}

//void HomeViewController::setupTableView()
//{
//    model = new QStandardItemModel(0, 4, this);
//    model->setHorizontalHeaderItem(0, new QStandardItem(tr("Картинка")));
//    model->setHorizontalHeaderItem(1, new QStandardItem(tr("Название")));
//    model->setHorizontalHeaderItem(2, new QStandardItem(tr("Описание")));
//    model->setHorizontalHeaderItem(3, new QStandardItem(tr("Номер телефона")));

//    if (!database.isValid()) {
//        try{
//            database = QSqlDatabase::addDatabase("QSQLITE", "cache");
//            database.setDatabaseName("//Mac/Home/Documents/Avito/baseUsers.sqlite");
//            database.open();

//            if (!database.open()) {
//                QMessageBox::critical(this, "Ошибка", "Не удалось подключиться к базе данных!");
//                qDebug() << "Ошибка открытия базы данных:" << database.lastError().text();
//                return;
//            }
//        } catch (QSqlError& error) {
//            QMessageBox::critical(this, "Ошибка", "Произошла ошибка при открытии базы данных: " + error.text());
//            return;
//        }
//    }
//    QSqlQuery query(database);
//    query.prepare("SELECT userID, productName, imageViewPub, aboutTextField, numberTextField FROM products");
//    if (query.exec()) {
//        QVector<int> userIDs;
//        QVector<QString> productNames;
//        QVector<QPixmap> images;
//        QVector<QString> aboutTexts;
//        QVector<QString> numberTexts;

//        while (query.next()) {
//            int userID = query.value(0).toInt();
//            QString productName = query.value(1).toString();
//            QByteArray imageData = query.value(2).toByteArray();
//            QPixmap image;
//            image.loadFromData(imageData);
//            QString aboutText = query.value(3).toString();
//            QString numberText = query.value(4).toString();

//            userIDs.append(userID);
//            productNames.append(productName);
//            images.append(image);
//            aboutTexts.append(aboutText);
//            numberTexts.append(numberText);
//        }
//        for(int index = 0; index < userIDs.count(); index++){
//            QList<QStandardItem*> row;
//            QStandardItem *item = new QStandardItem();
//            item->setData(images[index], Qt::DecorationRole);
//            row.append(item);
//            row.append(new QStandardItem(productNames[index]));
//            row.append(new QStandardItem(aboutTexts[index]));
//            row.append(new QStandardItem(numberTexts[index]));
//            model->appendRow(row);

//            ui->tableView->verticalHeader()->setDefaultSectionSize(192);
//            ui->tableView->setColumnWidth(0,256);
//            ui->tableView->verticalHeader()->setDefaultAlignment(Qt::AlignTop);
//            ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
//            QHeaderView *header = ui->tableView->horizontalHeader();
//            for (int i = 1; i < header->count(); ++i) {
//                header->setSectionResizeMode(i, QHeaderView::Stretch);
//            }
//            ui->tableView->setModel(model);
//        }
//    }
//}

//void HomeViewController::openFileExplorer()
//{
//    QString imagePath = QFileDialog::getOpenFileName(this, tr("Выберите картинку"), "", tr("Изображения (*.png *.jpg *.jpeg)"));
//    if (!imagePath.isEmpty()) {
//        QPixmap pixmap(imagePath);
//        QGraphicsScene* scene = new QGraphicsScene();
//        scene->setSceneRect(ui->imageViewPub->rect());
//        QPixmap scaledPixmap = pixmap.scaled(ui->imageViewPub->rect().size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

//        scene->addPixmap(scaledPixmap);
//        ui->imageViewPub->setScene(scene);
//    }
//}

//void HomeViewController::on_publicationButton_clicked()
//{
//    if (buttonCheck) {
//        buttonCheck = false;
//        return;
//    }

//    qDebug() << "Button clicked";
//    QString productName = ui->productName->text();
//    QImage image(ui->imageViewPub->grab().toImage());
//    QByteArray imageViewPub;
//    QBuffer buffer(&imageViewPub);
//    buffer.open(QIODevice::WriteOnly);
//    image.save(&buffer, "PNG");
//    buffer.close();
//    QString aboutTextField = ui->aboutTextField->text();
//    QString numberTextField = ui->numberTextField->text();
//    if (productName == "" || image.isNull() || aboutTextField == "" || numberTextField == "") {
//        QMessageBox::critical(this, "Ошибка", "Какое то поле не заполнено");
//        return;
//    }

//    if (!database.isValid()) {
//        try{
//            database = QSqlDatabase::addDatabase("QSQLITE", "cache");
//            database.setDatabaseName("//Mac/Home/Documents/Avito/baseUsers.sqlite");
//            database.open();

//            if (!database.open()) {
//                QMessageBox::critical(this, "Ошибка", "Не удалось подключиться к базе данных!");
//                qDebug() << "Ошибка открытия базы данных:" << database.lastError().text();
//                return;
//            }
//        } catch (QSqlError& error) {
//            QMessageBox::critical(this, "Ошибка", "Произошла ошибка при открытии базы данных: " + error.text());
//            return;
//        }
//    }


//    QSqlQuery query(database);
//    query.prepare("INSERT INTO products (userID, productName, imageViewPub, aboutTextField, numberTextField) ""VALUES (:userID, :productName, :imageViewPub, :aboutTextField, :numberTextField)");
//    QSettings settings("com.Fedotkin", "Avito");
//    query.bindValue(":userID", settings.value("userID"));
//    query.bindValue(":productName", productName);
//    query.bindValue(":imageViewPub", imageViewPub);
//    query.bindValue(":aboutTextField", aboutTextField);
//    query.bindValue(":numberTextField", numberTextField);

//    if (query.exec()) {
//        qDebug() << "Данные успешно добавлены в базу данных!";
//        ui->tabWidget->setCurrentIndex(0);
//    } else {
//        qDebug() << "Ошибка при добавлении данных в базу данных:" << query.lastError().text();
//    }
//}

//void HomeViewController::onAddTabChanged(int index) {
//    if (index == 1) {
//        QGraphicsScene* scene = new QGraphicsScene;
//        QPixmap image("//Mac/Home/Documents/Avito/32339.png");
//        scene->addPixmap(image.scaled(256,192,Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
//        ui->imageViewPub->setScene(scene);
//        ui->imageViewPub->show();
//        ui->productName->setText("");
//        ui->aboutTextField->setText("");
//        ui->numberTextField->setText("");
//        buttonCheck = true;
//    }
//    if (index == 0) {
//        setupTableView();
//    }
//    if (index == 2) {
//        setupTableViewSecond();
//    }

//}

//void HomeViewController::setupTableViewSecond()
//{
//    model = new QStandardItemModel(0, 4, this);
//    model->setHorizontalHeaderItem(0, new QStandardItem(tr("Картинка")));
//    model->setHorizontalHeaderItem(1, new QStandardItem(tr("Название")));
//    model->setHorizontalHeaderItem(2, new QStandardItem(tr("Описание")));
//    model->setHorizontalHeaderItem(3, new QStandardItem(tr("Номер телефона")));

//    if (!database.isValid()) {
//        try{
//            database = QSqlDatabase::addDatabase("QSQLITE", "cache");
//            database.setDatabaseName("//Mac/Home/Documents/Avito/baseUsers.sqlite");
//            database.open();

//            if (!database.open()) {
//                QMessageBox::critical(this, "Ошибка", "Не удалось подключиться к базе данных!");
//                qDebug() << "Ошибка открытия базы данных:" << database.lastError().text();
//                return;
//            }
//        } catch (QSqlError& error) {
//            QMessageBox::critical(this, "Ошибка", "Произошла ошибка при открытии базы данных: " + error.text());
//            return;
//        }
//    }
//    QSqlQuery query(database);
//    QSettings settings("com.Fedotkin", "Avito");
//    QString queryStr = QString("SELECT userID, productName, imageViewPub, aboutTextField, numberTextField FROM products WHERE userID=%1")
//            .arg(settings.value("userID").toInt());
//    if (query.exec(queryStr)) {
//        QVector<int> userIDs;
//        QVector<QString> productNames;
//        QVector<QPixmap> images;
//        QVector<QString> aboutTexts;
//        QVector<QString> numberTexts;

//        while (query.next()) {
//            int userID = query.value(0).toInt();
//            QString productName = query.value(1).toString();
//            QByteArray imageData = query.value(2).toByteArray();
//            QPixmap image;
//            image.loadFromData(imageData);
//            QString aboutText = query.value(3).toString();
//            QString numberText = query.value(4).toString();

//            userIDs.append(userID);
//            productNames.append(productName);
//            images.append(image);
//            aboutTexts.append(aboutText);
//            numberTexts.append(numberText);
//        }
//        for(int index = 0; index < userIDs.count(); index++){
//            QList<QStandardItem*> row;
//            QStandardItem *item = new QStandardItem();
//            item->setData(images[index], Qt::DecorationRole);
//            row.append(item);
//            row.append(new QStandardItem(productNames[index]));
//            row.append(new QStandardItem(aboutTexts[index]));
//            row.append(new QStandardItem(numberTexts[index]));
//            model->appendRow(row);

//            ui->tableView_2->verticalHeader()->setDefaultSectionSize(192);
//            ui->tableView_2->setColumnWidth(0,256);

//            QHeaderView *header = ui->tableView_2->horizontalHeader();
//            for (int i = 1; i < header->count(); ++i) {
//                header->setSectionResizeMode(i, QHeaderView::Stretch);
//            }
//            ui->tableView_2->setModel(model);
//        }
//    }
//}

//void HomeViewController::on_tableView_pressed(const QModelIndex &index)
//{
//    qDebug() << "TUT NAJALAS YACHEIKA" << index.row();
//}

//void HomeViewController::on_logoutButtonClicked_clicked()
//{
//    this->hide();
//    emit firstWindow();
//}
