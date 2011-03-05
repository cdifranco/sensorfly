% use serial port to get signature from anchors
function [sig packet_id] = get_sig_from_port(packet_id, serial_port, base_number)
sig = zeros(1, base_number);
invalid_anchors = [3, 6, 8, 10, 11, 12, 14];
for anchor_id = 1:base_number
    if ismember(anchor_id + 2, invalid_anchors)
        sig(anchor_id) = inf;
        continue;
    end
    %% generate ranging packet
    packet_id = mod(packet_id + 1, 255);
    msg_array = [uint8(packet_id),uint8('g'),uint8(0),uint8(anchor_id+2),uint8(1),uint8(24), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(single(0.0),'uint8'), typecast(single(0.0),'uint8')];
    % id, type, checksum, dest, src, length, data_int[5], data_float[2]
    msg_new = [uint8(255)];
    for i = 1 : length(msg_array)
        if msg_array(i) == uint8(255) || msg_array(i) == uint8(27) || msg_array(i) == uint8(239)
            msg_new = [msg_new uint8(27)];
        end
         msg_new = [msg_new msg_array(i)];
    end
    msg_new = [msg_new uint8(239)];
     %% write to serial port
    try
        fwrite(serial_port, msg_new, 'uint8','async');
    catch ME
        stopasync(serial_port);
        fclose(serial_port);
        error('fail to write to the serial port, check connection and name'); 
    end
     %% wait for respense of direction to anchor
    try
        %% reading from serial port
        rx_pkt_info = fscanf(serial_port);
        temp_double = textscan(rx_pkt_info, '%f, %f');
        data_double = temp_double{1};
        data_error = temp_double{2};
        %% check if the ranging is correct
        if data_error ~= 0.0
            throw(ME);
        end
        %% record
        sig(anchor_id) = data_double;
    catch ME
        fprintf(2, 'stuck at anchor %d: %s\n',anchor_id+2, rx_pkt_info);
        sig(anchor_id) = inf;
        stopasync(serial_port);
    end
    flushinput(serial_port);
end
%% Clear up
clear anchor_id;
clear i;
clear temp_double;
clear rx_pkt_info;
clear msg_new;
clear msg_array;
clear data_error;
clear data_double;