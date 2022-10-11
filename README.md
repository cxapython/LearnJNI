# LearnJNI
自用JNI项目以及资料

JNI 对引用数据类型的操作

### 1.字符串

JNI 把字符串当引用类型来处理，不像 Java 的字符串能直接使用，于是 JNI 提供了 Java 字符串之间相互转换的函数。因为 Java 中的字符串是不可变的，所以 JNI 也不提供任何修改现有字符串内容的函数。

*JNI 创建字符串*

```c
jstring javaString;
javaString = (*env)->NewStringUTF(env,"Hello World");
```

对应 frida 代码

```javascript
function jni_demo(){
    Java.perform(function(){
        var JNIEnv = Java.vm.getEnv(); //在 Frida 中获取 JNIEnv 对象
        var javaString = JNIEnv.newStringUtf("Hello World")
    });
  
}
```

frida里面对应的api名称和jni的还是有差异的具体可以通过https://github.com/frida/frida-java-bridge/blob/main/lib/env.js查询

*Java 字符串转换成 C 字符串*

```java
const jbyte* str;
jboolean isCopy;
str = (*env)->GetStringUTFChars(env,javaString,&isCopy);
```



对应 frida 代码
`Java.vm.getEnv().getStringUtfChars(javaString, null).readCString()`

也可以这样

```
console.log(javaString)
var obj = Java.use("java.lang.Object") 
console.log(Java.cast(javaString,obj)) 
```



释放字符串通过 GetStringChars 和 GetStringUTFChars 函数获取的C字符串在原生代码中使用完之后需要正确释放。

 ```java
(*env)->ReleaseStringUTFChars(env,javaString,str);
 ```

frida一般不用释放。

### 2.数组

JNI 把 Java 数组当成引用类型来处理,JNI提供必要的函数访问和处理Java数组

JNI 创建数组的函数格式:`New<Type>Array`其中的Type可以是Int、Char和Boolean等。

数组的创建

```java
jintArray javaArray;

javaArray = (*env)->NewIntArray(env,10);
```

JNI提供两种访问Java数组元素的方法，可以将数组的代码复制成C数组或者让JNI直接指向数组元素的指针。



第一种：将给定的Java数组复制到C数组中(操作副本):

```java
jint nativeArray[10];
(*env)->GetIntArrayRegion(env,javaArray,0,10,nativeArray);
```

其中的GetIntArrayRegion的类型也可以修改为其他类型 ,其函数格式`Get<Type>ArrayRegion`。

C修改数组然后传给Java。

```java
(*env)->SetIntArrayRegion(env,javaArray,0,10,nativeArray);
```



第二种:直接对指针进行操作

所用函数格式` Get<Type>ArrayElements`。

下面代码是获取指向Java数组元素的直接指针。

```java
jint* nativeDirectArray;
jbooleanisCopy;
nativeDirectArray = (*env)->`GetIntArrayElements(env,javaArray,&isCopy);
```

用完应立即释放防止内存溢出

函数格式 `Release<Type>ArrayElements`

```java
(*env)->ReleaseIntArrayElements(env,javaArray,nativeDirectArray,0);
```

这个函数最后函数代表释放模式。其中0表示 将内容复制回来并释放原生数组，JNI_COMMIT 将内容复制回来但是不释放原生数组，一般用于周期地更新一个Java数组，JNI_ABORT表示释放原生数组不用将内容复制回来。

### 3.NIO 操作

创建直接字节缓存区

```
unsigned char* buffer = (unsigned char*) malloc(1024);
jobject directBuffer;
directBuffer = (*env)->NewDirectByteBuffer(env,buffer,1024);
```

直接字节缓存区获取

```
unsigned char* buffer;
buffer = (unsigned char*)(*env)->GetDirectBufferAddress(env,directBuffer);
```



### 4.访问域

Java有两类域：实例域和静态域。类的每个实例都有自己的实例域副本，而一个类所有实例共享一个静态域。

JNI提供了访问两类域的函数。

```
Public class JavaClass{
	/**实例域*/
	private String instanceFiled = "Instance Field";
	
	/**静态域*/
	private static String staicField = "Static Field";
}
```



获取域ID

JNI提供了用域ID访问两类域的方法，可以通过给定实例的class对象获取域ID，用GetObjectClass函数获取class对象

```
jclass clazz;
clazz = (*env)GetObjectClass(env, instance);

