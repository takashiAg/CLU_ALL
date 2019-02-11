#include "EEPROM.h"

#define len(ARR) (sizeof(ARR)/sizeof(ARR[0]))
#define pwmpin 11

#define default_heart_wave_cycletime 300
#define default_heart_cycletime 1000
#define default_heart_wave_amplitude 256
#define default_breath_cycletime 4000
#define default_breath_wave_amplitude 1024

//#define debug_on
//#define debug_on_nograph
#define debug_wave

#define EEPmode_pin 2

/*
  unsigned long parameter[][6] = {
  {0, 300, 1000, 50, 4000, 200},
  {1, 300, 1000, 40, 3500, 100},
  {2, 300, 1000, 50, 3000, 200},
  {60, 300, 1000, 40, 3000, 40}
  };
  //*/
unsigned int  parameter[50][6];
int parameters_counter = 0;

boolean old_EEPmode_flag = false;

int mil_time = 0;

unsigned long start_time;

String wavetype = "heart";

int heart_wave_cycletime = default_heart_wave_cycletime;
int heart_wave_amplitude = default_heart_wave_amplitude;
int heart_cycletime = default_heart_cycletime;
int breath_wave_amplitude = default_breath_wave_amplitude;
int breath_cycletime = default_breath_cycletime;

void setup() {
  pwm_init(pwmpin, 1);
  analogWrite(pwmpin, 127);
  Serial.begin(115200);
  Serial.println("start the program");
  load_parameter();
  //show_parameter();
  parameters_counter = 0;
  start_time = millis();
}

void loop() {
  int EEPmode_or_serialmode = check_EEPmode_or_serialmode();

  //Serial.println(EEPmode_or_serialmode);

  if (EEPmode_or_serialmode == 1) {
    boolean save = parameter_serial();
    if (save) {
      show_parameter();
      save_parameter();
      parameters_counter = 0;
      start_time = millis();
    } else {
      Serial.println("nosave");
      load_parameter();
      show_parameter();
    }
  } else if (EEPmode_or_serialmode == 2) {
    load_parameter();
    show_parameter();
    parameters_counter = 0;
    start_time = millis();
  }
  update_parameter();

  double heart_wave = wave(wavetype, heart_cycletime, heart_wave_cycletime, heart_wave_amplitude);

  double breath_wave = make_breath_wave( breath_cycletime, breath_cycletime, breath_wave_amplitude);

  double wave_value = breath_wave + heart_wave;

  int normalized_wave = normalization(wave_value);

  analogWrite(pwmpin, normalized_wave );

  disp(mil_time, normalized_wave);

  delay(1);
}

int normalization (double __wave) {
  int int_wave = int(__wave);
  if (int_wave < 0) {
    return 0;
  } else if (int_wave > 255) {
    return 255;
  } else {
    return int_wave;
  }
}

void load_parameter() {
  for (int i = 0; i < 50; i++) {
    for (int j = 0; j < 6; j++) {
      unsigned long parameter_local = parameter[i][j];

      unsigned long parameter_local_1 = EEPROM.read((i * 6 + j) * 2);
      unsigned long parameter_local_2 = EEPROM.read((i * 6 + j) * 2 + 1);
      parameter[i][j] = parameter_local_1 + (parameter_local_2 << 8);
    }
  }
}

int save_parameter() {
  for (int i = 0; i < 50; i++) {
    for (int j = 0; j < 6; j++) {
      unsigned long parameter_local = parameter[i][j];
      int parameter_local_1 = parameter[i][j] & 0xFF;
      int parameter_local_2 = (parameter[i][j] & 0xFF00) >> 8;

      EEPROM.write((i * 6 + j) * 2, parameter_local_1);
      EEPROM.write((i * 6 + j) * 2 + 1, parameter_local_2);
      //Serial.println((i * 6 + j) * 2 + 1);
    }
  }
}

int check_EEPmode_or_serialmode() {
  int return_value = 0;
  boolean EEPmode_flag = digitalRead(EEPmode_pin);
  if (EEPmode_flag ^ old_EEPmode_flag) {
    if (EEPmode_flag) {
      return_value = 1;
    } else {
      return_value = 2;
    }
    old_EEPmode_flag = EEPmode_flag;
  }
  return return_value;
}

void load_default_parameter() {
  parameter[0][0] = 0;
  parameter[0][1] = 300;
  parameter[0][2] = 1000;
  parameter[0][3] = 50;
  parameter[0][4] = 4000;
  parameter[0][5] = 0;
  parameter[1][0] = 200;
}

void show_parameter() {
  Serial.println("");
  Serial.print("[");
  for (int i = 0; i < 10; i++) {
    if (i != 0) {
      Serial.print(",");
    }
    Serial.print("[");
    for (int j = 0; j < 6; j++) {
      Serial.print(parameter[i][j]);
      Serial.print(",");
    }
    Serial.print("]");
  }
  Serial.println("]");
  Serial.print("that was params\n");
}

