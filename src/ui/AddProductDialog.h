#ifndef ADDPRODUCTDIALOG_H
#define ADDPRODUCTDIALOG_H

#include <QDialog>
#include<QLineEdit>
#include<QComboBox>
#include<QDoubleSpinBox>
#include<QSpinBox>
#include<QPushButton>

class AddProductDialog: public QDialog{
    Q_OBJECT
public:
    explicit AddProductDialog(QWidget *parent = nullptr);

private slots:
    void saveProduct();
private:
    QLineEdit *nameField;
    QComboBox *categoryBox;
    QDoubleSpinBox *priceField;
    QSpinBox *quantityField;
    QSpinBox *thresholdField;
    QDoubleSpinBox *costPriceField;

    void setupUI();

};
#endif
