// #include <pigpio.h>

#define STEPPER_GPIO 15
#define STEPPER_DIR_GPIO 13
#define STATIC_DEVICE_STEPS 45

typedef enum
{
  COUNTER_CLOCKWISE,
  CLOCKWISE
} STEPDIRECTION;

void step_motor(STEPDIRECTION direction, int steps)
{
  // Set direction of steps
  // gpioWrite(STEPPER_DIR_GPIO, direction);

  // step the stepper motor
  for (volatile int i = 0; i < steps - 1; i++)
  {
    // gpioWrite(STEPPER_GPIO, 1);
    // slight delay
    // gpioWrite(STEPPER_GPIO, 0);
  }
}

int open_door()
{
  // door.status = ServoDoorStatus::OPENED;
  step_motor(STEPDIRECTION::CLOCKWISE, STATIC_DEVICE_STEPS);
  return 0;
}

int close_door()
{
  // door.status = ServoDoorStatus::CLOSED;
  step_motor(STEPDIRECTION::COUNTER_CLOCKWISE, STATIC_DEVICE_STEPS);
  return 0;
}