boolean  parameter_serial() {
  return serial_1byte();
}

void clear_params() {
  for (int j = 0; j < 50; j++) {
    for (int i = 0; i < 6; i++) {
      parameter[j][i] = 0;
    }
  }
};

int convert_char_to_int(char input_value) {
  int return_value = 0;
  switch (input_value) {
    case '0':
      return_value = 0;
      break;
    case '1':
      return_value = 1;
      break;
    case '2':
      return_value = 2;
      break;
    case '3':
      return_value = 3;
      break;
    case '4':
      return_value = 4;
      break;
    case '5':
      return_value = 5;
      break;
    case '6':
      return_value = 6;
      break;
    case '7':
      return_value = 7;
      break;
    case '8':
      return_value = 8;
      break;
    case '9':
      return_value = 9;
      break;
    case 'a':
      return_value = 10;
      break;
    case 'b':
      return_value = 11;
      break;
    case 'c':
      return_value = 12;
      break;
    case 'd':
      return_value = 13;
      break;
    case 'e':
      return_value = 14;
      break;
    case 'f':
      return_value = 15;
      break;
  }
  return return_value;
}

boolean serial_1byte() {
  while (Serial.available() > 0) {
    Serial.read();
  }
  Serial.println("start serial\n");
  int j = 0;
  clear_params();
  int flag_serial_number = 0;
  boolean end_flag = false;
  boolean escape_while_flag = false;
  boolean save = true;
  while (!escape_while_flag) {
    if (check_EEPmode_or_serialmode() == 2) {
      escape_while_flag = true;
      save = false;
      j = 0;
    }
    if (Serial.available() > 0) { // 受信したデータが存在する
      char data = Serial.read(); // 受信データを読み込む
      switch (flag_serial_number) {
        case 0:
          if (data == 's') {
            flag_serial_number += 1;
            end_flag = false;
          } else if (data == 'e') {
            end_flag = true;
            flag_serial_number += 1;
          } else {
            flag_serial_number = 0;
            end_flag = false;
          }
          break;
        case 1:
          if (data == 't' && end_flag == false) {
            flag_serial_number += 1;
          } else if (data == 'n') {
            end_flag = true;
            flag_serial_number += 1;
          } else {
            flag_serial_number = 0;
            end_flag = false;
          }
          break;
        case 2:
          if (data == 'a' && end_flag == false) {
            flag_serial_number += 1;
          } else if (data == 'd') {
            escape_while_flag = true;
          } else {
            flag_serial_number = 0;
            end_flag = false;
          }
          break;
        case 3:
          if (data == 'r') {
            flag_serial_number += 1;
          } else {
            flag_serial_number = 0;
          }
          break;
        case 4:
          if (data == 't') {
            flag_serial_number += 1;
          } else {
            flag_serial_number = 0;
          }
          break;
        case 29:
          flag_serial_number = 0;
          j++;
          break;

        default:
          unsigned int value = (flag_serial_number + 3) % 4;
          unsigned int value2 = (flag_serial_number - value) / 4 - 1;
          parameter[j][value2] += int(convert_char_to_int(data) * (4096 >> (value * 4)));
          flag_serial_number++;
          break;
      }
    }
  }
  return save;
}

void update_parameter() {

  if (len(parameter) <= parameters_counter) {
    return;
  }
  if (parameter[parameters_counter][0] == 0 && parameter[parameters_counter][1] == 0) {
    return;
  }
  if (millis() + start_time > 60000 * parameter[parameters_counter][0]) {
    heart_wave_cycletime  = parameter[parameters_counter][1];
    heart_cycletime       = parameter[parameters_counter][2];
    heart_wave_amplitude  = parameter[parameters_counter][3];
    breath_cycletime      = parameter[parameters_counter][4];
    breath_wave_amplitude = parameter[parameters_counter][5];
    parameters_counter++;
  }
}

double make_breath_wave( int wave_cycle_time, int cycle_time, int amplitude) {
  int time_now = miltime(wave_cycle_time);
  return  100.0 + ( - cos(float(time_now) * 2.0 * 3.14 / float(cycle_time))) * float(amplitude) / 2.0;
}

//wave
double wave(String __wave_type, int wave_cycle_time, int cycle_time, int amplitude) {
  int time_now = miltime(wave_cycle_time);
  if (time_now <  cycle_time) {
    if (__wave_type == "sin") {
      return sin_wave(time_now, cycle_time, amplitude);
    } else if (__wave_type == "heart") {
      return heart_wave(time_now, cycle_time, amplitude);
    } else if (__wave_type == "triangle") {
      return triangle_wave(time_now, cycle_time, amplitude);
    } else if (__wave_type == "square") {
      return square_wave( amplitude);
    } else {
      return 0.0;
    }
  } else {
    return 0.0;
  }
}

