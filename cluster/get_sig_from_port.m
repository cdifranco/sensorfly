function [sig packet_id] = get_sig_from_port(packet_id, serial_port, base_number)

sig = zeros(1, base_number);
%fprintf('generate array');
%tic;
msg_array = [uint8(packet_id),uint8('g'),uint8(0),uint8(2),uint8(1),uint8(24), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(uint16(0),'uint8'), typecast(single(0.0),'uint8'), typecast(single(0.0),'uint8')];
% id, type, checksum, dest, src, length, data_int[5], data_float[2]
msg_new = [uint8(255)];
for i = 1 : length(msg_array)
    if msg_array(i) == uint8(255) || msg_array(i) == uint8(27) || msg_array(i) == uint8(239)
        msg_new = [msg_new uint8(27)];
    end
     msg_new = [msg_new msg_array(i)];
end
msg_new = [msg_new uint8(239)];
%toc;


for anchor_id = 1:base_number
    fprintf('get sig: %d\n',anchor_id+1);
    packet_id = mod(packet_id + 1, 255);
    msg_new(strfind(msg_new,'g')-1) = uint8(packet_id);
    msg_new(strfind(msg_new,'g')+2) = uint8(anchor_id+1);
    
    % send request of direction to anchor
   % fprintf('write to port');
   % tic;
    try
        fwrite(serial_port, msg_new, 'uint8','async');
    catch ME
        stopasync(serial_port);
        fclose(serial_port);
        error('fail to write to the serial port, check connection and name'); 
    end
    %toc;
    %fprintf('wait for sig %d\n', anchor_id);
    tx_pkt_count = 1;
    %tic;
    while 1
        % wait for respense of direction to anchor
        try
            rx_pkt_info = fscanf(serial_port);
            temp_str = strread(rx_pkt_info, '%s', 'delimiter', sprintf(','));
            pkt_rx = char(temp_str);
            data_double = str2double(pkt_rx(1,:));
            data_error = str2double(pkt_rx(2,:));
            
            if data_error ~= 0.0
                continue;
            end
            sig(anchor_id) = data_double;
            break;
        catch ME
            packet_id = mod(packet_id + 1, 255);
            msg_new(strfind(msg_new,'g')-1) = uint8(packet_id);
            msg_new(strfind(msg_new,'g')+2) = uint8(anchor_id+1);
            try
                fwrite(serial_port, msg_new, 'uint8','async');
            catch ME
                stopasync(serial_port);
                fclose(serial_port);
                error('fail to write to the serial port, check connection and name'); 
            end
            tx_pkt_count = tx_pkt_count + 1;
            if tx_pkt_count > 4
                fclose(serial_port);
                error('fail to read from the serial port, check connection and name'); 
            end
            continue;
        end
    end
    %toc;
    %fprintf('flush input:');
    %tic;
    flushinput(serial_port);
    %toc;
end
sig