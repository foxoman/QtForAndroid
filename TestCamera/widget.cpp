#include "widget.h"
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include <QtAndroid>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QAndroidActivityResultReceiver>
#include <QDateTime>
#include <QFile>
using namespace QtAndroid;

#define CHECK_EXCEPTION() \
    if(env->ExceptionCheck())\
    {\
        qDebug() << "exception occured";\
        env->ExceptionClear();\
    }

//   ǰ���ҽ����������յ� IMAGE_CAPTURE ʱ���� startActivity() ����������һ�� resultReceiver �������� ResultReceiver �� ResultReceiver �� QAndroidActivityResultReceiver �̳ж�����ʵ���� handleActivityResult ������
class ResultReceiver: public QAndroidActivityResultReceiver
{
public:
    ResultReceiver(QString imagePath, QLabel *view)
        : m_imagePath(imagePath), m_imageView(view)
    {

    }

    void handleActivityResult(
            int receiverRequestCode,
            int resultCode,
            const QAndroidJniObject & data)
    {
        qDebug() << "handleActivityResult, requestCode - " << receiverRequestCode
                    << " resultCode - " << resultCode
                    << " data - " << data.toString();
        //RESULT_OK == -1
        if(resultCode == -1 && receiverRequestCode == 1)
        {
            qDebug() << "captured image to - " << m_imagePath;
            qDebug() << "captured image exist - " << QFile::exists(m_imagePath);
            m_imageView->setPixmap(QPixmap(m_imagePath));
        }
    }

    QString m_imagePath;
    QLabel *m_imageView;
};

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QHBoxLayout *actInfoLayout = new QHBoxLayout();
    layout->addLayout(actInfoLayout);
    QPushButton *getBtn = new QPushButton("Get Info");
    connect(getBtn, SIGNAL(clicked()), this, SLOT(onGetActivityInfo()));
    actInfoLayout->addWidget(getBtn);
    m_activityInfo = new QLabel();
    m_activityInfo->setWordWrap(true);
    actInfoLayout->addWidget(m_activityInfo, 1);
    layout->addSpacing(2);

    QHBoxLayout *actionLayout = new QHBoxLayout();
    layout->addLayout(actionLayout);
    m_actionEdit = new QLineEdit("android.settings.SETTINGS");
    actionLayout->addWidget(m_actionEdit, 1);
    QPushButton *launchBtn = new QPushButton("Launch");
    connect(launchBtn, SIGNAL(clicked()), this, SLOT(onLaunch()));
    actionLayout->addWidget(launchBtn);
    layout->addSpacing(2);

    QPushButton *capture = new QPushButton("CaptureImage");
    connect(capture, SIGNAL(clicked()), this, SLOT(onCapture()));
    layout->addWidget(capture);
    m_capturedImage = new QLabel();
    m_capturedImage->setScaledContents(true);
    layout->addWidget(m_capturedImage, 1);
}

Widget::~Widget()
{

}

void Widget::onLaunch() //�����--onLaunch() ��һ���ۣ���� "Launch" ��ť�ᱻ���ã��ź���۵��������� Widget �Ĺ��캯����ɵġ�
{
    QAndroidJniObject action = QAndroidJniObject::fromString(m_actionEdit->text()); //�ӽ����ϵı༭�����ȡ action ��Ӧ���ַ�������Ӧ Android ϵͳ�����ý���
    QAndroidJniObject intent("android/content/Intent","(Ljava/lang/String;)V", action.object<jstring>());   //�������ľ�̬����ֱ�ӹ���һ�� JNI ����
    startActivity(intent, 0);   ////��3�д���ͱȽϼ��ˣ����ǲ����Ľ��������ֻ�����˵�1����������intent����2��������0����3��������Ĭ��ֵ��NULL��������û���ݡ�
    QAndroidJniEnvironment env; // �� 4 ��5 ���д��룬ʵ�����Ǽ�� JNI �����Ƿ������쳣����������쳣����������쳣���������ͻῨ�������
    CHECK_EXCEPTION()       //����쳣��Ҫ QAndroidJniEnvironment �࣬����������ʵ��ʱ�����Զ���������ǰ�̵߳� JNI ������Ȼ�����ǾͿ��Ե��� JNIEnv �ķ��� ExceptionCheck() �� ExceptionClear() �����������쳣�ˡ���Ϊʾ���жദ�õ���ÿ�� JNI ���ö���Ҫ����쳣�����Ҷ�����һ����Ϊ CHECK_EXCEPTION �ĺ꣺
}

