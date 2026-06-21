#include "logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Вход в систему");
    setFixedSize(300, 150);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *label = new QLabel("Введите ваш никнейм:", this);
    nicknameEdit = new QLineEdit(this);
    loginButton = new QPushButton("Войти", this);

    layout->addWidget(label);
    layout->addWidget(nicknameEdit);
    layout->addWidget(loginButton);

    // При нажатии на "Войти" вызываем accept() - это закроет диалог с кодом Accepted
    connect(loginButton, &QPushButton::clicked, this, &QDialog::accept);
}

QString LoginDialog::getNickname() const {
    return nicknameEdit->text().trimmed();
}
