#include <jni.h>
#include <pthread.h>
#include <string>
#include <android/log.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define APPNAME "FridaDetectionTest"

#define TAG "cxaadd"

// 定义info信息

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)

// 定义debug信息

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

// 定义error信息

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

extern "C" JNIEXPORT jstring JNICALL

//int __fastcall Java_com_example_demoso1_MainActivity_stringFromJNI(_JNIEnv *a1)
//{
//    _JNIEnv *v1; // ST34_4
//    int v2; // ST2C_4
//    int v3; // ST28_4
//    int v4; // ST24_4
//    int v5; // ST20_4
//    char *v6; // ST08_4
//    int result; // r0
//    int v8; // [sp+4h] [bp-4Ch]
//    char v9; // [sp+38h] [bp-18h]
//    int v10; // [sp+44h] [bp-Ch]
//
//    v1 = a1;
//    v2 = _JNIEnv::FindClass(a1, "com/example/demoso1/Test");
//    v3 = _JNIEnv::GetStaticFieldID(v1, v2, "publicStaticField", "Ljava/lang/String;");
//    v4 = _JNIEnv::GetStaticObjectField(v1, v2, v3);
//    v5 = _JNIEnv::GetStringUTFChars(v1, v4, 0);
//    _android_log_print(4, "cxaadd", "now content is: %s", v5);
//    std::__ndk1::basic_string<char,std::__ndk1::char_traits<char>,std::__ndk1::allocator<char>>::basic_string<decltype(nullptr)>(
//            &v9,
//            "Hello from C++");
//    v6 = (char *)sub_9348(&v9);
//    v8 = _JNIEnv::NewStringUTF(v1, v6);
//    std::__ndk1::basic_string<char,std::__ndk1::char_traits<char>,std::__ndk1::allocator<char>>::~basic_string(&v9);
//    result = _stack_chk_guard;
//    if ( _stack_chk_guard == v10 )
//        result = v8;
//    return result;
//}
Java_com_example_demoso1_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {

    //Native层调用JNI的接口实现反射
    //来获取包com.example.demoso1里Test类的class对象
    jclass testClass = env->FindClass("com/example/demoso1/Test");
    //参数：java层的类，方法名或者属性名，签名
    jfieldID publicStaticField = env->GetStaticFieldID(testClass, "publicStaticField",
                                                       "Ljava/lang/String;");
    jstring publicStaticField_value = (jstring) env->GetStaticObjectField(testClass,
                                                                          publicStaticField);
    const char *value_ptr = env->GetStringUTFChars(publicStaticField_value, nullptr);
    LOGI("now content is: %s", value_ptr);

    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

void *detect_frida_loop(void *) {
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    inet_aton("0.0.0.0", &(sa.sin_addr));
    int sock;
    int i;
    int ret;
    char res[7];
    while(1){
        /*
         * 1:Frida Server Detection
         */
        //LOGI("entering frida server detect loop started");
        for(i=20000;i<30000;i++){
            sock = socket(AF_INET,SOCK_STREAM,0);
            sa.sin_port = htons(i);
            LOGI("entering frida server detect loop started,now i is %d",i);

            if (connect(sock , (struct sockaddr*)&sa , sizeof sa) != -1) {
                memset(res, 0 , 7);
                send(sock, "\x00", 1, NULL);
                send(sock, "AUTH\r\n", 6, NULL);
                usleep(500); // Give it some time to answer
                if ((ret = recv(sock, res, 6, MSG_DONTWAIT)) != -1) {
                    if (strcmp(res, "REJECT") == 0) {
                        LOGI("FOUND FRIDA SERVER: %s,FRIDA DETECTED [1] - frida server running on port %d!",APPNAME,i);
                    }else{
                        LOGI("not FOUND FRIDA SERVER");
                    }
                }
            }
            close(sock);
        }
    }
}


//extern "C" JNIEXPORT void JNICALL
//Java_com_example_demoso1_MainActivity_init(
//        JNIEnv* env,
//        jobject clazz) {
//
//    pthread_t t;
//    pthread_create(&t,NULL,detect_frida_loop,(void*)NULL);
//
//    LOGI("frida server detect loop started");
//}


extern "C" JNIEXPORT jstring JNICALL

//反编译之后的代码
//int __fastcall Java_com_example_demoso1_MainActivity_myfirsyjniJNI(_JNIEnv *a1, int a2, int a3)
//{
//    int v4; // [sp+8h] [bp-18h]
//    int v5; // [sp+Ch] [bp-14h]
//    _JNIEnv *v6; // [sp+14h] [bp-Ch]
//
//    v6 = a1;
//    v5 = a3;
//    v4 = _JNIEnv::GetStringUTFChars(a1, a3, 0);
//    _JNIEnv::GetStringUTFLength(v6, v5);
//    if ( v4 )
//        _android_log_print(4, "cxaadd", "now a is %s", v4);
//    _JNIEnv::ReleaseStringUTFChars(v6, v5, v4);
//    return _JNIEnv::NewStringUTF(v6, "Hello I'm from myfirstjni");
//}

Java_com_example_demoso1_MainActivity_myfirsyjniJNI(JNIEnv *env, jclass MainActivity,
                                                    jstring content) {
    const char *a = env->GetStringUTFChars(content, nullptr);
    int a_size = env->GetStringUTFLength(content);
    if (a != 0) {
        LOGI("now a is %s", a);

    }

    env->ReleaseStringUTFChars(content, a);
    jstring result = env->NewStringUTF("Hello I'm from myfirstjni");
    return result;
}



JNIEXPORT jstring JNICALL stringFromJNI2(
        JNIEnv *env,
        jclass MainActivity /* this */) {

    std::string hello = "Hello from C++ stringFromJNI2";
    return env->NewStringUTF(hello.c_str());
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved){
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);
    jclass clazz;

    //JNI允许我们提供一个函数映射表，注册给Java虚拟机，这样JVM就可以用函数映射表来调用相应的函数。这样就可以不必通过函数名来查找需要调用的函数了
    //Java与JNI通过JNINativeMethod的结构来建立联系，它被定义在jni.h中
//    第一个变量name，代表的是Java中的函数名
//    第二个变量signature，代表的是Java中的参数和返回值
//    第三个变量fnPtr，代表的是的指向C函数的函数指针
    JNINativeMethod methods[] = {
            {"stringFromJNI2", "()Ljava/lang/String;", (void *) stringFromJNI2},
    };
    //找到声明native方法的类
    clazz = env->FindClass("com/example/demoso1/MainActivity");
    //RegisterNatives将注册函数的Java类，以及注册函数的数组，以及个数注册在一起，这样就实现了绑定。
    env->RegisterNatives(clazz, methods, 1);
    return JNI_VERSION_1_6;
}