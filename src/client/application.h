#ifndef _APPLICATION_H_
#define _APPLICATION_H_



typedef struct
{
    GObject *mainWindow;
    GObject *quitButton;
    GObject *leaveButton;
    GObject *sendButton;
    GObject *inputField;
    GObject *outputField;
    GObject *scrollWindow;
    int fd;

} UI_Object_Set;





typedef struct
{
    gpointer data;
    int file_descriptor;

} Thread_Arguments;






// Shows the text on the display of the application.
// The first parameter the the structure UI_Object_Set
// whereas the second parameter is the text to be displayed.
// The text is an ascii null-terminated string.
void show_on_screen(gpointer, gchar *);



// Returns a null terminated ascii string from the GtkEntry.
// Sets the GtkEntry field to empty string when called.
gchar *get_entry(gpointer);



// Function that is executed after the button send is clicked.
// First parameter is unused but mandatory. Second is the pointer to the
// UI_Object_Set structure.
void on_send(GtkWidget *, gpointer);



// Function that reads bytes sent by the server and
// displays them on the screen.
// Takes as parameter a Thread_Arguments structure.
void *read_from_server(gpointer);






#endif /* ! _APPLICATION_H_ */
