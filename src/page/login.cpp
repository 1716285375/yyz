#include "login.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QRegularExpressionValidator>
#include <QMessageBox>
#include <QPixmap>
#include <QSizePolicy>

Login::Login(QWidget *parent) : QWidget(parent)
    , m_remainingTime(60)
    , m_countdownTimer(new QTimer(this))
{
    // 设置窗口属性
    setWindowTitle("登录");
    setFixedSize(800, 500);  // 固定窗口大小
    setWindowFlags(Qt::FramelessWindowHint);  // 无边框窗口（配合Banner拖动）

    initUI();
    setupLayout();
    setupConnections();
}

Login::~Login()
{
    // 手动释放定时器（可选，Qt父对象机制会自动处理）
    if (m_countdownTimer->isActive()) {
        m_countdownTimer->stop();
    }
}

void Login::initUI()
{
    // 1. 顶部标题栏
    m_banner = new Banner("言语宙登录", true, this);  // 使用Banner类，显示最小化按钮

    // 2. 左侧登录方式切换按钮
    m_loginModeGroup = new QButtonGroup(this);
    m_btnPhoneLogin = new QPushButton("手机验证码登录");
    m_btnAccountLogin = new QPushButton("账户登录");
    m_loginModeGroup->addButton(m_btnPhoneLogin, 0);  // ID=0：手机登录
    m_loginModeGroup->addButton(m_btnAccountLogin, 1);  // ID=1：账户登录

    // 设置切换按钮样式
    m_btnPhoneLogin->setCheckable(true);
    m_btnAccountLogin->setCheckable(true);
    m_btnPhoneLogin->setChecked(true);  // 默认选中手机登录

    // 3. 登录面板容器
    m_stackedLogin = new QStackedWidget();
    m_stackedLogin->addWidget(createPhoneLoginPanel());    // 索引0：手机登录面板
    m_stackedLogin->addWidget(createAccountLoginPanel());  // 索引1：账户登录面板

    // 4. 登录/注册按钮
    m_btnLogin = new QPushButton("登录");
    m_btnRegister = new QPushButton("还没有账号？注册");
    m_btnRegister->setFlat(true);  // 扁平样式

    // 5. 二维码登录相关（右侧）
    // （在createQRLoginPanel中初始化）
}

void Login::setupLayout()
{
    // 主布局：垂直布局（标题栏 + 内容区）
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 添加标题栏
    mainLayout->addWidget(m_banner);

    // 内容区：水平布局（左侧登录区 + 分隔线 + 右侧扫码区）
    QHBoxLayout* contentLayout = new QHBoxLayout();
    contentLayout->setContentsMargins(20, 20, 20, 20);
    contentLayout->setSpacing(20);

    // 左侧登录区：垂直布局（切换按钮 + 登录面板 + 登录按钮 + 注册按钮）
    QVBoxLayout* leftLayout = new QVBoxLayout();
    leftLayout->setSpacing(15);

    // 切换按钮布局（水平）
    QHBoxLayout* modeBtnLayout = new QHBoxLayout();
    modeBtnLayout->addWidget(m_btnPhoneLogin);
    modeBtnLayout->addWidget(m_btnAccountLogin);
    leftLayout->addLayout(modeBtnLayout);

    // 添加登录面板
    leftLayout->addWidget(m_stackedLogin);

    // 登录按钮
    leftLayout->addWidget(m_btnLogin);

    // 注册按钮（靠右）
    QHBoxLayout* auxBtnLayout = new QHBoxLayout();
    auxBtnLayout->addStretch();  // 左侧拉伸
    auxBtnLayout->addWidget(m_btnRegister);
    leftLayout->addLayout(auxBtnLayout);

    // 分隔线
    QFrame* separator = new QFrame();
    separator->setFrameShape(QFrame::VLine);
    separator->setStyleSheet("background-color: #EEEEEE;");

    // 右侧扫码区
    QWidget* qrPanel = createQRLoginPanel();

    // 添加到内容区布局
    contentLayout->addLayout(leftLayout, 6);  // 左侧占6份
    contentLayout->addWidget(separator);
    contentLayout->addWidget(qrPanel, 4);     // 右侧占4份

    // 添加内容区到主布局
    mainLayout->addLayout(contentLayout);
}

