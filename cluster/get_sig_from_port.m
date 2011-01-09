function [sig packet_id] = get_sig_from_port(packet_id, node_id, port, base_number)
% open the serial port
sig = [];
for anchor_id = 2:base_number+1
    serial_port = serial(port,'BaudRate',38400,'DataBits',8);
    packet_id = mod(packet_id + 1, 255);
    msg_array = [uint8(packet_id),uint8('t'),uint8(0),uint8(anchor_id),uint8(1),uint8(24), typecast(uint16(node_id),'uint8'), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(single(0.0),'uint8'), typecast(single(0.0),'uint8')];
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
        msg_new = [msg_new uint8(239)];
        fwrite(serial_port, msg_new, 'uint8','async');
    catch ME
        stopasync(serial_port);
        fclose(serial_port);
        error('fail to write to the serial port, check connection and name'); 
    end
    fprintf('wait for sig %d\n', anchor_id);
    tx_pkt_count = 1;
    while 1
        % wait for respense of direction to anchor
        try
            rx_pkt_info = fscanf(serial_port);
            temp_str = strread(rx_pkt_info, '%s', 'delimiter', sprintf(','));
            pkt_rx = char(temp_str);
            data_double = str2num(pkt_rx(10,:));
            sig = [sig data_double]
            break;
        catch ME
            packet_id = mod(packet_id + 1, 255);
            msg_array = [uint8(packet_id),uint8('t'),uint8(0),uint8(anchor_id),uint8(1),uint8(24), typecast(uint16(node_id),'uint8'), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(single(0.0),'uint8'), typecast(single(0.0),'uint8')];
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
end