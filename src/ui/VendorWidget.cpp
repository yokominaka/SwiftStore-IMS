#include "VendorWidget.h"
#include <QVBoxLayout>
#include<QHBoxLayout>
#include<QLabel>
#include<QHeaderView>
#include<QMessageBox>
#include<QSqlQuery>
#include<QSqlDatabase>
#include<QSqlError>

VendorWidget::VendorWidget(QWidget *parent):QWidget(parent){
    setupUI();
}

void VendorWidget::setupUI(){
    QVBoxLayout *layout=new QVBoxLayout(this);

    QHBoxLayout *headerLayout = new QHBoxLayout();
    headerLayout->addWidget(new QLabel("<h1> Vendor Fulfillment Portal</h1>"));
    headerLayout->addStretch();

    QPushButton *btnLogout = new QPushButton("Log Out");
    btnLogout->setStyleSheet("background-color: #c0392b; color: white; padding: 8px; font-weight: bold;");
    headerLayout->addWidget(btnLogout);
    
    QPushButton *refreshbutton= new QPushButton("Refresh requests");
    refreshbutton->setStyleSheet("background-color: #3498db; color: white; padding: 8px; font-weight: bold;");
    headerLayout->addWidget(refreshbutton);

    layout->addLayout(headerLayout);
    layout->addWidget(new QLabel("<p>Review and fulfill incoming restock requests from administration.</p>"));

    // seting up the request model
    requestModel=new QSqlTableModel(this);
    requestModel->setTable("purchase_requests");
    requestModel->setFilter("status='PENDING'");
    requestModel->select();

    //-the view--

    requestView= new QTableView();
    requestView->setModel(requestModel);
    requestView->setSelectionBehavior(QAbstractItemView::SelectRows);
    requestView->setSelectionMode(QAbstractItemView::SingleSelection);
    requestView->setSelectionBehavior(QAbstractItemView::SelectRows);
    requestView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //action button----
    btnFulfill=new QPushButton("✓ Fulfill & Ship Stock");
    btnFulfill->setStyleSheet("background-color: #2ecc71; color: white; padding: 12px; font-weight: bold; font-size: 14px;");

    layout->addWidget(requestView);
    layout->addWidget(btnFulfill);


    connect(btnFulfill,&QPushButton::clicked,this,&VendorWidget::fulfillRequest);
    connect(btnLogout, &QPushButton::clicked, this, &VendorWidget::logoutRequested);
    connect(refreshbutton,&QPushButton::clicked,this,&VendorWidget::onRefreshClicked);
}
void VendorWidget::fulfillRequest(){
    int row = requestView->currentIndex().row();
    if(row < 0){
        QMessageBox::warning(this,"Select Request","Please select a pending request to fullfill");
        return;
    }
    int requestid=requestModel->index(row,0).data().toInt();
    int productid=requestModel->index(row,1).data().toInt();
    int qtyToAdd=requestModel->index(row,2).data().toInt();

    //start transaction

    QSqlDatabase db=QSqlDatabase::database(); //this gets the default database connection that was set up in DatabaseManager. since we only have one connection, this is the one we want to use for our queries.
    db.transaction();

    QSqlQuery updatestock;
    QSqlQuery updaterequest;

    try{
        //injecting stock in products table
        updatestock.prepare("UPDATE products SET quantity = quantity + :qty WHERE id = :id");
        updatestock.bindValue(":qty",qtyToAdd);
        updatestock.bindValue(":id",productid);
        if(!updatestock.exec()) throw std::runtime_error("faileded to update request status");


        //2. change request status so it drops off vendors dashboard
        updaterequest.prepare("UPDATE purchase_requests SET status = 'FULFILLED' WHERE id = :rid");
        updaterequest.bindValue(":rid", requestid);
        if(!updaterequest.exec()) throw std::runtime_error("failed to update request status");


        //commit changes:
        if(db.commit()){//did we define commit? yes, commit is a method of QSqlDatabase that finalizes the transaction. if all queries executed successfully, we call commit to save the changes to the database. if any query failed, we throw an exception and skip the commit, which will cause the transaction to be rolled back when the QSqlDatabase object goes out of scope.
            
            QMessageBox::information(this, "Success", "Stock succesfully shipped and inventory updated!.");
            
            requestModel->select(); //refresh the model to reflect the changes in the database. this will cause the view to update and the fulfilled request will no longer be shown since we have a filter for "PENDING" status.
        } else {
            throw std::runtime_error("database commit failed!!");
        }
    } catch(const std::exception &e)// whats exception &e? std::exception is the base class for all standard exceptions in C++. by catching a reference to std::exception, we can handle any exception that might be thrown in the try block, including those we explicitly throw with std::runtime_error. the what() method of std::exception returns a C-style string describing the error, which we can display in the message box to give feedback on what went wrong.
    {
        db.rollback();
        QMessageBox::critical(this, "Error",e.what()); //e.what returns a c style string to show what went wrong.
    }

}
void VendorWidget::onRefreshClicked(){
    requestModel->select(); // this will re-query the database and update the model with any new pending requests that may have come in since the last time we loaded the data. it's a simple way to refresh the view without having to recreate the model or the view.
}