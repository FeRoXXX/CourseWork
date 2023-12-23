#ifndef USERMANAGER_H
#define USERMANAGER_H

class UserManager
{
public:
    static UserManager& getInstance()
    {
        static UserManager instance; // Единственный экземпляр класса UserManager
        return instance;
    }

    void setUserID(int userID)
    {
        this->userID = userID;
    }

    int getUserID() const
    {
        return userID;
    }

private:
    UserManager() {} // Приватный конструктор для предотвращения создания экземпляров извне
    UserManager(const UserManager&) = delete; // Запрещаем копирование
    UserManager& operator=(const UserManager&) = delete; // Запрещаем присваивание
    ~UserManager() {}

    int userID;
};

#endif // USERMANAGER_H
