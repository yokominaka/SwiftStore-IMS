#include "AddProductDialog.h"
#include "database/DatabaseManager.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>

AddProductDialog::AddProductDialog(QWidget *parent) : QDialog(parent){
    setWindowTitle("Add New Product");
    setMinimumWidth(350);
    setupUI();

}

void AddProductDialog::setupUI(){
    QVBoxLayout *mainLayout =new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    //name field:
    nameField= new QLineEdit();
    nameField->setPlaceholderText("e.g. Dell Latitude 7400");
    //category field
    categoryBox = new QComboBox();
    categoryBox->addItems({"Electronics", "Clothing", "Perishable", "Furniture"});
    //price field, max 99999.99, no text allowed

    priceField = new QDoubleSpinBox();
    priceField->setRange(0.01, 99999.99);
    priceField->setPrefix("$ ");

    costPriceField = new QDoubleSpinBox();
    costPriceField->setRange(0.01, 99999.99);
    costPriceField->setPrefix("Cost: $ ");

    // 4. quantity field (validation: only posivie integers)
    quantityField = new QSpinBox();
    quantityField->setRange(0,10000);

    //5. threshold field for low stock alerts
    thresholdField = new QSpinBox();
    thresholdField->setRange(1,1000);
    thresholdField->setValue(10);

    //add to form layout
    formLayout->addRow("Product Name:", nameField);
    formLayout->addRow("Category:", categoryBox);
    formLayout->addRow("Price:", priceField);
    formLayout->addRow("Stock Qty:", quantityField);
    formLayout->addRow("Low Stock Threshold:", thresholdField);
    formLayout->addRow("Cost Price:", costPriceField);

    //buttons:
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *saveBtn = new QPushButton("Save product");
    saveBtn->setStyleSheet("background-color: #3498db; color: white; padding: 6px;");
    QPushButton *cancelBtn = new QPushButton("cancel");

    buttonLayout->addWidget(cancelBtn);
    buttonLayout->addWidget(saveBtn);

    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);

    connect(saveBtn, &QPushButton::clicked, this, &AddProductDialog::saveProduct);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

}

void AddProductDialog::saveProduct(){
    if(nameField->text().trimmed().isEmpty()){
        QMessageBox::warning(this,"Input Error","Product name cant be empty.");
        return;
    }
    DatabaseManager dbManager;
    if(dbManager.addProduct(nameField->text().trimmed(), 
                            categoryBox->currentText(), 
                            priceField->value(), 
                            costPriceField->value(), 
                            quantityField->value(), 
                            thresholdField->value())) {
        accept(); // Closes the dialog
    } else {
        QMessageBox::critical(this,"Database Error","Failed to add product to database.");
    }
}
