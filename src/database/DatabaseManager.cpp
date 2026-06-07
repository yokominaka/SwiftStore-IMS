#include "DatabaseManager.h"
#include<QSqlQuery>
#include<QSqlError>

DatabaseManager::DatabaseManager(){
    if(QSqlDatabase::contains(QSqlDatabase::defaultConnection)){
        db=QSqlDatabase::database(); 
    } else{
        db=QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("inventory_system.db");
    }
}

bool DatabaseManager::openDatabase(){
    if(!db.open()){
        qDebug()<<"Error: connection with database failed";
        return false;
    }
    return true;

}

bool DatabaseManager::initTables(){
    QSqlQuery query;

    QString productTable = "CREATE TABLE IF NOT EXISTS products ("
                           "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                           "name TEXT, "
                           "category TEXT, "
                           "price REAL, "
                           "cost_price REAL, "
                           "quantity INTEGER, "
                           "threshold INTEGER)";

    QString userTable="CREATE TABLE IF NOT EXISTS users ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "username TEXT UNIQUE, "
                      "password_hash TEXT, "
                      "role TEXT)";
    
    QString salesTable = "CREATE TABLE IF NOT EXISTS sales ("
                         "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                         "product_id INTEGER, "
                         "quantity INTEGER, "
                         "total_price REAL, "
                         "sale_date DATETIME DEFAULT CURRENT_TIMESTAMP)";
    
    QString requestTable= "CREATE TABLE IF NOT EXISTS purchase_requests("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, " //index: 0
                          "product_id INTEGER, "//index: 1
                          "quantity INTEGER, "//index: 2
                          "status TEXT DEFAULT 'PENDING', "
                          "request_date DATETIME DEFAULT CURRENT_TIMESTAMP, "
                          "FOREIGN KEY(product_id) REFERENCES products(id))";

    if (!(query.exec(salesTable) && query.exec(productTable) && query.exec(userTable) && query.exec(requestTable))) {
        return false;
    }

    // Check if users table is empty and insert default admin user
    QSqlQuery checkUsers;
    checkUsers.exec("SELECT count(*) FROM users");
    if(checkUsers.next() && checkUsers.value(0).toInt()==0){
        QSqlQuery insertAdmin;
        insertAdmin.exec("INSERT INTO users (username, password_hash, role)"
                         "VALUES ('admin', 'admin123', 'Admin')");
    }
    return true;
}

// -- NEW ENCAPSULATION METHODS----

bool DatabaseManager::addPurchaseRequest(int productId, int quantity){
    QSqlQuery query;
    query.prepare("INSERT INTO purchase_requests (product_id, quantity, status) VALUES (:pid, :qty, 'PENDING')");
    query.bindValue(":pid", productId);
    query.bindValue(":qty", quantity);

    if(!query.exec()){ //if the query fails, log the error and return false
        qDebug()<<"database error:"<<query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::addUser(const QString& username, const QString& password, const QString& role){
    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password_hash, role) VALUES (:username, :password, :role)");

    query.bindValue(":username", username);
    query.bindValue(":password", password);
    query.bindValue(":role",role);

    if(!query.exec()){
        qDebug()<<"database error:" <<query.lastError().text();
        return false;

    }
    return true;

}

bool DatabaseManager::authenticateUser(const QString& username, const QString& password, QString& roleOut){
    QSqlQuery query;
    query.prepare("SELECT role FROM users WHERE username= :user AND password_hash = :pass");
    query.bindValue(":user",username);
    query.bindValue(":pass",password);

    if(query.exec()&&query.next()){
        roleOut=query.value(0).toString(); //
        return true;
    }
    return false;
}
bool DatabaseManager::addProduct(const QString& name, const QString& category, double price, double costPrice, int quantity, int threshold){
    QSqlQuery query;
    query.prepare("INSERT INTO products (name, category, price, cost_price, quantity, threshold) "
                  "VALUES (:name, :category, :price, :costPrice, :quantity, :threshold)");
    query.bindValue(":name", name);
    query.bindValue(":category", category);
    query.bindValue(":price", price);
    query.bindValue(":costPrice", costPrice);
    query.bindValue(":quantity", quantity);
    query.bindValue(":threshold", threshold);

    if(!query.exec()){
        qDebug()<<"database error:"<<query.lastError().text();
        return false;
    }
    return true;
}
