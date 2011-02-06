%% Initialization
clear all;
port = 'COM25'; 
serial_port = serial(port,'BaudRate',38400,'DataBits',8,'Timeout', 0.5);
packet_id = floor(rand*254);
base_number = 10;
node_id = 21;
reading_test_result = zeros(100000,3+base_number);
%% Open the serial port
try
    fopen(serial_port);
catch ME
   % print out warning
   error('fail to open the serial port, check connection and name'); 
end
%% Main loop
for mainloop = 1 : 100000
    fprintf('the %d round\n', mainloop);
    % reading_test_result(timestamp, node_id, compass_reading, signature);
    reading_test_result(mainloop, 1) = now;
    reading_test_result(mainloop, 2) = node_id; % node id is 21-25
    [reading_test_result(mainloop, 3) packet_id] = get_dir_from_port(packet_id, node_id, serial_port);
    [reading_test_result(mainloop, 4:3+base_number) packet_id] = get_sig_from_port(packet_id, serial_port, base_number, node_id);
    reading_test_result(mainloop,:)
    node_id = node_id + 1;
    if node_id > 25
        node_id = 21;
    end
    save 'reading_test_result_timestamp.mat';
    pause(1);
end
%% Close port
try
    stopasync(serial_port);
    fclose(serial_port);
    delete(serial_port);
    clear serial_port;
catch ME
    % print out warning
    error('fail to close the serial port, check connection and name'); 
end
clear packet_id;
clear mainloop;
save 'reading_test_result_timestamp.mat';

