%% init
serial_port = serial(port, 'BaudRate', 38400, 'DataBits', 8, 'Timeout', 0.5);
clr = input('clear area cluster relation form?');
if clr ~=0
    area_cluster_relation = cell(1,27);
    for i = 1 : area_number
        area_cluster_relation{i} = -1;
    end
end
%% Open port
try
    fopen(serial_port);
catch ME
   % print out warning
   error('fail to open the serial port, check connection and name'); 
end
%% loop for each area
while 1
    area_id = input('area id: ');
    area_cluster_relation{area_id} = [];
    cluster_in_one_area = [];  
    for j = 1 : 100
      current_sig = get_sig_from_port(packet_id, serial_port, base_number); 
      cluster_in_one_area = [cluster_in_one_area, get_cluster_sig(center_sig, current_sig)]
    end
    temp = [];
    while size(cluster_in_one_area, 2) > 0
      [max count] = mode(cluster_in_one_area);
      temp = [temp, ones(1,count)*max];
      cluster_in_one_area(cluster_in_one_area(:) == mode(cluster_in_one_area))=[];
    end
    area_cluster_relation{area_id} = temp;
    save '1_18_morning_afterwards_movement_1p0_ac_4(re_all).mat'
    cont = input('still continue?');
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
    % print out warning
   error('fail to close the serial port, check connection and name'); 
end
%% Clear up
clear clr;
clear j;
clear temp;
clear cluster_in_one_area;