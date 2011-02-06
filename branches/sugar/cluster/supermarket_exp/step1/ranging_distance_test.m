%% Initialization
iteration = 10;
base_number = 1;
port = 'COM11';
packet_id = floor(rand*254);
node_id = 2;
sig = inf;
serial_port = serial(port,'BaudRate',38400,'DataBits',8,'Timeout', 0.5);
%% Open the serial port
try
    fopen(serial_port);
catch ME
   error('fail to open the serial port, check connection and name'); 
end
%% Main loop
while 1
    for c = 1 : iteration
        [sig packet_id] = get_sig_from_port(packet_id, node_id, serial_port, base_number);
        if sig ~= inf
            break;
        end
    end
    if sig == inf
        fprintf('failed\n');
    else
        fprintf('ranging success\n');
    end
    cont = input('continue?');
    if cont == 0 
        break;
    end
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
clear all;