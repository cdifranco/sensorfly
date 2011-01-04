function sig = get_sig_from_port(node_id, port, base_number)
% open the serial port
sig = [];
for anchor_id = 1:base_number
    serial_port = serial(port,'BaudRate',38400,'DataBits',8);
    msg_array = [uint8('0'),uint8('q'),uint8(0),uint8(anchor_id),uint8('0'),uint8(32),typecast(uint16('r'),'uint8'),typecast(uint16(node_id),'uint8')]
    try
        fopen(serial_port);
    catch ME
       % print out warning
       fclose(serial_port);
       error('fail to open the serial port, check connection and name'); 
    end

    % send request of direction to anchor
    try
        % id, type, checksum, dest, src, length, data_int[5], data_float[2]
        msg_new = [uint8(255)];
        for i = 1 : length(msg_array)
            if msg_array(i) == uint8(255) || msg_array(i) == uint8(27) || msg_array(i) == uint8(239)
                msg_new = [msg_new uint8(27)];
            end
             msg_new = [msg_new msg_array(i)];
        end
        msg_new = [msg_new uint8(239)]
        fwrite(serial_port, msg_new, 'uint8','async');
    catch ME
        stopasync(serial_port);
        fclose(serial_port);
        error('fail to write to the serial port, check connection and name'); 
    end

    % wait for respense of direction to anchor
    try
        while serial_port.BytesAvailable < 3
        end
        rx_pkt_info = fscanf(serial_port)
        sig = [sig 1]
    catch ME
        fclose(serial_port);
        error('fail to read from the serial port, check connection and name'); 
    end

    try    
        stopasync(serial_port);
        fclose(serial_port);
        delete(serial_port);
        clear serial_port;
    catch ME
        % print out warning
       error('fail to close the serial port, check connection and name'); 
    end
end