package com.example.xzy.myhome.db;

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

/**
 * Created by xzy on 2016/10/24.
 */

public class DeviceDatabaseHelper extends SQLiteOpenHelper {
    public static final String CREATE_DEVICE="create table device ("
                    + "id integer primary key autoincrement, "
            + "mac text, "
            + "type integer, "
            + "count integer, "
            + "timing_off integer, "
            + "timing_on integer, "
            + "countdown_off integer, "
            + "countdown_on integer, "
            + "state integer)";

    public DeviceDatabaseHelper(Context context, String name, SQLiteDatabase.CursorFactory factory, int version) {
        super(context, name, factory, version);
    }

    @Override
    public void onCreate(SQLiteDatabase db) {
        db.execSQL(CREATE_DEVICE);

    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {

    }
}
