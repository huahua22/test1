//
// Created by xwr on 2019/12/13.
//

#include <stdio.h>
#include <jni.h>
#include <string.h>
#include "speex/speex_preprocess.h"
#include "speex/speex_echo.h"
#include "speex/speex_config_types.h"
#include <stdio.h>
#include <android/log.h>
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "keymatch", __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, "keymatch", __VA_ARGS__)


SpeexPreprocessState *st;
int nInitSuccessFlag = 0;
SpeexEchoState* m_pState;
SpeexPreprocessState* m_pPreprocessorState;
int      m_nFrameSize;
int      m_nFilterLen;
int      m_nSampleRate;
int 	 iArg;

JNIEXPORT jint JNICALL
Java_com_xwr_test_SpeexUtil_CancelNoiseInit(JNIEnv *env, jobject instance, jint frame_size,
                                            jint sample_rate) {

    int i;
    int count=0;
    float f;

    st = speex_preprocess_state_init(frame_size/2, sample_rate);

    i=1;
    speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_DENOISE, &i);//降噪
    i = -25;//负的32位整数
    speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_NOISE_SUPPRESS,&i); //设置噪声的dB

    i=1;
    speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_AGC, &i);//增益
    i=24000;
    speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_AGC_LEVEL, &i);
    i=0;
    speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_DEREVERB, &i);
    f=.0;
    speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_DEREVERB_DECAY, &f);
    f=.0;
    speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_DEREVERB_LEVEL, &f);
    LOGI("speex init");
    return 1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_xwr_test_SpeexUtil_CancelNoiseDestroy(JNIEnv *env, jobject instance) {

    if(st != NULL){
        speex_preprocess_state_destroy(st);
        st = NULL;
    }
    LOGI("speex destory");
    return 1;

}

extern "C"
JNIEXPORT jint JNICALL
Java_com_xwr_test_SpeexUtil_InitAudioAEC(JNIEnv *env, jobject instance, jint frame_size,
                                         jint filter_length, jint sampling_rate) {

    if(nInitSuccessFlag == 1)
        return 1 ;

    if (frame_size<=0 || filter_length<=0 || sampling_rate<=0)
    {
        m_nFrameSize  =160;
        m_nFilterLen  = 160*8;
        m_nSampleRate = 8000;
    }
    else
    {
        m_nFrameSize  =frame_size;
        m_nFilterLen  = filter_length;
        m_nSampleRate = sampling_rate;
    }

    m_pState = speex_echo_state_init(m_nFrameSize, m_nFilterLen);
    if(m_pState == NULL)
        return -1 ;

    m_pPreprocessorState = speex_preprocess_state_init(m_nFrameSize, m_nSampleRate);
    if(m_pPreprocessorState == NULL)
        return -2 ;

    iArg = m_nSampleRate;
    speex_echo_ctl(m_pState, SPEEX_ECHO_SET_SAMPLING_RATE, &iArg);
    speex_preprocess_ctl(m_pPreprocessorState, SPEEX_PREPROCESS_SET_ECHO_STATE, m_pState);
    nInitSuccessFlag = 1 ;
    return 1 ;

}

extern "C"
JNIEXPORT jint JNICALL
Java_com_xwr_test_SpeexUtil_AudioAECProc(JNIEnv *env, jobject instance, jbyteArray recordArray_,
                                         jbyteArray playArray_, jbyteArray szOutArray_) {
    if(nInitSuccessFlag == 0)
        return 0 ;
    jbyte *recordArray = env->GetByteArrayElements(recordArray_, NULL);
    jbyte *playArray = env->GetByteArrayElements(playArray_, NULL);
    jbyte *szOutArray = env->GetByteArrayElements(szOutArray_, NULL);

    speex_echo_cancellation(m_pState,(spx_int16_t *)recordArray,
                            (spx_int16_t *)playArray,(spx_int16_t *)szOutArray);
    int flag=speex_preprocess_run(m_pPreprocessorState,(spx_int16_t *)szOutArray);

    env->ReleaseByteArrayElements(recordArray_, recordArray, 0);
    env->ReleaseByteArrayElements(playArray_, playArray, 0);
    env->ReleaseByteArrayElements(szOutArray_, szOutArray, 0);
    return  1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_xwr_test_SpeexUtil_ExitSpeexDsp(JNIEnv *env, jobject instance) {

    if(nInitSuccessFlag == 0)
        return 0 ;

    if (m_pState != NULL)
    {
        speex_echo_state_destroy(m_pState);
        m_pState = NULL;
    }
    if (m_pPreprocessorState != NULL)
    {
        speex_preprocess_state_destroy(m_pPreprocessorState);
        m_pPreprocessorState = NULL;
    }

    nInitSuccessFlag = 0 ;

    return 1 ;

}