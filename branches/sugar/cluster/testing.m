len = 0;
error = 0;
e = 0;
serial_port = serial(port,'BaudRate',38400,'DataBits',8,'Timeout', 0.5);
try
    fopen(serial_port);
catch ME
   % print out warning
   error('fail to open the serial port, check connection and name'); 
end

for j = 1:testing_round
    destArea = input('destiny area: ');
    %create matrix
    noOfNodes = size(trans_history,1);
    matrix = zeros(noOfNodes, noOfNodes);
    for l = 1:noOfNodes
        for m = 1:noOfNodes
            sumIJ = sum(trans_history(l,:,m));
            if sumIJ == trans_init_number*direction_number
                matrix(l, m) = inf;
            else
                matrix(l, m) = sum(trans_history(l,:,m))/sum(sum(trans_history(l,:,:)));
                matrix(l, m) = 1/matrix(l, m);
            end;
        end;
    end;
    [succ sigRoute startClus destClus] = navigate_basic(packet_id, serial_port, reading, destArea, base_number, trans_history, center_sig, matrix);
    fprintf('you have forwarded %d steps\n',length(sigRoute));
    stillcontinue = input('still continue? (yes:1/no:0)');
    if stillcontinue == 0
        break;
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