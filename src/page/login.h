#ifndef LOGIN_H
#define LOGIN_H

#include "banner.h"  // 引入已实现的Banner类"

#include <QWidget>
#include <QStackedWidget>  // 用于QStackedWidget
#include <QButtonGroup>    // 用于QButtonGroup
#include <QLineEdit>       // 用于QLineEdit
#include <QPushButton>     // 用于QPushButton
#include <QLabel>          // 用于QLabel
#include <QTimer>          // 用于QTimer
#include <QCheckBox>       // 关键：补充QCheckBox头文件
#include <QVBoxLayout>     // 用于垂直布局
#include <QHBoxLayout>     // 用于水平布局
#include <QRegularExpressionValidator>  // 用于输入验证

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login() override;

private slots:
    // 切换登录方式
    void onLoginModeChanged(int index);
    // 获取验证码
    void onGetVerifyCode();
    // 倒计时更新
    void updateCountdown();
    // 登录按钮点击
    void onLoginClicked();
    // 注册/忘记密码按钮点击
    void onAuxButtonClicked();

private:
    // 初始化UI
    void initUI();
    // 设置布局
    void setupLayout();
    // 连接信号槽
    void setupConnections();
    // 创建手机验证码登录面板
    QWidget* createPhoneLoginPanel();
    // 创建账户登录面板
    QWidget* createAccountLoginPanel();
    // 创建扫码登录面板
    QWidget* createQRLoginPanel();

private:
    // 顶部标题栏
    Banner* m_banner;

    // 左侧登录方式切换
    QButtonGroup* m_loginModeGroup;
    QPushButton* m_btnPhoneLogin;    // 手机验证码登录按钮
    QPushButton* m_btnAccountLogin;  // 账户登录按钮
    QStackedWidget* m_stackedLogin;  // 登录面板容器

    // 手机验证码登录控件
    QLineEdit* m_lePhone;           // 手机号输入框
    QLineEdit* m_leVerifyCode;      // 验证码输入框
    QPushButton* m_btnGetCode;       // 获取验证码按钮
    QTimer* m_countdownTimer;        // 倒计时定时器
    int m_remainingTime;             // 剩余时间（秒）

    // 账户登录控件
    QLineEdit* m_leAccount;         // 账户输入框
    QLineEdit* m_lePassword;        // 密码输入框
    QCheckBox* m_chkRememberPwd;     // 记住密码复选框
    QPushButton* m_btnForgetPwd;     // 忘记密码按钮

    // 通用控件
    QPushButton* m_btnLogin;         // 登录按钮
    QPushButton* m_btnRegister;      // 注册按钮

    // 右侧扫码登录控件
    QLabel* m_lblQRCode;             // 二维码显示
    QLabel* m_lblQRHint;             // 扫码提示
};

#endif // LOGIN_H
