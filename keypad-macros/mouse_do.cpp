#include <gtkmm-3.0/gtkmm.h>
#include <iostream>
#include <thread>

extern "C" {
#include <xcb/xtest.h>
#include <xcb/xcb_aux.h>
}

#if CMAKE_BUILD_TYPE==Debug
#define DEBUG_MSG(str) std::cout << str << std::endl
#else
#define DEBUG_MSG(str) 0
#endif

class mainwindow : public Gtk::Window {
 public:
  int x, y, w, h;
  int8_t performClick = -1;

  mainwindow ()
  {
    set_title ("Mouse Do");
    set_type_hint (Gdk::WINDOW_TYPE_HINT_SPLASHSCREEN);
    set_app_paintable (true);
    set_size_request (100, 100);

    Glib::RefPtr<Gdk::Window> active = Gdk::Screen::get_default ()->get_active_window ();
    w = active->get_width ();
    h = active->get_height ();
    active->get_origin (x, y);
    resize (w, h);
    move (x, y);

    set_decorated (false);
    set_has_resize_grip (false);
    signal_key_press_event ().connect (sigc::mem_fun (*this, &mainwindow::on_key));

    grid.set_column_homogeneous (true);
    grid.set_row_homogeneous (true);
    add (grid);

    for (int i = 0; i < 9; ++i)
      {
        const char str[] = {(char) (i + 49), '\0'};
        int col = i % 3;
        labels[i].set_text (str);
        grid.attach (labels[i], col, 2 - (int) ((i - col) / 3), 1, 1);
      }
    set_text_size ();

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
    if (this->performClick == 0) {
        if (key_event->keyval >= GDK_KEY_KP_0 && key_event->keyval <= GDK_KEY_KP_9)
          {
            this->performClick = key_event->keyval - GDK_KEY_KP_0 + XCB_BUTTON_INDEX_ANY;
            DEBUG_MSG("Using button: " << this->performClick);
            close ();
            return true;
          }
        this->performClick = -1;
        this->set_display_choose_button (false);
        return false;
    }
    if (key_event->keyval == GDK_KEY_KP_0)
      {
        DEBUG_MSG ("Select button");
        this->performClick = 0;
        this->set_display_choose_button (true);
        return true;
      }
    if (key_event->keyval >= GDK_KEY_KP_1 && key_event->keyval <= GDK_KEY_KP_9)
      {
        DEBUG_MSG("handled");
        int i = std::stoi (key_event->string) - 1;
        unsigned int col = i % 3;
        unsigned int row = 2 - (i - col) / 3;
        DEBUG_MSG("selected: row " << row << " col " << col);

        if ((col ^ 1) || (row ^ 1))
          {
            DEBUG_MSG ("Normal size");
            x += w * col / 3;
            y += h * row / 3;
            w /= 3;
            h /= 3;
          }
        else
          {
            DEBUG_MSG ("Halving");
            x += (w * col / 3) - (w / 12);
            y += (h * row / 3) - (h / 12);
            w /= 2;
            h /= 2;
          }

        if (h < 100 || w < 100)
            goto PERFORMCLICK;

        gdk_window_move_resize (GDK_WINDOW (key_event->window), x, y, w, h);
        set_text_size ();
        return true;
      }
      if (key_event->keyval == GDK_KEY_KP_Decimal)
        goto PERFORMCLICK;

    return false;

PERFORMCLICK:
    // Input device should not be controlled inside a key_press event,
    // Set signal for caller instead.
    performClick = XCB_BUTTON_INDEX_1;
    close ();
    return true;
  }

  void set_text_size ()
  {
    Pango::AttrList pg_attr_list;
    Pango::Attribute pg_attr = Pango::Attribute::create_attr_size_absolute ((int) (h * 0.2 * PANGO_SCALE));
    pg_attr_list.change (pg_attr);

    for (int i = 8; i >= 0; --i)
      labels[i].set_attributes (pg_attr_list);
  }

  inline void set_display_choose_button(const bool enabled)
  {
    if (enabled)
      this->labels[4].set_text ("*");
    else
      this->labels[4].set_text ("5");
    for (int i = 0; i < 9; ++i)
      {
        if (__glibc_unlikely (i != 4))
          this->labels[i].set_opacity(1 - 0.8 * enabled);
      }
  }

  Gtk::Grid grid;
  Gtk::Label labels[9];
};

int
main (int argc,
      char **argv)
{
  Glib::RefPtr<Gtk::Application> app = Gtk::Application::create (argc, argv, "org.servc.mousedo");
  mainwindow window;
  int status = app->run (window);

  if (window.performClick != -1)
    {
      // TODO mouse click in centre of region
      DEBUG_MSG("Clicking");
      int default_screen;
      xcb_connection_t *dpy = xcb_connect (NULL, &default_screen);
      xcb_test_fake_input (dpy, XCB_MOTION_NOTIFY, false, XCB_CURRENT_TIME, XCB_NONE, (int16_t) (
          window.x + window.w / 2), (int16_t) (window.y + window.h / 2), 0);
      if (window.performClick > XCB_BUTTON_INDEX_ANY)
        {
          xcb_test_fake_input (dpy, XCB_BUTTON_PRESS, window.performClick, XCB_CURRENT_TIME, XCB_NONE, 0, 0, 0);
          std::this_thread::sleep_for (std::chrono::operator ""ms (50));
          xcb_test_fake_input (dpy, XCB_BUTTON_RELEASE, window.performClick, XCB_CURRENT_TIME, XCB_NONE, 0, 0, 0);
        }
      xcb_aux_sync (dpy);
      xcb_disconnect (dpy);
    }

  return status;
}
