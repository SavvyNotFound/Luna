#include <lunapch.h>
#include "Luna/Utils/PlatformUtils.h"
#include "Luna/Core/Application.h"

#include <gio/gio.h>

namespace Luna {

    struct PortalResponseData {
        GMainLoop* loop = nullptr;
        std::string selected_file;
    };

    // D-Bus signal callback for the portal response
    static void on_portal_response(GDBusConnection* connection,
                                   const gchar* sender_name,
                                   const gchar* object_path,
                                   const gchar* interface_name,
                                   const gchar* signal_name,
                                   GVariant* parameters,
                                   gpointer user_data)
    {
        PortalResponseData* data = static_cast<PortalResponseData*>(user_data);
        
        guint32 response = 0;
        GVariant* results = nullptr;
        
        // Portal Response signature: (u a{sv}) -> response code (0 = success/accepted), results dict
        g_variant_get(parameters, "(u@a{sv})", &response, &results);
        
        if (response == 0) // 0 means user accepted the dialog
        {
            GVariantIter iter;
            g_variant_iter_init(&iter, results);
            gchar* key = nullptr;
            GVariant* value = nullptr;
            
            while (g_variant_iter_loop(&iter, "{sv}", &key, &value))
            {
                if (g_strcmp0(key, "uris") == 0)
                {
                    GVariantIter uri_iter;
                    g_variant_iter_init(&uri_iter, value);
                    gchar* uri = nullptr;
                    if (g_variant_iter_loop(&uri_iter, "s", &uri))
                    {
                        // Convert file:// URI returned by portal into a standard file system path
                        GFile* file = g_file_new_for_uri(uri);
                        gchar* path = g_file_get_path(file);
                        if (path)
                        {
                            data->selected_file = path;
                            g_free(path);
                        }
                        g_object_unref(file);
                    }
                    break;
                }
            }
        }
        
        if (results)
            g_variant_unref(results);
            
        if (data->loop)
            g_main_loop_quit(data->loop);
    }

    std::string FileDialogs::OpenFile(const char* filter)
    {
        GError* err = NULL;
        GDBusConnection* connection = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, &err);
        if (!connection)
        {
            if (err)
            {
                LUNA_CORE_CRITICAL("Failed to establish D-Bus connection: \n{0}", err->message);
                g_error_free(err);
            }
            return std::string();
        }

        // Generate a unique token handle for this request
        static int request_counter = 0;
        std::string handle_token = "luna_open_" + std::to_string(++request_counter);

        GVariantBuilder opt_builder;
        g_variant_builder_init(&opt_builder, G_VARIANT_TYPE("a{sv}"));
        g_variant_builder_add(&opt_builder, "{sv}", "handle_token", g_variant_new_string(handle_token.c_str()));
        g_variant_builder_add(&opt_builder, "{sv}", "title", g_variant_new_string("Open File"));

        // Call org.freedesktop.portal.FileChooser.OpenFile synchronously to get the request object path
        GVariant* result = g_dbus_connection_call_sync(
            connection,
            "org.freedesktop.portal.Desktop",
            "/org/freedesktop/portal/desktop",
            "org.freedesktop.portal.FileChooser",
            "OpenFile",
            g_variant_new("(ss@a{sv})", "", "Open File", g_variant_builder_end(&opt_builder)),
            G_VARIANT_TYPE("(o)"),
            G_DBUS_CALL_FLAGS_NONE,
            -1,
            NULL,
            &err
        );

        if (!result)
        {
            if (err)
            {
                LUNA_CORE_CRITICAL("Failed to call OpenFile portal: \n{0}", err->message);
                g_error_free(err);
            }
            g_object_unref(connection);
            return std::string();
        }

        gchar* request_path = nullptr;
        g_variant_get(result, "(o)", &request_path);
        g_variant_unref(result);

        PortalResponseData resp_data;
        resp_data.loop = g_main_loop_new(NULL, FALSE);

        // Subscribe to the Response signal on the specific request object path returned by the portal
        guint subscription_id = g_dbus_connection_signal_subscribe(
            connection,
            "org.freedesktop.portal.Desktop",
            "org.freedesktop.portal.Request",
            "Response",
            request_path,
            NULL,
            G_DBUS_SIGNAL_FLAGS_NO_MATCH_RULE,
            on_portal_response,
            &resp_data,
            NULL
        );

        // Block execution synchronously until the user interacts with the dialog and the signal fires
        g_main_loop_run(resp_data.loop);

        // Cleanup resources
        g_dbus_connection_signal_unsubscribe(connection, subscription_id);
        g_main_loop_unref(resp_data.loop);
        g_free(request_path);
        g_object_unref(connection);

        return resp_data.selected_file;
    }

    std::string FileDialogs::SaveFile(const char* filter)
    {
        GError* err = NULL;
        GDBusConnection* connection = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, &err);
        if (!connection)
        {
            if (err)
            {
                LUNA_CORE_CRITICAL("Failed to establish D-Bus connection: \n{0}", err->message);
                g_error_free(err);
            }
            return std::string();
        }

        static int request_counter = 0;
        std::string handle_token = "luna_save_" + std::to_string(++request_counter);

        GVariantBuilder opt_builder;
        g_variant_builder_init(&opt_builder, G_VARIANT_TYPE("a{sv}"));
        g_variant_builder_add(&opt_builder, "{sv}", "handle_token", g_variant_new_string(handle_token.c_str()));
        g_variant_builder_add(&opt_builder, "{sv}", "title", g_variant_new_string("Save File"));

        GVariant* result = g_dbus_connection_call_sync(
            connection,
            "org.freedesktop.portal.Desktop",
            "/org/freedesktop/portal/desktop",
            "org.freedesktop.portal.FileChooser",
            "SaveFile",
            g_variant_new("(ss@a{sv})", "", "Save File", g_variant_builder_end(&opt_builder)),
            G_VARIANT_TYPE("(o)"),
            G_DBUS_CALL_FLAGS_NONE,
            -1,
            NULL,
            &err
        );

        if (!result)
        {
            if (err)
            {
                LUNA_CORE_CRITICAL("Failed to call SaveFile portal: \n{0}", err->message);
                g_error_free(err);
            }
            g_object_unref(connection);
            return std::string();
        }

        gchar* request_path = nullptr;
        g_variant_get(result, "(o)", &request_path);
        g_variant_unref(result);

        PortalResponseData resp_data;
        resp_data.loop = g_main_loop_new(NULL, FALSE);

        guint subscription_id = g_dbus_connection_signal_subscribe(
            connection,
            "org.freedesktop.portal.Desktop",
            "org.freedesktop.portal.Request",
            "Response",
            request_path,
            NULL,
            G_DBUS_SIGNAL_FLAGS_NO_MATCH_RULE,
            on_portal_response,
            &resp_data,
            NULL
        );

        g_main_loop_run(resp_data.loop);

        g_dbus_connection_signal_unsubscribe(connection, subscription_id);
        g_main_loop_unref(resp_data.loop);
        g_free(request_path);
        g_object_unref(connection);

        return resp_data.selected_file;
    }

}