```

获取实例域

```
jfieldID instanceFieldId;
instanceFieldId = (*env)->GetFieldID(env,clazz,"instanceFieldId","Ljava/lang/String;")
```

获取静态域

```
jfieldID staitcFieldId;
staitcFieldId=(*env)->GetStaticFieldID(env,clazz,"staticField","Ljava/lang/String;")
```

最后一个参数表示域类型的域描述符。



获取域ID之后就可以使用`Get<Type>Field`函数获得实际的实例域。

```
jstring instanceFiled;
instanceFiled = (*env)->GetObjectField(env,instance,instanceFieldId)
```

获取静态域类似

```
jstring staitcField;
staitcField = (*env)->GetStaticObjectField(env,instance,staitcFieldId)
```



### 5.调用方法

Java有两类方法: 实例方法和静态方法，JNI提供访问这两类方法的函数。



```
public class JavaClass{
  
  /**实例方法*/
	private String instanceMethod(){
		return "Instance Method";
	}
	 /**静态方法*/
	private static String staticMethod(){
		return "Static Method";
	}
}
```



1.获取方法的ID

```
jmethodID instanceMethodId;
instanceMethod = (*env)->GetMethodID(env,clazz,"instanceMethod","()Ljava/lang/String")
```

获取静态方法的类似

```
jmethodID staticMethodId;
staticMethodId = (*env)->GetStaticMethodID(env,clazz,"staticMethodId","()Ljava/lang/String")
```

使用函数`Call<Type>Method`可以调用实际的方法。

调用实例方法

```
jstring instanceMethodResult;
instanceMethodResult =(*env)->CallStringMethod(env,instance,instanceMethodId)
```



使用函数`CallStatic<Type>Method`可以调用静态的方法。

```
jstring staticMethodResult;
staticMethodResult =(*env)->CallStaticStringMethod(env,instance,staticMethodId)
```



　在内存溢出时以上的函数均返回NULL，此时原生代码不会继续执行。



### 6.域和方法描述符

JNI 对引用数据类型的操作

### 1.字符串

JNI 把字符串当引用类型来处理，不像 Java 的字符串能直接使用，于是 JNI 提供了 Java 字符串之间相互转换的函数。因为 Java 中的字符串是不可变的，所以 JNI 也不提供任何修改现有字符串内容的函数。

*JNI 创建字符串*

```c
jstring javaString;
javaString = (*env)->NewStringUTF(env,"Hello World");
```

对应 frida 代码

```
var JNIEnv = Java.vm.getEnv(); //在 Frida 中获取 JNIEnv 对象
var javaString = JNIEnv.newStringUtf("Hello World")
```



*Java 字符串转换成 C 字符串*

```java
const jbyte* str;
jboolean isCopy;
str = (*env)->GetStringUTFChars(env,javaString,&isCopy);
```



对应 frida 代码
`Java.vm.getEnv().getStringUtfChars(args[2], null).readCString()`



释放字符串通过 GetStringChars 和 GetStringUTFChars 函数获取的C字符串在原生代码中使用完之后需要正确释放。

 ```java
(*env)->ReleaseStringUTFChars(env,javaString,str);
 ```



### 2.数组

JNI 把 Java 数组当成引用类型来处理,JNI提供必要的函数访问和处理Java数组

JNI 创建数组的函数格式:`New<Type>Array`其中的Type可以是Int、Char和Boolean等。

数组的创建

```java
jintArray javaArray;

javaArray = (*env)->NewIntArray(env,10);
```

JNI提供两种访问Java数组元素的方法，可以将数组的代码复制成C数组或者让JNI直接指向数组元素的指针。



第一种：将给定的Java数组复制到C数组中(操作副本):

```java
jint nativeArray[10];
(*env)->GetIntArrayRegion(env,javaArray,0,10,nativeArray);
```

其中的GetIntArrayRegion的类型也可以修改为其他类型 ,其函数格式`Get<Type>ArrayRegion`。

C修改数组然后传给Java。

```java
(*env)->SetIntArrayRegion(env,javaArray,0,10,nativeArray);
```



第二种:直接对指针进行操作

所用函数格式` Get<Type>ArrayElements`。

下面代码是获取指向Java数组元素的直接指针。

```java
jint* nativeDirectArray;
jbooleanisCopy;
nativeDirectArray = (*env)->`GetIntArrayElements(env,javaArray,&isCopy);
```

