#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSql>
#include <QDebug>
#include <QSqlError>
#include <QFileInfo>
#include <iostream>
#include "feedviewcontroller.h"
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->signInButton, &QPushButton::clicked, this, &MainWindow::onSignInClicked);
    connect(ui->signUpButton, &QPushButton::clicked, this, &MainWindow::onSingUpClicked);

    feedViewController = new FeedViewController();
    connect(feedViewController, &FeedViewController::firstWindow, this, &MainWindow::show);
    if (chechAuth()) {
        this->close();
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onSignInClicked() // Нажатие кнопки войти
{
    QString enteredLogin = ui->loginTextField->text();
    QString enteredPassword = ui->passwordTextField->text();
    int userID;

    try{
        database = QSqlDatabase::addDatabase("QSQLITE");
        database.setDatabaseName("//Mac/Home/Documents/Avito/baseUsers.sqlite");
        if (!database.open()) {
            QMessageBox::critical(this, "Ошибка", "Не удалось подключиться к базе данных!");
            return;
        }
    } catch (const QSqlError& error) {
        QMessageBox::critical(this, "Ошибка", "Произошла ошибка при открытии базы данных: " + error.text());
        return;
    }
    try{
        QSqlQuery query(database);
        query.prepare("SELECT * FROM users WHERE username = :username AND password = :password");
        query.bindValue(":username", enteredLogin);
        query.bindValue(":password", enteredPassword);

        if (query.exec() && query.next()) {
            QSqlQuery query;
            query.prepare("SELECT id FROM users WHERE username = :username AND password = :password");
            query.bindValue(":username", enteredLogin);
            query.bindValue(":password", enteredPassword);
            if (query.exec()) {
                if (query.next()) {
                    userID = query.value(0).toInt();
                }
            } else {
                qDebug() << "Ошибка при выполнении запроса:" << query.lastError().text();
            }

            database.close();
            if (userID != -1) {
                QSettings settingsAuth("com.Fedotkin", "Avito");
                settingsAuth.setValue("userID", userID);
                bool alreadyAuth = true;
                QSettings settings("com.Fedotkin", "Avito");
                settings.setValue("UserAuth", alreadyAuth);
                database.close();
                ui->loginTextField->setText("");
                ui->passwordTextField->setText("");
                feedViewController->updateWindow();
                feedViewController->show();
                this->close();
            }
        } else {
            QMessageBox::critical(this, "Ошибка", "Введенный логин или пароль неверны!");
        }
    } catch (const QSqlError& error) {
        QMessageBox::critical(this, "Ошибка", "Произошла ошибка при выполнении запроса: " + error.text());
    }
}

void MainWindow::onSingUpClicked() // Нажатие кнопки зарегистрироваться
{
    QString enteredLogin = ui->loginTextField->text();
    QString enteredPassword = ui->passwordTextField->text();

    int userID;

    if (enteredLogin == "" || enteredPassword == "") {
        QMessageBox::critical(this, "Ошибка", "Некорректный логин или пароль");
        return;
    }

    try{
        database = QSqlDatabase::addDatabase("QSQLITE", "cache");
        database.setDatabaseName("//Mac/Home/Documents/Avito/baseUsers.sqlite");
        database.open();

        if (!database.open()) {
            QMessageBox::critical(this, "Ошибка", "Не удалось подключиться к базе данных!");
            return;
        }
    } catch (QSqlError& error) {
        QMessageBox::critical(this, "Ошибка", "Произошла ошибка при открытии базы данных: " + error.text());
        return;
    }

    QSqlQuery query(database);
    query.prepare("SELECT * FROM users WHERE username = :username");
    query.bindValue(":username", enteredLogin);
    if (query.exec() && query.next()) {
        QMessageBox::critical(this, "Ошибка", "Логин занят!");
    } else {
        try{
            QSqlQuery createTableQuery(database);
            createTableQuery.exec("CREATE TABLE IF NOT EXISTS users (username TEXT, password TEXT)");
            QSqlQuery query(database);
            query.prepare("INSERT INTO users (username, password) VALUES (:username, :password)");
            query.bindValue(":username", enteredLogin);
            query.bindValue(":password", enteredPassword);


            if (query.exec()) {
                QMessageBox::information(this, "Успех", "Регистрация прошла успешно!");
                QSqlQuery query;
                query.prepare("SELECT id FROM users WHERE username = :username AND password = :password");
                query.bindValue(":username", enteredLogin);
                query.bindValue(":password", enteredPassword);
                if (query.exec()) {
                    if (query.next()) {
                        userID = query.value(0).toInt();
                    }
                } else {
                    qDebug() << "Ошибка при выполнении запроса:" << query.lastError().text();
                }

                database.close();
                if (userID != -1) {
                    QSettings settingsAuth("com.Fedotkin", "Avito");
                    settingsAuth.setValue("userID", userID);
                    bool alreadyAuth = true;
                    QSettings settings("com.Fedotkin", "Avito");
                    settings.setValue("UserAuth", alreadyAuth);
                    database.close();
                    ui->loginTextField->setText("");
                    ui->passwordTextField->setText("");
                    database = QSqlDatabase::addDatabase("QSQLITE");
                    database.setDatabaseName("//Mac/Home/Documents/Avito/baseUsers.sqlite");
                    feedViewController->updateWindow();
                    feedViewController->show();
                    this->close();
                }
            } else {
                QMessageBox::critical(this, "Ошибка", "Не удалось зарегистрироваться!");
            }
        } catch (const QSqlError& error) {
            QMessageBox::critical(this, "Ошибка", "Произошла ошибка при выполнении запроса: " + error.text());
        }
    }
}

bool MainWindow::chechAuth() { // Проверка авторизации пользователя при запуске программы
    QSettings settings("com.Fedotkin", "Avito");
    if (settings.value("UserAuth").toBool()) {
        feedViewController->updateWindow();
        feedViewController->show();
        this->close();
        return true;
    }
    return false;
}

