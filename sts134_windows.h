#include "windows.h"
// Launch window definitions for mission #sts134
window_c sts134_windows[] =
{
    window_c (  4,19,11, 7,43,53,PM, 7,48,53,PM, 7,53,53,PM ),
    window_c (  4,20,11, 7,18,10,PM, 7,23,10,PM, 7,28,10,PM ),
    window_c (  4,21,11, 6,55,39,PM, 7, 0,39,PM, 7, 5,39,PM ),
    window_c (  4,22,11, 6,29,56,PM, 6,34,56,PM, 6,39,56,PM ),
    window_c (  4,23,11, 6, 7,25,PM, 6,12,25,PM, 6,17,25,PM ),
    window_c (  4,24,11, 5,41,43,PM, 5,46,43,PM, 5,51,43,PM ),
    window_c (  4,25,11, 5,19,11,PM, 5,24,11,PM, 5,29,11,PM ),
    window_c (  4,26,11, 4,53,29,PM, 4,58,29,PM, 5, 3,29,PM ),
    window_c (  4,27,11, 4,30,58,PM, 4,35,58,PM, 4,40,58,PM ),
    window_c (  4,28,11, 4, 5,44,PM, 4,10,16,PM, 4,15,16,PM ),
    window_c (  4,29,11, 3,42,44,PM, 3,47,44,PM, 3,52,44,PM ),
    window_c (  4,30,11, 3,20,22,PM, 3,22, 2,PM, 3,27, 2,PM ),
    window_c (  5, 1,11, 2,54,30,PM, 2,59,30,PM, 3, 4,30,PM ),
    window_c (  5, 3,11, 2, 6,16,PM, 2,11,16,PM, 2,16,16,PM ),
    window_c (  5, 5,11, 1,18, 3,PM, 1,23, 3,PM, 1,28, 3,PM ),
    window_c (  5, 7,11,12,29,49,PM,12,34,49,PM,12,39,49,PM ),
    window_c (  5, 9,11,11,41,35,AM,11,46,35,AM,11,51,35,AM ),
};
const int num_sts134_windows = sizeof(sts134_windows)/sizeof(window_c);

window_c* windows = sts134_windows;
int num_windows = num_sts134_windows;
