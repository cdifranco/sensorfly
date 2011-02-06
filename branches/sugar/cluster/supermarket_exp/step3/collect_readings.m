%% Initialization
clear_record = input('clear record?(yes: 1/ no:0)\n');
if clear_record == 1
	sig_count = 1;
	x = 0;
	y = 0;
	reading = []; % reading(real_x, real_y, dir, compass_reading, sig)
else
    load ('raw_data_supermarket.mat');
    sig_count = floor(sig_count/20)*20+1;  %input('from which round?\n');
    fprintf('current location: %d, %d \n',reading(sig_count-1,1),reading(sig_count-1,2));
end
iteration = -1; % initialization
base_number = 15;
port = 'COM11';
packet_id = floor(rand*254);
node_id = 2;
serial_port = serial(port,'BaudRate',38400,'DataBits',8,'Timeout', 0.5);
%% Open the serial port
try
    fopen(serial_port);
catch ME
   error('fail to open the serial port, check connection and name'); 
end
%% Main loop
while 1
	fprintf('round %d\n',sig_count);
    if sig_count ~=1
        [x y] = get_real_location(x, y); %real location
    end
    if x == -1 && y == -1
        break;
    end
    if mod(x, 100) == 0 && mod(y, 100) == 0
        iteration = 20;
    else 
        iteration = 1;
    end
    tic;
    [dir compass_reading packet_id] = get_dir_from_port(packet_id, node_id, serial_port); % 2 is virtual dir, 3 is real dir
    for c = 1 : iteration
        reading(sig_count, 1:2) = [x, y];
        reading(sig_count,3:4) = [dir compass_reading];
        [reading(sig_count, 5:4+base_number) packet_id] = get_sig_from_port(packet_id, node_id, serial_port, base_number);
        sig_count = sig_count + 1;
    end
    toc;
    save 'raw_data_supermarket.mat';
end
%% Close port
try
    stopasync(serial_port);
    fclose(serial_port);
    delete(serial_port);
    clear serial_port;
catch ME
    error('fail to close the serial port, check connection and name'); 
end
%% Clear up
clear port;
clear packet_id;
clear node_id;
clear serial_port;
clear cont;
clear c;
%% Save data
save 'raw_data_supermarket.mat';