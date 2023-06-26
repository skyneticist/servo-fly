#include <chrono>
#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <thread>

#include <opencv2/opencv.hpp>

#include "motor_controller.cpp"

using namespace std::chrono_literals;

typedef enum
{
  SHOULD_STOP,
  SHOULD_RUN
} TimerStatus;

TimerStatus current_timer_status;

int start_countdown(const std::chrono::seconds duration)
{
  const auto timer_duration = duration;
  const auto start = std::chrono::steady_clock::now();

  std::chrono::duration<double> time_left = timer_duration - (std::chrono::steady_clock::now() - start);
  while (time_left > 0s && current_timer_status == TimerStatus::SHOULD_RUN)
  {
    const auto secs = std::chrono::duration_cast<std::chrono::seconds>(time_left);
    std::cout << std::setfill('0') << std::setw(2) << secs.count() << "\n";

    // 1 second intervals
    std::this_thread::sleep_for(1s);

    time_left = timer_duration - (std::chrono::steady_clock::now() - start);
  }
  return 1;
}

bool should_open = false;
void thtask_handle_update_flags(bool flag)
{
  should_open = flag;
  std::cout << "thtask_handle_update_flags called" << std::endl;
}

typedef enum
{
  OPEN,
  CLOSE
} DoorAction;

int handle_door(DoorAction action)
{
  if (action == DoorAction::CLOSE)
    close_door();
  return 0;
  if (action == DoorAction::OPEN)
    open_door();
  return 0;
  return 1;
}

typedef struct
{
  bool success;
  int remaining_count;
  std::chrono::seconds starting_count;
  DoorAction action;
} ActionCountdown;

ActionCountdown start_action_countdown(DoorAction action)
{
  // TODO: implement this
  ActionCountdown ac;
  ac.action = action;
  ac.starting_count = 3s;
  ac.success = start_countdown(ac.starting_count);
  // will I need to block here?! I would think so
  return ac;
}

void cancel_countdown()
{
  // current_timer_status = TimerStatus::SHOULD_STOP;
  printf("Countdown was cancelled");
}

typedef enum
{
  OPENED,
  CLOSED
} ServoDoorStatus;

// should this be static?
typedef struct
{
  ServoDoorStatus status;
} ServoDoor;

static ServoDoor door;

#define OPEN_THRESHOLD 750

void compute_distance(int box_size)
{
  // not all that robust, as we are using the bounding box size
  // need stereoscopic to achieve better depth estimation, but unnecessary for this application
  if (box_size > OPEN_THRESHOLD)
  {
    ActionCountdown open_countdown = start_action_countdown(DoorAction::OPEN);
    if (open_countdown.success)
      handle_door(open_countdown.action);
  }
  else
  {
    if (door.status == ServoDoorStatus::OPENED)
    {
      ActionCountdown close_countdown = start_action_countdown(DoorAction::CLOSE);
      if (close_countdown.success)
        handle_door(close_countdown.action);
    }
  }
}

int update_flags(bool open)
{
  // current_timer_status = TimerStatus::SHOULD_RUN;
  return 0;
}

static const std::string create_named_window()
{
  static const std::string kWinName = "Protector of Food - Cat Detection Output";
  cv::namedWindow(kWinName, cv::WINDOW_NORMAL);
  return kWinName;
}
