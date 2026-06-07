#include "AdminWidget.h"
#include "AddProductDialog.h"
#include "../database/DatabaseManager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QSqlQuery>
#include <QHeaderView>
#include <QMessageBox>
#include<QInputDialog>
#include<QSqlError>
#include<QCheckBox>

AdminWidget::AdminWidget(QWidget *parent):QWidget(parent)
{
    setupUI();
}

void AdminWidget::setupUI()
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    //--sidebar--
    QWidget *sidebar = new QWidget();
    sidebar->setFixedWidth(200);
    sidebar->setStyleSheet("background-color: #2c3e50; color: white;");
    QVBoxLayout *sideLayout = new QVBoxLayout(sidebar);

    QPushButton *btnDash=new QPushButton("Dashboard");
    QPushButton *btnInventory= new QPushButton("Inventory");
    QPushButton *btnReports= new QPushButton("Reports");
    QPushButton *btnUsers= new QPushButton("Users");
    QPushButton *btnLogout = new QPushButton("Log Out");

    // Modern CSS for Sidebar Buttons
    QString btnStyle="QPushButton{text-align: left; padding: 10px;border:none; color: white;font-size: 14px; }"
                       "QPushButton:hover{ background-color: #34495e; }";

    btnDash->setStyleSheet(btnStyle);
    btnInventory->setStyleSheet(btnStyle);
    btnReports->setStyleSheet(btnStyle);
    btnUsers->setStyleSheet(btnStyle);
    btnLogout->setStyleSheet("QPushButton{text-align: left; padding: 10px;border:none; color: white;font-size: 14px; background-color: #c0392b;}"
                             "QPushButton:hover{ background-color: #a93226; }");

    sideLayout->addWidget(btnDash);
    sideLayout->addWidget(btnInventory);
    sideLayout->addWidget(btnReports);
    sideLayout->addWidget(btnUsers);
    sideLayout->addStretch(); // pushes buttons to the top;
    sideLayout->addWidget(btnLogout);

    //---content area---
    contentStack = new QStackedWidget();
    contentStack->addWidget(createDashboardPage());
    contentStack->addWidget(createInventoryPage());
    contentStack->addWidget(createReportsPage());
    contentStack->addWidget(createUsersPage()); // add this line to include the users page

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(contentStack);

    connect(btnDash,&QPushButton::clicked, this, &AdminWidget::navToDashboard);
    connect(btnInventory, &QPushButton::clicked, this, &AdminWidget::navToInventory);
    connect(btnReports, &QPushButton::clicked, this, &AdminWidget::navToReports); // explain the syntax: connect(sender, signal, receiver, slot) when btnReports is clicked, it emits a signal which is connected to the navToReports slot of this class (AdminWidget), so that function will be called and the content stack will switch to the reports page.
    connect(btnUsers, &QPushButton::clicked, this, &AdminWidget::navToUsers);     // connect the users button to the navToUsers slot)
    connect(btnLogout, &QPushButton::clicked, this, &AdminWidget::logoutRequested);
}

QWidget *AdminWidget::createInventoryPage()
{
    QWidget *page  =new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);

    QHBoxLayout *topLayout =new QHBoxLayout();

    QLabel *title= new QLabel("<h1>Inventory</h1>");
    // topLayout->addWidget(new QLabel("<h1> Inventory Management </h1>"));

    searchBar = new QLineEdit();
    searchBar->setPlaceholderText("Search by product name...");
    searchBar->setFixedWidth(300);
    searchBar->setStyleSheet("padding: 8px; border-radius: 5px; border: 1px solid #ccc;");

    QCheckBox *checkLowStock = new QCheckBox("Show low stock only");
    checkLowStock->setStyleSheet("color: white; font-weight: bold; padding-left: 10px;");

    QPushButton *btnAddProduct = new QPushButton("+ Add Product");
    btnAddProduct->setStyleSheet("background-color: #2ecc71; color: white; font-weight: bold; padding: 8px;");

    QPushButton *btnDeleteProduct = new QPushButton("Delete Selected Product");
    btnDeleteProduct->setStyleSheet("background-color: #e74c3c; color: white; font-weight: bold; padding: 8px;");

    //restock buton
    QPushButton *btnRequestStock=new QPushButton("Request Restock");
    btnRequestStock->setStyleSheet("background-color: #e67e22; color: white; font-weight: bold; padding: 8px;");

    QPushButton *btnrefresh = new QPushButton("Refresh");
    btnrefresh->setStyleSheet("background-color: #3498db; color: white; font-weight: bold; padding: 8px;");



    topLayout->addWidget(title);
    topLayout->addStretch(); // puts space in middle
    topLayout->addWidget(searchBar);
    topLayout->addWidget(checkLowStock);
    topLayout->addWidget(btnAddProduct);
    topLayout->addWidget(btnDeleteProduct);
    topLayout->addWidget(btnrefresh);

    topLayout->addWidget(btnRequestStock);

    layout->addLayout(topLayout);

    // set up the SQL Model:
    //  model and proxy logic.....

    inventoryModel = new QSqlTableModel(this);
    inventoryModel->setTable("products");
    inventoryModel->select(); 

    // inventoryModel->setEditStrategy(QSqlTableModel::OnFieldChange);

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(inventoryModel);
    proxyModel->setFilterKeyColumn(1);                         // column 1 is "name" in our database
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive); // ignore lowercase and uppercase when filtering

    // --- The view ---

    inventoryView = new QTableView(this);
    inventoryView->setModel(proxyModel);                                           // important: view now looks at  the proxy model, not the original model
    inventoryView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // make columns stretch to fill available space


    // inventoryView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    inventoryView->setAlternatingRowColors(true);
    inventoryView->setStyleSheet("QTableView { background-color: #2d2d2d; color: white; }"
                        "QTableView::item:selected { background-color: #34495e; }"
                        "QHeaderView::section { background-color: #1f1f1f; color: white; padding: 6px; }");

    layout->addWidget(inventoryView);

    connect(searchBar, &QLineEdit::textChanged, proxyModel, &QSortFilterProxyModel::setFilterFixedString);
    connect(btnAddProduct, &QPushButton::clicked, this, &AdminWidget::onAddProductClicked);
    connect(btnDeleteProduct, &QPushButton::clicked, this, &AdminWidget::onDeleteProductClicked);
    connect(btnRequestStock, &QPushButton::clicked, this, &AdminWidget::onRequestStockClicked);
    connect(checkLowStock, &QCheckBox::checkStateChanged, this, &AdminWidget::onFilterLowStockChanged);
    connect(btnrefresh, &QPushButton::clicked, this, &AdminWidget::onRefreshInventoryClicked);

        return page;
}


