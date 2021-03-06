#include "notification.hpp"

namespace radiotray
{
Notification::Notification(const char* app_name, std::shared_ptr<Config>& cfg)
    : app_name(app_name)
    , config(cfg)
{
    logo_path = std::string(kImagePath) + std::string(kAppIcon);
}

Notification::~Notification()
{
    if (notification != nullptr) {
        g_object_unref(G_OBJECT(notification));
    }

    if (notify_is_initted() != 0) {
        notify_uninit();
    }
}

bool
Notification::init()
{
    auto initialized = (notify_init(app_name.c_str()) != 0);
    logo = Gdk::Pixbuf::create_from_file(logo_path);

    return (initialized and bool(logo));
}

void
Notification::on_broadcast_info_changed_signal(const Glib::ustring& station, Glib::ustring info)
{
    if (notify_is_initted() == 0) {
        LOG(WARNING) << "libnotify is not initialized!";
        return;
    }

    if (not config->has_notifications()) {
        return;
    }

    std::stringstream ss;

    ss << notify_get_app_name() << " - " << station;
    auto summary = Glib::ustring(ss.str());

    const auto& text = info;
    if ((not last_text.empty()) and text == last_text) {
        return;
    }

    if (notification == nullptr) {
        notification = notify_notification_new(summary.c_str(), text.c_str(), nullptr);
        if (notification != nullptr) {
            notify_notification_set_timeout(notification, NOTIFY_EXPIRES_DEFAULT);
            notify_notification_set_urgency(notification, NOTIFY_URGENCY_LOW);
            notify_notification_set_icon_from_pixbuf(notification, logo->gobj());
            notify_notification_show(notification, nullptr);
        }
    } else {
        auto rc = notify_notification_update(notification, summary.c_str(), text.c_str(), nullptr);
        if (rc != 0) {
            notify_notification_set_icon_from_pixbuf(notification, logo->gobj());
            notify_notification_show(notification, nullptr);
        }
    }

    last_text = text;
}

} // namespace radiotray
