//customers dasboard 

#ifndef CUSTOMERWIDGET_H
#define CUSTOMERWIDGET_H

#include <QWidget>
#include <QSqlTableModel>
#include <QTableView>
#include<QListWidget>
#include<QLabel>
#include<QPushButton>
#include<vector>
#include"models/CartItem.h"

class CustomerWidget : public QWidget{
    Q_OBJECT // enables signals and slots.

public: 
    explicit CustomerWidget(QWidget *parent= nullptr);

signals:
    void logoutRequested();

private slots:
    void addToCart();
    void processCheckout();
    void onLogoutClicked();
private:
    QSqlTableModel *productModel;
    QTableView *catalogView;

    QListWidget *cartList;
    QLabel *totalLabel; // shows total price of items in cart
    QPushButton *btnCheckout;
    //vector syntax: std::vector<Type> name;
    
    std::vector<CartItem> shoppingCart; // vector to hold items in cart. 
    
    //CartItems is a struct to hold productId, name, price, quantity
    
    double currentTotal; // keeps track of total price of items in cart

    void setupUI();
    void updateCartUI();

    //
};
#endif