QWidget *AdminWidget::createDashboardPage()
{
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->addWidget(new QLabel("<h1>Welcome, Admin</h1><p>Manage your inventory, users, and reports from the sidebar.</p>"));
    layout->addStretch();
    return page;
}
QWidget *AdminWidget::createReportsPage()
{
    QWidget *page = new QWidget();
    QVBoxLayout *layout= new QVBoxLayout(page);

    layout->addWidget(new QLabel("<h1> Financial Reports </h1>"));
    QHBoxLayout *statsLayout= new QHBoxLayout();

    revenueLabel= new QLabel("$0.00");
    profitLabel= new QLabel("$0.00");
 
    statsLayout->addWidget(createCard("Total Revenue: ",revenueLabel, "#3498db"));
    statsLayout->addWidget(createCard("Total Profit: ",profitLabel, "#2ecc71"));
    layout->addLayout(statsLayout);

    layout->addWidget(new QLabel("<h3> Recent Transactions </h3>"));

    salesModel = new QSqlTableModel(this); // explain syntax: we create a new QSqlTableModel to represent the "sales" table from the database. The constructor takes 'this' as the parent, which means the model will be automatically deleted when the AdminWidget is destroyed.
    salesModel->setTable("sales");

    QTableView *salesView = new QTableView();
    salesView->setModel(salesModel);
    salesView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    layout->addWidget(salesView);
    return page;
}

QFrame *AdminWidget::createCard(const QString &title, QLabel *value,const QString &color)
{
    QFrame *card= new QFrame();
    card->setStyleSheet(QString("background-color: %1; color: white; border-radius: 10px; padding: 15px;").arg(color));

    QVBoxLayout *l= new QVBoxLayout(card);
    l->addWidget(new QLabel(title));

    value->setStyleSheet("font-size: 18px;font-weight: bold;");
    l->addWidget(value);

    // QLabel *val = new QLabel(value);
    // val->setStyleSheet("font-size: 18px;font-weight: bold;");
    // l->addWidget(val);


    return card;
}
QWidget *AdminWidget::createUsersPage()
{
    QWidget *page= new QWidget();
    QVBoxLayout *layout=new QVBoxLayout(page);

    layout->addWidget(new QLabel("<h1> User Management </h1>"));

    userModel = new QSqlTableModel(this);
    userModel->setTable("users");
    userModel->select();

    userModel->setEditStrategy(QSqlTableModel::OnFieldChange);

    QTableView *userView=new QTableView();
    userView->setModel(userModel);
    userView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // action buttons:
    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnDeleteUser = new QPushButton("Delete Selected User");
    btnDeleteUser->setStyleSheet("background-color:#e74c3c; color: white; padding: 8px;");

    newUserField=new QLineEdit();
    newUserField->setPlaceholderText("New username");

    newPassField=new QLineEdit();
    newPassField->setPlaceholderText("New password");
    newPassField->setEchoMode(QLineEdit::Password);

    roleCombo=new QComboBox();
    roleCombo->addItems({"Customer","Vendor","Admin"});

    QPushButton* btnAddUser=new QPushButton("Add User");
    btnAddUser->setStyleSheet("background-color: #2ecc71; color: white; padding: 8px;");

    QFormLayout *userFormLayout = new QFormLayout();
    userFormLayout->addRow("Username:", newUserField);
    userFormLayout->addRow("Password:", newPassField);
    userFormLayout->addRow("Role:", roleCombo);
    //
    btnLayout->addStretch();
    btnLayout->addWidget(btnDeleteUser);
    btnLayout->addWidget(btnAddUser);
    layout->addLayout(userFormLayout);
    layout->addWidget(userView);
    layout->addLayout(btnLayout);
    
    // connect(btnAddUser, &QPushButton::clicked, this,[=]()

    connect(btnAddUser,&QPushButton::clicked,this,&AdminWidget::onAddUserClicked);
    connect(btnDeleteUser,&QPushButton::clicked,this,&AdminWidget::onDeleteUserClicked);

    return page;
}
  
