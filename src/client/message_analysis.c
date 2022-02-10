#include <gtk/gtk.h>
#include <string.h>
#include <assert.h>

#include "message_analysis.h"



int get_length_received_msg(__attribute__((unused)) gchar *msg)
{
    int length = 0;

    assert(msg != NULL);

    for (size_t i = 0; i < PADDING_LENGTH; i++)
    {
        length *= 10;
        length += msg[i] - '0';
    }

    return length;
}







gchar *pad_msg_with_length(gchar *msg)
{
    size_t length_padded        = 0;
    size_t length_not_padded    = 0;
    gchar *padded_msg           = NULL;


    assert(msg != NULL);
    assert(strlen(msg) <= 9999);



    length_not_padded = strlen(msg);
    length_padded = length_not_padded + PADDING_LENGTH;



    // Don't forget the ending null-byte.
    padded_msg = malloc(sizeof(gchar) * length_padded + 1);
    memset(padded_msg, 0, length_padded + 1);



    // Padding the message with the length.
    for (int i = PADDING_LENGTH; i > 0; i--)
    {
        padded_msg[i - 1] = (length_not_padded % 10) + '0';
        length_not_padded /= 10;
    }

    // Copying the remaining of the message.
    for (size_t i = PADDING_LENGTH; i < length_padded; i++)
    {
        padded_msg[i] = msg[i - PADDING_LENGTH];
    }


    free(msg);
    return padded_msg;
}