void Login::setupConnections()
{
    // 登录方式切换
    connect(m_loginModeGroup, &QButtonGroup::idClicked,
            this, &Login::onLoginModeChanged);

    // 验证码倒计时
    connect(m_countdownTimer, &QTimer::timeout, this, &Login::updateCountdown);

    // 获取验证码
    connect(m_btnGetCode, &QPushButton::clicked, this, &Login::onGetVerifyCode);

    // 登录按钮
    connect(m_btnLogin, &QPushButton::clicked, this, &Login::onLoginClicked);

    // 注册/忘记密码按钮
    connect(m_btnRegister, &QPushButton::clicked, this, &Login::onAuxButtonClicked);
    connect(m_btnForgetPwd, &QPushButton::clicked, this, &Login::onAuxButtonClicked);

    // 标题栏信号（关闭/最小化）
    connect(m_banner, &Banner::signalClose, this, &Login::close);
    connect(m_banner, &Banner::signalMinimize, this, &Login::showMinimized);
}

QWidget* Login::createPhoneLoginPanel()
{
    // 手机验证码登录面板
    QWidget* panel = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(panel);
    layout->setSpacing(15);

    // 手机号输入框
    m_lePhone = new QLineEdit();
    m_lePhone->setPlaceholderText("请输入手机号");
    m_lePhone->setValidator(new QRegularExpressionValidator(
        QRegularExpression("^1[3-9]\\d{9}$"), this));  // 手机号验证
    m_lePhone->setStyleSheet("padding: 8px; border: 1px solid #DDDDDD; border-radius: 4px;");

    // 验证码输入框 + 获取按钮
    QHBoxLayout* codeLayout = new QHBoxLayout();
    codeLayout->setSpacing(10);

    m_leVerifyCode = new QLineEdit();
    m_leVerifyCode->setPlaceholderText("请输入验证码");
    m_leVerifyCode->setValidator(new QRegularExpressionValidator(
        QRegularExpression("^\\d{6}$"), this));  // 6位数字验证
    m_leVerifyCode->setStyleSheet("padding: 8px; border: 1px solid #DDDDDD; border-radius: 4px;");

    m_btnGetCode = new QPushButton("获取验证码");
    m_btnGetCode->setStyleSheet("padding: 8px;");

    codeLayout->addWidget(m_leVerifyCode);
    codeLayout->addWidget(m_btnGetCode);

    // 添加到面板
    layout->addWidget(m_lePhone);
    layout->addLayout(codeLayout);
    layout->addStretch();  // 底部拉伸（使内容靠上）

    return panel;
}

QWidget* Login::createAccountLoginPanel()
{
    // 账户登录面板
    QWidget* panel = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(panel);
    layout->setSpacing(15);

    // 账户输入框
    m_leAccount = new QLineEdit();
    m_leAccount->setPlaceholderText("请输入手机号/用户名/邮箱");
    m_leAccount->setStyleSheet("padding: 8px; border: 1px solid #DDDDDD; border-radius: 4px;");

    // 密码输入框
    m_lePassword = new QLineEdit();
    m_lePassword->setPlaceholderText("请输入密码");
    m_lePassword->setEchoMode(QLineEdit::Password);  // 密码隐藏
    m_lePassword->setStyleSheet("padding: 8px; border: 1px solid #DDDDDD; border-radius: 4px;");

    // 记住密码 + 忘记密码
    QHBoxLayout* pwdAssistLayout = new QHBoxLayout();
    m_chkRememberPwd = new QCheckBox("记住密码");
    m_btnForgetPwd = new QPushButton("忘记密码");
    m_btnForgetPwd->setFlat(true);  // 扁平样式
    pwdAssistLayout->addWidget(m_chkRememberPwd);
    pwdAssistLayout->addStretch();
    pwdAssistLayout->addWidget(m_btnForgetPwd);

    // 添加到面板
    layout->addWidget(m_leAccount);
    layout->addWidget(m_lePassword);
    layout->addLayout(pwdAssistLayout);
    layout->addStretch();  // 底部拉伸

    return panel;
}

