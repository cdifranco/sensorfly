function [dir packet_id] = get_dir_from_port(packet_id, node_id, serial_port)
packet_id = mod(packet_id + 1, 255);
msg_array = [uint8(packet_id),uint8('t'),uint8(0),uint8(node_id),uint8(1),uint8(24), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(single(0.0),'uint8'), typecast(single(0.0),'uint8')];
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
    msg_new = [msg_new uint8(239)];
    fwrite(serial_port, msg_new, 'uint8','async');
catch ME
     stopasync(serial_port);
     fclose(serial_port);
     error('fail to write to the serial port, check connection and name'); 
end
%fprintf('wait for dir\n');
tx_pkt_count = 1;
while 1
    % wait for respense of direction to anchor
    try
        rx_pkt_info = fscanf(serial_port);
        temp_double = textscan(rx_pkt_info,  '%d, %c, %d, %d, %d, %d, %d, %d, %d, %f, %f');
        data_int = temp_double{7}
        if  data_int <= 900
            dir = 1;
        elseif data_int <= 1800
            dir = 2;
        elseif data_int <= 2700
            dir = 3;
        else
            dir = 4;
        end
        break;
    catch ME
        % send request of direction to anchor
        fprintf(2, 'stack at getting dir: %s\n',rx_pkt_info);
        try
            packet_id = mod(packet_id + 1, 255);
            msg_array = [uint8(packet_id),uint8('t'),uint8(0),uint8(node_id),uint8(1),uint8(24), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(single(0.0),'uint8'), typecast(single(0.0),'uint8')];
            % id, type, checksum, dest, src, length, data_int[5], data_float[2]
            msg_new = [uint8(255)];
            for i = 1 : length(msg_array)
                if msg_array(i) == uint8(255) || msg_array(i) == uint8(27) || msg_array(i) == uint8(239)
                    msg_new = [msg_new uint8(27)];
                end
                 msg_new = [msg_new msg_array(i)];
            end
            msg_new = [msg_new uint8(239)];
            fwrite(serial_port, msg_new, 'uint8','async');
        catch ME
             stopasync(serial_port);
             fclose(serial_port);
             error('fail to write to the serial port, check connection and name'); 
        end
        tx_pkt_count = tx_pkt_count + 1;
        if tx_pkt_count > 20
            fclose(serial_port);
            error('fail to read from the serial port, check connection and name'); 
        end
        continue;
    end
end
flushinput(serial_port);            