/*
����ʹ�õ�QAndroidJniObject�Ĺ��캯��--QAndroidJniObject(const char * className, const char * signature, ...);
��һ������Ϊ�������ڶ�������Ϊ����ǩ�������������Ժ�Ĳ�����Ӧ Java �����Ĳ�����
��1�������� Java �� Intent ���ȫ·�������� JNI �е��ַ�����ʾ��Intent ��ȫ·�������� android.content.Intent ����Ӧ���ַ�����ʾΪ��android/content/Intent������.���滻Ϊ��/����
��2������ָ��ʹ�� Intent ���캯���е� Intent(String action) ������汾�Ĺ��캯������һ���ַ�����ʽ�� action �����ķ���ǩ���ǡ�(Ljava/lang/String;)V����ע�⹹�캯��û�з���ֵ������ǩ������ VOID ����ʾ��
��3����������Ҫ���ݸ� Intent ���캯���Ĳ����� Java ����Ϊ String ������Ҫ���� 1 �д���õ��� QAndroidJniObject ����ת��Ϊʵ�ʵ� JNI ����QAndroidJniObject�� Qt ��ʵ�� JNI ����ķ�װ�����������ͨ������object()����ʵ�ֵġ�object()��ģ�巽����ģ������� JNI ���ͣ������� jstring �����Դ���Ϊ action.object<jstring>() ��
*/

/**
 * @brief Widget::onGetActivityInfo
 * QtAndroid���ֿռ���һ�� androidActivity() ���������Ի�ȡ�� Qt on Android App ʹ�õ� Android Activity �������� Activity ��Ӧ�� JNI �������ǾͿ���ͨ�� QAndroidJniObject �ṩ�ķ��������� Activity ��һЩ�����������ˣ� onGetActivityInfo() ����۾�����ô���ġ�
 */
void Widget::onGetActivityInfo()    //��ȡ���Ϣ
{
    QAndroidJniEnvironment env;

    QAndroidJniObject activity = androidActivity();
    //androidActivity() ��ȡ��һ�� QAndroidJniObject ���󣬴��� Android ������ Activity �� Android Activity ����һ������ getLocalClassName() �����Ի�ȡ Activity ��Ӧ�� Java ���������ʹ�� JNI �õ������Ĵ�������
    //ʹ�� QAndroidJniObject �� QAndroidJniObject	callObjectMethod(const char * methodName) const ���������������ģ�巽����ģ�������Ӧ Java �����ķ���ֵ���͡�Android Activity �� getLocalClassName() ���ص��� String ��JNI ����Ϊ jstring ������ C++ �����д�����������
    QAndroidJniObject className =
            activity.callObjectMethod<jstring>("getLocalClassName");
    CHECK_EXCEPTION()
    QString name = className.toString();
    // QAndroidJniObject �� toString() ����������������һ�� JNI ������ַ�����������������Ϊ String��jstring�� �� JNI ���󣬷��صľ����ַ��������ֵ����Ҳ��һ�ַ���ġ���jstringת��Ϊ QString �ķ�ʽ�����ڷ� String��jstring�����͵� JNI ����toString() ����ö�Ӧ Java ��� toString() ���������صľ�����ʲô�ַ������Ϳ� Java ����ôʵ�� toString() �ˣ����� Java �� File �࣬toString() �ͻ᷵���ļ�����Ҳ���ܴ�·����
    int index = name.lastIndexOf('.');
    if(index != -1)
    {
        name = name.mid(index + 1);
    }
// Ҫ����һ�� Java ��JNI�������ʵ����������Ҫ����ʵ����Qt 5.3 �Ժ����ǲ�����д QtActivity �Ϳ��Ի�ȡ�� Qt App ��Ӧ�� Android Activity ʵ��ඣ��Ϳ���ֱ�ӵ������ʵ���ķ�������ȡһЩ��Ϣ�������һ�ȡ Activity ��Ӧ�� taskId ����ʹ�� QAndroidJniObject �� callMethod() ���ģ�淽��
    jint taskId = activity.callMethod<jint>("getTaskId");
    CHECK_EXCEPTION()
            //Ϊ����ʾ QAndroidJniObject �ľ�̬���� T	 callStaticMethod(const char * className, const char * methodName) ����ר�ſ����� android.app.Activity �� API ������ getInstanceCount() ��������Ҫ�����Ǿ��� onGetActivityInfo() �����������ش���
    jlong instanceCount = QAndroidJniObject::callStaticMethod<jlong>(
                "android/app/Activity",
                "getInstanceCount"
                );
    CHECK_EXCEPTION()

    QString activityInfo = QString("%1,task-%2,inst-%3")
            .arg(name).arg(taskId).arg(instanceCount);

    m_activityInfo->setText(activityInfo);
    m_activityInfo->adjustSize();
}

