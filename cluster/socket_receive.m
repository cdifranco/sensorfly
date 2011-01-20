function message = socket_receive(d_input_stream, bytes_available)
fprintf(1, 'Reading %d bytes\n', bytes_available);
message = zeros(1, bytes_available, 'uint8');
for i = 1:bytes_available
    message(i) = d_input_stream.readByte;
end
