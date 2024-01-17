#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include <string.h>

/***
 * 获取dex文件并解密返回dex文件数据
 * @param env
 * @param thiz
 * @return
 */
jbyteArray getDex(JNIEnv *env,jobject thiz);

/***
 * 获取lib文件搜索目录
 * @param env
 * @param thiz
 * @return
 */
jstring getSearchDir(JNIEnv *env,jobject thiz);

/****
 * 将dex文件数据封装成ByteBuffer数组
 * @param env
 * @param thiz
 * @param dex
 * @return
 */
jobjectArray getDexBuffers(JNIEnv *env,jobject thiz,jbyteArray dex);

/***
 * 创建DexClassLoader对象
 * @param env
 * @param thiz
 * @param dexBuffers dex文件数据
 * @param searchDir so库搜索目录
 * @param cls_loader 当前类加载器
 * @return
 */
jobject newDexClassLoader(JNIEnv *env,jobject thiz,jobjectArray dexBuffers,jstring searchDir,jobject cls_loader);

/****
 * 替换当前类加载器的dex文件
 * @param env
 * @param thiz
 * @param dex
 * @param classLoader
 * @param base
 */
void entryApp(JNIEnv *env,jobject thiz,jobject dex,jobject classLoader,jobject base);


extern "C"
JNIEXPORT void JNICALL
Java_com_example_pack_PackApp_loadApp(JNIEnv *env, jobject thiz, jobject cls_loader,jobject base) {

    jbyteArray dex=getDex(env,thiz);
    jstring searchDir= getSearchDir(env,thiz);
    jobjectArray dexBuffers= getDexBuffers(env,thiz,dex);
    jobject objDexClassLoader= newDexClassLoader(env,thiz,dexBuffers,searchDir,cls_loader);

    entryApp(env,thiz,objDexClassLoader,cls_loader,base);
}

jbyteArray getDex(JNIEnv *env,jobject thiz){
    jclass clsContextWrapper= env->FindClass("android/content/ContextWrapper");
    if(clsContextWrapper==NULL || env->ExceptionCheck()){
        env->ExceptionDescribe();

        __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                            "FindClass faild android/content/ContextWrapper");

        return nullptr;
    }
    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "FindClass android/content/ContextWrapper %p",clsContextWrapper);


    jmethodID  mthgetAssets=env->GetMethodID(clsContextWrapper,"getAssets",
                                             "()Landroid/content/res/AssetManager;");

    if(mthgetAssets==NULL || env->ExceptionCheck()){
        env->ExceptionDescribe();
        return nullptr;
    }
    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "mthgetAssets %p",mthgetAssets);



    jobject objAssets= env->CallObjectMethod(thiz,mthgetAssets);
    if(objAssets==NULL || env->ExceptionCheck()){
        env->ExceptionDescribe();
        return nullptr;
    }
    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "objAssets %p",objAssets);

    jclass clsAssetManager= env->FindClass("android/content/res/AssetManager");
    if(clsAssetManager==NULL || env->ExceptionCheck()){
        env->ExceptionDescribe();
        return nullptr;
    }
    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "clsAssetManager %p",clsAssetManager);

    jmethodID  m_open=env->GetMethodID(clsAssetManager,"open",
                                       "(Ljava/lang/String;)Ljava/io/InputStream;");
    if(m_open==NULL || env->ExceptionCheck()){
        env->ExceptionDescribe();
        return nullptr;
    }
    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "m_open %p",objAssets);

    jobject  obj_inputstream= env->CallObjectMethod(objAssets,m_open,env->NewStringUTF("classes.dex"));
    if(obj_inputstream==NULL || env->ExceptionCheck()){
        env->ExceptionDescribe();
        return nullptr;
    }
    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "obj_inputstream %p",obj_inputstream);


    jclass cls_InputStream= env->FindClass("java/io/InputStream");
    if(cls_InputStream==NULL || env->ExceptionCheck()){
        env->ExceptionDescribe();
        return nullptr;
    }
    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "cls_InputStream %p",clsAssetManager);

    jmethodID  m_readNBytes=env->GetMethodID(cls_InputStream,"readNBytes",
                                             "(I)[B");
    if(m_readNBytes==NULL || env->ExceptionCheck()){
        env->ExceptionDescribe();
        return nullptr;
    }
    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "m_readNBytes %p",m_readNBytes);


    jbyteArray dexData= static_cast<jbyteArray>(env->CallObjectMethod(obj_inputstream, m_readNBytes,
                                                                      (jint) 0x10000000));

    jsize dexBuffSize= env->GetArrayLength(dexData);
    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "dexBuffSize %d",dexBuffSize);

    jbyte* data= env->GetByteArrayElements(dexData,JNI_FALSE);

    for(int i=0;i<dexBuffSize;i++){
        *(data+i)=(*(data+i))^48;
    }
    env->SetByteArrayRegion(dexData,0,dexBuffSize,data);

    if(dexData==NULL || env->ExceptionCheck()){
        env->ExceptionDescribe();
        return nullptr;
    }
    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "dexData %p",dexData);

    jmethodID  m_close=env->GetMethodID(cls_InputStream,"close",
                                        "()V");
    if(m_close==NULL || env->ExceptionCheck()){
        env->ExceptionDescribe();
        return nullptr;
    }
    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "m_close %p",m_close);

    env->CallVoidMethod(obj_inputstream,m_close);
    if( env->ExceptionCheck()){
        env->ExceptionDescribe();
        return nullptr;
    }
    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "m_close call ok");

    return dexData;
}