void AdminWidget::refreshReportsData()
{
    QSqlQuery query;
    query.exec("SELECT SUM(sales.total_price), "
               "SUM(sales.total_price-(sales.quantity*products.cost_price)) "
               "FROM sales "
               "INNER JOIN products ON sales.product_id= products.id");
    if(query.next()){
        revenueLabel->setText(QString("$%1").arg(query.value(0).toDouble(),0,'f',2));
        profitLabel->setText(QString("$%1").arg(query.value(1).toDouble(),0,'f',2));
    }
    salesModel->select(); // refresh the sales data in the table view
}



void AdminWidget::navToDashboard() {contentStack->setCurrentIndex(0); }

void AdminWidget::navToInventory() {
    inventoryModel->select(); //refreshes data
    contentStack->setCurrentIndex(1); 
}

void AdminWidget::navToReports() {
    refreshReportsData(); //used reresh function 
    contentStack->setCurrentIndex(2); 
}

void AdminWidget::navToUsers() {
    userModel->select();
    contentStack->setCurrentIndex(3); 
}

// --- Button Click Slots (These replaced the lambdas!) ---

void AdminWidget::onAddProductClicked(){
    AddProductDialog dialog(this);
    if(dialog.exec()==QDialog::Accepted){
        inventoryModel->select();
    }
}

void AdminWidget::onDeleteProductClicked(){
    QModelIndex proxyIndex=inventoryView->currentIndex();
    if(!proxyIndex.isValid()){
        QMessageBox::warning(this,"Selection Error","Please select a product to delete.");
        return;
    }
    QModelIndex sourceIndex=proxyModel->mapToSource(proxyIndex);
    QString productName=inventoryModel->data(inventoryModel->index(sourceIndex.row(),1)).toString();
    
    if(QMessageBox::question(this,"Confirm delete",QString("Delete product '%1'?").arg(productName))!=QMessageBox::Yes){
        return;
    }
    if(inventoryModel->removeRow(sourceIndex.row()) && inventoryModel->submitAll()){
        inventoryModel->select();
        QMessageBox::information(this,"Success","Product deleted successfully.");
    }else{
        QMessageBox::critical(this, "Database Error", inventoryModel->lastError().text());
        inventoryModel->revertAll();
    }
}

void AdminWidget::onRequestStockClicked(){
    int row= inventoryView->currentIndex().row();
    if(row <0){
        QMessageBox::warning(this,"Selection Error","Please select a product from the table to restock.");
        return;
    }
    int productId=proxyModel->index(row,0).data().toInt();
    QString productName=proxyModel->index(row,1).data().toString();

    bool ok;
    int qty=QInputDialog::getInt(this, "Purchase Request", QString("How many units of '%1' do you want to request from the vendor?").arg(productName),50,1,10000,1,&ok);

    if(ok&& qty >0){
        DatabaseManager dbManager;
        if(dbManager.addPurchaseRequest(productId,qty)){
            QMessageBox::information(this,"Success","Purchase request sent to vendor successfully!");
        }else{
            QMessageBox::critical(this,"Database error","Failed to send purchase request.");
        }
    }
}

void AdminWidget::onFilterLowStockChanged(int state){
    if(state==Qt::Checked){
        inventoryModel->setFilter("quantity <= threshold");
    } else{
        inventoryModel->setFilter("");
    }
    inventoryModel->select();
}

void AdminWidget::onRefreshInventoryClicked(){
    inventoryModel->select();
}

void AdminWidget::onAddUserClicked(){
    QString username=newUserField->text();
    QString password=newPassField->text();
    QString role=roleCombo->currentText();

    if(username.isEmpty() || password.isEmpty()){
        QMessageBox::warning(this,"Input Error","username and password cannot be empty.");
        return;
    }

    DatabaseManager dbManager;
    if(dbManager.addUser(username,password,role)){
        QMessageBox::information(this,"Success","New user added successfully!");
        userModel->select();
        newUserField->clear();
        newPassField->clear();
    }else{
        QMessageBox::critical(this,"Database Error","Failed to add new user.");
    }
}

void AdminWidget::onDeleteUserClicked(){
    int row=userView->currentIndex().row();
    if(row>=0){
        if(QMessageBox::question(this,"Confirm Delete","Are you sure you want to delete this user?")==QMessageBox::Yes){
            userModel->removeRow(row);
            userModel->select();
        }
    }
}