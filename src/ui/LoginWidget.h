#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include<QLineEdit>
#include<QPushButton>
#include<QLabel>


class LoginWidget: public QWidget { // Inherits from QWidget to create a custom login widget
    Q_OBJECT
public:
    explicit LoginWidget(QWidget *parent = nullptr);
signals: // Emitted when login is successful, passing user role and username
    void loginSuccessful(QString role, QString username);
private slots:
    void handleLogin();
private:
    // QVBoxLayout *layout
    QLineEdit *usernameField;
    QLineEdit *passwordField;
    QLabel *errorLabel;

    void setupUI();

};

#endif
