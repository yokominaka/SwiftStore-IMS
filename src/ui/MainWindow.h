#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include<QMainWindow>
#include<QStackedWidget>
#include<QVBoxLayout>

class LoginWidget;
class AdminWidget;
class VendorWidget;
class CustomerWidget;

class MainWindow : public QMainWindow{
    Q_OBJECT //enables signals and slots mechanism in this class
public:
    MainWindow(QWidget *parent=nullptr);//constructor
    ~MainWindow(); //destructor

    void showLogin();
    void showAdminMenu();
    void showVendorMenu();
    void showCustomerMenu();
private slots:
    //dedicated slot to handle the login event
    void handleLoginSuccessful(QString role, QString username);
    
private:
    QStackedWidget *stackedWidget;

    LoginWidget *loginWidget;
    AdminWidget *adminWidget = nullptr;
    VendorWidget *vendorWidget = nullptr;
    CustomerWidget *customerWidget = nullptr;

    void setupUI();
    void setupRoleConnections();
};

#endif

