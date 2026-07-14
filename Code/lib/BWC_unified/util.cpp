#include "util.h"

float C2F(float c)
{
    return c*1.8+32;
}

float F2C(float f)
{
    return (f-32)/1.8;
}


int gpio2dp(int gpio)
{
    for(int i = 0; i < 9; i++)
    {
        if(gpio == D_pin[i]) return i;
    }
    return -1;
}

int dp2gpio(int dp)
{
#ifdef ESP8266
    switch(dp)
    {
        case D0: return D0;
        case D1: return D1;
        case D2: return D2;
        case D3: return D3;
        case D4: return D4;
        case D5: return D5;
        case D6: return D6;
        case D7: return D7;
        case D8: return D8;
        default: return -1;
    }
#else
    // Sur ESP32, les GPIO sont déjà des numéros directs — pas de mapping
    return dp;
#endif
}