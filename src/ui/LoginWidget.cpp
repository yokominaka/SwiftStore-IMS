#include "LoginWidget.h"
#include "database/DatabaseManager.h"
#include <QVBoxLayout>
#include <QSqlQuery>
#include <QSqlError>

LoginWidget::LoginWidget(QWidget *parent): QWidget(parent){
    setupUI();
}

void LoginWidget::setupUI(){
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *title = new QLabel("Welcome to SwiftStore!");
    title->setStyleSheet("font-size: 20px; font-weight: bold; margin-bottom: 20px;");

    usernameField = new QLineEdit();
    usernameField->setPlaceholderText("Username");
    usernameField->setFixedWidth(250);

    passwordField = new QLineEdit();
    passwordField->setPlaceholderText("Password");
    passwordField->setEchoMode(QLineEdit::Password);
    passwordField->setFixedWidth(250);

    QPushButton *loginBtn = new QPushButton("Login");
    loginBtn->setFixedWidth(250);
    loginBtn->setStyleSheet("background-color: #2ecc71; color: white; padding: 8px;");

    errorLabel = new QLabel();
    errorLabel->setStyleSheet("color: red;");

    layout->addWidget(title);
    layout->addWidget(usernameField);
    layout->addWidget(passwordField);
    layout->addWidget(loginBtn);
    layout->addWidget(errorLabel);

    connect(loginBtn, &QPushButton::clicked, this, &LoginWidget::handleLogin);

}

void LoginWidget::handleLogin(){
    QString user = usernameField->text();
    QString pass = passwordField->text();
    QString role;

    DatabaseManager dbManager;
    if(dbManager.authenticateUser(user,pass,role)){
        emit loginSuccessful(role, user);
    } else{
        errorLabel->setText("Invalid username or password");
    }
}