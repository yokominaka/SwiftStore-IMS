#include "MainWindow.h"
#include <QLabel>
#include <QPushButton>
#include "LoginWidget.h"
#include "AdminWidget.h"
#include "CustomerWidget.h"
#include "VendorWidget.h"

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent){
    setupUI();
    setMinimumSize(900,600);
    setWindowTitle("SwiftStore");
}
void MainWindow::setupUI(){
    stackedWidget=new QStackedWidget(this);

    loginWidget=new LoginWidget(this);
    stackedWidget->addWidget(loginWidget); // Index 0

    adminWidget=new AdminWidget(this);
    stackedWidget->addWidget(adminWidget); // Index 1

    vendorWidget=new VendorWidget(this);
    stackedWidget->addWidget(vendorWidget);   // Index 2

    // 1. Removed lambdas: We can pass the member function address directly!
    connect(adminWidget,&AdminWidget::logoutRequested,this,&MainWindow::showLogin);
    connect(vendorWidget,&VendorWidget::logoutRequested,this,&MainWindow::showLogin);

    setCentralWidget(stackedWidget);

    // 2. Connect the LoginWidget's signal to our new Slot method
    connect(loginWidget, &LoginWidget::loginSuccessful,this,&MainWindow::handleLoginSuccessful);
}

// 3. Implemented the logic here instead of hiding it inside a lambda in setupUI()
void MainWindow::handleLoginSuccessful(QString role,QString username) {
    if(role=="Admin"){
        stackedWidget->setCurrentWidget(adminWidget);
    }
    else if(role=="Vendor"){
        stackedWidget->setCurrentWidget(vendorWidget);
    }
    else if(role=="Customer") {
        if(!customerWidget){
            customerWidget=new CustomerWidget(this);
            stackedWidget->addWidget(customerWidget); // Index 3
            // Connect customer logout directly without lambda
            connect(customerWidget, &CustomerWidget::logoutRequested, this, &MainWindow::showLogin);
        }
        stackedWidget->setCurrentWidget(customerWidget);
    }
}

void MainWindow::showAdminMenu(){
    if(adminWidget) {
        stackedWidget->setCurrentWidget(adminWidget);
    }
}

void MainWindow::showLogin(){
    stackedWidget->setCurrentIndex(0);
}

void MainWindow::showVendorMenu(){
    if(vendorWidget){
        stackedWidget->setCurrentWidget(vendorWidget);
    }
}

void MainWindow::showCustomerMenu(){
    if(customerWidget){
        stackedWidget->setCurrentWidget(customerWidget);
    }
}
MainWindow::~MainWindow() {}