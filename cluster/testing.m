len = 0;
error = 0;
e = 0;
guiding_success = zeros(testing_round);
guiding_error = zeros(testing_round);
serial_port = serial(port,'BaudRate',38400,'DataBits',8,'Timeout', 0.5);
%%
try
    fopen(serial_port);
catch ME
   % print out warning
   error('fail to open the serial port, check connection and name'); 
end
%%
%area_to_cluster;
%%
for j = 1:testing_round
    destArea = input('destiny area: ');
    %create matrix
    number_of_center = size(center_new,1);
    matrix = zeros(number_of_center, number_of_center);
    for l = 1:number_of_center
        for m = 1:number_of_center
            sumIJ = sum(trans_history(count_to_id(l),:,count_to_id(m)));
            if sumIJ == trans_init_number*direction_number
                matrix(l, m) = inf;
            else
                sum_of_l_to_m = 0;
                for n = 1 : number_of_center
                    sum_of_l_to_m = sum_of_l_to_m + sum(trans_history(count_to_id(l),:,count_to_id(n)));
                end
                matrix(l, m) = sum(trans_history(count_to_id(l),:,count_to_id(m)))/sum_of_l_to_m;
                matrix(l, m) = 1/matrix(l, m);
            end;
        end;
    end;
    %[succ sigRoute] = navigate_basic(packet_id, serial_port, reading, destArea, base_number, trans_history, center_sig, count_to_id, area_cluster_relation, matrix);
    [succ sigRoute] = navigate_basic(packet_id, serial_port, reading, destArea, base_number, trans_history, center_sig, count_to_id, matrix, top);
    fprintf('you have forwarded %d steps\n',size(sigRoute,1));
    stillcontinue = input('still continue? (yes:1/no:0)');
    if stillcontinue == 0
        break;
    end
end
%%
try    
    stopasync(serial_port);
    fclose(serial_port);
    delete(serial_port);
    clear serial_port;
catch ME
    % print out warning
   error('fail to close the serial port, check connection and name'); 
end