void Widget::onCapture()    //���� Android ���չ���
{
    QAndroidJniEnvironment env;

    // Android ��ʹ�������׽ͼƬ���¶���Ҫ����ͼƬ����Ҫ���� android.media.action.IMAGE_CAPTURE ���ڵ��� IMAGE_CAPTURE ʱ�������Ը�������һ�� EXTRA_OUTPUT ��ָ��ͼƬ�Ĵ洢λ�á�IMAGE_CAPTURE �����������գ����Ҹ��ݲ���������� Activity �� result �����㷵�ؼ�������������ɺ������ս����ϵ�ȡ����ťʱ��result code ������Ϊ RESULT_FAILED ����������ɰ�ťʱ���ĵ���ͼƬ�ᱻ���浽ָ����λ�ò��� result code ������Ϊ RESULT_OK ��
    //���� onCapture() ����Ǹ��� IMAGE_CAPTURE ��������Ϊ��д JNI ���ô��롣
    //constuct Intent for IMAGE_CAPTURE
    QAndroidJniObject action = QAndroidJniObject::fromString(
                "android.media.action.IMAGE_CAPTURE");
    QAndroidJniObject intent("android/content/Intent",
                             "(Ljava/lang/String;)V",
                             action.object<jstring>());

    //���챣��·��
    //setup saved image location
    QString date = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QAndroidJniObject fileName = QAndroidJniObject::fromString(date + ".jpg");
    QAndroidJniObject savedDir = QAndroidJniObject::callStaticObjectMethod(
                "android/os/Environment",
                "getExternalStorageDirectory",  // �������ȸ��ݵ�ǰ��������ͼƬ�ļ�����Ȼ��ͨ�� QAndroidJniObject::callStaticObjectMethod() ���� Android android.os.Environment ��� getExternalStorageDirectory() ��������ȡ�ⲿ�洢��Ŀ¼�� getExternalStorageDirectory() û�в���������ֵΪ Java String ���������䷽��ǩ��Ϊ ��()Ljava/io/File;�� ��
                "()Ljava/io/File;"
                );
    CHECK_EXCEPTION()
    qDebug() << "savedDir - " << savedDir.toString();
    QAndroidJniObject savedImageFile(
                "java/io/File",
                "(Ljava/io/File;Ljava/lang/String;)V",
                savedDir.object<jobject>(),
                fileName.object<jstring>());
    CHECK_EXCEPTION()
    qDebug() << "savedImageFile - " << savedImageFile.toString();
    QAndroidJniObject savedImageUri =
            QAndroidJniObject::callStaticObjectMethod(
                "android/net/Uri",
                "fromFile",
                "(Ljava/io/File;)Landroid/net/Uri;",
                savedImageFile.object<jobject>());
    CHECK_EXCEPTION()

    //tell IMAGE_CAPTURE the output location
    QAndroidJniObject mediaStoreExtraOutput
            = QAndroidJniObject::getStaticObjectField(
                "android/provider/MediaStore",
                "EXTRA_OUTPUT",
                "Ljava/lang/String;");
    CHECK_EXCEPTION()
    qDebug() << "MediaStore.EXTRA_OUTPUT - " << mediaStoreExtraOutput.toString();
    intent.callObjectMethod(
                "putExtra",
                "(Ljava/lang/String;Landroid/os/Parcelable;)Landroid/content/Intent;",
                mediaStoreExtraOutput.object<jstring>(),
                savedImageUri.object<jobject>());

    //�Ҵ�����һ�� QAndroidActivityResultReceiver ���󣬴��ݸ� startActivity ��ͬʱҲָ�� requestCode Ϊ 1
    //launch activity for result
    ResultReceiver *resultReceiver =    //�����ؽ��
            new ResultReceiver(savedImageFile.toString(), m_capturedImage);
    startActivity(intent, 1, resultReceiver);
    CHECK_EXCEPTION()
}

