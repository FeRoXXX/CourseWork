#include "correctpost.h"
#include "ui_correctpost.h"
#include <QDebug>
#include <QMessageBox>
#include <QSqlError>

CorrectPost::CorrectPost(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CorrectPost)
{
    ui->setupUi(this);
    connect(ui->toAddImage, &QPushButton::clicked, this, &CorrectPost::openFileExplorer);
}

CorrectPost::~CorrectPost()
{
    delete ui;
}

void CorrectPost::on_publicationButton_clicked() // Публикация обновленных данных
{
    QString productName = ui->productName->text();
    QImage image(ui->imageViewPub->grab().toImage());
    QByteArray imageViewPub;
    QBuffer buffer(&imageViewPub);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");
    buffer.close();
    QString aboutTextField = ui->aboutTextField->toPlainText();
    QString numberTextField = ui->numberTextField->text();
    if (productName.isEmpty() || aboutTextField.isEmpty() || numberTextField.isEmpty() || ui->imageViewPub == NULL) {
        QMessageBox::critical(this, "Ошибка", "Присутствуют пустые поля или нет картинки");
        return;
    }
    try{

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
    query.prepare("UPDATE products SET productName=:productName, aboutTextField=:aboutTextField, numberTextField=:numberTextField WHERE id=:id");
    query.bindValue(":productName", productName);
    query.bindValue(":aboutTextField", aboutTextField);
    query.bindValue(":numberTextField", numberTextField);
    query.bindValue(":id", id);
    if (query.exec()) {
        qDebug() << "ok";
    } else {
        qDebug() << database.lastError();
        return;
    }
    QSqlQuery queryNew(database);
    queryNew.prepare("UPDATE images SET imageViewPub=:imageViewPub WHERE id=:id");
    queryNew.bindValue(":imageViewPub", imageViewPub);
    queryNew.bindValue(":id", id);
    if (queryNew.exec()) {
        qDebug() << "ok";
    } else {
        qDebug() << database.lastError();
        return;
    }
    database.close();

    this->close();
    emit aboutToClose();
    emit firstWindow();
}


void CorrectPost::updateData(int id) { // Обновление даннах в окне
    CorrectPost::id = id;

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
    queryStr = QString("SELECT userID, productName, aboutTextField, numberTextField FROM products WHERE id='%1'")
        .arg(id);
    if (query.exec(queryStr)) {
        QString productNames;
        QPixmap images;
        QString aboutTexts;
        QString numberTexts;

        while (query.next()) {
            QString productName = query.value(1).toString();
            qDebug() << productName << id;

            QString aboutText = query.value(2).toString();
            QString numberText = query.value(3).toString();
            QSqlQuery queryNew(database);
            QString queryStr = QString("SELECT imageViewPub FROM images WHERE id=%1")
                    .arg(id);
            if (queryNew.exec(queryStr)) {
                while(queryNew.next()) {
                    QByteArray imageData = queryNew.value(0).toByteArray();
                    QPixmap image;
                    image.loadFromData(imageData);
                    QGraphicsScene* scene = new QGraphicsScene();
                    scene->addPixmap(image);
                    ui->imageViewPub->setScene(scene);
                }
            }
            ui->productName->setText(productName);
            ui->aboutTextField->setText(aboutText);
            ui->numberTextField->setText(numberText);
        }
    }
}

void CorrectPost::openFileExplorer() // Открытие Проводника для работы с файлами
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


void CorrectPost::on_pushButton_clicked()// Нажатие на кнопку удаления данных
{

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
    QString queryStr = QString("DELETE FROM products WHERE id=%1")
            .arg(id);
    if(query.exec(queryStr)){
        QSqlQuery queryNew(database);
        QString queryStr = QString("DELETE FROM images WHERE id=%1")
                .arg(id);
        if(queryNew.exec(queryStr)){

        }
    }
    database.close();

    this->close();
    emit aboutToClose();
    emit firstWindow();
}
