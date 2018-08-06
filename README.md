# OTRACK_wheel_encoder
Ardiono wheel encoder project. 

This simple code is for use with Otrack PCB and pulse wheel encoder. 
Targer Arduino board - Arduino DUE

Three pins of PCB connected to encoder driver: 
DIO8 to channel A 
DIO9 to channel B
DIO10 to reference channel

Code initializes interrupts for channel A and referece channel. 
During interrupt on channel A, code recognizes spin direction and increase/decrease pulse counter 
During reference channel interrupt code jsut count number of pulses (full circle spin).

Encoder provides 2000 pulses per 360 degree. 
In experimental setup wheel of 160 mm diameter utilized (500 mm circle lenght).  it means that each pulse equivalent to 25 um distance. 
this value hardcoded as  const int32_t um_per_pulse = 25;  

Scetch sends a data to host PC via virtual com port interface (115200-8-N-1) (use Programming USB for connection). 
By default it sends data once a 500 milliseconds - this might be adjusted by changing: 
const int32_t ms_vcom_update_rate = 500; 

Device also could accept simple commands: 
1) "RESET\r\n\" - resets all values to 0; 
2) "REVERSE\r\n\" - reverses a directions of spin. 

typical UART output is: 
  
Reverse                                         #defines last spin direction  

Number of turns: 5                              #number of full turns (from reference channel)

Number of pulses overall: 112686                #overall number of pulses

Overall distance (mm): 2817                     #overall distance in mm. 
