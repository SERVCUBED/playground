#include <libnotify/notify.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>

const char *descriptors[2] = { "CPU", "Memory" };

const char *levelDescriptors[4] = { "Low", "Medium", "High", "Critical" };

//                    Current Medium  High  Critical
int levels[2][4] = {{ 0,      2,      4,    6        },  // levels[0] CPU
                    { 0,      55,     85,   92       }}; // levels[1] Memory

NotifyNotification *notification = nullptr;
const char *notifyText = "CPU Usage at    ,   % memory used";
const char *notifyTitle = "Resource alert:          ";

void get_mem_tokens (unsigned long (&tokenVals)[2])
{
//  unsigned long tokenVals[] = {0, 0};
  const char *tokens[2] = { "MemTotal:", "MemAvailable:" };

  std::string token;
  std::ifstream file("/proc/meminfo");
  while(file >> token) {
      for (int i = 0; i < 2; ++i)
        {
          if (token == tokens[i])
            {
              unsigned long mem;
              if (file >> mem)
                  tokenVals[i] = mem;
              else
                tokenVals[i] = 0;
            }
        }
      file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

void getValues (double (&values)[2])
{
  double load[3];
  unsigned long tokenVals[2] = {0, 0};

  get_mem_tokens (tokenVals);

  getloadavg (load, 3);
  values[0] = load[2];
  values[1] = 100 - 100.0 * tokenVals[1]/tokenVals[0];

  std::cout << "load: " << values[0] << " mem: " << values[1] << std::endl;
}

void getInputValues (double (&values)[2])
{
  for (int i = 0; i < 2; ++i)
    {
      std::string val;
      std::cout << "Enter value for " << descriptors[i] << ": " << std::endl;
      std::getline (std::cin, val);
      values[i] = std::stoi (val);
    }

  std::cout << "load: " << values[0] << " mem: " << values[1] << std::endl;
}

NotifyUrgency getNotifyUrgency (int maxLevel) {
  switch (maxLevel)
  {
      case 0:
        return NOTIFY_URGENCY_LOW;
      case 1:
        return NOTIFY_URGENCY_NORMAL;
      case 2:
        return NOTIFY_URGENCY_NORMAL;
      default:
        return NOTIFY_URGENCY_CRITICAL;
  }
}

int main (int argc, char *argv[])
{
  bool isDebug = argc > 1 && strcmp (argv[1], "debug") == 0;

  notify_init ("notify-usage");

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
  while (true)
    {
      double values[] = {0, 0};
      int newLevels[] = {0, 0};
      bool shouldWarn[] = {false, false};

      if (isDebug)
        getInputValues (values);
      else
        getValues (values);

      for (int i = 0; i < 2; ++i)
        {
          for (int j = 1; j < 4; ++j)
            if (values[i] >= levels[i][j])
              newLevels[i] = j;

          if (newLevels[i] > levels[i][0] || newLevels[i] == 3)
            shouldWarn[i] = true;
        }


      int highestLevel = std::max (newLevels[0], newLevels[1]);
      std::cout << "Highest level: " << highestLevel << std::endl;

      if (shouldWarn[0] || shouldWarn[1])
        {
          auto *text = (char *)malloc (strlen (notifyText) + 1);
          std::cout << " origtext: " << notifyText << std::endl;
          strcpy (text, notifyText);
          std::cout << " text: " << text << std::endl;
          if (shouldWarn[1])
            {
              std::string v = std::to_string (int(values[1]));
              for (int i = 0; i < v.length (); i++)
                {
                  text[18 + int(values[1] < 10) + i] = v[i];
                }
            }
          else
            text[16] = 0;

          std::cout << " text: " << text << std::endl;
          if (shouldWarn[0])
            {
              std::string v = std::to_string (values[0]);
              for (int i = 0; i < std::min(static_cast<int> (v.length ()), 3); ++i)
                {
                  text[13 + i] = v[i];
                }
            }
          else
            text += 18;

          std::cout << " text: " << text << std::endl;

          //auto *title = (char *)malloc (strlen (notifyTitle) + 1);
          char *title = strdup (notifyTitle);
          strcpy (title + 16, levelDescriptors[highestLevel]);

          if (notification == nullptr)
              notification = notify_notification_new (title, text, "dialog-warning");
          else
            notify_notification_update (notification, title, text, "dialog-warning");

          notify_notification_set_urgency (notification, getNotifyUrgency (highestLevel));
          notify_notification_show (notification, nullptr);
        }
      else if (notification != nullptr)
        notify_notification_close (notification, nullptr);

      // Set for next time
      for (int i = 0; i < 2; ++i)
        levels[i][0] = newLevels[i];

      if (!isDebug)
        std::this_thread::sleep_for (std::chrono_literals::operator""min (2));
    }
#pragma clang diagnostic pop
  // TODO handle signal
  notify_uninit ();
}