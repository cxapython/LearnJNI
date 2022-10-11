#include <jni.h>
#include <pthread.h>
#include <string>
#include <android/log.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define APPNAME "FridaDetectionTest"

#define TAG "CXADemoso1"

// 定义info信息

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)

// 定义debug信息

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

// 定义error信息

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

extern "C" JNIEXPORT jstring JNICALL


Java_com_example_demoso1_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {

    //Native层调用JNI的接口实现反射
    //来获取包com.example.demoso1里Test类的class对象
    jclass testClass = env->FindClass("com/example/demoso1/Test");
    //========调用属性===============
    //1.调用静态属性
    //获取Test.java文件的Test类的静态域publicStaticField
    // 一般先获取id，再进行调用
    // 参数：java层的类，方法名或者属性名，签名
    jfieldID publicStaticField = env->GetStaticFieldID(testClass, "publicStaticField",
                                                       "Ljava/lang/String;");
    jstring publicStaticField_value = (jstring) env->GetStaticObjectField(testClass,
                                                                          publicStaticField);

    const char *value_ptr = env->GetStringUTFChars(publicStaticField_value, nullptr);


    LOGI("publicStaticField_value is: %s", value_ptr);


    jfieldID privateStaticField = env->GetStaticFieldID(testClass, "privateStaticFiled",
                                                        "Ljava/lang/String;");
    jstring privateStaticField_value = (jstring) env->GetStaticObjectField(testClass,
                                                                           privateStaticField);

    const char *value_ptr11 = env->GetStringUTFChars(privateStaticField_value, nullptr);


    LOGI("privateStaticField_value is: %s", value_ptr11);
    //2.调用非静态属性
    //获取非静态的就比较麻烦了，需要用到反射
    //获取构造函数方法名为<init>,()V不参数的构造函数,如果还定义了其他构造函数这个时候默认的构造函数会被覆盖，下面代码会出错，需要自己额外补充一个空的构造函数。
    jmethodID testContruct = env->GetMethodID(testClass, "<init>", "()V");
    //根据构造函数实例化对象
    jobject testObject = env->NewObject(testClass, testContruct);
    jfieldID publicField = env->GetFieldID(testClass, "publicField",
                                           "Ljava/lang/String;");

    jstring publicField_value = (jstring) env->GetObjectField(testObject, publicField);

    const char *value_ptr2 = env->GetStringUTFChars(publicField_value, nullptr);


    LOGI("publicField_value is: %s", value_ptr2);
    //调用返回值为其他类型的属性
    jfieldID intPublicField = env->GetFieldID(testClass, "intpublicField", "I");
    jint intPublicField_value = env->GetIntField(testObject, intPublicField);
    LOGI("intpublicField_value is: %d", intPublicField_value);
    //3.调用私有属性
    jfieldID privateFiled = env->GetFieldID(testClass, "privateFiled", "Ljava/lang/String;");
    jstring privateFiled_value = (jstring) env->GetObjectField(testObject, privateFiled);
    const char *value_ptr3 = env->GetStringUTFChars(privateFiled_value, nullptr);
    LOGI("privateFiled_value is: %s", value_ptr3);

    env->ReleaseStringUTFChars(publicStaticField_value, value_ptr);
    env->ReleaseStringUTFChars(publicField_value, value_ptr2);
    env->ReleaseStringUTFChars(privateFiled_value, value_ptr3);

    //=======方法调用==================
    //1.调用静态方法
    //公共方法
    jmethodID publicStaticFunc = env->GetStaticMethodID(testClass, "publicStaticFunc", "()V");
    env->CallStaticVoidMethod(testClass, publicStaticFunc);
    //私有方法
    jmethodID privateStaticFunc = env->GetStaticMethodID(testClass, "privateStaticFunc", "()V");
    env->CallStaticVoidMethod(testClass, privateStaticFunc);


    //2.普通方法
    jmethodID publicFunc = env->GetMethodID(testClass, "publicFunc", "()V");
    env->CallVoidMethod(testObject, publicFunc);

    jmethodID privateFunc = env->GetMethodID(testClass, "privateFunc", "()V");
    env->CallVoidMethod(testObject, privateFunc);

    //调用赋值之前的flag
    jfieldID flag = env->GetFieldID(testClass, "flag", "Ljava/lang/String;");
    jstring before_flag_value = (jstring) env->GetObjectField(testObject, flag);
    LOGI("before_flag_value is: %s", before_flag_value);

    //调用其他构造函数
    jmethodID testContruct1 = env->GetMethodID(testClass, "<init>", "(Ljava/lang/String;)V");
    std::string flag_value = "set flag value is s";
    env->CallVoidMethod(testObject, testContruct1, env->NewStringUTF(flag_value.c_str()));

    //调用赋值之后的flag
    std::string value_ = "再次调用flag";
    LOGI("after_flag_value is: %s", value_.c_str());
    jstring after_flag_value = (jstring) env->GetObjectField(testObject, flag);
    const char *value_ptr4 = env->GetStringUTFChars(after_flag_value, nullptr);
    LOGI("after_flag_value is: %s", value_ptr4);
    //手动赋值
    jstring buffer = env->NewStringUTF("set new flag");
    (env)->SetObjectField(testObject, flag, buffer);

    jstring set_after_flag_value = (jstring) env->GetObjectField(testObject, flag);
    const char *value_ptr5 = env->GetStringUTFChars(set_after_flag_value, nullptr);
    LOGI("set_after_flag_value is: %s", value_ptr5);

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
    while (1) {
        /*
         * 1:Frida Server Detection
         */
        //LOGI("entering frida server detect loop started");
        for (i = 20000; i < 30000; i++) {
            sock = socket(AF_INET, SOCK_STREAM, 0);
            sa.sin_port = htons(i);
            LOGI("entering frida server detect loop started,now i is %d", i);

            if (connect(sock, (struct sockaddr *) &sa, sizeof sa) != -1) {
                memset(res, 0, 7);
                send(sock, "\x00", 1, NULL);
                send(sock, "AUTH\r\n", 6, NULL);
                usleep(500); // Give it some time to answer
                if ((ret = recv(sock, res, 6, MSG_DONTWAIT)) != -1) {
                    if (strcmp(res, "REJECT") == 0) {
                        LOGI("FOUND FRIDA SERVER: %s,FRIDA DETECTED [1] - frida server running on port %d!",
                             APPNAME, i);
                    } else {
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


extern "C"
JNIEXPORT jstring JNICALL
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

extern "C" JNIEXPORT jdoubleArray JNICALL Java_com_example_demoso1_MainActivity_myArray(JNIEnv *env, jobject obj, jintArray jnumbers) {

    //获取指向Java数组元素的直接指针
    jint *numbers = env->GetIntArrayElements(jnumbers, nullptr);
    if (numbers == nullptr) {
        return nullptr;
    }
    jsize length = env->GetArrayLength(jnumbers); //获取数组长度
    LOGI("jarray length is: %d", length);
    jint sum = 0;
    for (int i = 0; i < length; i++) {
        sum += numbers[i];
    }
    jdouble average = (jdouble) sum / length;
    //用完应立即释放防止内存溢出和GetIntArrayElements成对出现
    env->ReleaseIntArrayElements(jnumbers, numbers, 0);
    jdouble results[] = {(jdouble) sum, average};

    //新的数组创建
    jdoubleArray resultArray = env->NewDoubleArray(2);
    if (resultArray == nullptr) {
        return nullptr;
    }
    //C修改数组然后传给Java。
    env->SetDoubleArrayRegion(resultArray, 0, 2, results);
    return resultArray;
}


JNIEXPORT jstring JNICALL stringFromJNI2(
        JNIEnv *env,
        jclass MainActivity /* this */) {

    std::string hello = "Hello from C++ stringFromJNI2";
    return env->NewStringUTF(hello.c_str());
}

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
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