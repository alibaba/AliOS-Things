#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "mn_process_commands.h"

char *get_id_name(int i)
{
    if (i == 0)
        return MN_SPEECH_COMMAND_ID0;
    else if (i == 1)
        return MN_SPEECH_COMMAND_ID1;
    else if (i == 2)
        return MN_SPEECH_COMMAND_ID2;
    else if (i == 3)
        return MN_SPEECH_COMMAND_ID3;
    else if (i == 4)
        return MN_SPEECH_COMMAND_ID4;
    else if (i == 5)
        return MN_SPEECH_COMMAND_ID5;
    else if (i == 6)
        return MN_SPEECH_COMMAND_ID6;
    else if (i == 7)
        return MN_SPEECH_COMMAND_ID7;
    else if (i == 8)
        return MN_SPEECH_COMMAND_ID8;
    else if (i == 9)
        return MN_SPEECH_COMMAND_ID9;
    else if (i == 10)
        return MN_SPEECH_COMMAND_ID10;
    else if (i == 11)
        return MN_SPEECH_COMMAND_ID11;
    else if (i == 12)
        return MN_SPEECH_COMMAND_ID12;
    else if (i == 13)
        return MN_SPEECH_COMMAND_ID13;
    else if (i == 14)
        return MN_SPEECH_COMMAND_ID14;
    else if (i == 15)
        return MN_SPEECH_COMMAND_ID15;
    else if (i == 16)
        return MN_SPEECH_COMMAND_ID16;
    else if (i == 17)
        return MN_SPEECH_COMMAND_ID17;
    else if (i == 18)
        return MN_SPEECH_COMMAND_ID18;
    else if (i == 19)
        return MN_SPEECH_COMMAND_ID19;
    else if (i == 20)
        return MN_SPEECH_COMMAND_ID20;
    else if (i == 21)
        return MN_SPEECH_COMMAND_ID21;
    else if (i == 22)
        return MN_SPEECH_COMMAND_ID22;
    else if (i == 23)
        return MN_SPEECH_COMMAND_ID23;
    else if (i == 24)
        return MN_SPEECH_COMMAND_ID24;
    else if (i == 25)
        return MN_SPEECH_COMMAND_ID25;
    else if (i == 26)
        return MN_SPEECH_COMMAND_ID26;
    else if (i == 27)
        return MN_SPEECH_COMMAND_ID27;
    else if (i == 28)
        return MN_SPEECH_COMMAND_ID28;
    else if (i == 29)
        return MN_SPEECH_COMMAND_ID29;
    else if (i == 30)
        return MN_SPEECH_COMMAND_ID30;
    else if (i == 31)
        return MN_SPEECH_COMMAND_ID31;
    else if (i == 32)
        return MN_SPEECH_COMMAND_ID32;
    else if (i == 33)
        return MN_SPEECH_COMMAND_ID33;
    else if (i == 34)
        return MN_SPEECH_COMMAND_ID34;
    else if (i == 35)
        return MN_SPEECH_COMMAND_ID35;
    else if (i == 36)
        return MN_SPEECH_COMMAND_ID36;
    else if (i == 37)
        return MN_SPEECH_COMMAND_ID37;
    else if (i == 38)
        return MN_SPEECH_COMMAND_ID38;
    else if (i == 39)
        return MN_SPEECH_COMMAND_ID39;
    else if (i == 40)
        return MN_SPEECH_COMMAND_ID40;
    else if (i == 41)
        return MN_SPEECH_COMMAND_ID41;
    else if (i == 42)
        return MN_SPEECH_COMMAND_ID42;
    else if (i == 43)
        return MN_SPEECH_COMMAND_ID43;
    else if (i == 44)
        return MN_SPEECH_COMMAND_ID44;
    else if (i == 45)
        return MN_SPEECH_COMMAND_ID45;
    else if (i == 46)
        return MN_SPEECH_COMMAND_ID46;
    else if (i == 47)
        return MN_SPEECH_COMMAND_ID47;
    else if (i == 48)
        return MN_SPEECH_COMMAND_ID48;
    else if (i == 49)
        return MN_SPEECH_COMMAND_ID49;
    else if (i == 50)
        return MN_SPEECH_COMMAND_ID50;
    else if (i == 51)
        return MN_SPEECH_COMMAND_ID51;
    else if (i == 52)
        return MN_SPEECH_COMMAND_ID52;
    else if (i == 53)
        return MN_SPEECH_COMMAND_ID53;
    else if (i == 54)
        return MN_SPEECH_COMMAND_ID54;
    else if (i == 55)
        return MN_SPEECH_COMMAND_ID55;
    else if (i == 56)
        return MN_SPEECH_COMMAND_ID56;
    else if (i == 57)
        return MN_SPEECH_COMMAND_ID57;
    else if (i == 58)
        return MN_SPEECH_COMMAND_ID58;
    else if (i == 59)
        return MN_SPEECH_COMMAND_ID59;
    else if (i == 60)
        return MN_SPEECH_COMMAND_ID60;
    else if (i == 61)
        return MN_SPEECH_COMMAND_ID61;
    else if (i == 62)
        return MN_SPEECH_COMMAND_ID62;
    else if (i == 63)
        return MN_SPEECH_COMMAND_ID63;
    else if (i == 64)
        return MN_SPEECH_COMMAND_ID64;
    else if (i == 65)
        return MN_SPEECH_COMMAND_ID65;
    else if (i == 66)
        return MN_SPEECH_COMMAND_ID66;
    else if (i == 67)
        return MN_SPEECH_COMMAND_ID67;
    else if (i == 68)
        return MN_SPEECH_COMMAND_ID68;
    else if (i == 69)
        return MN_SPEECH_COMMAND_ID69;
    else if (i == 70)
        return MN_SPEECH_COMMAND_ID70;
    else if (i == 71)
        return MN_SPEECH_COMMAND_ID71;
    else if (i == 72)
        return MN_SPEECH_COMMAND_ID72;
    else if (i == 73)
        return MN_SPEECH_COMMAND_ID73;
    else if (i == 74)
        return MN_SPEECH_COMMAND_ID74;
    else if (i == 75)
        return MN_SPEECH_COMMAND_ID75;
    else if (i == 76)
        return MN_SPEECH_COMMAND_ID76;
    else if (i == 77)
        return MN_SPEECH_COMMAND_ID77;
    else if (i == 78)
        return MN_SPEECH_COMMAND_ID78;
    else if (i == 79)
        return MN_SPEECH_COMMAND_ID79;
    else if (i == 80)
        return MN_SPEECH_COMMAND_ID80;
    else if (i == 81)
        return MN_SPEECH_COMMAND_ID81;
    else if (i == 82)
        return MN_SPEECH_COMMAND_ID82;
    else if (i == 83)
        return MN_SPEECH_COMMAND_ID83;
    else if (i == 84)
        return MN_SPEECH_COMMAND_ID84;
    else if (i == 85)
        return MN_SPEECH_COMMAND_ID85;
    else if (i == 86)
        return MN_SPEECH_COMMAND_ID86;
    else if (i == 87)
        return MN_SPEECH_COMMAND_ID87;
    else if (i == 88)
        return MN_SPEECH_COMMAND_ID88;
    else if (i == 89)
        return MN_SPEECH_COMMAND_ID89;
    else if (i == 90)
        return MN_SPEECH_COMMAND_ID90;
    else if (i == 91)
        return MN_SPEECH_COMMAND_ID91;
    else if (i == 92)
        return MN_SPEECH_COMMAND_ID92;
    else if (i == 93)
        return MN_SPEECH_COMMAND_ID93;
    else if (i == 94)
        return MN_SPEECH_COMMAND_ID94;
    else if (i == 95)
        return MN_SPEECH_COMMAND_ID95;
    else if (i == 96)
        return MN_SPEECH_COMMAND_ID96;
    else if (i == 97)
        return MN_SPEECH_COMMAND_ID97;
    else if (i == 98)
        return MN_SPEECH_COMMAND_ID98;
    else if (i == 99)
        return MN_SPEECH_COMMAND_ID99;
    else
        return NULL;
}

void reset_speech_commands_v1(model_iface_data_t *model_data, char* command_str, char *err_phrase_id);
void reset_speech_commands_v2(model_iface_data_t *model_data, char* command_str, char *err_phrase_id);

void reset_speech_commands(model_iface_data_t *model_data, char* command_str, char *err_phrase_id)
{
#if CONFIG_SINGLE_RECOGNITION
    reset_speech_commands_v1(model_data, command_str, err_phrase_id);
#elif CONFIG_SR_CHINESE && CONFIG_CONTINUOUS_RECOGNITION
    reset_speech_commands_v2(model_data, command_str, err_phrase_id);
#endif
}