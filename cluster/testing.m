%% init
clr = input('clear record?');
if clr == 1
    step = [];
    observed_step = [];
    error = [];
    success = [];
    start_round = 1;
else
    start_round = input('from which round:');
end
serial_port = serial(port, 'BaudRate', 38400, 'DataBits', 8, 'Timeout', 0.5);
%% Open port
try
    fopen(serial_port);
catch ME
   % print out warning
   error('fail to open the serial port, check connection and name'); 
end
%% testing loop
for j = start_round:testing_round
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
    [succ sigRoute] = navigate_basic(packet_id, serial_port, destArea, base_number, trans_history, center_sig, count_to_id, matrix, area_cluster_relation);
    %[succ sigRoute] = navigate_basic(packet_id, serial_port, reading, destArea, base_number, trans_history, center_sig, count_to_id, matrix, top);
    fprintf('you have forwarded %d steps\n',size(sigRoute,1));
    s = input('success?(yes:1/no:0)');
    success=  [success, s];
    if s == 1
        os = input('observed step: ');
        observed_step = [observed_step, os];
        e = input('error: ');
        error = [error, e];
        step = [step, size(sigRoute,1)];
    else
        observed_step = [observed_step, -1];
        error = [error, -1];
        step = [step, -1];
    end
    save '1_18_afternoon_afterwards_movement_t_cf0p7_1.mat'
    cont = input('still continue? (yes:1/no:0)');
    if cont == 0
        break;
    end
end
step_rate = step(step(:)~=-1)/observed_step(observed_step(:)~=-1)
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
%%
clear cont;
clear l;
clear m;
clear j;