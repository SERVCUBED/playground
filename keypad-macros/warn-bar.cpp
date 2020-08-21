#include <gtkmm-3.0/gtkmm.h>
#include <iostream>
#include <thread>

#if CMAKE_BUILD_TYPE==Debug
#define DEBUG_MSG(str) std::cout << str << std::endl
#else
#define DEBUG_MSG(str) 0
#endif

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
    move(rect.get_x (), rect.get_y ());
    resize(rect.get_width (), height);

    set_decorated (false);
    set_has_resize_grip (false);
    signal_key_press_event ().connect (sigc::mem_fun (*this, &mainwindow::on_key));

    DEBUG_MSG ("Message: " << text);
    label.set_text (text);
    add(label);
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

  void set_text_size (int h)
  {
    Pango::AttrList pg_attr_list;
    Pango::Attribute pg_attr = Pango::Attribute::create_attr_size_absolute ((int) (h * 0.7 * PANGO_SCALE));
    pg_attr_list.change (pg_attr);

    label.set_attributes (pg_attr_list);
  }

  Gtk::Label label;
};

int
main (int argc,
      char **argv)
{
  int mnum = 0, texti = 0, height = 200;
  if (argc < 2)
    {
      std::cerr << "Usage: " << argv[0] << " [-n=<Monitor number=0>] [-h=<Height=200>] <Message>" << std::endl;
      return EXIT_FAILURE;
    }
  for (int i = 1; i < argc; ++i)
    {
      if (argv[i][0] == '-')
        {
          if (argv[i][1] == 'n' && argv[i][2] == '=')
            {
              DEBUG_MSG ("Monitor number: " << (argv[i] + 3));
              mnum = std::stoi (argv[i] + 3);
            }
          else if (argv[i][1] == 'h' && argv[i][2] == '=')
            {
              DEBUG_MSG ("Height: " << (argv[i] + 3));
              height = std::stoi (argv[i] + 3);
            }
        }
      else
        texti = i;
    }

  int argc1 = 1;
  Glib::RefPtr<Gtk::Application> app = Gtk::Application::create (argc1, argv, "org.servc.warnbar");
  mainwindow window(mnum, argv[texti], height);
  return app->run (window);
}
