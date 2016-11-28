package com.example.xzy.myhome.util;

/**
 * Created by xzy on 16/9/22.
 */

public class BooleanTranslateUtil {
    private static String transLateTrue = "是";
    private static String transLateFalse = "否";

    public static String translateString(boolean i) {
        if (i == true) {
            return transLateTrue;
        } else {
            return transLateFalse;
        }
    }
}
