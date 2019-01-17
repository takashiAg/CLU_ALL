#define len(ARR) (sizeof(ARR)/sizeof(ARR[0]))
#define pwmpin 11

#define default_heart_wave_cycletime 300
#define default_heart_cycletime 1000
#define default_heart_wave_amplitude 256
#define default_breath_cycletime 4000
#define default_breath_wave_amplitude 1024

#define debug_on
//#define debug_on_nograph
//#define debug_wave


/*
  unsigned long parameter[][6] = {
  {0, 300, 1000, 50, 4000, 200},
  {1, 300, 1000, 40, 3500, 100},
  {2, 300, 1000, 50, 3000, 200},
  {60, 300, 1000, 40, 3000, 40}
  };
  //*/
//*
unsigned long parameter[][6] = {
  {0, 300, 1000, 50,  4000, 200}
};
//*/
int parameters_counter = 0;

int mil_time = 0;

unsigned long start_time;

String wavetype = "sin";

int heart_wave_cycletime = default_heart_wave_cycletime;
int heart_wave_amplitude = default_heart_wave_amplitude;
int heart_cycletime = default_heart_cycletime;
int breath_wave_amplitude = default_breath_wave_amplitude;
int breath_cycletime = default_breath_cycletime;

void setup() {
  pwm_init(pwmpin, 1);
  Serial.begin(115200);
  start_time = millis();
}

void loop() {

  //
  update_parameter();

  double heart_wave = wave(wavetype, heart_cycletime, heart_wave_cycletime, heart_wave_amplitude);

  double breath_wave = wave("sin", breath_cycletime, breath_cycletime, breath_wave_amplitude);

  analogWrite(pwmpin, int(breath_wave + heart_wave));

  disp(mil_time, breath_wave + heart_wave);

  delay(5);

}

void update_parameter() {

  if (len(parameter) <= parameters_counter) {
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

//wave
double wave(String __wave_type, int wave_cycle_time, int cycle_time, int amplitude) {
  int time_now = miltime(wave_cycle_time);
  if (time_now <  cycle_time) {
    if (__wave_type == "sin") {
      return sin_wave(time_now, cycle_time, amplitude);
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
  int disp[] = {j, int( breath_wave_amplitued * 1.1), int(breath_wave_amplitude * -0.1)};
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
