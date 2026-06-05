#ifndef ADMINWIDGET_H
#define ADMINWIDGET_H

#include <QWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QSqlTableModel>
#include <QTableView>
#include <QSortFilterProxyModel>
#include <QLineEdit>
#include <QFrame>
#include <QLabel>
#include <QComboBox>

class AdminWidget : public QWidget {
    Q_OBJECT

public:
    explicit AdminWidget(QWidget *parent = nullptr);

signals:
    void logoutRequested();

private slots:
    void navToDashboard();
    void navToInventory();
    void navToReports();
    void navToUsers();

    //Slot Methods
    void onAddProductClicked();
    void onDeleteProductClicked();
    void onRequestStockClicked();
    void onFilterLowStockChanged(int state);
    void onRefreshInventoryClicked();
    void onAddUserClicked();
    void onDeleteUserClicked();

private:
    QStackedWidget *contentStack;
    
    // UI Pointers 
    QLineEdit *searchBar;
    QTableView *inventoryView;
    QTableView *userView;
    QLineEdit *newUserField;
    QLineEdit *newPassField;
    QComboBox *roleCombo;
    QLabel *revenueLabel;
    QLabel *profitLabel;

    // Database Models
    QSqlTableModel *userModel;
    QSqlTableModel *inventoryModel;
    QSortFilterProxyModel *proxyModel;
    QSqlTableModel *salesModel;
    
    void setupUI();
    void refreshReportsData();
    QWidget* createInventoryPage();
    QWidget* createDashboardPage();
    QWidget* createReportsPage();
    QWidget* createUsersPage();

    QFrame* createCard(const QString& title, QLabel *value, const QString& color);
};

#endif