#ifndef COMM_DEF_H
#define COMM_DEF_H

//******************************************************************************
//  Definitions
//******************************************************************************
#define AMPLIFIER_GAIN_1_SETTING        0
#define AMPLIFIER_GAIN_5_SETTING        1
#define AMPLIFIER_GAIN_10_SETTING       2
#define AMPLIFIER_GAIN_25_SETTING       3
#define AMPLIFIER_GAIN_50_SETTING       4
#define AMPLIFIER_GAIN_100_SETTING      5
#define AMPLIFIER_GAIN_250_SETTING      6
#define AMPLIFIER_GAIN_500_SETTING      7
#define CALC_METHOD_ARB_MU              2
#define CALC_METHOD_DERIVED_MU          1
#define CALC_METHOD_SIMPLE_E            0
#define CYCLE_TIME_MAX                  10
#define CYCLE_TIME_MIN                  2
#define MAT_DENSITY_MAX                 500
#define MAT_DENSITY_MAX_MET             8010
#define MAT_DENSITY_MET_TO_IMP          0.062428
#define MAT_DENSITY_IMP_TO_MET          16
#define MAT_DENSITY_MIN                 50
#define MAT_DENSITY_MIN_MET             801
#define MAT_TRAVEL_DIST_MAX             600
#define MAT_TRAVEL_DIST_MAX_MET         15.240000
#define MAT_TRAVEL_DIST_MET_TO_IMP      39.370079
#define MAT_TRAVEL_DIST_IMP_TO_MET      254 / 10000
#define MAT_TRAVEL_DIST_MIN             0.1
#define MAT_TRAVEL_DIST_MIN_MET         0.002540
#define MAT_TRAVEL_VEL_MAX              40000
#define MAT_TRAVEL_VEL_MAX_MET          12191
#define MAT_TRAVEL_VEL_MET_TO_IMP       3.280851
#define MAT_TRAVEL_VEL_IMP_TO_MET       3047 / 10000
#define MAT_TRAVEL_VEL_MIN              1000
#define MAT_TRAVEL_VEL_MIN_MET          304
#define PICTURE_RATE_250MHZ             0
#define PICTURE_RATE_500MHZ             1
#define PICTURE_RATE_1000MHZ            2
#define PICTURE_RATE_2000MHZ            3
#define PULSE_CALC_DISTANCE             0
#define PULSE_CALC_VELOCITY             1
#define PULSES_PER_SEQ_1                1
#define PULSES_PER_SEQ_3                3
#define PULSES_PER_SEQ_10               10
#define REVIEW_TEST_NUM_MAX             1800
#define REVIEW_TEST_NUM_MIN             1
#define TEST_EXEC_NUM_MAX               1800
#define TEST_EXEC_NUM_MIN               1
#define UNITS_IMPERIAL                  0
#define UNITS_METRIC                    1

#define REMOTE_CTRL_HEADER              0x5A
#define REMOTE_CTRL_FOOTER              -93//0xA3  used decimal for negative number warning
#define REMOTE_CTRL_MSG_SIZE            5

#define MSG_CODE_PULSE_FREQ             0x00
#define MSG_CODE_PULSE_CYCLE_TIME       0x01
#define MSG_CODE_SAVE_TEST              0x02
#define MSG_CODE_DISPLAY_SIG            0x03
#define MSG_CODE_MSMT_MODE              0x04
#define MSG_CODE_CH_DISTANCE            0x05
#define MSG_CODE_CH_VELOCITY            0x06
#define MSG_CODE_DISTANCE               0x00
#define MSG_CODE_VELOCITY               0x01
#define MSG_CODE_RUN_A_TEST             0x07
#define MSG_CODE_AMP_GAIN               0x10
#define MSG_CODE_PICTURE_RATE           0x11
#define MSG_CODE_PULSER_VOLTAGE         0x12
#define MSG_CODE_HIVOLT                 0x01
#define MSG_CODE_LOVOLT                 0x00
#define MSG_CODE_WAVE_TYPE              0x20
#define MSG_CODE_COMPRESSION            0x00
#define MSG_CODE_SHEAR                  0x01
#define MSG_CODE_DENSITY                0x21
#define MSG_CODE_E_MU_CALC_METHOD       0x22
#define MSG_CODE_REVIEW_TEST_NUM        (0x30)
#define MSG_CODE_INITIAL_SETTINGS       0x40
#define MSG_CODE_WATCHDOG_RESET         0x50
#define MSG_CODE_FILL                   -1//0xFF used decimal for negative number warning
#define MSG_CODE_OFF                    0x00
#define MSG_CODE_ON                     0x01

#endif // COMM_DEF_H
