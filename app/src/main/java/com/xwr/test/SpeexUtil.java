package com.xwr.test;

/**
 * Create by xwr on 2019/12/13
 * Describe:
 */
public class SpeexUtil {
  static {
    System.loadLibrary("speex");
  }
 public native  int CancelNoiseInit(int frame_size,int sample_rate);
  public native  int CancelNoiseDestroy();
  public  native  int InitAudioAEC(int frame_size,int filter_length,int sampling_rate);
  public native int AudioAECProc(byte[] recordArray,byte[] playArray,byte[] szOutArray);
  public native int ExitSpeexDsp();
}
