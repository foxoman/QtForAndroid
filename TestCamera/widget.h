#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

public slots:
    void onLaunch();
    void onGetActivityInfo();
    void onCapture();

private:
    QLineEdit *m_actionEdit;    //�������� action
    QLabel*m_activityInfo;  //����ʾ Activity ��������Ϣ��
    QLabel *m_capturedImage;    //������ʾ��׽������Ƭ
};

#endif // WIDGET_H