用完应立即释放防止内存溢出

函数格式 `Release<Type>ArrayElements`

```java
(*env)->ReleaseIntArrayElements(env,javaArray,nativeDirectArray,0);
```

这个函数最后函数代表释放模式。其中0表示 将内容复制回来并释放原生数组，JNI_COMMIT 将内容复制回来但是不释放原生数组，一般用于周期地更新一个Java数组，JNI_ABORT表示释放原生数组不用将内容复制回来。

### 3.NIO 操作

创建直接字节缓存区

```
unsigned char* buffer = (unsigned char*) malloc(1024);
jobject directBuffer;
directBuffer = (*env)->NewDirectByteBuffer(env,buffer,1024);
```

直接字节缓存区获取

```
unsigned char* buffer;
buffer = (unsigned char*)(*env)->GetDirectBufferAddress(env,directBuffer);
```



### 4.访问域

Java有两类域：实例域和静态域。类的每个实例都有自己的实例域副本，而一个类所有实例共享一个静态域。

JNI提供了访问两类域的函数。

```
Public class JavaClass{
	/**实例域*/
	private String instanceFiled = "Instance Field";
	
	/**静态域*/
	private static String staicField = "Static Field";
}
```



获取域ID

JNI提供了用域ID访问两类域的方法，可以通过给定实例的class对象获取域ID，用GetObjectClass函数获取class对象

```
jclass clazz;
clazz = (*env)GetObjectClass(env, instance);

```

获取实例域

```
jfieldID instanceFieldId;
instanceFieldId = (*env)->GetFieldID(env,clazz,"instanceFieldId","Ljava/lang/String;")
```

获取静态域

```
jfieldID staitcFieldId;
staitcFieldId=(*env)->GetStaticFieldID(env,clazz,"staticField","Ljava/lang/String;")
```

最后一个参数表示域类型的域描述符。



获取域ID之后就可以使用`Get<Type>Field`函数获得实际的实例域。

```
jstring instanceFiled;
instanceFiled = (*env)->GetObjectField(env,instance,instanceFieldId)
```

获取静态域类似

```
jstring staitcField;
staitcField = (*env)->GetStaticObjectField(env,instance,staitcFieldId)
```



### 5.调用方法

Java有两类方法: 实例方法和静态方法，JNI提供访问这两类方法的函数。



```
public class JavaClass{
  
  /**实例方法*/
	private String instanceMethod(){
		return "Instance Method";
	}
	 /**静态方法*/
	private static String staticMethod(){
		return "Static Method";
	}
}
```



1.获取方法的ID

```
jmethodID instanceMethodId;
instanceMethod = (*env)->GetMethodID(env,clazz,"instanceMethod","()Ljava/lang/String")
```

获取静态方法的类似

```
jmethodID staticMethodId;
staticMethodId = (*env)->GetStaticMethodID(env,clazz,"staticMethodId","()Ljava/lang/String")
```

使用函数`Call<Type>Method`可以调用实际的方法。

调用实例方法

```
jstring instanceMethodResult;
instanceMethodResult =(*env)->CallStringMethod(env,instance,instanceMethodId)
```



使用函数`CallStatic<Type>Method`可以调用静态的方法。

```
jstring staticMethodResult;
staticMethodResult =(*env)->CallStaticStringMethod(env,instance,staticMethodId)
```



　在内存溢出时以上的函数均返回NULL，此时原生代码不会继续执行。



### 6.域和方法描述符

java类型签名映射

