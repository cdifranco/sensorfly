%% Initialization
clear_record = input('clear record?(yes: 1/ no:0)\n');
if clear_record == 1
	sig_count = 1;
	x = 100;
	y = 0;
	reading = zeros(main_loop_count, 4+base_number);
end
serial_port = serial(port,'BaudRate',38400,'DataBits',8,'Timeout', 0.5);
%% Open the serial port
try
    fopen(serial_port);
catch ME
   % print out warning
   error('fail to open the serial port, check connection and name'); 
end
%% Main loop
while 1
	if mod(sig_count,100) == 0
		fprintf(2,'continue?');
		cont = input('(yes: 1/ no:0)');
		if cont == 0 
			break;
		end
	end
	fprintf('round %d\n',sig_count);
    node_id = 12;
    %% get readings
    fprintf('get readings\n');
    tic;
	%reading(realx, realy, dir, dir_reading, signature)
    if sig_count ~=1
        [x y] = get_real_location(x, y); %real location
    end
    for c = 1:50
        reading(sig_count, 1:2) = [x, y];
        [reading(sig_count, 5:4+base_number) packet_id] = get_sig_from_port(packet_id, serial_port, base_number);
        [reading(sig_count,3:4) packet_id] = get_dir_from_port(packet_id, node_id, serial_port); % 2 is virtual dir, 3 is real dir
        sig_count = sig_count + 1;
    end
    toc;
    %% calculate the believe
    save '1_26_raw_data.mat';
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
save '1_26_raw_data.mat';