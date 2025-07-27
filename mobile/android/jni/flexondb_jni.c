#include <jni.h>
#include "../../../core/flexondb.h"

// Helper macros to simplify JNI function definitions.
#define JNI_METHOD(name) Java_com_flexondb_FlexonDB_##name

JNIEXPORT jint JNICALL JNI_METHOD(createDatabase)(JNIEnv *env, jobject obj, jstring jpath, jstring jschema) {
    const char *path = (*env)->GetStringUTFChars(env, jpath, NULL);
    const char *schema = (*env)->GetStringUTFChars(env, jschema, NULL);
    int result = createDatabase(path, schema);
    (*env)->ReleaseStringUTFChars(env, jpath, path);
    (*env)->ReleaseStringUTFChars(env, jschema, schema);
    return result;
}

JNIEXPORT jint JNICALL JNI_METHOD(insertData)(JNIEnv *env, jobject obj, jstring jpath, jstring jjson) {
    const char *path = (*env)->GetStringUTFChars(env, jpath, NULL);
    const char *json = (*env)->GetStringUTFChars(env, jjson, NULL);
    int result = insertData(path, json);
    (*env)->ReleaseStringUTFChars(env, jpath, path);
    (*env)->ReleaseStringUTFChars(env, jjson, json);
    return result;
}

JNIEXPORT jstring JNICALL JNI_METHOD(readData)(JNIEnv *env, jobject obj, jstring jpath) {
    const char *path = (*env)->GetStringUTFChars(env, jpath, NULL);
    char* resultStr = readData(path);
    (*env)->ReleaseStringUTFChars(env, jpath, path);
    jstring jresult = (*env)->NewStringUTF(env, resultStr);
    free(resultStr);
    return jresult;
}

JNIEXPORT jint JNICALL JNI_METHOD(deleteDatabase)(JNIEnv *env, jobject obj, jstring jpath) {
    const char *path = (*env)->GetStringUTFChars(env, jpath, NULL);
    int result = deleteDatabase(path);
    (*env)->ReleaseStringUTFChars(env, jpath, path);
    return result;
}

JNIEXPORT jint JNICALL JNI_METHOD(updateDatabase)(JNIEnv *env, jobject obj, jstring jpath, jstring jjson) {
    const char *path = (*env)->GetStringUTFChars(env, jpath, NULL);
    const char *json = (*env)->GetStringUTFChars(env, jjson, NULL);
    int result = updateDatabase(path, json);
    (*env)->ReleaseStringUTFChars(env, jpath, path);
    (*env)->ReleaseStringUTFChars(env, jjson, json);
    return result;
}

JNIEXPORT jint JNICALL JNI_METHOD(csvToFlexonDB)(JNIEnv *env, jobject obj, jstring jcsvPath, jstring jdbPath) {
    const char *csvPath = (*env)->GetStringUTFChars(env, jcsvPath, NULL);
    const char *dbPath = (*env)->GetStringUTFChars(env, jdbPath, NULL);
    int result = csvToFlexonDB(csvPath, dbPath);
    (*env)->ReleaseStringUTFChars(env, jcsvPath, csvPath);
    (*env)->ReleaseStringUTFChars(env, jdbPath, dbPath);
    return result;
}