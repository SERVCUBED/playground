#include <gtkmm.h>
#include <iostream>
#include <thread>
#include <fcntl.h>

#if CMAKE_BUILD_TYPE==Debug
#define DEBUG_MSG(str) std::cout << str << std::endl
#else
#define DEBUG_MSG(str) 0
#endif

char pipename[100];
int pipefd = -1;

class mainwindow : public Gtk::Window {
 public:
  mainwindow (int mnum, const char *text, int height)
  {
    set_title (text);
    set_type_hint (Gdk::WINDOW_TYPE_HINT_DOCK);
    set_app_paintable (true);
    set_size_request (200, height);

    Glib::RefPtr<Gdk::Screen> screen = Gdk::Screen::get_default ();
    if (mnum >= screen->get_n_monitors ())
      {
        std::cerr << "Monitor number " << mnum << " is out of range." << std::endl;
        exit (EXIT_FAILURE);
      }

    Gdk::Rectangle rect;
    screen->get_monitor_geometry (mnum, rect);
    move (rect.get_x (), rect.get_y ());
    resize (rect.get_width (), height);

    set_decorated (false);
    set_has_resize_grip (false);
    signal_key_press_event ().connect (sigc::mem_fun (*this, &mainwindow::on_key));

    DEBUG_MSG ("Message: " << text);
    label.set_text (text);
    add (label);
    set_text_size (height);

    // TODO: Set window background colour to yellow
//    Glib::RefPtr<Gtk::StyleProvider> styleProvider = Gtk::StyleProvider()
//    styleContext->render_background ();
//    get_style_context ()->add_provider (, GTK_STYLE_PROVIDER_PRIORITY_USER)
    show_all ();
    set_opacity (0.5);
  }

 protected:

  bool on_key (GdkEventKey *key_event)
  {
    DEBUG_MSG("key:" << key_event->string << ": " << key_event->keyval);

    if (key_event->keyval == GDK_KEY_Escape)
      {
        close ();
        return true;
      }

    return false;
  }

 private:

  void set_text_size (int h)
  {
    Pango::AttrList pg_attr_list;
    Pango::Attribute pg_attr = Pango::Attribute::create_attr_size_absolute ((int) (h * 0.7 * PANGO_SCALE));
    pg_attr_list.change (pg_attr);

    label.set_attributes (pg_attr_list);
  }

  Gtk::Label label;
};

static
void handle_timeout (Glib::RefPtr<Gtk::Application> * app)
{
   g_info ("Quitting due to local timeout");
   app->get()->quit();
}

void
onexit ()
{
  if (pipefd != -1)
    {
      close(pipefd);
      unlink(pipename);
    }
}

