package com.example.demoso1;

import android.util.Log;

public class Test {
    public String flag = null;

    // 构造函数
    public Test() {
    }

    public Test(String arg) {
        this.flag = arg;
    }

    public Test(String arg, int arg2) {
        this.flag = "Test(String arg,int arg2)";
    }

    public static String publicStaticField = "i am a publicStaticField";
    public String publicField = "i am a publicField";
    public int intpublicField = -1234567;
    private static String privateStaticFiled = "i am a privateStaticField";
    private String privateFiled = "i am a privateStaticField";

    public static void publicStaticFunc() {
        Log.i("cxareflection", "I'm from publicStaticFunc");
    }

    public void publicFunc() {
        Log.i("cxareflection", "I'm from publicFunc");
    }

    private static void privateStaticFunc() {
        Log.i("cxareflection", "I'm from privateStaticFunc");
    }


    private void privateFunc() {
        Log.i("cxareflection", "I'm from privateFunc");
    }


}
