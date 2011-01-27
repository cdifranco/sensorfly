% use serial port to get signature from anchors
function [sig packet_id] = get_sig_from_port(packet_id, node_id, serial_port, base_number)
sig = zeros(1, base_number);
for anchor_id = 1:base_number
    %% generate ranging packet
    packet_id = mod(packet_id + 1, 255);
    msg_array = [uint8(packet_id),uint8('t'),uint8(0),uint8(node_id),uint8(1),uint8(24), typecast(uint16(anchor_id+2),'uint8'), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(single(0.0),'uint8'), typecast(single(0.0),'uint8')];
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
    tx_pkt_count = 1;
    %% wait for respense of direction to anchor
    while 1
        try
            %% reading from serial port
            rx_pkt_info = fscanf(serial_port)
            temp_double = textscan(rx_pkt_info,  '%d, %c, %d, %d, %d, %d, %d, %d, %d, %f, %f');
            data_double = temp_double{10};
            data_error = temp_double{11};
            %% check if the ranging is correct
            if data_error ~= 0.0
                throw(ME);
            end
            %% record
            sig(anchor_id) = data_double;
            break;
        catch ME
            %%
            pause(tx_pkt_count/4);
            fprintf(2, 'stack at anchor %d: %s\n',anchor_id+2, rx_pkt_info);
            %% generate the ranging packet again
            packet_id = mod(packet_id + 1, 255);
            msg_array = [uint8(packet_id),uint8('t'),uint8(0),uint8(node_id),uint8(1),uint8(24), typecast(uint16(anchor_id+2),'uint8'), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(single(0.0),'uint8'), typecast(single(0.0),'uint8')];
            % id, type, checksum, dest, src, length, data_int[5], data_float[2]
            msg_new = [uint8(255)];
            for i = 1 : length(msg_array)
                if msg_array(i) == uint8(255) || msg_array(i) == uint8(27) || msg_array(i) == uint8(239)
                    msg_new = [msg_new uint8(27)];
                end
                 msg_new = [msg_new msg_array(i)];
            end
            msg_new = [msg_new uint8(239)];
            %% write to the serial port again
            try
                fwrite(serial_port, msg_new, 'uint8','async');
            catch ME
                stopasync(serial_port);
                fclose(serial_port);
                error('fail to write to the serial port, check connection and name'); 
            end
            %% check for time out
            tx_pkt_count = tx_pkt_count + 1;
            if tx_pkt_count > 20
                sig(anchor_id) = -1;
                stopasync(serial_port);
                fclose(serial_port);
                error('fail to read from the serial port, check connection and name'); 
            end
            continue;
        end
    end
    flushinput(serial_port);
end