int
main (int argc,
      char **argv)
{
  int mnum = 0, texti = 0, height = 200;
  guint timeout = 0;
  pipename[0] = 0u;
  pid_t pid;
  if (argc < 2)
    {
exit_argerr:
      std::cerr << "Usage: " << argv[0] << " [-n=<Monitor number=0>] [-h=<Height=200>] [-t=<Timeout (milliseconds)=0 (Unset)>] [-b[=print]] [-l] [-r=PID] <Message>" << std::endl;
      std::cerr << " Required arguments:" << std::endl;
      std::cerr << " \t<Message>\tThe text to display. Must be quoted as one single argument." << std::endl;
      std::cerr << " Optional arguments:" << std::endl;
      std::cerr << " \t-n=<Monitor number>\tSet monitor for the bar to appear on. Default: 0 (Primary monitor)." << std::endl;
      std::cerr << " \t-h=<Height>\tSet the height of the bar in pixels." << std::endl;
      std::cerr << " \t-t=<Timeout>\tSet a timeout for the bar in milliseconds, before it exits. The default value of zero sets the bar to never expire." << std::endl;
      std::cerr << " \t-b[=print]\tFork to background, and, optionally, if b=print, output the process ID of the bar so that it can be killed by a supervising script or process." << std::endl;
      std::cerr << " \t-l\tListen for replacement messages and other parameters. Implies -b=print to output a new process ID to use in replacement command." << std::endl;
      std::cerr << " \t-r=PID\ttSend a command to a running bar with a process ID of PID,  setting a message. If the bar with the specified PID does not exist, show a new bar." << std::endl;
      return EXIT_FAILURE;
    }
  for (int i = 1; i < argc; ++i)
    {
      if (argv[i][0] == '-')
        {
          switch (argv[i][1])
            {
              case 'n':
                DEBUG_MSG ("Monitor number: " << (argv[i] + 3));
                mnum = std::stoi (argv[i] + 3);
                break;
              case 'h':
                DEBUG_MSG ("Height: " << (argv[i] + 3));
                height = std::stoi (argv[i] + 3);
                break;
              case 't':
                DEBUG_MSG ("Timeout: " << (argv[i] + 3) << "ms");
                timeout = std::stoi (argv[i] + 3);
                break;
              case 'l':
              case 'b':
                DEBUG_MSG ("Forking");
                pid = fork();
                if (pid == -1)
                  g_error ("Error forking to background");
                if (pid != 0)
                  {
                    if ((*(argv[i] + 2) == '=' && *(argv[i] + 3) == 'p') || argv[i][1] == 'l')
                      {
                        fprintf (stdout, "%d\n", pid);
                      }
                    return EXIT_SUCCESS;
                  }
                atexit(onexit);
                if (argv[i][1] == 'b')
                  break;
              case 'r':
                char *xdgrd;
                xdgrd = getenv("XDG_RUNTIME_DIR");
                if (argv[i][1] == 'r')
                  {
                    if (argv[i][2] != '=')
                      goto exit_argerr;
                    pid = std::stoi (argv[i] + 3);
                  }
                else if (pid == 0)
                    pid = getpid();
                if (xdgrd == nullptr || strnlen(xdgrd, 5) < 5)
                  {
                    DEBUG_MSG("Failed to get XDG_RUNTIME_DIR from environment");
                    uid_t uid;
                    uid = getuid();
                    sprintf(pipename, "/run/user/%d/warn-bar-%d.sock", uid, pid);
                  }
                else
                    sprintf(pipename, "%s/warn-bar-%d.sock", xdgrd, pid);
                DEBUG_MSG ("Using socket: " << pipename);
                pipename[98] = 0u;
                pipename[99] = argv[i][1];
                if (argv[i][1] == 'r')
                  {
                    pipefd = open (pipename, O_WRONLY | O_NONBLOCK);
                    if (pipefd == -1)
                      {
                        DEBUG_MSG ("Failed to open pipe for writing. Starting fresh.");
                      }
                  }
                else
                  {
                    pipefd = mkfifo(pipename, 0666);
                    if (pipefd == -1)
                      {
                        DEBUG_MSG ("Failed to open pipe for writing");
                        goto exit_argerr;
                      }
                  }
                break;
              default:
                g_error("Unknown argument -%c\n", argv[i][1]);
                goto exit_argerr;
            }
        }
      else
        texti = i;
    }

  if (pipename[0]  != 0u && pipename[99] == 'r')
    {
      // TODO write to pipe
      if (pipefd == -1)
        {
          DEBUG_MSG ("Error writing to pipe");
          goto exit_argerr;
        }
      else
        {
            char buf[strlen(argv[texti])];
        }
      // TODO should continue to display new bar?
      return EXIT_SUCCESS;
    }

  int argc1 = 1;
  Glib::RefPtr<Gtk::Application> app = Gtk::Application::create (argc1, argv, "org.servc.warnbar");
  mainwindow window(mnum, argv[texti], height);
  if (timeout > 0)
    g_timeout_add(timeout, G_SOURCE_FUNC (&handle_timeout), &app);;
  return app->run (window);
}