jstring getSearchDir(JNIEnv *env,jobject thiz){
    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "Java_com_example_pack_PackApp_loadApp");

    jclass clsContextWrapper= env->FindClass("android/content/ContextWrapper");
    if(clsContextWrapper==NULL || env->ExceptionCheck()){
        env->ExceptionDescribe();
        return nullptr;
    }
    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "clsContextWrapper %p",clsContextWrapper);

    jmethodID  m_getApplicationInfo=env->GetMethodID(clsContextWrapper,"getApplicationInfo",
                                                     "()Landroid/content/pm/ApplicationInfo;");

    if(m_getApplicationInfo==NULL || env->ExceptionCheck()){
        env->ExceptionDescribe();
        return nullptr;
    }

    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "m_getApplicationInfo %p",m_getApplicationInfo);

    jobject obj_ApplicationInfo= env->CallObjectMethod(thiz,m_getApplicationInfo);
    if(obj_ApplicationInfo==NULL || env->ExceptionCheck()){
        env->ExceptionDescribe();
        return nullptr;
    }
    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "obj_ApplicationInfo %p",obj_ApplicationInfo);


    jclass cls_ApplicationInfo= env->FindClass("android/content/pm/ApplicationInfo");
    if(cls_ApplicationInfo==NULL || env->ExceptionCheck()){
        env->ExceptionDescribe();
        return nullptr;
    }
    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "cls_ApplicationInfo %p",cls_ApplicationInfo);

    jfieldID f_sourceDir=env->GetFieldID(cls_ApplicationInfo,"sourceDir",
                                         "Ljava/lang/String;");

    if(f_sourceDir==NULL || env->ExceptionCheck()){
        env->ExceptionDescribe();
        return nullptr;
    }

    jstring  sourceDir= static_cast<jstring>(env->GetObjectField(obj_ApplicationInfo,
                                                                 f_sourceDir));
    if(sourceDir==NULL || env->ExceptionCheck()){
        env->ExceptionDescribe();
        return nullptr;
    }
    const char* dir=env->GetStringUTFChars(sourceDir,JNI_FALSE);

    char searchDir[1000];
    memset(searchDir,0,1000);

    strcpy(searchDir,dir);

    const char* libDir="!/lib/x86_64";
    strcat(searchDir,libDir);

    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "nativeDir %p %s",sourceDir,searchDir);
    return env->NewStringUTF(searchDir);
}

jobjectArray getDexBuffers(JNIEnv *env,jobject thiz,jbyteArray dex){
    jclass cls_ByteBuffer = env->FindClass("java/nio/ByteBuffer");
    if(cls_ByteBuffer==NULL || env->ExceptionCheck()){
        env->ExceptionDescribe();
        return nullptr;
    }
    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "cls_ByteBuffer %p",cls_ByteBuffer);

    jmethodID m_wrap =env->GetStaticMethodID(cls_ByteBuffer,"wrap", "([B)Ljava/nio/ByteBuffer;");
    if(m_wrap==NULL || env->ExceptionCheck()){
        env->ExceptionDescribe();
        return nullptr;
    }
    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "m_wrap %p",m_wrap);

    jobject dexBuffer= env->CallStaticObjectMethod(cls_ByteBuffer,m_wrap,dex);
    if(dexBuffer==NULL || env->ExceptionCheck()){
        env->ExceptionDescribe();
        return nullptr;
    }
    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "dexBuffer %p",dexBuffer);

    jobjectArray dexBuffers= env->NewObjectArray(1,cls_ByteBuffer,0);
    env->SetObjectArrayElement(dexBuffers,0,dexBuffer);
    if(env->ExceptionCheck()){
        env->ExceptionDescribe();
        return nullptr;
    }
    return dexBuffers;
}

