#ifndef VENDORWIDGET_H
#define VENDORWIDGET_H

#include <QWidget>
#include<QSqlTableModel>
#include <QTableView>
#include <QPushButton>

class VendorWidget: public QWidget{
    Q_OBJECT //enables signals and slots mechanism in this class
public:
    explicit VendorWidget(QWidget *parent=nullptr);

signals:
    void logoutRequested();

private slots:
    void fulfillRequest();
    void onRefreshClicked();
private:
    QSqlTableModel *requestModel;
    QTableView *requestView;
    QPushButton *btnFulfill;
    void setupUI();
};

#endif //gaurd 