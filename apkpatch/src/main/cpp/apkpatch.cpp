#include <jni.h>
#include <string>
#include <android/log.h>


#include "bzlib.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <fcntl.h>
#include "bsdiff.h"

static void log_error(void *opaque, const char *errmsg)
{
	(void)opaque;
	fprintf(stderr, "%s", errmsg);
}

int applypatch(int argc, char * argv[])
{
    int ret = 1;
    struct bsdiff_stream oldfile = { 0 }, newfile = { 0 }, patchfile = { 0 };
    struct bsdiff_ctx ctx = { 0 };
    struct bsdiff_patch_packer packer = { 0 };

    if (argc != 4) {
        fprintf(stderr, "usage: %s oldfile newfile patchfile\n", argv[0]);
        goto cleanup;
    }

    if ((ret = bsdiff_open_file_stream(BSDIFF_MODE_READ, argv[1], &oldfile)) != BSDIFF_SUCCESS) {
        fprintf(stderr, "can't open oldfile: %s\n", argv[1]);
        goto cleanup;
    }
    if ((ret = bsdiff_open_file_stream(BSDIFF_MODE_WRITE, argv[2], &newfile)) != BSDIFF_SUCCESS) {
        fprintf(stderr, "can't open newfile: %s\n", argv[2]);
        goto cleanup;
    }
    if ((ret = bsdiff_open_file_stream(BSDIFF_MODE_READ, argv[3], &patchfile)) != BSDIFF_SUCCESS) {
        fprintf(stderr, "can't open patchfile: %s\n", argv[3]);
        goto cleanup;
    }
    if ((ret = bsdiff_open_bz2_patch_packer(BSDIFF_MODE_READ, &patchfile, &packer)) != BSDIFF_SUCCESS) {
        fprintf(stderr, "can't create BZ2 patch packer\n");
        goto cleanup;
    }

    ctx.log_error = log_error;

    if ((ret = bspatch(&ctx, &oldfile, &newfile, &packer)) != BSDIFF_SUCCESS) {
        fprintf(stderr, "bspatch failed: %d\n", ret);
        goto cleanup;
    }

    cleanup:
    bsdiff_close_patch_packer(&packer);
    bsdiff_close_stream(&patchfile);
    bsdiff_close_stream(&newfile);
    bsdiff_close_stream(&oldfile);

    return ret;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_feifan_apkpatch_PatchUtils_patch(JNIEnv *env, jclass clazz, jstring old_apk_path,
                                          jstring new_apk_path, jstring patch_path) {
    char * ch[4];
    ch[0] = "bspatch";
    ch[1] = (char*) ((*env).GetStringUTFChars(old_apk_path, 0));
    ch[2] = (char*) ((*env).GetStringUTFChars(new_apk_path, 0));
    ch[3] = (char*) ((*env).GetStringUTFChars(patch_path, 0));

    __android_log_print(ANDROID_LOG_INFO, "ApkPatchLibrary", "old = %s ", ch[1]);
    __android_log_print(ANDROID_LOG_INFO, "ApkPatchLibrary", "new = %s ", ch[2]);
    __android_log_print(ANDROID_LOG_INFO, "ApkPatchLibrary", "patch = %s ", ch[3]);

    int ret = applypatch(4, ch);

    __android_log_print(ANDROID_LOG_INFO, "ApkPatchLibrary", "applypatch result = %d ", ret);

    (*env).ReleaseStringUTFChars(old_apk_path, ch[1]);
    (*env).ReleaseStringUTFChars(new_apk_path, ch[2]);
    (*env).ReleaseStringUTFChars( patch_path, ch[3]);

    return ret;
}