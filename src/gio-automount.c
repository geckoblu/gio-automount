#include <stdio.h>
#include <stdlib.h>
#include <gio/gio.h>
#include <glib.h>

static GMutex mutex;
static gboolean mounting;
static int mounting_count;

static void mount_finish(GObject *object, GAsyncResult *result, gpointer user_data) {
	GVolume *volume = G_VOLUME(object);
	GError *error = NULL;

	if (!g_volume_mount_finish(volume, result, &error)) {
		 /* ignore GIO errors handled internally */
		 if (error->domain != G_IO_ERROR || error->code != G_IO_ERROR_FAILED_HANDLED) {
			 gchar *volume_name = g_volume_get_name(volume);
			 g_print("mounting failed for %s\n", volume_name);
			 g_free(volume_name);
		 }
		 g_error_free (error);
	}

	g_mutex_lock(&mutex);
	mounting_count = mounting_count - 1;
	if (! mounting && mounting_count <= 0 && gtk_main_level() > 0) {
		gtk_main_quit();
	}
	g_mutex_unlock(&mutex);
}

void mount_volume(gpointer data, gpointer user_data) {
	GVolume * volume = (GVolume *) data;

	g_mutex_lock(&mutex);
	mounting = TRUE;
	g_mutex_unlock(&mutex);

	if (g_volume_should_automount(volume)) {
		if (g_volume_can_mount(volume)) {
			GMount *mount = g_volume_get_mount(volume);
			if (! mount ) {
				char *volume_name = g_volume_get_name(volume);
				g_print("mounting %s\n", volume_name);

				g_mutex_lock(&mutex);
				mounting_count = mounting_count + 1;
				g_mutex_unlock(&mutex);

				g_volume_mount(volume, G_MOUNT_MOUNT_NONE, NULL, NULL, mount_finish, NULL);

				g_free(volume_name);
			} else {
				g_object_unref(mount);
			}
		}
	}

	g_mutex_lock(&mutex);
	mounting = FALSE;
	g_mutex_unlock(&mutex);
}

void show_volume(gpointer data, gpointer user_data) {
	GVolume * volume = (GVolume *) data;
	char *volume_name = g_volume_get_name(volume);

	char *should_automount = "NOT TO MOUNT";
	if (g_volume_should_automount(volume)) {
		should_automount = "TO AUTOMOUNT";
	}

	char *mount_name = "";
	GMount *mount = g_volume_get_mount(volume);
	if (mount) {
		mount_name = g_mount_get_name(mount);
	}


	g_print("Volume %s\t %s %s \n", volume_name, should_automount, mount_name);
	g_free(volume_name);
	if (mount) {
		g_free(mount_name);
	}
}

int main(int argc, char** argv) {
	g_print("gio-automount start.\n");

	g_type_init();
	gtk_init(&argc, &argv);

	GVolumeMonitor *monitor = g_volume_monitor_get();

	GList *volumes = g_volume_monitor_get_volumes(G_VOLUME_MONITOR(monitor));
	g_list_foreach(volumes, mount_volume, NULL);
	g_list_free(volumes);

	if (mounting_count > 0) {
		gtk_main();
	}

	g_print("gio-automount end.\n");
	return (EXIT_SUCCESS);
}
