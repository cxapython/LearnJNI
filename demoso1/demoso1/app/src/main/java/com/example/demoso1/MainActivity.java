package com.example.demoso1;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Base64;
import android.util.Log;
import android.widget.TextView;

import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "CXADemoso1";

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());
//        Log.i("cxaadd", MainActivity.myfirsyjniJNI("from java"));
//        try {
//            testField();
//        } catch (ClassNotFoundException e) {
//            e.printStackTrace();
//        } catch (NoSuchFieldException e) {
//            e.printStackTrace();
//        } catch (IllegalAccessException e) {
//            e.printStackTrace();
//        }
//        while (true) {
//            try {
//                Thread.sleep(1000);
//
//            } catch (InterruptedException e) {
//                e.printStackTrace();
//            }
//            Log.i("cxaadd",MainActivity.myfirsyjniJNI("fromJava"));
//            Log.i("cxaadd",MainActivity.stringFromJNI2());
//            //init();
//
//        }
        Log.i(TAG, MainActivity.myfirsyjniJNI("from Java"));
        Log.i(TAG, MainActivity.stringFromJNI2());
        Log.i(TAG, "start jarray");
        int[] numbers = {1, 2, 3, 4, 5, 6};
        double[] result = this.myArray(numbers);
        Log.i(TAG,"The sum is: " + result[0]);
        Log.i(TAG,"The average is: " + result[1]);
        Log.i(TAG, "end jarray");



    }

    public static byte[] b(byte[] bArr) {
        return Base64.encode(bArr, 2);
    }

    public static void testField() throws ClassNotFoundException, NoSuchFieldException, IllegalAccessException {
        /**
         *
         *???????????????
         *3?????????Class??????
         */

        Class testClazz = null;
        testClazz = MainActivity.class.getClassLoader().loadClass("com.example.demoso1.Test");
        Log.i("cxareflection", "Classloader.loadClass->" + testClazz);

        Class testClazz2 = null;
        testClazz2 = Class.forName("com.example.demoso1.Test");
        Log.i("cxareflection", "Class.forName->" + testClazz2);

        Class testClazz3 = Test.class;
        Log.i("cxareflection", ".class->" + testClazz3);

        //??????
        Field publicStaticField_field = testClazz3.getDeclaredField("publicStaticField"); //???????????????????????????
        Log.i("cxareflection", "getDeclaredField->" + publicStaticField_field);
        String value = (String) publicStaticField_field.get(null); // ??????get??????????????????
        Log.i("cxareflection", "publicStaticField_fiield.get->" + value);

//
        //????????????????????????
        try {
            Field privateField_field = testClazz3.getDeclaredField("privateStaticFiled");
            privateField_field.setAccessible(true); ///?????????true???????????????????????????
            privateField_field.set(null, "modified"); //?????????modified
            Log.i("cxareflection", "getDeclaredField privateField_field->" + privateField_field);

            String value2 = (String) privateField_field.get(null);
            Log.i("cxareflection", "privateField_field.get->" + value2);
        } catch (Exception e) {
            // java.lang.IllegalAccessException: Class java.lang.Class<com.example.demoso1.MainActivity>
            // cannot access private static  field java.lang.String com.example.demoso1.Test.privateStaticFiled of
            // class java.lang.Class<com.example.demoso1.Test>
            e.printStackTrace();

        }


        Field[] fields = testClazz2.getDeclaredFields();// ????????????????????????
        for (Field i : fields) {
            Log.i("cxareflection", "testClazz2.getDeclaredField->" + i);
        }


        Field[] fields2 = testClazz3.getFields();
        for (Field i : fields2) {
            Log.i("cxareflection", "testClazz3.getDeclaredField->" + i);
        }

    }

    public void testMethod() throws NoSuchMethodException, InvocationTargetException, IllegalAccessException {
        // ??????????????????java????????? ??????????????????????????????
        Class testClazz = Test.class;

        Method publicStaticFunc_method = testClazz.getDeclaredMethod("publicStaticFunc");

        Log.i("cxareflction", "testClazz.getDeclaredMethod()->" + publicStaticFunc_method);
        Method privateStaticFunc_method = testClazz.getDeclaredMethod("privateStaticFunc");
        privateStaticFunc_method.setAccessible(true);
        privateStaticFunc_method.invoke(null); //????????????
        Log.i("cxareflction", "testClazz.getDeclaredMethod()->" + privateStaticFunc_method);

        Method[] methods = testClazz.getMethods();
        for (Method i : methods) {
            Log.i("cxareflction", "testClazz.getMethods()->" + i);
        }

        Method[] methods2 = testClazz.getDeclaredMethods();
        for (Method i : methods2) {
            Log.i("cxareflction", "testClazz.getDeclaredMethods->" + i);
        }

    }


    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();//ctrl???????????????

    //????????????
    public static native String stringFromJNI2();

    public static native String myfirsyjniJNI(String context);

    public  native double[] myArray(int[] numbers);
//????????????frida???ptrace??????
//    public native int init();

}
