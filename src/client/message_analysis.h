#ifndef _MESSAGE_ANALYSIS_H_
#define _MESSAGE_ANALYSIS_H_
#define PADDING_LENGTH 4





// Returns the length of the received message assuming that the
// message is padded with its length.
// Parameter `nb_bytes_to_consider` is used to determine the characters
// that should be considered as the padded length.
int get_length_received_msg(gchar *msg);





// Returns the message padded with the length in front of it.
// We assume that the length will be 9999 at the most so the
// padding will always be 4 characters long. The returned string
// has to be freed. String `msg` is freed.
gchar *pad_msg_with_length(gchar *msg);





// Returns the message in `msg` without the padded length.
// Frees the string `msg`.
gchar *extract_message_only(gchar *msg);





#endif /* !_MESSAGE_ANALYSIS_H_ */
