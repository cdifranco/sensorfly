function ack = socket_send(d_output_stream, message)
d_output_stream.writeBytes(char(message));
d_output_stream.flush;