//正弦波
double sin_wave(int Time, int cycle_time, int amplitude) {
  return (1.0 - cos(Time * 2.0 * 3.14 / cycle_time)) * amplitude / 2;
}

//心拍波形
double heart_wave(int Time, int cycle_time, int amplitude) {
  if (Time * 4 > 3 * cycle_time) {
    //Q
    return (1.6  * Time / cycle_time - 1.6) * amplitude;
  } else if (Time * 4 > 2 * cycle_time) {
    //R
    return (-5.6  * Time / cycle_time + 3.8) * amplitude;
  } else if (Time * 4 > 1 * cycle_time) {
    //R
    return (4.8 * Time / cycle_time - 1.4 ) * amplitude;
  } else {
    //S
    return -0.8 * amplitude * Time / cycle_time;
  }
}

//三角波
double triangle_wave(int time_now, int cycle_time, int amplitude)
{
  if (time_now <  cycle_time / 2) {
    return time_now * 2.0 / cycle_time * amplitude;
  } else {
    return (( cycle_time - time_now) * 2.0 /  cycle_time) * amplitude;
  }
}


//方形波
double square_wave(int __wave_aplitude)
{
  return  __wave_aplitude;
}

void disp(int mil_time, int  j) {
#ifdef debug_on
  int disp[] = {mil_time, j, int( breath_wave_amplitude * 1.1), int(breath_wave_amplitude * -0.1)};
  display_graph(disp, len(disp));
#else
#ifdef debug_wave
  int disp[] = {j, int( breath_wave_amplitude * 1.1), int(breath_wave_amplitude * -0.1)};
  display_graph(disp, len(disp));
#else
#ifdef debug_on_nograph
  int disp[] = {0, 1, 2, 3, 4};
  display_graph(disp, len(disp));
#else
  Serial.print("no display")
#endif
#endif
#endif
}

void  display_graph(int disp[], int len) {
  for (int i = 0; i < len ; i++) {
    Serial.print(disp[i]);
    Serial.print(",");
  }
  Serial.println("");

}


int miltime(int cycletime) {
  return  int((millis() - start_time) % cycletime);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

int split(String data, String *dst, char delimiter) {
  int index = 0;
  int arraySize = (sizeof(data) / sizeof((data)[0]));
  int datalength = data.length();
  for (int i = 0; i < datalength; i++) {
    char tmp = data.charAt(i);
    if ( tmp == delimiter ) {
      index++;
      if ( index > (arraySize - 1)) return -1;
    }
    else dst[index] += tmp;
  }
  return (index + 1);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void pwm_init(int __pwm_pin, int __division_ratio) {
  int tcc = 0;
  if (__pwm_pin == 5 || __pwm_pin == 6) {
    pinMode(__pwm_pin, OUTPUT);
    Serial.println("pwm pin 5 and 6 \n ");
    switch (__division_ratio) {
      case 1:
        tcc = B00000001;
        break;
      case 8:
        tcc = B00000010;
        break;
      case 64:
        tcc = B00000011;
        break;
      case 256:
        tcc = B00000100;
        break;
      case 1024:
        tcc = B00000101;
        break;
      default:
        tcc = B00000011;
        Serial.println("pwm pin 5 and 6 , i cant set this value. \n please set 1,8,64,256 or 1024.");
        break;
    }
    TCCR0B &= B11111000;
    TCCR0B |= tcc;
  }
  if (__pwm_pin == 9 || __pwm_pin == 10) {
    pinMode(__pwm_pin, OUTPUT);

    Serial.println("pwm pin 5 and 6 \n ");
    switch (__division_ratio) {
      case 1:
        tcc = B00000001;
        break;
      case 8:
        tcc = B00000010;
        break;
      case 64:
        tcc = B00000011;
        break;
      case 256:
        tcc = B00000100;
        break;
      case 1024:
        tcc = B00000101;
        break;
      default:
        tcc = B00000011;
        Serial.println("pwm pin 5 and 6 , i cant set this value. \n please set 1,8,64,256 or 1024.");
        break;
    }
    TCCR1B &= B11111000;
    TCCR1B |= tcc;
  }
  if (__pwm_pin == 3 || __pwm_pin == 11) {
    pinMode(__pwm_pin, OUTPUT);
    Serial.println("pwm pin 5 and 6 \n ");
    switch (__division_ratio) {
      case 1:
        tcc = B00000001;
        break;
      case 8:
        tcc = B00000010;
        break;
      case 32:
        tcc = B00000011;
        break;
      case 64:
        tcc = B00000100;
        break;
      case 128:
        tcc = B00000101;
        break;
      case 256:
        tcc = B00000110;
        break;
      case 1024:
        tcc = B00000111;
        break;
      default:
        tcc = B00000011;
        Serial.println("pwm pin 5 and 6 , i cant set this value. \n please set 1,8,64,256 or 1024.");
        break;
    }
    TCCR2B &= B11111000;
    TCCR2B |= tcc;
  }
}