QWidget* Login::createQRLoginPanel()
{
    // 扫码登录面板
    QWidget* panel = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(panel);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);

    // 二维码图片（这里用占位图，实际项目中可生成实时二维码）
    m_lblQRCode = new QLabel();
    m_lblQRCode->setFixedSize(180, 180);
    m_lblQRCode->setStyleSheet("border: 1px solid #DDDDDD; background-color: white;");
    // 临时占位图（实际项目中替换为真实二维码）
    QPixmap qrPlaceholder(180, 180);
    qrPlaceholder.fill(Qt::white);
    m_lblQRCode->setPixmap(qrPlaceholder);
    m_lblQRCode->setScaledContents(true);

    // 扫码提示
    m_lblQRHint = new QLabel("请使用言语宙App扫码登录");
    m_lblQRHint->setAlignment(Qt::AlignCenter);
    m_lblQRHint->setStyleSheet("color: #666666; font-size: 14px;");

    // 添加到面板
    layout->addWidget(m_lblQRCode, 0, Qt::AlignCenter);
    layout->addWidget(m_lblQRHint);

    return panel;
}

void Login::onLoginModeChanged(int index)
{
    // 切换登录面板
    m_stackedLogin->setCurrentIndex(index);
}

void Login::onGetVerifyCode()
{
    // 验证手机号
    QString phone = m_lePhone->text().trimmed();
    if (phone.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入手机号");
        return;
    }
    int cursorPos = 0; // 定义一个可修改的变量
    if (m_lePhone->validator()->validate(phone, cursorPos) != QValidator::Acceptable) {
        QMessageBox::warning(this, "提示", "请输入正确的手机号");
        return;
    }

    // 开始倒计时
    m_btnGetCode->setEnabled(false);
    m_btnGetCode->setText(QString("重新获取(%1s)").arg(m_remainingTime));
    m_countdownTimer->start(1000);  // 每秒触发一次

    // 实际项目中这里会调用接口发送验证码
    QMessageBox::information(this, "提示", "验证码已发送至手机号：" + phone);
}

void Login::updateCountdown()
{
    m_remainingTime--;
    m_btnGetCode->setText(QString("重新获取(%1s)").arg(m_remainingTime));

    if (m_remainingTime <= 0) {
        m_countdownTimer->stop();
        m_btnGetCode->setEnabled(true);
        m_btnGetCode->setText("获取验证码");
        m_remainingTime = 60;  // 重置倒计时
    }
}

void Login::onLoginClicked()
{
    // 根据当前登录方式验证
    if (m_stackedLogin->currentIndex() == 0) {
        // 手机验证码登录验证
        QString phone = m_lePhone->text().trimmed();
        QString code = m_leVerifyCode->text().trimmed();
        if (phone.isEmpty() || code.isEmpty()) {
            QMessageBox::warning(this, "提示", "请完善登录信息");
            return;
        }
        // 实际项目中添加接口验证逻辑
        QMessageBox::information(this, "提示", "手机验证码登录成功");
    } else {
        // 账户登录验证
        QString account = m_leAccount->text().trimmed();
        QString pwd = m_lePassword->text().trimmed();
        if (account.isEmpty() || pwd.isEmpty()) {
            QMessageBox::warning(this, "提示", "请输入账户和密码");
            return;
        }
        // 实际项目中添加接口验证逻辑
        QMessageBox::information(this, "提示", "账户登录成功");
    }
}

void Login::onAuxButtonClicked()
{
    QPushButton* senderBtn = qobject_cast<QPushButton*>(sender());
    if (senderBtn == m_btnRegister) {
        QMessageBox::information(this, "提示", "跳转到注册页面");
    } else if (senderBtn == m_btnForgetPwd) {
        QMessageBox::information(this, "提示", "跳转到找回密码页面");
    }
}
