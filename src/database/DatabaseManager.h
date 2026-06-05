#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include<QtSql>
#include<QSqlDatabase>
#include<QDebug>

class DatabaseManager{
public:
    DatabaseManager();

    //these function manage the database connection
    bool openDatabase();
    void closeDatabase();
    bool initTables();

    // 3. New Helper Methods: Encapsulating our SQL queries so they aren't scattered in the UI
    bool addPurchaseRequest(int productId, int quantity);
    bool addUser(const QString& name, const QString& password, const QString& role);
    bool authenticateUser(const QString& username, const QString& password,QString& roleOut);
    bool addProduct(const QString& name,const QString& category,double price,double costPrice,int quantity, int threshold);
private:
    QSqlDatabase db;

};

#endif