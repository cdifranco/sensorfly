%% Initialization
iteration = 100;
base_number = 1;
port = 'COM12';
packet_id = floor(rand*254);
node_id = 3;
sig = inf;
sig_record = [];
serial_port = serial(port,'BaudRate',38400,'DataBits',8,'Timeout', 0.5);
success_rate = [];
%% Open the serial port
try
    fopen(serial_port);
catch ME
   error('fail to open the serial port, check connection and name'); 
end
%% Main loop
while 1
    
    succ = 0;
    realx = input('real x: ');
    realy = input('real y: '); 
    for c = 1 : iteration
        [sig packet_id] = get_sig_from_port(packet_id, serial_port, base_number);
        sig_record = [sig_record, sig];
        if sig ~= inf
            succ = succ + 1;
        end
        sig
    end

    success_rate = [success_rate; realx, realy, succ, succ/iteration];
    fprintf('success rate: %d\n',succ/iteration);
    save 'ranging_test_2_7_15_40.mat';
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