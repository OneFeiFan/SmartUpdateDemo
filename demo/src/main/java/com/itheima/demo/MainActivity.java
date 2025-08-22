package com.itheima.demo;

import android.content.Context;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.FileProvider;

import com.feifan.apkpatch.PatchUtils;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    private Context mContext;
//    private ProgressDialog mDialog;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mContext = this;
//        mDialog = new ProgressDialog(mContext);
    }

    /**
     * com.ss.android.article.news
     *
     * @param v
     */
    public void update(View v) {
        try {
            PackageManager pm = getPackageManager();
            ApplicationInfo appInfo = pm.getApplicationInfo(getPackageName(), 0);
            final String oldPath = appInfo.sourceDir;//旧版本路径
            // 修改这部分代码
            final File newApkFile = new File(getFilesDir(), "toutiao_new.apk"); // 内部存储目录
            final File patchFile = new File(getFilesDir(), "toutiao.patch"); // 内部存储目录

            if (!patchFile.exists()) {
                showToast("请将差分包toutiao.patch保存到sdcard");
                return;
            }

//            mDialog.show();
            new Thread(new Runnable() {
                @Override
                public void run() {
                    int result = 1;
                    try {
                        result = PatchUtils.patch(oldPath, newApkFile.getAbsolutePath(), patchFile.getAbsolutePath());
                    } catch (Exception e) {
                        e.printStackTrace();
                        System.out.println("合并失败");
                        System.out.println(e.getMessage());
                    }
                    if (result == 0) {
                        //合并成功，安装apk
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
//                                mDialog.dismiss();
                                install(newApkFile.getAbsolutePath());
                            }
                        });
                    } else {
                        showToast("合并失败");
                    }
                }
            }).start();

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void install(String apkPath) {
        File file = new File(apkPath);
        Uri uri;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            // Android 7.0及以上需要使用FileProvider
            uri = FileProvider.getUriForFile(this,
                    getPackageName() + ".fileprovider", file);
        } else {
            uri = Uri.fromFile(file);
        }

        Intent intent = new Intent(Intent.ACTION_VIEW);
        intent.setDataAndType(uri, "application/vnd.android.package-archive");
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);

        // 添加临时读取权限
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
        }

        startActivity(intent);
    }


    private void showToast(String msg) {
        Toast.makeText(mContext, "" + msg, Toast.LENGTH_SHORT).show();
    }
}
