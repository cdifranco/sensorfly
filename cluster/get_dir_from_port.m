function [dir packet_id] = get_dir_from_port(packet_id, node_id, port)
serial_port = serial(port,'BaudRate',38400,'DataBits',8);
packet_id = mod(packet_id + 1, 255);
msg_array = [uint8(packet_id),uint8('t'),uint8(0),uint8(node_id),uint8(1),uint8(24), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(single(0.0),'uint8'), typecast(single(0.0),'uint8')];
% open the serial port
try
    fopen(serial_port);
catch ME
   % print out warning
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
    msg_new = [msg_new uint8(239)];
    fwrite(serial_port, msg_new, 'uint8','async');
catch ME
     stopasync(serial_port);
     fclose(serial_port);
     error('fail to write to the serial port, check connection and name'); 
end
fprintf('wait for dir\n');
tx_pkt_count = 1;
while 1
    % wait for respense of direction to anchor
    try
        rx_pkt_info = fscanf(serial_port);
        temp_str = strread(rx_pkt_info, '%s', 'delimiter', ',')';
        pkt_rx = char(temp_str);
        data_int = str2num(pkt_rx(7,:))
        if data_int <= 450 || data_int > 3150
            dir = 1;
        elseif data_int > 450 && data_int <= 1350
            dir = 2;
        elseif data_int > 1350 && data_int <= 2250
            dir = 3;
        else
            dir = 4;
        end
        break;
    catch ME
        % send request of direction to anchor
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

try    
    stopasync(serial_port);
    fclose(serial_port);
    delete(serial_port);
    clear serial_port;
catch ME
    % print out warning
   error('fail to close the serial port, check connection and name'); 
end