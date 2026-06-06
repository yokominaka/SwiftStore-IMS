#include "CustomerWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include<QHeaderView>
#include<QMessageBox>
#include<QInputDialog>
#include<QSqlQuery>
#include<QSqlError>
#include<QDebug>

CustomerWidget ::CustomerWidget(QWidget *parent):QWidget(parent), currentTotal(0.0){
    setupUI();
}

void CustomerWidget::setupUI(){
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    
    //left panel: product catalog

    QWidget *catalogPanel = new QWidget();
    QVBoxLayout *catalogLayout= new QVBoxLayout(catalogPanel);

    catalogLayout->addWidget(new QLabel("<h2> Store Catalog </h2>"));

    productModel= new QSqlTableModel(this);
    productModel->setTable("products");
    productModel->setFilter("quantity > 0"); 
    productModel->select();

    catalogView= new QTableView();
    catalogView->setModel(productModel);
    catalogView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    catalogView->setSelectionBehavior(QAbstractItemView::SelectRows);
    catalogView->setSelectionMode(QAbstractItemView::SingleSelection);// only one product can be selected at a time
    catalogView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    catalogView->hideColumn(6); // hide low stock threshold
    catalogView->hideColumn(4); // hide cost_price for customers

    QPushButton *btnAdd= new QPushButton("Add Selected to Cart ->");
    btnAdd->setStyleSheet("background-color: #3498db; color: white; padding: 10px; font-weight: bold;");

    catalogLayout->addWidget(catalogView);
    catalogLayout->addWidget(btnAdd);

    // right panel : shopping cart -

    QWidget *cartPanel = new QWidget();
    cartPanel->setFixedWidth(350);
    QVBoxLayout *cartLayout = new QVBoxLayout(cartPanel);

    QHBoxLayout *cartHeaderLayout = new QHBoxLayout();
    QLabel *cartTitle = new QLabel("<h2> Your Cart</h2>");
    QPushButton *btnLogout = new QPushButton("Log Out");
    btnLogout->setStyleSheet("background-color: #c0392b; color: white; padding: 8px; font-weight: bold;");

    cartHeaderLayout->addWidget(cartTitle);
    cartHeaderLayout->addStretch();
    cartHeaderLayout->addWidget(btnLogout);

    cartLayout->addLayout(cartHeaderLayout);

    cartList= new QListWidget();

    totalLabel = new QLabel("<b> Total : $0.00</b>");
    totalLabel->setStyleSheet("font-size: 18px; color: #2c3e50;");

    btnCheckout =new QPushButton("Proceed to checkout");
    btnCheckout->setStyleSheet("background-color: #2ecc71; color: white; padding: 12px; font-weight: bold; font-size: 14px;");

    cartLayout->addWidget(cartList);
    cartLayout->addWidget(totalLabel);
    cartLayout->addWidget(btnCheckout);

    //add to main layout
    mainLayout->addWidget(catalogPanel);
    mainLayout->addWidget(cartPanel);

    //connections:
    connect(btnAdd, &QPushButton::clicked, this, &CustomerWidget::addToCart);
    connect(btnCheckout, &QPushButton::clicked,this, &CustomerWidget::processCheckout);
    connect(btnLogout, &QPushButton::clicked, this,&CustomerWidget::onLogoutClicked);
}

void CustomerWidget::addToCart(){
    int row = catalogView->currentIndex().row(); // get the row number of the selected item
    if(row < 0){
        QMessageBox::warning(this, "Select item","Please select an item from the catalog first."); 
        return;
    }

    int id = productModel->index(row, 0).data().toInt();//productModel is a pointer to a QSqlTableModel, which represents the data from the "products" table in the database. index(row, 0) creates a QModelIndex that points to the cell at the specified row and column (column 0 is typically the ID column). data() retrieves the data stored in that cell, which is expected to be the product ID. toInt() converts that data to an integer type.
    QString name = productModel->index(row, 1).data().toString();
    double price = productModel->index(row,3).data().toDouble();
    int availableQty= productModel->index(row,4).data().toInt();

    //ask user for quantity
    bool ok;
    int qtyToBuy=QInputDialog::getInt(this, "Quantity","How many ?",1,1, availableQty, 1,&ok); 
    //getInt is a static method of QInputDialog that shows a dialog asking the user to input an integer value. the parameters are as follows:
    //1. this: the parent widget for the dialog
    //2. "Quantity": the title of the dialog window
    //3. "How many?": the label text prompting the user for input
    //4. 1: the default value shown in the input field when the dialog appears
    //5. 1: the minimum value that the user can enter (in this case, they must buy at least 1 item)
    //6. availableQty: the maximum value that the user can enter, which is limited by the available stock quantity for that product
    //7. 1: the step value, which determines how much the value changes when the user clicks the up/down arrows in the input field (in this case, it increments/decrements by 1)
    //8. &ok: a pointer to a boolean variable that will be set to true if the user clicks "OK" and enters a valid value, or false if they cancel the dialog or enter an invalid value.
    if(ok && qtyToBuy>0){
        //check if already in cart
        bool found = false;
        for(CartItem &item:shoppingCart){
            if(item.getProductId()==id){ //used getter
                if(item.getQuantity() + qtyToBuy <= availableQty){
                    item.addQuantity(qtyToBuy); 
                } else{
                    QMessageBox::warning(this,"Stock error", "Not enough stock available");
                }
                found = true;
                break;
            }
        }
        if(!found){
            shoppingCart.push_back(CartItem(id, name, price, qtyToBuy));
            //shoppingCart is a std::vector that holds CartItem structs. push_back is a method of std::vector that adds a new element to the end of the vector. the argument to push_back is a CartItem struct initialized with the values {id, name, price, qtyToBuy}. this creates a new CartItem with the product ID, name, price, and quantity specified by the user and adds it to the shopping cart.
        }
        updateCartUI(); // after adding the item to the cart, we call updateCartUI() to refresh the cart display and update the total price shown to the user.
    }

}

