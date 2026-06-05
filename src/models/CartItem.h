#ifndef CARTITEM_H
#define CARTITEM_H

#include <QString>

class CartItem{
private:
    int productId;
    QString name;
    double price;
    int quantity;
public:
    CartItem(int id, const QString& n, double p, int q): productId(id),name(n),price(p),quantity(q){}

    //getters:
    int getProductId() const { return productId;}
    QString getName() const { return name;}
    double getPrice() const { return price; }
    int getQuantity() const { return quantity;}

    //setters:
    void addQuantity(int qty){quantity+=qty;}


    double getTotal() const{return price*quantity;}

};

#endif
