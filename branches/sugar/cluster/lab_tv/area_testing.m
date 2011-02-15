%% init
serial_port = serial(port, 'BaudRate', 38400, 'DataBits', 8, 'Timeout', 0.5);
%% Open port
try
    fopen(serial_port);
catch ME
   % print out warning
   error('fail to open the serial port, check connection and name'); 
end
%%
while 1

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