package com.example.xzy.myhome.util;

import android.text.Editable;
import android.text.InputType;
import android.text.TextWatcher;
import android.util.Log;
import android.widget.EditText;

/**
 * Created by xzy on 16/9/9.
 */

public class CustomTextWatcher implements TextWatcher {

    private static final String TAG = "CustomTextWatcher";

    private boolean mFormat;

    private boolean mInvalid;

    private int mSelection;

    private String mLastText;

    /**
     * The editText to edit text.
     */
    private EditText mEditText;

    /**
     * Creates an instance of <code>CustomTextWatcher</code>.
     *
     * @param editText
     *        the editText to edit text.
     */
    public CustomTextWatcher(EditText editText) {

        super();
        mFormat = false;
        mInvalid = false;
        mLastText = "";
        this.mEditText = editText;
        this.mEditText.setInputType(InputType.TYPE_TEXT_VARIATION_PASSWORD);
    }

    @Override
    public void beforeTextChanged(CharSequence charSequence, int start,
                                  int count, int after) {

    }

    @Override
    public void onTextChanged(CharSequence charSequence, int start, int before,
                              int count) {

        try {

            String temp = charSequence.toString();

            // Set selection.
            if (mLastText.equals(temp)) {
                if (mInvalid) {
                    mSelection -= 1;
                } else {
                    if ((mSelection >= 1) && (temp.length() > mSelection - 1)
                            && (temp.charAt(mSelection - 1)) == ' ') {
                        mSelection += 1;
                    }
                }
                int length = mLastText.length();
                if (mSelection > length) {

                    mEditText.setSelection(length);
                } else {

                    mEditText.setSelection(mSelection);
                }
                mFormat = false;
                mInvalid = false;
                return;
            }

            mFormat = true;
            mSelection = start;

            // Delete operation.
            if (count == 0) {
                if ((mSelection >= 1) && (temp.length() > mSelection - 1)
                        && (temp.charAt(mSelection - 1)) == ' ') {
                    mSelection -= 1;
                }

                return;
            }

            // Input operation.
            mSelection += count;
            char[] lastChar = (temp.substring(start, start + count))
                    .toCharArray();
            int mid = lastChar[0];
            if (mid >= 48 && mid <= 57) {
                /* 1-9. */
            } else if (mid >= 65 && mid <= 70) {
                /* A-F. */
            } else if (mid >= 97 && mid <= 102) {
                /* a-f. */
            } else {
                /* Invalid input. */
                mInvalid = true;
                temp = temp.substring(0, start)
                        + temp.substring(start + count, temp.length());
                mEditText.setText(temp);
                return;
            }

        } catch (Exception e) {
            Log.i(TAG, e.toString());
        }
    }

    @Override
    public void afterTextChanged(Editable editable) {

        try {

            /* Format input. */
            if (mFormat) {
                StringBuilder text = new StringBuilder();
                text.append(editable.toString().replace(" ", ""));
                int length = text.length();
                int sum = (length % 2 == 0) ? (length / 2) - 1 : (length / 2);
                for (int offset = 2, index = 0; index < sum; offset += 3, index++) {

                    text.insert(offset, " ");
                }
                mLastText = text.toString();
                mEditText.setText(text);
            }
        } catch (Exception e) {
            Log.i(TAG, e.toString());
        }
    }

}