jobject newDexClassLoader(JNIEnv *env,jobject thiz,jobjectArray dexBuffers,jstring searchDir,jobject cls_loader){
    jclass  cls_InMemoryDexClassLoader= env->FindClass("dalvik/system/InMemoryDexClassLoader");
    if(cls_InMemoryDexClassLoader==NULL || env->ExceptionCheck()){
        env->ExceptionDescribe();
        return nullptr;
    }
    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "cls_InMemoryDexClassLoader %p",cls_InMemoryDexClassLoader);


    jmethodID m_InMemoryDexClassLoader= env->GetMethodID(cls_InMemoryDexClassLoader,"<init>",
                                                         "([Ljava/nio/ByteBuffer;Ljava/lang/String;Ljava/lang/ClassLoader;)V");

    jobject obj_InMemoryDexClassLoader= env->NewObject(cls_InMemoryDexClassLoader,m_InMemoryDexClassLoader,dexBuffers,
                                                       searchDir,cls_loader);
    if(obj_InMemoryDexClassLoader==NULL || env->ExceptionCheck()){
        env->ExceptionDescribe();
        return nullptr;
    }
    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "obj_InMemoryDexClassLoader %p",obj_InMemoryDexClassLoader);
    return obj_InMemoryDexClassLoader;
}

void entryApp(JNIEnv *env,jobject thiz,jobject dex,jobject classLoader,jobject base){

    jclass cls_ClassLoader= env->FindClass("java/lang/ClassLoader");
    if(cls_ClassLoader==NULL || env->ExceptionCheck()){
        env->ExceptionDescribe();
        return;
    }
    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "cls_ClassLoader %p",cls_ClassLoader);

    jmethodID m_loadClass= env->GetMethodID(cls_ClassLoader,"loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
    if(m_loadClass==NULL || env->ExceptionCheck()){
        env->ExceptionDescribe();
        return;
    }
    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "m_loadClass %p",m_loadClass);

    jclass cls_ContextImpl=(jclass)env->CallObjectMethod(classLoader,m_loadClass,
                                                         env->NewStringUTF("android.app.ContextImpl"));
    if(cls_ContextImpl==NULL || env->ExceptionCheck()){
        env->ExceptionDescribe();
        return;
    }
    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "cls_ContextImpl %p",cls_ContextImpl);


    jfieldID f_mPackageInfo= env->GetFieldID(cls_ContextImpl,"mPackageInfo","Landroid/app/LoadedApk;");
    if(f_mPackageInfo==NULL || env->ExceptionCheck()){
        env->ExceptionDescribe();
        return;
    }
    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "f_mPackageInfo %p",f_mPackageInfo);


    jobject obj_mPackageInfo= env->GetObjectField(base,f_mPackageInfo);
    if(obj_mPackageInfo==NULL || env->ExceptionCheck()){
        env->ExceptionDescribe();
        return;
    }
    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "obj_mPackageInfo %p",obj_mPackageInfo);


    jclass cls_LoadedApk=(jclass)env->CallObjectMethod(classLoader,m_loadClass,
                                                         env->NewStringUTF("android.app.LoadedApk"));
    if(cls_LoadedApk==NULL || env->ExceptionCheck()){
        env->ExceptionDescribe();
        return;
    }
    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "cls_LoadedApk %p",cls_LoadedApk);

    jfieldID f_mClassLoader= env->GetFieldID(cls_LoadedApk,"mClassLoader","Ljava/lang/ClassLoader;");
    if(f_mClassLoader==NULL || env->ExceptionCheck()){
        env->ExceptionDescribe();
        return;
    }
    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "f_mClassLoader %p",f_mClassLoader);

    jobject obj_mClassLoader= env->GetObjectField(obj_mPackageInfo,f_mClassLoader);
    if(obj_mClassLoader==NULL || env->ExceptionCheck()){
        env->ExceptionDescribe();
        return;
    }
    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "obj_mClassLoader %p",obj_mClassLoader);

    env->SetObjectField(obj_mPackageInfo,f_mClassLoader,dex);
    __android_log_print(ANDROID_LOG_DEBUG,"ithuiyilu",
                        "replace dex ok");
}