| Java 类型             | 签名                    |
| --------------------- | ----------------------- |
| Boolean               | Z                       |
| Byte                  | B                       |
| Char                  | C                       |
| Short                 | S                       |
| Int                   | I                       |
| Long                  | J                       |
| Float                 | F                       |
| Double                | D                       |
| Fully-qualified-class | Lfully-qualified-class; |
| type[]                | [type                   |
| Method type           | (arg-type)ret-type      |

### 7.异常处理

JNI要求开发人员在异常发生后显示的实现异常处理流

JNI提供 ExceptionOccured 函数查询虚拟机中是否有挂起的异常。在使用之后，异常处理程序用 ExceptionClear 函数显式地清楚异常。

```
jthrowable ex;
(*env)->CallVoidMethod(env,instance,throwingMethodId);
ex = (*env)->ExceptionOccured(env);
if(0!=ex){
	(*env)->ExceptionClear(env);
  /* Exception handler*/
}
```

抛出异常

JNI的异常时 java 类，所以应该用 FindClass 函数找到异常类，用ThrowNew函数可以初始化且抛出新的异常。

```
jclass clazz;
clazz = (*env)->FindClass(env,"java/lang/NullPointerException");
if(0!=clazz){
	(*env)->ThrowNew(env,clazz,"Exception Message");
}

```

### 8.引用

*局部引用*

大多数JNI函数返回局部引用。他们的使用期限仅限于原生方法，一旦原生函数返回，局部引用即被释放。

例如

```
jclass clazz;
clazz =(*env)->FindClass(env,"java/lang/String");
....
(*env)->DeleteLocalRef(env,clazz);
```

 根据JNI的规范，虚拟机应该允许原生代码创建最少16个局部引用。在单个方法调用时进行多个内存密集型操作的最佳实践方案是删除未用的局部引用。如果不可能，原生代码可以在使用之前用EnsureLocalCapacity 方法请求更多的局部引用槽。



*全局引用*

可以用NewGlobalRef函数将局部引用初始化为全局引用。

用给定的局部引用创建全局引用。

```
jclass localClazz;
jclass globalClazz;
...
localClazz = (*env)->FindClass(env,"java/lang/String");
globalClazz = (*env)->NewGlobalRef(env,localClazz);
...

```

原生代码不需要一个全局引用时，就可以使用DeleteGlobalRef 函数释放它。



*删除全局引用*

```
(*env)->DeleteGlobalRef(env,localClazz);
```

调用NewGlobalRef基于局部引用创建，会阻GC回收所引用的对象。可以跨方法、跨线程使用。JVM不会自动释放，必须调用DeleteGlobalRef手动释放。

*弱全局引用*

调用NewWeakGlobalRef基于局部引用或全局引用创建，不会阻止GC回收所引用的对象，可以跨方法、跨线程使用。引用不会自动释放，在JVM认为应该回收它的时候（比如内存紧张的时候）进行回收而被释放。或调用DeleteWeakGlobalRef手动释放。

### 9.线程

作为多线程环境的一部分，虚拟机支持运行的原生代码。在开发原生构件时要记住JNI技术的一些约束。

- 只在原生方法执行期间以及正在执行原生方法的线程环境下局部引用是有效的，局部引用不能在多线程之间共享，只有全局引用可以被多个线程共享，
- 被传递给每个原生方法的JNIEnv接口指针在与方法调用相关的线程中也是有效的，它不能被其他线程缓存或使用。



同步

JNI的监视器允许原生代码利用Java对象同步，虚拟机保证存取监视器的线程能安全执行，而其他线程等待监视器对象变成可用状态。Java应用程序的同步代码如下

```
synchronized(obj){
	/*同步线程安全代码块*/
}
```

在原生代码中，相同级别同步可以用JNI的监视器方法实现的。

```
if(JNI_OK==(*env)->MonitorEnter(env,obj)){
	/*错误处理*/
}

/*同步线程安全代码块*/
if(JNI_OK==(*env)->MonitorExit(env,obj)){
	/*错误处理*/
}
```

为了防止死锁，MonitorEnter函数的调用应该与MonitorExit的调用匹配。

### 10.原生线程

JNI通过JavaVM接口指针提供了AttachCurrentThread函数以便于原生代码讲原生线程附着到虚拟机上。JavaVM接口指针应该尽早被缓存，否则的话它不能被获取。

```
JavaVM* cachedJVM;
...
JNIEnv* env;
...
/*将当前线程附着到虚拟机*/
(*cachedJVM)->AttachCurrentThread(cachedJVM,&env,NULL);
/*可以用 JNIEnv 接口实现线程与 Java 应用程序的通信*/
/*将当前线程与虚拟机分离*/
(*cachedJVM)->DeleteCurrentThread(cachedJVM);

```

