void CustomerWidget::updateCartUI(){
    cartList->clear(); 
     // clear the current contents of the cartList widget, which is a QListWidget that displays the items in the shopping cart. this ensures that we start with an empty list before repopulating it with the updated cart items.
     currentTotal=0.0; // reset the current total price to 0 before recalculating it based on the items in the shopping cart.

     for(const CartItem &item: shoppingCart){
        QString displayText = QString::number(item.getQuantity()) +" x "+item.getName()+ " - $" + QString::number(item.getPrice()*item.getQuantity(),'f',2); // used getters and setters available in cartitem file in models folder
        // this line constructs a display string for each item in the shopping cart. it uses QString::number to convert the quantity and total price to strings, and concatenates them with the product name to create a readable format like "2 x Widget - $19.98". the total price for each item is calculated by multiplying the unit price (item.getPrice()) by the quantity (item.getQuantity()), and formatted as a fixed-point number with 2 decimal places.
        cartList->addItem(displayText);
        currentTotal +=item.getTotal();

     }
     totalLabel->setText(QString("<b>TOTAL: $%1</b>").arg(currentTotal,0,'f',2));
    }

void CustomerWidget::processCheckout(){
    if(shoppingCart.empty()){
        QMessageBox::warning(this, "empty cart","Your cart is empty!");
        return;
    }
// handle payment:
    bool ok ;
    double cashReceived = QInputDialog::getDouble(this, "Payment", QString("Total is $%1. Enter cash amount: ").arg(currentTotal, 0,'f',2),
                                                    currentTotal, currentTotal, 1000000,2, &ok); // this shows a dialog asking the user to enter the amount of cash received for the purchase. the parameters are:
    //1. this: the parent widget for the dialog
    //2. "Payment": the title of the dialog window
    //3. QString("Total is $%1. Enter cash received: ").arg(current
    //   Total, 0, 'f', 2): the label text prompting the user for input, which includes the total price formatted as a fixed-point number with 2 decimal places.
    //4. currentTotal: the default value shown in the input field, which is set to the total price of the items in the cart.
    //5. currentTotal: the minimum value that the user can enter, which is set to the total price to ensure that they cannot enter an amount less than the total.
    //6. 1000000: the maximum value that the user can enter, which is set to a large number to allow for any reasonable amount of cash received.
    //7. 2: the number of decimal places allowed in the input, which is set to 2 for currency formatting.
    
    if(!ok) return;
    if(cashReceived < currentTotal){
            QMessageBox::critical(this, "Error", "Insufficient cash provided.");
            return;
    }
    //2. start database transaction:
    QSqlDatabase db= QSqlDatabase::database();
    db.transaction(); // this starts a new transaction on the database connection. a transaction is a sequence of database operations that are treated as a single unit of work. if any operation within the transaction fails, the entire transaction can be rolled back to maintain data integrity.
    QSqlQuery updateStock;
    QSqlQuery recordSale;

    try {
        for(const CartItem &item: shoppingCart){
            //update inventory: subtract the quantity
            updateStock.prepare("UPDATE products SET quantity = quantity - :qty WHERE id = :id");
            updateStock.bindValue(":qty", item.getQuantity()); //bindvalue? is a function of 
            updateStock.bindValue(":id", item.getProductId());

            if(!updateStock.exec()) throw  std::runtime_error("stock update failed:");

            //record the sale for admin reports 
            recordSale.prepare("INSERT INTO sales (product_id, quantity, total_price) "
                               "VALUES (:pid, :qty, :total)");
            recordSale.bindValue(":pid", item.getProductId()); //used getter
            recordSale.bindValue(":qty", item.getQuantity()); // used getter
            recordSale.bindValue(":total", item.getTotal());

            if(!recordSale.exec()) throw std::runtime_error("Sale recording failed");
        }
        //commit changes:
        if(db.commit()){
            double change = cashReceived - currentTotal;
            QMessageBox::information(this, "Success", 
                                     QString("Purchase Success!\n\nTotal: $%1\nCash: $%2\nChange: $%3").arg(currentTotal, 0, 'f',2).arg(cashReceived,0,'f',2).arg(change,0,'f',2));
            shoppingCart.clear();
            updateCartUI();
            productModel->select();
        }else{
            throw std::runtime_error("transaction commit failed");
        }
    } catch (const std::exception &e){
        db.rollback();//undo everything if any part failed
        // dxtguhj
        QMessageBox::critical(this, "Transaction Error", e.what()); // show an error message to the user with the details of the exception that was caught. e.what() returns a C-style string describing the error, which is displayed in the critical message box.
    }
}
void CustomerWidget::onLogoutClicked(){
    shoppingCart.clear();
    updateCartUI();
    emit logoutRequested(); // this emits the logoutRequested signal, which can be connected to a slot in the MainWindow to handle the logout process, such as showing the login screen again.
}
