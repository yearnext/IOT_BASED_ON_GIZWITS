package com.example.xzy.myhome.util;

import android.app.Activity;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by xzy on 2016/11/4.
 */

public class ActivityCollector {
    public static List<Activity> activityList = new ArrayList<>();

    public static void addActivity(Activity activity) {
        activityList.add(activity);
    }

    public static void finishAll() {
        activityList.remove(activityList);
    }

    public static void removeActivity(Activity activity) {
        activityList.remove(activity);
    }


}