//�� Android �ϣ�/data ��������Ϊ�ڲ��洢��Ӧ��һ��ᱻ��װ�� /data/data Ŀ¼�£����ⲿ�洢ʵ�����ַ�Ϊ�����֣����� SD ��������չ SD ���������� SD �����Ǵ� FLASH �л��ֳ���һ��洢����һ����ص� /mnt/sdcard Ŀ¼�¡���չ SD �������������ֻ��ϲ���� microSD ֮��Ĵ洢������Ӧ�ķ�������Ϊ���ںܶ� Android ϵͳ����� FLASH �ϻ���һ����Ϊ���� SD ʹ�ã����� Environment ��� getExternalStorageDirectory() ������ȡ���ģ�һ�������� SD ���������� microSD �����ⲿ�洢����Ӧ�ķ�����
//�� Java �Ŀ¼���ļ���ͨ�� java.io.File ������ʾ�����ǻ�ȡ�����ⲿ�洢Ŀ¼��Ҳ��һ�� File ���� File ����һ�����캯�� File(File dir, String fileName) �����Դ���һ�� File ���󣬴��� dir ������ָ����Ŀ¼�µ� fileName �ļ��� File ����� toString() �����᷵������������ļ���·����
/*
������� savedImageFile ����ʹ�� ��(Ljava/io/File;Ljava/lang/String;)V����Ϊ File �๹�캯����ǩ����Ȼ���� savedDir.object<jobject>() ����ⲿ�洢Ŀ¼��Ӧ�� JNI ���������ݸ����캯����
//������ͨ�� Intent ���� IMAGE_CAPTURE ʱ��Ҫָ���Ĵ洢λ�ã���ͨ�� android.net.Uri ���ʾ�ģ�������ֵ��� Uri �ľ�̬���� fromFile() ������һ�� Uri ����fromFile()ֻ��һ������Ϊ File ����Σ�����ֵ���� Uri ����˷���ǩ��Ϊ��(Ljava/io/File;)Landroid/net/Uri;����

Ҳ����ע�⵽��ʹ�� QAndroidJniObject �� callStaticObjectMethod() �������� Java �ഫ��ʱ������õ� xxx.object<jobject>() ���ֵ��á����ǵ�֮ǰ���ǽ� QAndroidJniObject תΪ jstring �Ĵ����� xxx.object<jstring> ����ת������ JNI File ����� QAndroidJniObject ����ʱΪ�β�ʹ�� xxx.object<jfile>() �أ�������Ϊ���� JNI ������ϵͳ�Java String ��Ϊ�������ͣ��������Ķ������ͣ�ͳһ��ʹ�� jobject ����ʾ������Ժ����ǲ����õ�ʲô���� QAndroidJniObject ����ֻҪ�Ǵ���һ���� String ���͵� Java ������ת��Ϊʵ�ʵ� JNI ���󣬶��� xxx.object<jobject>() ���ַ�ʽ�����ǵõ��� Uri �����ڴ��ݸ� Intent ʱ������ô�õġ�

Android Intent ���ṩ��һϵ�е� putExtra �������������ܹ�������Ϊ int �� float �� String ��int[] ��double ��char �� byte �ȵȵ����ݴ洢�� Intent ʵ���У����������÷��������÷������� getExtra ������ȡ���������һ�����ݵ����ݡ������ʹ�� Intent �������ݵ�һ���÷��� Intent ����һ�� putExtras���������Դ���һ���������ݶ��󡪡�Bundle�������Ҫ���ݵ����ݺܶ࣬�Ϳ��԰��������Ϊһ�� Bundle ��

    Ҫ���������֮�䴫�����ݣ���Ҫ�и�Լ������Ϊ���ݵ����ݿ��ܲ�֪һ��������Ҫ��ÿ������������֣������ key ��putExtra() �����ĵ�һ����������Ϊ�ַ���������ָ�����ݵ� key ������ Intent ��һ�����Ը������ key ������ȡ������
���� IMAGE_CAPTURE �������ܵ������� Uri ����Ӧ�� key ��Ϊ android.provider.MediaStore.EXTRA_OUTPUT ���� MediaStore ��ľ�̬��Ա��������Ϊ���ڴ�������ȵ��� QAndroidJniObject::getStaticObjectField() ������ȡ�� EXTRA_OUTPUT �������ֵò�����һ��ඡ�

    QAndroidJniObject �����ṩ�˵��� Java �����Ľӿڣ����ṩ�˷��ʺ��޸� Java ���ԵĽӿڡ� getStaticObjectField ��һϵ�е����ط��������Ƿ��� Java ��ľ�̬��Ա�����ģ��� getObjectField ��һϵ�е����ط��������Ƿ��� Java ���ʵ����Ա���������ֳ�Ա������ÿ��ʵ������һ�ݣ��ġ�

    �ڷ��� Java ���Ա����ʱ����Ҫ�������ֺͱ���ǩ���������� EXTRA_OUTPUT �Ĵ��룬���ݵ� fieldName Ϊ ��EXTRA_OUTPUT�� �����ݵ�ǩ��Ϊ ��Ljava/lang/String;�� ����ʾ EXTRA_OUTPUT ��һ������Ϊ String �ľ�̬��Ա��

    �õ��� EXTRA_OUTPUT ������ Intent �� putExtra �Ϳ��ܰ� savedImageUri ���� Intent ��������keyΪ EXTRA_OUTPUT�����ҵ��õ� putExtra ����ԭ��Ϊ��
Android �ṩ��һ������������Ч�ʵĽ��̼�ͨ�Ż��ƣ� Parcel ��ʹ�� Parcel ͨ��ʱ��������������Ҫ���л������ Android ����� Parcelable �ӿڣ����㴫�����ݡ����봫�ݵ����ݣ�ֻҪʵ�� Parcelable �ӿں��ض���ģ�巽�����ɡ������õ��� Uri ������ Parcelable �����ࡣҲ�����ѡ��������� putExtra() ������
//������봫�ݸ����ӵ����ݣ�����ͨ�� Intent ��Ҳ����˵�ˣ������õ� Activity ��Ҳ���Դ���һ�� Intent ��������һ�����㿴��Android Activity �� onActivityResult() ����ԭ�����£�
void onActivityResult(int requestCode, int resultCode, Intent data);


�������ǵ� handleActivityResult �����Ƕ�Ӧ�ģ����������������;��� Intent ��

������ô����ʱ��ͨ����Ҫд Java ������ʵ����ʹ�� Intent �������ݵ�����������ͨ������ķ����� Intent ���ݸ������Ƿ���

[cpp] view plain copy

void setResult(int resultCode, Intent data);
*/
