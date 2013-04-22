/*
 * Copyright (C) 2012-2013 The CyanogenMod Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.cyanogenmod.settings.device;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.preference.DialogPreference;
import android.preference.PreferenceManager;
import android.util.AttributeSet;
import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Button;
import android.util.Log;

public class VibratorIntensityPreference extends DialogPreference {

    private static final String TAG = "XperiaSettings_Vibrator";
    private static String FILE_PATH = null;
    private vibratorSeekBar mVibratorSeekBar;
    private static int MAX_VALUE;
    private static int OFFSET_VALUE;

    public VibratorIntensityPreference(Context context, AttributeSet attrs) {
        super(context, attrs);
        FILE_PATH = context.getResources().getString(R.string.vibrator_sysfs_file);
        MAX_VALUE = Integer.valueOf(context.getResources().getString(R.string.max_intensity_value));
        OFFSET_VALUE = Integer.valueOf(context.getResources().getString(R.string.intensity_value_offset));

        setDialogLayoutResource(R.layout.preference_dialog_vibrator_intensity);
    }

    @Override
    protected void onBindDialogView(View view) {
        super.onBindDialogView(view);

        SeekBar seekBar = (SeekBar) view.findViewById(R.id.vibrator_seekbar);
        TextView valueDisplay = (TextView) view.findViewById(R.id.vibrator_value);
        mVibratorSeekBar = new vibratorSeekBar(seekBar, valueDisplay, FILE_PATH, OFFSET_VALUE, MAX_VALUE);
    }

    /**
     * Restore intensity level from SharedPreferences. (Write to kernel.)
     *
     * @param context The context to read the SharedPreferences from
     */
    public static void restore(Context context) {
        if (!isSupported()) {
            return;
        }

        SharedPreferences sharedPrefs = PreferenceManager.getDefaultSharedPreferences(context);
        Boolean bFirstTime = sharedPrefs.getBoolean("FirstTimeVibrator", true);

        String sDefaultValue = Utils.readOneLine(FILE_PATH);
        int iValue = sharedPrefs.getInt(FILE_PATH, Integer.valueOf(sDefaultValue));
        if (bFirstTime)
            Utils.writeValue(FILE_PATH, (String) String.valueOf((long) MAX_VALUE));
        else
            Utils.writeValue(FILE_PATH, String.valueOf((long) iValue));

        if (bFirstTime)
        {
            SharedPreferences.Editor editor = sharedPrefs.edit();
            editor.putBoolean("FirstTimeVibrator", false);
            editor.commit();
        }
    }

    /**
     * Check whether the running kernel supports setting vibrator intensity or not.
     *
     * @return Wether vibrator intensity is supported or not
     */
    public static boolean isSupported() {
        boolean supported = true;

        if (!Utils.fileExists(FILE_PATH)) {
            supported = false;
        }

        return supported;
    }

    class vibratorSeekBar implements SeekBar.OnSeekBarChangeListener {

        private String mFilePath;

        private int mOriginal;

        private SeekBar mSeekBar;

        private TextView mValueDisplay;

        private int iOffset;

        private int iMax;

        public vibratorSeekBar(SeekBar seekBar, TextView valueDisplay, String filePath, Integer offsetValue, Integer maxValue) {
            int iValue;

            mSeekBar = seekBar;
            mValueDisplay = valueDisplay;
            mFilePath = filePath;
            iOffset = offsetValue;
            iMax = maxValue;

            SharedPreferences sharedPreferences = getSharedPreferences();

            // Read original value
            if (Utils.fileExists(mFilePath)) {
                String sDefaultValue = Utils.readOneLine(mFilePath);
                iValue = Integer.valueOf(sDefaultValue);
            } else {
                iValue = iMax - iOffset;
            }
            mOriginal = iValue;

            mSeekBar.setMax(iMax);

            reset();
            mSeekBar.setOnSeekBarChangeListener(this);
        }

        public void reset() {
            int iValue;

            iValue = mOriginal + iOffset;
            mSeekBar.setProgress(iValue);
            updateValue(mOriginal);
        }

        public void save() {
            int iValue;

            iValue = mSeekBar.getProgress() - iOffset;
            Editor editor = getEditor();
            editor.putInt(mFilePath, iValue);
            editor.commit();
        }

        @Override
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            int iValue;

            iValue = progress - iOffset;
            Utils.writeValue(mFilePath, String.valueOf((long) iValue));
            updateValue(iValue);
        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {
            // Do nothing
        }

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {
            // Do nothing
        }

        private void updateValue(int progress) {
            mValueDisplay.setText(String.format("%d", (int) progress));
        }

        public void setNewValue(int iValue) {
            mOriginal = iValue;
            reset();
        }
    }
}
