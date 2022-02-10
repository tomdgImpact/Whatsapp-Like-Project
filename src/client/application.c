#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <err.h>

#include <gtk/gtk.h>

#include "application.h"
#include "networking.h"
#include "message_analysis.h"


#define BUFFER_SIZE 64



GMutex mutex;



// ===========================================
//                    MAIN
// ===========================================

int main()
{
    gtk_init(NULL, NULL);

    // VARIABLES
    // ========================================================

    GtkBuilder          *builder;

    GObject             *mainWindow;
    GObject             *quitButton;
    GObject             *leaveButton;
    GObject             *sendButton;
    GObject             *inputField;
    GObject             *outputField;
    GObject             *scrollWindow;
    int                 to_server_fd;


    // The UI objects
    UI_Object_Set       *ui_objects;

    // Thread related.
    GThread             *recv_handler __attribute__((unused)); // The thread.
    Thread_Arguments    *thread_args;


    // ========================================================


    // Initialize the mutex.
    g_mutex_init(&mutex);


    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "interface.glade", NULL);


    mainWindow   = gtk_builder_get_object(builder, "mainWindow");
    quitButton   = gtk_builder_get_object(builder, "quitButton");
    leaveButton  = gtk_builder_get_object(builder, "leaveButton");
    sendButton   = gtk_builder_get_object(builder, "sendButton");
    inputField   = gtk_builder_get_object(builder, "inputField");
    outputField  = gtk_builder_get_object(builder, "outputField");
    scrollWindow = gtk_builder_get_object(builder, "scrollWindow");


    g_signal_connect(mainWindow, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(quitButton, "clicked", G_CALLBACK(gtk_main_quit), NULL);


    to_server_fd = establish_connection();


    // Structure UI_Object_Set to access all elements of the display.
    ui_objects = malloc(sizeof(UI_Object_Set));
    ui_objects->mainWindow      = mainWindow;
    ui_objects->quitButton      = quitButton;
    ui_objects->leaveButton     = leaveButton;
    ui_objects->sendButton      = sendButton;
    ui_objects->inputField      = inputField;
    ui_objects->outputField     = outputField;
    ui_objects->scrollWindow    = scrollWindow;
    ui_objects->fd              = to_server_fd;



    g_signal_connect(sendButton, "clicked", G_CALLBACK(on_send), ui_objects);



    // Structure Thread_Arguments for the thread function.
    thread_args = malloc(sizeof(Thread_Arguments));
    thread_args->data               = (gpointer)ui_objects;
    thread_args->file_descriptor    = to_server_fd;


    // Creating the thread.
    recv_handler = g_thread_new("recv_handler",
        (GThreadFunc) read_from_server, (gpointer)thread_args);


    // Main loop.
    gtk_main();


    free(ui_objects);
    free(thread_args);


    return EXIT_SUCCESS;
}








// ===========================================
//                 FUNCTIONS
// ===========================================





void show_on_screen(gpointer data, gchar *text)
{
    GtkTextView         *outputField;
    GtkTextBuffer       *text_buffer;
    gchar               *utf8_text;
    gsize               bytes_read;
    gsize               bytes_written;
    GError              *error;
    GtkTextMark         *mark;
    GtkTextIter         iter;
    gchar               *text_with_newline;

    error = NULL;

    outputField = GTK_TEXT_VIEW(((UI_Object_Set *) data)->outputField);

    text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(outputField));


    text_with_newline = malloc(sizeof(gchar) * strlen(text) + 2);
    for (size_t i = 0; i < strlen(text); i++)
    {
        text_with_newline[i] = text[i];
    }

    // Add newline character and null-byte to the ascii text.
    text_with_newline[strlen(text)] = '\n';
    text_with_newline[strlen(text) + 1] = 0;



    utf8_text = g_convert(text_with_newline, strlen(text_with_newline),
        "UTF-8", "ISO-8859-1", &bytes_read, &bytes_written, &error);

    if (error != NULL)
    {
        fprintf(stderr, "%s\n", error->message);
        g_error_free(error);
    }

    assert(bytes_written == (unsigned long) g_utf8_strlen(utf8_text, -1));


    g_mutex_trylock(&mutex);


    // Returns the mark that represents the cursor (insertion point).
    mark = gtk_text_buffer_get_insert(text_buffer);


    gtk_text_buffer_get_iter_at_mark(text_buffer, &iter, mark);


    gtk_text_buffer_insert(text_buffer, &iter, utf8_text,
        g_utf8_strlen(utf8_text, -1));


    if (g_mutex_trylock(&mutex) == FALSE)
        g_mutex_unlock(&mutex);


    g_free(utf8_text);
    g_free(text_with_newline);
}






gchar *get_entry(gpointer data)
{
    GtkEntry            *inputField;
    gchar               *ascii_string;

    inputField = GTK_ENTRY(((UI_Object_Set *) data)->inputField);

    // Parameter "C" to do utf-8 translation independently on current locale.
    ascii_string =  g_str_to_ascii(gtk_entry_get_text(inputField), "C");

    gtk_entry_set_text(inputField, "");

    return ascii_string;
}







void on_send(__attribute__ ((unused)) GtkWidget *widget, gpointer data)
{
    gchar           *ascii_string;
    int             written;
    int             length;

    ascii_string = get_entry(data);


    show_on_screen(data, ascii_string);

    length = strlen(ascii_string);


    // Locking the access for write().
    g_mutex_trylock(&mutex);


    while(length > 0)
    {
        // Sending length + 1 characters because we want to include
        // the null-byte
        written = write(((UI_Object_Set *)data)->fd, ascii_string, length + 1);

        if (written == -1)
        {
            printf("Write error in on_send().\n");
        }

        else if (written == length)
        {
            break;
        }

        length -= written;
        ascii_string += written;
    }

    // Unlocking the mutex
    if (g_mutex_trylock(&mutex) == FALSE)
        g_mutex_unlock(&mutex);
}







void *read_from_server(gpointer args)
{
    gchar           buffer[BUFFER_SIZE + 1];
    gpointer        data;
    int             fd;
    int             nb_read;


    data = ((Thread_Arguments *)args)->data;
    fd = ((Thread_Arguments *)args)->file_descriptor;

    memset(buffer, 0, BUFFER_SIZE + 1);


    while (1)
    {
        // Locking for read().
        g_mutex_trylock(&mutex);


        nb_read = read(fd, buffer, BUFFER_SIZE);


        if (nb_read == -1)
        {
            printf("Read error in read_from_server().\n");
            break;
        }

        show_on_screen(data, buffer);
        memset(buffer, 0, BUFFER_SIZE + 1);


        if (g_mutex_trylock(&mutex) == FALSE)
            g_mutex_unlock(&mutex);
    }


    return NULL;
}
