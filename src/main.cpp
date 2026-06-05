#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include "database/DatabaseManager.h"
#include "ui/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    
    qDebug() << "[1] Application Started. Attempting to open database...";

    DatabaseManager dbManager; // database object
    
    if(!dbManager.openDatabase()){
        QMessageBox::critical(nullptr, "Error", "Failed to open database!");
        return -1;
    }
    
    qDebug() << "[2] Database Opened. Attempting to init tables...";
    
    if(!dbManager.initTables()){
        QMessageBox::critical(nullptr, "Error", "Failed to init tables!");
        return -1;
    }
    
    qDebug()<<"[3] Tables Initialized. Creating MainWindow object...";
    
    MainWindow w;
    
    qDebug() << "[4] MainWindow Created. Attempting to show UI...";
    
    w.show();
    
    qDebug() << "[5] UI Shown. Entering event loop...";
    
    return a.exec();
}