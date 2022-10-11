package com.example.demoso1;

import android.util.Log;

public class Test {
    public String flag = null;
    // 构造函数
    public Test(String arg) {
        flag = "Test(String arg)";
    }

    public Test(String arg, int arg2) {
        flag = "Test(String arg,int arg2)";
    }

    public static String publicStaticField = "i am a publicStaticField";
    public String publicField = "im a public Field";
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
