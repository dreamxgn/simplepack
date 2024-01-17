package com.example.pack;

import android.app.Application;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.res.AssetManager;
import android.util.Log;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.lang.reflect.Array;
import java.lang.reflect.Field;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

import dalvik.system.DexClassLoader;
import dalvik.system.InMemoryDexClassLoader;
import dalvik.system.PathClassLoader;

public class PackApp extends Application {

    public static final String TAG="ithuiyilu";

    static {
        try{
            System.loadLibrary("pack");
        }catch (Exception ex){
            ex.printStackTrace();
        }
    }

    @Override
    protected void attachBaseContext(Context base) {

        super.attachBaseContext(base);

        try {
            loadApp(getClassLoader(),base);
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    public native void loadApp(ClassLoader clsLoader,Context base);

}
